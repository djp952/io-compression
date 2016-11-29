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
#include "LzmaLiteralPositionBits.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// LzmaLiteralPositionBits Constructor
//
// Arguments:
//
//	bits		- Number of literal position bits to use during encoding

LzmaLiteralPositionBits::LzmaLiteralPositionBits(int bits) : m_bits(bits)
{
	if((bits < 0) || (bits > 4)) throw gcnew ArgumentOutOfRangeException("bits");
}

//---------------------------------------------------------------------------
// LzmaLiteralPositionBits::operator == (static)

bool LzmaLiteralPositionBits::operator==(LzmaLiteralPositionBits lhs, LzmaLiteralPositionBits rhs)
{
	return lhs.m_bits == rhs.m_bits;
}

//---------------------------------------------------------------------------
// LzmaLiteralPositionBits::operator != (static)

bool LzmaLiteralPositionBits::operator!=(LzmaLiteralPositionBits lhs, LzmaLiteralPositionBits rhs)
{
	return lhs.m_bits != rhs.m_bits;
}

//---------------------------------------------------------------------------
// LzmaLiteralPositionBits::operator int (static)

LzmaLiteralPositionBits::operator int(LzmaLiteralPositionBits rhs)
{
	return rhs.m_bits;
}

//---------------------------------------------------------------------------
// LzmaLiteralPositionBits::Equals
//
// Compares this LzmaLiteralPositionBits to another LzmaLiteralPositionBits
//
// Arguments:
//
//	rhs		- Right-hand LzmaLiteralPositionBits to compare against

bool LzmaLiteralPositionBits::Equals(LzmaLiteralPositionBits rhs)
{
	return (*this == rhs);
}

//---------------------------------------------------------------------------
// LzmaLiteralPositionBits::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool LzmaLiteralPositionBits::Equals(Object^ rhs)
{
	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a LzmaLiteralPositionBits instance
	LzmaLiteralPositionBits^ rhsref = dynamic_cast<LzmaLiteralPositionBits^>(rhs);
	if(rhsref == nullptr) return false;

	return (*this == *rhsref);
}

//---------------------------------------------------------------------------
// LzmaLiteralPositionBits::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int LzmaLiteralPositionBits::GetHashCode(void)
{
	return m_bits.GetHashCode();
}

//---------------------------------------------------------------------------
// LzmaLiteralPositionBits::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ LzmaLiteralPositionBits::ToString(void)
{
	return m_bits.ToString();
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
