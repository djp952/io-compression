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
#include "LzmaHashBytes.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// LzmaHashBytes Constructor
//
// Arguments:
//
//	bytes		- Number of Hash bytes to use during encoding

LzmaHashBytes::LzmaHashBytes(int bytes) : m_bytes(bytes)
{
	if((bytes < 2) || (bytes > 4)) throw gcnew ArgumentOutOfRangeException("bytes");
}

//---------------------------------------------------------------------------
// LzmaHashBytes::operator == (static)

bool LzmaHashBytes::operator==(LzmaHashBytes lhs, LzmaHashBytes rhs)
{
	return lhs.m_bytes == rhs.m_bytes;
}

//---------------------------------------------------------------------------
// LzmaHashBytes::operator != (static)

bool LzmaHashBytes::operator!=(LzmaHashBytes lhs, LzmaHashBytes rhs)
{
	return lhs.m_bytes != rhs.m_bytes;
}

//---------------------------------------------------------------------------
// LzmaHashBytes::operator LzmaHashBytes (static)

LzmaHashBytes::operator LzmaHashBytes(int bytes)
{
	return LzmaHashBytes(bytes);
}

//---------------------------------------------------------------------------
// LzmaHashBytes::operator int (static)

LzmaHashBytes::operator int(LzmaHashBytes rhs)
{
	return rhs.m_bytes;
}

//---------------------------------------------------------------------------
// LzmaHashBytes::Equals
//
// Compares this LzmaHashBytes to another LzmaHashBytes
//
// Arguments:
//
//	rhs		- Right-hand LzmaHashBytes to compare against

bool LzmaHashBytes::Equals(LzmaHashBytes rhs)
{
	return (*this == rhs);
}

//---------------------------------------------------------------------------
// LzmaHashBytes::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool LzmaHashBytes::Equals(Object^ rhs)
{
	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a LzmaHashBytes instance
	LzmaHashBytes^ rhsref = dynamic_cast<LzmaHashBytes^>(rhs);
	if(rhsref == nullptr) return false;

	return (*this == *rhsref);
}

//---------------------------------------------------------------------------
// LzmaHashBytes::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int LzmaHashBytes::GetHashCode(void)
{
	return m_bytes.GetHashCode();
}

//---------------------------------------------------------------------------
// LzmaHashBytes::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ LzmaHashBytes::ToString(void)
{
	return m_bytes.ToString();
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
