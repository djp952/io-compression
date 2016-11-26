//-----------------------------------------------------------------------------
// Copyright (c) 2016 Michael G. Brehm
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//-----------------------------------------------------------------------------

#include <7zCrc.h>
#include <XzCrc64.h>
#include <Windows.h>

#pragma warning(push, 4)

// g_initonce (local)
//
// Global one-time initialization context
static INIT_ONCE g_initonce = INIT_ONCE_STATIC_INIT;

// InitOnceCrcGenerateTables (local)
//
// One-time initialization handler to initialize the LZMA/XZ CRC tables
static BOOL CALLBACK InitOnceCrcGenerateTables(PINIT_ONCE, PVOID, PVOID*)
{
	CrcGenerateTable();				// Initialize the 32-bit CRC table
	Crc64GenerateTable();			// Initialize the 64-bit CRC table

	return TRUE;
}

//-----------------------------------------------------------------------------
// crcinit
//
// Helper function to invoke the __fastcall CrcGenerateTable().  __fastcall 
// is not compatible with /clr compiled modules
//
// Arguments:
//
//	NONE

void crcinit(void)
{
	InitOnceExecuteOnce(&g_initonce, InitOnceCrcGenerateTables, nullptr, nullptr);
}

//-----------------------------------------------------------------------------

#pragma warning(pop)