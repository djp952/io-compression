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

#ifndef __LZAMFASTBYTES_H_
#define __LZAMFASTBYTES_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::IO;

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Class LzmaFastBytes
//
// Indicates the number of fast bytes to use with the LZMA encoder
//---------------------------------------------------------------------------

public value class LzmaFastBytes
{
public:

	// Instance Constructors
	//
	LzmaFastBytes(int bytes);

	//-----------------------------------------------------------------------
	// Overloaded Operators

	// operator== (static)
	//
	static bool operator==(LzmaFastBytes lhs, LzmaFastBytes rhs);

	// operator!= (static)
	//
	static bool operator!=(LzmaFastBytes lhs, LzmaFastBytes rhs);

	// operator LzmaFastBytes (static)
	//
	static operator LzmaFastBytes(int bytes);

	//-----------------------------------------------------------------------
	// Member Functions

	// Equals
	//
	// Overrides Object::Equals()
	virtual bool Equals(Object^ rhs) override;

	// Equals
	//
	// Compares this LzmaFastBytes to another LzmaFastBytes
	bool Equals(LzmaFastBytes rhs);

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

	static initonly LzmaFastBytes Default		= LzmaFastBytes(32);
	static initonly LzmaFastBytes Minimum		= LzmaFastBytes(5);
	static initonly LzmaFastBytes Maximum		= LzmaFastBytes(273);

internal:

	//-----------------------------------------------------------------------
	// Internal Operators

	// operator int
	//
	// Exposes the value as an integer
	static operator int(LzmaFastBytes rhs);

private:

	//-----------------------------------------------------------------------
	// Member Variables

	int						m_bytes;		// Underlying number of bytes
};

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)

#endif	// __LZAMFASTBYTES_H_
