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

//---------------------------------------------------------------------------
// LZ4 Library
// Copyright (c) 2011-2016, Yann Collet
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice, this
//   list of conditions and the following disclaimer in the documentation and/or
//   other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//---------------------------------------------------------------------------

#include "stdafx.h"
#include "Lz4CompressionLevel.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Lz4CompressionLevel Constructor
//
// Arguments:
//
//	level		- Compression level to use as an integer

Lz4CompressionLevel::Lz4CompressionLevel(int level) : m_level(level)
{
	if((level < 0) || (level > LZ4HC_MAX_CLEVEL)) throw gcnew ArgumentOutOfRangeException("level");
}

//---------------------------------------------------------------------------
// Lz4CompressionLevel Constructor
//
// Arguments:
//
//	level		- System::IO::Compression::CompressionLevel to convert

Lz4CompressionLevel::Lz4CompressionLevel(Compression::CompressionLevel level)
{
	if(level == Compression::CompressionLevel::Fastest) m_level = 1;
	else if(level == Compression::CompressionLevel::Optimal) m_level = 9;
	else throw gcnew ArgumentOutOfRangeException("level");
}

//---------------------------------------------------------------------------
// Lz4CompressionLevel::operator == (static)

bool Lz4CompressionLevel::operator==(Lz4CompressionLevel lhs, Lz4CompressionLevel rhs)
{
	return lhs.m_level == rhs.m_level;
}

//---------------------------------------------------------------------------
// Lz4CompressionLevel::operator != (static)

bool Lz4CompressionLevel::operator!=(Lz4CompressionLevel lhs, Lz4CompressionLevel rhs)
{
	return lhs.m_level != rhs.m_level;
}

//---------------------------------------------------------------------------
// Lz4CompressionLevel::operator Lz4CompressionLevel (static)

Lz4CompressionLevel::operator Lz4CompressionLevel(int level)
{
	return Lz4CompressionLevel(level);
}

//---------------------------------------------------------------------------
// Lz4CompressionLevel::operator int (static)

Lz4CompressionLevel::operator int(Lz4CompressionLevel rhs)
{
	return rhs.m_level;
}

//---------------------------------------------------------------------------
// Lz4CompressionLevel::Equals
//
// Compares this Lz4CompressionLevel to another Lz4CompressionLevel
//
// Arguments:
//
//	rhs		- Right-hand Lz4CompressionLevel to compare against

bool Lz4CompressionLevel::Equals(Lz4CompressionLevel rhs)
{
	return (*this == rhs);
}

//---------------------------------------------------------------------------
// Lz4CompressionLevel::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool Lz4CompressionLevel::Equals(Object^ rhs)
{
	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a Lz4CompressionLevel instance
	Lz4CompressionLevel^ rhsref = dynamic_cast<Lz4CompressionLevel^>(rhs);
	if(rhsref == nullptr) return false;

	return (*this == *rhsref);
}

//---------------------------------------------------------------------------
// Lz4CompressionLevel::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int Lz4CompressionLevel::GetHashCode(void)
{
	return m_level.GetHashCode();
}

//---------------------------------------------------------------------------
// Lz4CompressionLevel::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ Lz4CompressionLevel::ToString(void)
{
	return m_level.ToString();
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
