/*
Copyright (c) 2023 tremwil@gmail.com

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#pragma once
#include "mem/pattern.h"
#include "mem/module.h"
#include <concepts>
#include <exception>
#include <format>

namespace BS2
{
	// Get a particular code section of the main module of the process, and cache it for later querying.
	static const mem::region& MainModuleSection(const char* name = ".text")
	{
		static mem::region region = [name] {
			auto main_module = mem::module::main();
			for (const auto& section : main_module.section_headers()) {
				if (strncmp((char*)section.Name, name, sizeof(section.Name))) continue;
				return mem::region(main_module.start.add(section.VirtualAddress), section.Misc.VirtualSize);
			}
			throw std::runtime_error(std::format("{} section does not exist in main executable module", name));
		}();
		return region;
	}

	struct Signature
	{
		enum class Type
		{
			/// Just returns the address of the AOB + the offset.
			Address,
			/// Assumes the AOB + offset points to an instruction and returns the address
			/// being read/written/executed by said instruction.
			InInstruction
		};

		mem::pattern pattern; // Bytes to search for
		mem::region scan_region = MainModuleSection(); // Region of memory to search said bytes in
		intptr_t offset = 0; // Offset of the target address w.r.t the signature
		Type type = Type::Address; // type of the signature.

		explicit Signature(const mem::pattern& pattern, const mem::region& region, intptr_t offset = 0, Type type = Type::Address) : 
			pattern(pattern), scan_region(region), offset(offset), type(type) {}

		explicit Signature(const mem::pattern& pattern, intptr_t offset = 0, Type type = Type::Address) :
			pattern(pattern), offset(offset), type(type) {}

		Signature(const char* pattern, const mem::region& region, intptr_t offset = 0, Type type = Type::Address) :
			pattern(pattern), scan_region(region), offset(offset), type(type) {}

		Signature(const char* pattern, intptr_t offset = 0, Type type = Type::Address) :
			pattern(pattern), offset(offset), type(type) {}

		// Returns the first match of the signature.
		intptr_t Scan() const;
	};

	// Pointer to an address found by signature scanning.
	template<class T>
	struct SigScannedPtr
	{
		// Create a SigScannedPtr based on the same arguments a Signature would have. 
		// Scanning is performed on construction. *PANICS* if the address could not be found.
		SigScannedPtr(const Signature& sig, const char* debug_name = "unnamed")
		{
			pointer = (T*)sig.Scan();
			if (!pointer) {
				throw std::runtime_error(std::format("failed to find signature for \"{}\"", debug_name));
			}
		}

		T* ptr() const 
		{
			return pointer;
		}

		T* operator->() const
		{
			return pointer;
		}

		T& operator*() const
		{
			return *pointer;
		}

		T& operator[](size_t index) const
		{
			return *(pointer + index);
		}

	protected:
		T* pointer = nullptr;
	};

	// Specialisation of SigScannedPtr for function pointers.
	template<class Ret, class... Args>
	struct SigScannedPtr<Ret(Args...)>
	{
		using T = Ret(Args...);

		// Create a SigScannedPtr based on the same arguments a Signature would have. 
		// Scanning is performed on construction. *PANICS* if the address could not be found.
		SigScannedPtr(const Signature& sig, const char* debug_name = "unnamed")
		{
			pointer = (T*)sig.Scan();
			if (!pointer) {
				throw std::runtime_error(std::format("failed to find signature for \"{}\"", debug_name));
			}
		}

		T* ptr() const
		{
			return pointer;
		}

		Ret operator()(Args... args)
		{
			return (*pointer)(args...);
		}

		T* operator->() const
		{
			return pointer;
		}

		T& operator*() const
		{
			return *pointer;
		}

	protected:
		T* pointer = nullptr;
	};
}