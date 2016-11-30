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

#ifndef __LZMAPOSITIONBITS_H_
#define __LZMAPOSITIONBITS_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::IO;

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Class LzmaPositionBits
//
// Indicates the number of position bits to use with the LZMA encoder
//---------------------------------------------------------------------------

public value class LzmaPositionBits
{
public:

	// Instance Constructors
	//
	LzmaPositionBits(int bits);

	//-----------------------------------------------------------------------
	// Overloaded Operators

	// operator== (static)
	//
	static bool operator==(LzmaPositionBits lhs, LzmaPositionBits rhs);

	// operator!= (static)
	//
	static bool operator!=(LzmaPositionBits lhs, LzmaPositionBits rhs);

	// operator LzmaPositionBits (static)
	//
	static operator LzmaPositionBits(int bits);

	//-----------------------------------------------------------------------
	// Member Functions

	// Equals
	//
	// Overrides Object::Equals()
	virtual bool Equals(Object^ rhs) override;

	// Equals
	//
	// Compares this LzmaPositionBits to another LzmaPositionBits
	bool Equals(LzmaPositionBits rhs);

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

	static initonly LzmaPositionBits Default		= LzmaPositionBits(2);
	static initonly LzmaPositionBits Minimum		= LzmaPositionBits(0);
	static initonly LzmaPositionBits Maximum		= LzmaPositionBits(4);

internal:

	//-----------------------------------------------------------------------
	// Internal Operators

	// operator int
	//
	// Exposes the value as an integer
	static operator int(LzmaPositionBits rhs);

private:

	//-----------------------------------------------------------------------
	// Member Variables

	int						m_bits;		// Underlying number of bits
};

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)

#endif	// __LZMAPOSITIONBITS_H_
