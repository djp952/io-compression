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

#ifndef __LZMA2BLOCKSIZE_H_
#define __LZMA2BLOCKSIZE_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::IO;

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Class Lzma2BlockSize
//
// Indicates the block size to use with the XZ (LZMA2) encoder
//---------------------------------------------------------------------------

public value class Lzma2BlockSize
{
public:

	// Instance Constructors
	//
	Lzma2BlockSize(__int64 size);

	//-----------------------------------------------------------------------
	// Overloaded Operators

	// operator== (static)
	//
	static bool operator==(Lzma2BlockSize lhs, Lzma2BlockSize rhs);

	// operator!= (static)
	//
	static bool operator!=(Lzma2BlockSize lhs, Lzma2BlockSize rhs);

	// operator Lzma2BlockSize (static)
	//
	static operator Lzma2BlockSize(int size);

	// operator Lzma2BlockSize (static)
	//
	static operator Lzma2BlockSize(__int64 size);

	//-----------------------------------------------------------------------
	// Member Functions

	// Equals
	//
	// Overrides Object::Equals()
	virtual bool Equals(Object^ rhs) override;

	// Equals
	//
	// Compares this Lzma2BlockSize to another Lzma2BlockSize
	bool Equals(Lzma2BlockSize rhs);

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

	static initonly Lzma2BlockSize Disabled		= Lzma2BlockSize(0);

internal:

	//-----------------------------------------------------------------------
	// Internal Operators

	// operator __int64
	//
	// Exposes the value as an integer
	static operator __int64(Lzma2BlockSize rhs);

private:

	//-----------------------------------------------------------------------
	// Member Variables

	__int64						m_size;		// Underlying block size
};

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)

#endif	// __LZMA2BLOCKSIZE_H_
