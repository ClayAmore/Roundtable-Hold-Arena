#pragma once
#include "Global.h"
#include "Pointer.h"
#include <regex>
#include <cstring> // For C-string functions

using namespace BS2;

class File {
public:

    struct DLString_EldenRing {
        void* unk;
        wchar_t* string; // Assuming the string is wide character (wchar_t) based on the previous code.
        void* unk2;
        UINT64 length;
        UINT64 capacity;

        wchar_t* str()
        {
            if (sizeof(wchar_t) * capacity >= 15) {
                return string;
            }

            return reinterpret_cast<wchar_t*>(&string);
        }
    };

    static std::vector<std::wstring> ModFiles;

    typedef HANDLE(WINAPI* tCreateFileW)
        (LPCWSTR lpFileName, 
        DWORD dwDesiredAccess, DWORD dwShareMode,
        LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
        DWORD dwCreationDisposition, 
        DWORD dwFlagsAndAttributes,
        HANDLE hTemplateFile);
    typedef void* VirtualToArchivePathFunction(DLString_EldenRing* path, UINT64 p2, UINT64 p3, UINT64 p4, UINT64 p5, UINT64 p6);

    static tCreateFileW tCreateFileWOriginal;
    static VirtualToArchivePathFunction* VirtualToArchivePathOriginal;

    static uintptr_t CreateFileWPtr();
    static uintptr_t VirtualToArchivePtr();

    static bool FilePathContainsNames(const std::wstring& filePath, const std::vector<std::wstring>& namesToCheck);
    static void ReplaceFilePath(std::wstring& filePath, const std::vector<std::wstring>& pathsToCheck);
    static void ProcessArchivePath(wchar_t* rawPath, size_t rawPathLen);
    static void TransformPath(DLString_EldenRing* path, const wchar_t* source, const wchar_t* target);
    static void LoadModFilePaths(const std::wstring& directoryPath);

private:
    static std::wstring GetFileName(const std::wstring& path);
};


inline void File::ReplaceFilePath(std::wstring& filePath, const std::vector<std::wstring>& pathsToCheck) {
    std::wstring lowerFileName = GetFileName(filePath);
    std::transform(lowerFileName.begin(), lowerFileName.end(), lowerFileName.begin(), ::towlower);

    for (const std::wstring& path : pathsToCheck) {
        std::wstring lowerPath = GetFileName(path);
        std::transform(lowerPath.begin(), lowerPath.end(), lowerPath.begin(), ::towlower);

        // Check if the lowercased file name exists in the lowercased path
        if (lowerPath == lowerFileName) {
            std::wcout << L"Matched File Name: " << lowerFileName << std::endl;
            std::wcout << L"Replacing File Path: " << filePath << std::endl;
            // Replace the filePath with the matched path
            filePath = path;
            std::wcout << L"New File Path: " << filePath << std::endl;
            return; // Exit the function after the replacement is done
        }
    }
}

// Function to check if the file path contains certain names
inline bool File::FilePathContainsNames(const std::wstring& filePath, const std::vector<std::wstring>& pathsToCheck) {
    std::wstring lowerFileName = GetFileName(filePath);
    std::transform(lowerFileName.begin(), lowerFileName.end(), lowerFileName.begin(), ::towlower);

    for (const std::wstring& path : pathsToCheck) {
        std::wstring lowerPath = GetFileName(path);
        std::transform(lowerPath.begin(), lowerPath.end(), lowerPath.begin(), ::towlower);

        // Check if the lowercased name exists in the lowercased file path
        if (lowerPath == lowerFileName) {
            return true;
        }
    }

    // No match found, return false
    return false;
}

inline void File::ProcessArchivePath(wchar_t* rawPath, size_t rawPathLen)
{
    // Regular expression pattern to match archive file paths
    static const std::wregex archiveFilePattern(
        L"("
        L"data.:\\/"   // References to main data archives (data1:/ etc)
        L").+"
    );

    std::wstring path(rawPath, rawPathLen);

    std::wsmatch matches;
    std::regex_match(path, matches, archiveFilePattern);

    // If the regular expression does not match, return
    if (matches.empty()) {
        return;
    }

    const size_t prefixLen = matches[1].length();
    rawPath[0] = L'.';

    for (size_t i = 1; i < prefixLen && i < rawPathLen; i++) {
        rawPath[i] = L'/';
    }
}

inline void File::TransformPath(DLString_EldenRing* path, const wchar_t* source, const wchar_t* target) {
    // Perform the in-place transformation in the path struct.
    std::wstring originalPath(path->str(), path->length);

    std::wstring sourceStr(source);
    std::wstring targetStr(target);

    size_t pos = originalPath.find(sourceStr);
    if (pos != std::wstring::npos) {
        std::wstring transformedPath = originalPath.replace(pos, sourceStr.length(), targetStr);
        size_t newLength = transformedPath.length();

        if (newLength < path->capacity) {
            wcscpy(path->str(), transformedPath.c_str());
            path->length = newLength;
        }
    }
}


inline void File::LoadModFilePaths(const std::wstring& directoryPath) {
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            ModFiles.push_back(entry.path().wstring());
        }
        else if (entry.is_directory()) {
            // Recursively get file paths from subdirectories.
            LoadModFilePaths(entry.path().wstring());
        }
    }
}

inline std::wstring File::GetFileName(const std::wstring& path) {
    size_t pos = path.find_last_of(L"\\/"); // Search for either backslash or forward slash
    return (pos != std::wstring::npos) ? path.substr(pos + 1) : path;
}

inline uintptr_t File::CreateFileWPtr() {
    TCHAR system_folder[MAX_PATH];
    if (!GetSystemDirectory(system_folder, MAX_PATH)) {
        throw std::runtime_error("GetSystemDirectory(..., MAX_PATH) failed");
    }
    std::filesystem::path systemPath = std::move(std::filesystem::path(system_folder));
    auto kernel32_path = systemPath / "kernel32.dll";
    auto mod = GetModuleHandleW(kernel32_path.c_str());
    if (mod == nullptr || mod == 0) return 0;
    auto addr = (uintptr_t)GetProcAddress(mod, "CreateFileW");
    return addr;
}

inline uintptr_t File::VirtualToArchivePtr() {
    uint8_t* ptr = Global::VIRTUAL_TO_ARCHIVE_PATH.ptr();
    // The size of the call instruction, usually 5 bytes for x86 architecture
    const size_t callInstructionSize = 5;

    // Assuming the format of the call instruction is: CALL [address]
    if (ptr[0] == 0xE8) {
        // Calculate the relative offset of the call target (4 bytes signed integer)
        int32_t relativeOffset = *reinterpret_cast<const int32_t*>(ptr + 1);

        // Calculate the target address by adding the relative offset to the next instruction
        uintptr_t targetAddress = reinterpret_cast<uintptr_t>(ptr) + callInstructionSize + relativeOffset;
        return targetAddress;
    }

    // If it's not a CALL instruction, you might need to handle other cases accordingly.
    // For example, you may encounter different call instructions (CALL, CALLQ, etc.),
    // each with its specific encoding.

    return 0; // Return 0 to indicate failure or unsupported call instruction.
}
