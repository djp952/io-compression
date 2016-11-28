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
#include "Bzip2CompressionLevel.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Bzip2CompressionLevel Constructor
//
// Arguments:
//
//	level		- Compression level to use as an integer

Bzip2CompressionLevel::Bzip2CompressionLevel(int level) : m_level(level)
{
	if((level < 1) || (level > 9)) throw gcnew ArgumentOutOfRangeException("level");
}

//---------------------------------------------------------------------------
// Bzip2CompressionLevel Constructor
//
// Arguments:
//
//	level		- System::IO::Compression::CompressionLevel to convert

Bzip2CompressionLevel::Bzip2CompressionLevel(Compression::CompressionLevel level)
{
	// BZIP2 does not provide a 'no compression' option
	if(level == Compression::CompressionLevel::Fastest) m_level = 1;
	else if(level == Compression::CompressionLevel::Optimal) m_level = 9;
	else throw gcnew ArgumentOutOfRangeException("level");
}

//---------------------------------------------------------------------------
// Bzip2CompressionLevel::operator == (static)

bool Bzip2CompressionLevel::operator==(Bzip2CompressionLevel lhs, Bzip2CompressionLevel rhs)
{
	return lhs.m_level == rhs.m_level;
}

//---------------------------------------------------------------------------
// Bzip2CompressionLevel::operator != (static)

bool Bzip2CompressionLevel::operator!=(Bzip2CompressionLevel lhs, Bzip2CompressionLevel rhs)
{
	return lhs.m_level != rhs.m_level;
}

//---------------------------------------------------------------------------
// Bzip2CompressionLevel::operator int (static)

Bzip2CompressionLevel::operator int(Bzip2CompressionLevel rhs)
{
	return rhs.m_level;
}

//---------------------------------------------------------------------------
// Bzip2CompressionLevel::Equals
//
// Compares this Bzip2CompressionLevel to another Bzip2CompressionLevel
//
// Arguments:
//
//	rhs		- Right-hand Bzip2CompressionLevel to compare against

bool Bzip2CompressionLevel::Equals(Bzip2CompressionLevel rhs)
{
	return (*this == rhs);
}

//---------------------------------------------------------------------------
// Bzip2CompressionLevel::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool Bzip2CompressionLevel::Equals(Object^ rhs)
{
	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a Bzip2CompressionLevel instance
	Bzip2CompressionLevel^ rhsref = dynamic_cast<Bzip2CompressionLevel^>(rhs);
	if(rhsref == nullptr) return false;

	return (*this == *rhsref);
}

//---------------------------------------------------------------------------
// Bzip2CompressionLevel::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int Bzip2CompressionLevel::GetHashCode(void)
{
	return m_level.GetHashCode();
}

//---------------------------------------------------------------------------
// Bzip2CompressionLevel::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ Bzip2CompressionLevel::ToString(void)
{
	return m_level.ToString();
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
