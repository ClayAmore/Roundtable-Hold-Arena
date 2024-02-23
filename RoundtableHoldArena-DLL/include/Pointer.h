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
#ifndef _DEEP_PTR_H
#define _DEEP_PTR_H

#include <Windows.h>
#include <initializer_list>

// Helper functions for reading memory addresses
inline uintptr_t GetDeepPtrAddr(uintptr_t* base, std::initializer_list<uintptr_t> offsets)
{
    for (uintptr_t o : offsets)
    {
        if ((uintptr_t)base < 0x10000 || IsBadReadPtr(base, sizeof(void*))) return NULL;
        base = (uintptr_t*)(*base + o);
    }
    return (uintptr_t)base;
}

template<typename TRet, typename TBase, typename ... TOffsets> TRet* AccessDeepPtr(TBase base, TOffsets... offsets)
{
    uintptr_t addr = GetDeepPtrAddr((uintptr_t*)base, { (uintptr_t)offsets... });
    return (addr < 0x10000 || IsBadReadPtr((void*)addr, sizeof(TRet))) ? NULL : (TRet*)addr;
}

#endif