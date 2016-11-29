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
#include "LzmaPositionBits.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// LzmaPositionBits Constructor
//
// Arguments:
//
//	bits		- Number of position bits to use during encoding

LzmaPositionBits::LzmaPositionBits(int bits) : m_bits(bits)
{
	if((bits < 0) || (bits > 4)) throw gcnew ArgumentOutOfRangeException("bits");
}

//---------------------------------------------------------------------------
// LzmaPositionBits::operator == (static)

bool LzmaPositionBits::operator==(LzmaPositionBits lhs, LzmaPositionBits rhs)
{
	return lhs.m_bits == rhs.m_bits;
}

//---------------------------------------------------------------------------
// LzmaPositionBits::operator != (static)

bool LzmaPositionBits::operator!=(LzmaPositionBits lhs, LzmaPositionBits rhs)
{
	return lhs.m_bits != rhs.m_bits;
}

//---------------------------------------------------------------------------
// LzmaPositionBits::operator int (static)

LzmaPositionBits::operator int(LzmaPositionBits rhs)
{
	return rhs.m_bits;
}

//---------------------------------------------------------------------------
// LzmaPositionBits::Equals
//
// Compares this LzmaPositionBits to another LzmaPositionBits
//
// Arguments:
//
//	rhs		- Right-hand LzmaPositionBits to compare against

bool LzmaPositionBits::Equals(LzmaPositionBits rhs)
{
	return (*this == rhs);
}

//---------------------------------------------------------------------------
// LzmaPositionBits::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool LzmaPositionBits::Equals(Object^ rhs)
{
	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a LzmaPositionBits instance
	LzmaPositionBits^ rhsref = dynamic_cast<LzmaPositionBits^>(rhs);
	if(rhsref == nullptr) return false;

	return (*this == *rhsref);
}

//---------------------------------------------------------------------------
// LzmaPositionBits::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int LzmaPositionBits::GetHashCode(void)
{
	return m_bits.GetHashCode();
}

//---------------------------------------------------------------------------
// LzmaPositionBits::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ LzmaPositionBits::ToString(void)
{
	return m_bits.ToString();
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
