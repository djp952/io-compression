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
// This program, "bzip2", the associated library "libbzip2", and all
// documentation, are copyright (C) 1996-2010 Julian R Seward.  All
// rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 
// 2. The origin of this software must not be misrepresented; you must 
//    not claim that you wrote the original software.  If you use this 
//    software in a product, an acknowledgment in the product 
//    documentation would be appreciated but is not required.
// 
// 3. Altered source versions must be plainly marked as such, and must
//    not be misrepresented as being the original software.
// 
// 4. The name of the author may not be used to endorse or promote 
//    products derived from this software without specific prior written 
//    permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// Julian Seward, jseward@bzip.org
// bzip2/libbzip2 version 1.0.6 of 6 September 2010
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
