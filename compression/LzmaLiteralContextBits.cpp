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

#include "stdafx.h"
#include "LzmaLiteralContextBits.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// LzmaLiteralContextBits Constructor
//
// Arguments:
//
//	bits		- Number of literal context bits to use during encoding

LzmaLiteralContextBits::LzmaLiteralContextBits(int bits) : m_bits(bits)
{
	if((bits < 0) || (bits > 8)) throw gcnew ArgumentOutOfRangeException("bits");
}

//---------------------------------------------------------------------------
// LzmaLiteralContextBits::operator == (static)

bool LzmaLiteralContextBits::operator==(LzmaLiteralContextBits lhs, LzmaLiteralContextBits rhs)
{
	return lhs.m_bits == rhs.m_bits;
}

//---------------------------------------------------------------------------
// LzmaLiteralContextBits::operator != (static)

bool LzmaLiteralContextBits::operator!=(LzmaLiteralContextBits lhs, LzmaLiteralContextBits rhs)
{
	return lhs.m_bits != rhs.m_bits;
}

//---------------------------------------------------------------------------
// LzmaLiteralContextBits::operator LzmaLiteralContextBits (static)

LzmaLiteralContextBits::operator LzmaLiteralContextBits(int bits)
{
	return LzmaLiteralContextBits(bits);
}

//---------------------------------------------------------------------------
// LzmaLiteralContextBits::operator int (static)

LzmaLiteralContextBits::operator int(LzmaLiteralContextBits rhs)
{
	return rhs.m_bits;
}

//---------------------------------------------------------------------------
// LzmaLiteralContextBits::Equals
//
// Compares this LzmaLiteralContextBits to another LzmaLiteralContextBits
//
// Arguments:
//
//	rhs		- Right-hand LzmaLiteralContextBits to compare against

bool LzmaLiteralContextBits::Equals(LzmaLiteralContextBits rhs)
{
	return (*this == rhs);
}

//---------------------------------------------------------------------------
// LzmaLiteralContextBits::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool LzmaLiteralContextBits::Equals(Object^ rhs)
{
	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a LzmaLiteralContextBits instance
	LzmaLiteralContextBits^ rhsref = dynamic_cast<LzmaLiteralContextBits^>(rhs);
	if(rhsref == nullptr) return false;

	return (*this == *rhsref);
}

//---------------------------------------------------------------------------
// LzmaLiteralContextBits::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int LzmaLiteralContextBits::GetHashCode(void)
{
	return m_bits.GetHashCode();
}

//---------------------------------------------------------------------------
// LzmaLiteralContextBits::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ LzmaLiteralContextBits::ToString(void)
{
	return m_bits.ToString();
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
