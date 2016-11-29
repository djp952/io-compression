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
//	level		- Memory usage level as an integer

LzmaDictionarySize::LzmaDictionarySize(int level) : m_level(level)
{
#ifndef _M_X64
	if((level < (1 << 12)) || (level > (1 << 27))) throw gcnew ArgumentOutOfRangeException("level");
#else
	if((level < (1 << 12)) || (level > (1 << 30))) throw gcnew ArgumentOutOfRangeException("level");
#endif
}

//---------------------------------------------------------------------------
// LzmaDictionarySize::operator == (static)

bool LzmaDictionarySize::operator==(LzmaDictionarySize lhs, LzmaDictionarySize rhs)
{
	return lhs.m_level == rhs.m_level;
}

//---------------------------------------------------------------------------
// LzmaDictionarySize::operator != (static)

bool LzmaDictionarySize::operator!=(LzmaDictionarySize lhs, LzmaDictionarySize rhs)
{
	return lhs.m_level != rhs.m_level;
}

//---------------------------------------------------------------------------
// LzmaDictionarySize::operator int (static)

LzmaDictionarySize::operator int(LzmaDictionarySize rhs)
{
	return rhs.m_level;
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
	return m_level.GetHashCode();
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
	return m_level.ToString();
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
