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
#include "LzmaCompressionLevel.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// LzmaCompressionLevel Constructor
//
// Arguments:
//
//	level		- Compression level to use as an integer

LzmaCompressionLevel::LzmaCompressionLevel(int level) : m_level(level)
{
	if((level < 1) || (level > 9)) throw gcnew ArgumentOutOfRangeException("level");
}

//---------------------------------------------------------------------------
// LzmaCompressionLevel Constructor
//
// Arguments:
//
//	level		- System::IO::Compression::CompressionLevel to convert

LzmaCompressionLevel::LzmaCompressionLevel(Compression::CompressionLevel level)
{
	// LZMA does not provide a 'no compression' option
	if(level == Compression::CompressionLevel::Fastest) m_level = 1;
	else if(level == Compression::CompressionLevel::Optimal) m_level = 9;
	else throw gcnew ArgumentOutOfRangeException("level");
}

//---------------------------------------------------------------------------
// LzmaCompressionLevel::operator == (static)

bool LzmaCompressionLevel::operator==(LzmaCompressionLevel lhs, LzmaCompressionLevel rhs)
{
	return lhs.m_level == rhs.m_level;
}

//---------------------------------------------------------------------------
// LzmaCompressionLevel::operator != (static)

bool LzmaCompressionLevel::operator!=(LzmaCompressionLevel lhs, LzmaCompressionLevel rhs)
{
	return lhs.m_level != rhs.m_level;
}

//---------------------------------------------------------------------------
// LzmaCompressionLevel::operator int (static)

LzmaCompressionLevel::operator int(LzmaCompressionLevel rhs)
{
	return rhs.m_level;
}

//---------------------------------------------------------------------------
// LzmaCompressionLevel::Equals
//
// Compares this LzmaCompressionLevel to another LzmaCompressionLevel
//
// Arguments:
//
//	rhs		- Right-hand LzmaCompressionLevel to compare against

bool LzmaCompressionLevel::Equals(LzmaCompressionLevel rhs)
{
	return (*this == rhs);
}

//---------------------------------------------------------------------------
// LzmaCompressionLevel::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool LzmaCompressionLevel::Equals(Object^ rhs)
{
	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a LzmaCompressionLevel instance
	LzmaCompressionLevel^ rhsref = dynamic_cast<LzmaCompressionLevel^>(rhs);
	if(rhsref == nullptr) return false;

	return (*this == *rhsref);
}

//---------------------------------------------------------------------------
// LzmaCompressionLevel::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int LzmaCompressionLevel::GetHashCode(void)
{
	return m_level.GetHashCode();
}

//---------------------------------------------------------------------------
// LzmaCompressionLevel::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ LzmaCompressionLevel::ToString(void)
{
	return m_level.ToString();
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
