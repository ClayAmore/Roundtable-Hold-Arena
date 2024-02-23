#pragma once
#include "ParamEditor.h"
#include "LockCamParam.h"

class Camera {
public:
	static bool IsFreeCamOn;
	
	static void ToggleFreeCam();
	static void TurnOffFreeCam();
	static void ResetY();
	static void SetY(float y);

private:    
	// Helper function to construct the static member variable on first use
	static ParamEditor<LockCamParam>& GetParamEditor() {
		static ParamEditor<LockCamParam> pEditor;
		return pEditor;
	}

	static uintptr_t& GetFreeCamMovePtr() {
		static uintptr_t freeCamMovePtr;
		printf("Camera update function pointer is: %p\n", Global::CAMREA_UPDATE_FUNCTION.ptr());
		if (freeCamMovePtr == 0) freeCamMovePtr = FindFreeCamMovePtr(Global::CAMREA_UPDATE_FUNCTION.ptr(), 6)+0x3;
		return freeCamMovePtr;
	}

	static uintptr_t FindFreeCamMovePtr(uint8_t* ptr, int maxRecursionDepth, int currentDepth = 0);
};

inline void Camera::ToggleFreeCam() {
	uintptr_t eldenring = (uintptr_t)GetModuleHandle(0);
	uintptr_t freeCamMovePtr = GetFreeCamMovePtr();
	uint16_t* freeCamMove = AccessDeepPtr<uint16_t>(freeCamMovePtr);
	uint32_t* freeCamFlag = AccessDeepPtr<uint32_t>(Global::FIELD_AREA.ptr(), 0x20, 0xC8);
	uint8_t* freeCamPos = AccessDeepPtr<uint8_t>(Global::FIELD_AREA.ptr(), 0x20, 0xD0, 0);

	if (freeCamMove == nullptr || freeCamFlag == nullptr || freeCamPos == nullptr) {
		printf("Something went wrong. One of the freecam pointers were null.\n");
		return;
	}

	if (*freeCamFlag == 0) {
		if (freeCamPos != nullptr) {
			float* x = AccessDeepPtr<float>(freeCamPos + 0x40);
			float* y = AccessDeepPtr<float>(freeCamPos + 0x44);
			float* z = AccessDeepPtr<float>(freeCamPos + 0x48);

			if (x != nullptr && y != nullptr && z != nullptr) {
				if (*x > 100.0f) {
					*x = 0.0f; *y = 0.0f; *z = 0.0f;
				}
			}
		}
		*freeCamMove = 0x01B1;
		*freeCamFlag = 2;
		
		IsFreeCamOn = true;
		return;
	}

	*freeCamMove = 0xC032;
	*freeCamFlag = 0;
	IsFreeCamOn = false;
}
inline void Camera::TurnOffFreeCam() {
	uintptr_t eldenring = (uintptr_t)GetModuleHandle(0);
	uint16_t* freeCamMove = AccessDeepPtr<uint16_t>(GetFreeCamMovePtr());
	uint32_t* freeCamFlag = AccessDeepPtr<uint32_t>(Global::FIELD_AREA.ptr(), 0x20, 0xC8);

	if (freeCamMove == nullptr || freeCamFlag == nullptr) {
		printf("Something went wrong. One of the freecam pointers were null.\n");
		return;
	}

	*freeCamMove = 0xC032;
	*freeCamFlag = 0;
	IsFreeCamOn = false;
}

inline void Camera::ResetY() {
	SetY(CAMERA_DEFAULT_Y);
}

inline void Camera::SetY(float y) {
	ParamHeader* header = GetParamEditor().GetParamHeader();
	LockCamParam* pParam = GetParamEditor().GetParamDataPtr();
	pParam[0].chrOrgOffset_Y = y;
}

inline uintptr_t Camera::FindFreeCamMovePtr(uint8_t* ptr, int maxRecursionDepth, int currentDepth) {
	// The size of the call instruction, usually 5 bytes for x86 architecture
	const size_t callInstructionSize = 5;

	// The size of the jmp instruction, 1 byte for the opcode and 4 bytes for the relative offset (total 5 bytes)
	const size_t shortJmpInstructionSize = 5;

	// Check if the current recursion depth has reached the maximum limit
	if (currentDepth >= maxRecursionDepth) {
		return 0; // Return 0 to indicate that the pattern was not found within the recursion limit
	}

	uint8_t opcode = ptr[0];
	bool isCall = (opcode == 0xE8);
	bool isJmp = (opcode == 0xE9);
	bool isShortJmp = (opcode == 0xEB);

	if (isCall || isJmp || isShortJmp) {

        // Calculate the relative offset of the call/jmp target
		int32_t relativeOffset;
		if (isCall || isJmp) {
			// CALL or JMP instruction (4 bytes signed integer)
			relativeOffset = *reinterpret_cast<const int32_t*>(ptr + 1);
		}
		else {
			// Short JMP instruction (1 byte signed integer)
			relativeOffset = *reinterpret_cast<const int8_t*>(ptr + 1);
		}

		// Calculate the target address based on the relative offset
		uintptr_t targetAddress;
		if (isCall) {
			// Call instruction
			targetAddress = reinterpret_cast<uintptr_t>(ptr) + callInstructionSize + relativeOffset;
		}
		else if (isJmp || isShortJmp) {
			// JMP instruction or short JMP instruction
			targetAddress = reinterpret_cast<uintptr_t>(ptr) + 2 + relativeOffset;
		}

		// Check if the target address matches the pattern
		uint8_t* targetPtr = reinterpret_cast<uint8_t*>(targetAddress);
		if (memcmp(targetPtr, "\x2A\x02\xCC\x32\xC0\x48\x8D\x64\x24\x08\xFF\x64\x24\xF8", 14) == 0) {
			std::cout << "Pattern found at address: " << reinterpret_cast<void*>(targetAddress) << std::endl;
			// Pattern found, return the target address
			return targetAddress;
		}

		// If the target address doesn't match, continue searching recursively
		return FindFreeCamMovePtr(targetPtr, maxRecursionDepth, currentDepth + 1);
	}

	// If it's not a CALL or JMP instruction, you might need to handle other cases accordingly.
	// For example, you may encounter different call instructions (CALL, CALLQ, etc.),
	// each with its specific encoding.

	// Return 0 to indicate failure or unsupported call/jmp instruction.
	return 0;
}