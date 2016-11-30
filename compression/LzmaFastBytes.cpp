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
#include "LzmaFastBytes.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// LzmaFastBytes Constructor
//
// Arguments:
//
//	bytes		- Number of fast bytes to use during encoding

LzmaFastBytes::LzmaFastBytes(int bytes) : m_bytes(bytes)
{
	if((bytes < 5) || (bytes > 273)) throw gcnew ArgumentOutOfRangeException("bytes");
}

//---------------------------------------------------------------------------
// LzmaFastBytes::operator == (static)

bool LzmaFastBytes::operator==(LzmaFastBytes lhs, LzmaFastBytes rhs)
{
	return lhs.m_bytes == rhs.m_bytes;
}

//---------------------------------------------------------------------------
// LzmaFastBytes::operator != (static)

bool LzmaFastBytes::operator!=(LzmaFastBytes lhs, LzmaFastBytes rhs)
{
	return lhs.m_bytes != rhs.m_bytes;
}

//---------------------------------------------------------------------------
// LzmaFastBytes::operator LzmaFastBytes (static)

LzmaFastBytes::operator LzmaFastBytes(int bytes)
{
	return LzmaFastBytes(bytes);
}

//---------------------------------------------------------------------------
// LzmaFastBytes::operator int (static)

LzmaFastBytes::operator int(LzmaFastBytes rhs)
{
	return rhs.m_bytes;
}

//---------------------------------------------------------------------------
// LzmaFastBytes::Equals
//
// Compares this LzmaFastBytes to another LzmaFastBytes
//
// Arguments:
//
//	rhs		- Right-hand LzmaFastBytes to compare against

bool LzmaFastBytes::Equals(LzmaFastBytes rhs)
{
	return (*this == rhs);
}

//---------------------------------------------------------------------------
// LzmaFastBytes::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool LzmaFastBytes::Equals(Object^ rhs)
{
	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a LzmaFastBytes instance
	LzmaFastBytes^ rhsref = dynamic_cast<LzmaFastBytes^>(rhs);
	if(rhsref == nullptr) return false;

	return (*this == *rhsref);
}

//---------------------------------------------------------------------------
// LzmaFastBytes::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int LzmaFastBytes::GetHashCode(void)
{
	return m_bytes.GetHashCode();
}

//---------------------------------------------------------------------------
// LzmaFastBytes::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ LzmaFastBytes::ToString(void)
{
	return m_bytes.ToString();
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
