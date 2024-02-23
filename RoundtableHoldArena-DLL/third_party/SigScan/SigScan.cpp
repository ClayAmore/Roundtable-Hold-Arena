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
#include "SigScan.h"
#include "hde/hde64.h"

namespace BS2
{
	intptr_t Signature::Scan() const
	{
		mem::default_scanner scanner(pattern);
		if (mem::pointer ptr = scanner.scan(scan_region)) {
			auto addr = ptr.as<intptr_t>() + offset;
			hde64s instr{};

			switch (type) {
			case Type::Address:
				return addr;
			case Type::InInstruction:
				hde64_disasm((void*)addr, &instr);
				// call, jmp, etc
				if (instr.flags & F_RELATIVE) {
					intptr_t imm = 0;
					if (instr.flags & F_IMM8) imm = instr.imm.imm8;
					else if (instr.flags & F_IMM16) imm = instr.imm.imm16;
					else if (instr.flags & F_IMM32) imm = instr.imm.imm32;
					else imm = instr.imm.imm64;
					
					return addr + instr.len + imm;
				}
				// Access to static memory address
				else if (instr.flags & F_MODRM && instr.modrm_mod == 0 && instr.modrm_rm == 0b101) {
					return addr + instr.len + (intptr_t)instr.disp.disp32;
				}
				else return 0;
			default:
				throw std::runtime_error(std::format("Signature type not implemented: {}", static_cast<uint32_t>(type)));
			}
		}
		else return 0;
	}
}
