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
#include "GzipCompressionLevel.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// GzipCompressionLevel Constructor
//
// Arguments:
//
//	level		- Compression level to use as an integer

GzipCompressionLevel::GzipCompressionLevel(int level) : m_level(level)
{
	if((level < Z_DEFAULT_COMPRESSION) || (level > Z_BEST_COMPRESSION)) throw gcnew ArgumentOutOfRangeException("level");
}

//---------------------------------------------------------------------------
// GzipCompressionLevel Constructor
//
// Arguments:
//
//	level		- System::IO::Compression::CompressionLevel to convert

GzipCompressionLevel::GzipCompressionLevel(Compression::CompressionLevel level)
{
	if(level == Compression::CompressionLevel::NoCompression) m_level = Z_NO_COMPRESSION;
	else if(level == Compression::CompressionLevel::Fastest) m_level = Z_BEST_SPEED;
	else if(level == Compression::CompressionLevel::Optimal) m_level = Z_BEST_COMPRESSION;
	else throw gcnew ArgumentOutOfRangeException("level");
}

//---------------------------------------------------------------------------
// GzipCompressionLevel::operator == (static)

bool GzipCompressionLevel::operator==(GzipCompressionLevel lhs, GzipCompressionLevel rhs)
{
	return lhs.m_level == rhs.m_level;
}

//---------------------------------------------------------------------------
// GzipCompressionLevel::operator != (static)

bool GzipCompressionLevel::operator!=(GzipCompressionLevel lhs, GzipCompressionLevel rhs)
{
	return lhs.m_level != rhs.m_level;
}

//---------------------------------------------------------------------------
// GzipCompressionLevel::operator int (static)

GzipCompressionLevel::operator int(GzipCompressionLevel rhs)
{
	return rhs.m_level;
}

//---------------------------------------------------------------------------
// GzipCompressionLevel::Equals
//
// Compares this GzipCompressionLevel to another GzipCompressionLevel
//
// Arguments:
//
//	rhs		- Right-hand GzipCompressionLevel to compare against

bool GzipCompressionLevel::Equals(GzipCompressionLevel rhs)
{
	return (*this == rhs);
}

//---------------------------------------------------------------------------
// GzipCompressionLevel::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool GzipCompressionLevel::Equals(Object^ rhs)
{
	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a GzipCompressionLevel instance
	GzipCompressionLevel^ rhsref = dynamic_cast<GzipCompressionLevel^>(rhs);
	if(rhsref == nullptr) return false;

	return (*this == *rhsref);
}

//---------------------------------------------------------------------------
// GzipCompressionLevel::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int GzipCompressionLevel::GetHashCode(void)
{
	return m_level.GetHashCode();
}

//---------------------------------------------------------------------------
// GzipCompressionLevel::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ GzipCompressionLevel::ToString(void)
{
	return m_level.ToString();
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
