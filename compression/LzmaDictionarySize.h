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

#ifndef __LZMADICTIONARYSIZE_H_
#define __LZMADICTIONARYSIZE_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::IO;

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Class LzmaDictionarySize
//
// Indicates the compression level to use with the GZIP encoder
//---------------------------------------------------------------------------

public value class LzmaDictionarySize
{
public:

	// Instance Constructors
	//
	LzmaDictionarySize(int level);

	//-----------------------------------------------------------------------
	// Overloaded Operators

	// operator== (static)
	//
	static bool operator==(LzmaDictionarySize lhs, LzmaDictionarySize rhs);

	// operator!= (static)
	//
	static bool operator!=(LzmaDictionarySize lhs, LzmaDictionarySize rhs);

	//-----------------------------------------------------------------------
	// Member Functions

	// Equals
	//
	// Overrides Object::Equals()
	virtual bool Equals(Object^ rhs) override;

	// Equals
	//
	// Compares this LzmaDictionarySize to another LzmaDictionarySize
	bool Equals(LzmaDictionarySize rhs);

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

	static initonly LzmaDictionarySize Default		= LzmaDictionarySize(1 << 24);
	static initonly LzmaDictionarySize Minimum		= LzmaDictionarySize(1 << 12);
#ifndef _M_X64
	static initonly LzmaDictionarySize Maximum		= LzmaDictionarySize(1 << 27);
#else
	static initonly LzmaDictionarySize Maximum		= LzmaDictionarySize(1 << 30);
#endif

internal:

	//-----------------------------------------------------------------------
	// Internal Operators

	// operator int
	//
	// Exposes the value as an integer
	static operator int(LzmaDictionarySize rhs);

private:

	//-----------------------------------------------------------------------
	// Member Variables

	int						m_level;		// Underlying memory usage level
};

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)

#endif	// __LZMADICTIONARYSIZE_H_
