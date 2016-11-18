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

#ifndef __BZIP2HANDLE_H_
#define __BZIP2HANDLE_H_
#pragma once

#include <bzlib.h>

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::IO;
using namespace System::Runtime::ConstrainedExecution;
using namespace System::Runtime::InteropServices;

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Class Bzip2SafeHandle (internal)
//
// Specialization of SafeHandle for an unmanaged bz_stream instance
//---------------------------------------------------------------------------

ref class Bzip2SafeHandle : public SafeHandle
{
public:

	// Instance Constructor
	//
	Bzip2SafeHandle(Compression::CompressionMode mode, Compression::CompressionLevel level);

	// Pointer-to-member operator
	//
	bz_stream* operator->();

	// bz_stream conversion operator
	//
	operator bz_stream*();
		
	//-----------------------------------------------------------------------
	// Member Functions

	// ReleaseHandle (SafeHandle)
	//
	// Releases the contained unmanaged handle/resource
	[ReliabilityContractAttribute(Consistency::MayCorruptProcess, Cer::Success)]
	virtual bool ReleaseHandle(void) override;

	//-----------------------------------------------------------------------
	// Properties

	// IsInvalid (SafeHandle)
	//
	// Gets a value indicating whether the handle/resource value is invalid
	property bool IsInvalid
	{
		virtual bool get(void) override;
	}

private:

	//-----------------------------------------------------------------------
	// Member Variables

	Compression::CompressionMode		m_mode;			// Stream mode
};

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)

#endif	// __BZIP2HANDLE_H_
