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
#include "LzmaDictionarySize.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// LzmaDictionarySize Constructor
//
// Arguments:
//
//	size		- Dictionary size as an integer

LzmaDictionarySize::LzmaDictionarySize(int size) : m_size(size)
{
#ifndef _M_X64
	if((size < (1 << 12)) || (size > (1 << 27))) throw gcnew ArgumentOutOfRangeException("size");
#else
	if((size < (1 << 12)) || (size > (1 << 30))) throw gcnew ArgumentOutOfRangeException("size");
#endif
}

//---------------------------------------------------------------------------
// LzmaDictionarySize::operator == (static)

bool LzmaDictionarySize::operator==(LzmaDictionarySize lhs, LzmaDictionarySize rhs)
{
	return lhs.m_size == rhs.m_size;
}

//---------------------------------------------------------------------------
// LzmaDictionarySize::operator != (static)

bool LzmaDictionarySize::operator!=(LzmaDictionarySize lhs, LzmaDictionarySize rhs)
{
	return lhs.m_size != rhs.m_size;
}

//---------------------------------------------------------------------------
// LzmaDictionarySize::operator LzmaDictionarySize (static)

LzmaDictionarySize::operator LzmaDictionarySize(int size)
{
	return LzmaDictionarySize(size);
}

//---------------------------------------------------------------------------
// LzmaDictionarySize::operator int (static)

LzmaDictionarySize::operator int(LzmaDictionarySize rhs)
{
	return rhs.m_size;
}

//---------------------------------------------------------------------------
// LzmaDictionarySize::Equals
//
// Compares this LzmaDictionarySize to another LzmaDictionarySize
//
// Arguments:
//
//	rhs		- Right-hand LzmaDictionarySize to compare against

bool LzmaDictionarySize::Equals(LzmaDictionarySize rhs)
{
	return (*this == rhs);
}

//---------------------------------------------------------------------------
// LzmaDictionarySize::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool LzmaDictionarySize::Equals(Object^ rhs)
{
	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a LzmaDictionarySize instance
	LzmaDictionarySize^ rhsref = dynamic_cast<LzmaDictionarySize^>(rhs);
	if(rhsref == nullptr) return false;

	return (*this == *rhsref);
}

//---------------------------------------------------------------------------
// LzmaDictionarySize::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int LzmaDictionarySize::GetHashCode(void)
{
	return m_size.GetHashCode();
}

//---------------------------------------------------------------------------
// LzmaDictionarySize::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ LzmaDictionarySize::ToString(void)
{
	return m_size.ToString();
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
