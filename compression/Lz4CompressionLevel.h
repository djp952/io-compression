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

//---------------------------------------------------------------------------
// LZ4 Library
// Copyright (c) 2011-2016, Yann Collet
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice, this
//   list of conditions and the following disclaimer in the documentation and/or
//   other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//---------------------------------------------------------------------------

#ifndef __LZ4COMPRESSIONLEVEL_H_
#define __LZ4COMPRESSIONLEVEL_H_
#pragma once

#include <lz4frame.h>
#include <lz4hc.h>

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::IO;

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Class Lz4CompressionLevel
//
// Indicates the compression level to use with the LZ4 encoder
//---------------------------------------------------------------------------

public value class Lz4CompressionLevel
{
public:

	// Instance Constructors
	//
	Lz4CompressionLevel(int level);
	Lz4CompressionLevel(Compression::CompressionLevel level);

	//-----------------------------------------------------------------------
	// Overloaded Operators

	// operator== (static)
	//
	static bool operator==(Lz4CompressionLevel lhs, Lz4CompressionLevel rhs);

	// operator!= (static)
	//
	static bool operator!=(Lz4CompressionLevel lhs, Lz4CompressionLevel rhs);

	//-----------------------------------------------------------------------
	// Member Functions

	// Equals
	//
	// Overrides Object::Equals()
	virtual bool Equals(Object^ rhs) override;

	// Equals
	//
	// Compares this Lz4CompressionLevel to another Lz4CompressionLevel
	bool Equals(Lz4CompressionLevel rhs);

	// GetHashCode
	//
	// Overrides Object::GetHashCode()
	virtual int GetHashCode(void) override;

	// ToString
	//
	// Overrides Object::ToString()
	virtual String^ ToString(void) override;

	//-----------------------------------------------------------------------
	// Fields

	static initonly Lz4CompressionLevel Default	= Lz4CompressionLevel(0);
	static initonly Lz4CompressionLevel Fastest	= Lz4CompressionLevel(1);
	static initonly Lz4CompressionLevel Optimal	= Lz4CompressionLevel(LZ4HC_DEFAULT_CLEVEL);

internal:

	//-----------------------------------------------------------------------
	// Internal Operators

	// operator int
	//
	// Exposes the value as an integer
	static operator int(Lz4CompressionLevel rhs);

private:

	//-----------------------------------------------------------------------
	// Member Variables

	int							m_level;		// Underlying compression level
};

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)

#endif	// __LZ4COMPRESSIONLEVEL_H_
