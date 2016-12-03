//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------

#ifndef __XZCHECKSUM_H_
#define __XZCHECKSUM_H_
#pragma once

#include <XzEnc.h>

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Enum XzChecksum
//
// Indicates the checksum mode to use for XZ encoding
//---------------------------------------------------------------------------

public enum class XzChecksum
{
	Default			= XZ_CHECK_CRC64,
	None			= XZ_CHECK_NO,
	CRC32			= XZ_CHECK_CRC32,
	CRC64			= XZ_CHECK_CRC64,
	SHA256			= XZ_CHECK_SHA256,
};

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)

#endif	// __XZCHECKSUM_H_
