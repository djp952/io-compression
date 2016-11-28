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

#ifndef __BZIP2COMPRESSIONLEVEL_H_
#define __BZIP2COMPRESSIONLEVEL_H_
#pragma once

#include <bzlib.h>

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::IO;

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Class Bzip2CompressionLevel
//
// Indicates the compression level to use with the BZIP2 encoder
//---------------------------------------------------------------------------

public value class Bzip2CompressionLevel
{
public:

	// Instance Constructors
	//
	Bzip2CompressionLevel(int level);
	Bzip2CompressionLevel(Compression::CompressionLevel level);

	//-----------------------------------------------------------------------
	// Overloaded Operators

	// operator== (static)
	//
	static bool operator==(Bzip2CompressionLevel lhs, Bzip2CompressionLevel rhs);

	// operator!= (static)
	//
	static bool operator!=(Bzip2CompressionLevel lhs, Bzip2CompressionLevel rhs);

	//-----------------------------------------------------------------------
	// Member Functions

	// Equals
	//
	// Overrides Object::Equals()
	virtual bool Equals(Object^ rhs) override;

	// Equals
	//
	// Compares this Bzip2CompressionLevel to another Bzip2CompressionLevel
	bool Equals(Bzip2CompressionLevel rhs);

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

	static initonly Bzip2CompressionLevel Default	= Bzip2CompressionLevel(9);
	static initonly Bzip2CompressionLevel Fastest	= Bzip2CompressionLevel(1);
	static initonly Bzip2CompressionLevel Optimal	= Bzip2CompressionLevel(9);

internal:

	//-----------------------------------------------------------------------
	// Internal Operators

	// operator int
	//
	// Exposes the value as an integer
	static operator int(Bzip2CompressionLevel rhs);

private:

	//-----------------------------------------------------------------------
	// Member Variables

	int							m_level;		// Underlying compression level
};

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)

#endif	// __BZIP2COMPRESSIONLEVEL_H_
