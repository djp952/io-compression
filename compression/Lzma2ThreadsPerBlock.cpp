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
#include "Lzma2ThreadsPerBlock.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Lzma2ThreadsPerBlock Constructor
//
// Arguments:
//
//	threads		- Number of threads to use per block

Lzma2ThreadsPerBlock::Lzma2ThreadsPerBlock(int threads) : m_threads(threads)
{
	if(threads < 0) throw gcnew ArgumentOutOfRangeException("threads");
}

//---------------------------------------------------------------------------
// Lzma2ThreadsPerBlock::operator == (static)

bool Lzma2ThreadsPerBlock::operator==(Lzma2ThreadsPerBlock lhs, Lzma2ThreadsPerBlock rhs)
{
	return lhs.m_threads == rhs.m_threads;
}

//---------------------------------------------------------------------------
// Lzma2ThreadsPerBlock::operator != (static)

bool Lzma2ThreadsPerBlock::operator!=(Lzma2ThreadsPerBlock lhs, Lzma2ThreadsPerBlock rhs)
{
	return lhs.m_threads != rhs.m_threads;
}

//---------------------------------------------------------------------------
// Lzma2ThreadsPerBlock::operator Lzma2ThreadsPerBlock (static)

Lzma2ThreadsPerBlock::operator Lzma2ThreadsPerBlock(int threads)
{
	return Lzma2ThreadsPerBlock(threads);
}

//---------------------------------------------------------------------------
// Lzma2ThreadsPerBlock::operator int (static)

Lzma2ThreadsPerBlock::operator int(Lzma2ThreadsPerBlock rhs)
{
	return rhs.m_threads;
}

//---------------------------------------------------------------------------
// Lzma2ThreadsPerBlock::Equals
//
// Compares this Lzma2ThreadsPerBlock to another Lzma2ThreadsPerBlock
//
// Arguments:
//
//	rhs		- Right-hand Lzma2ThreadsPerBlock to compare against

bool Lzma2ThreadsPerBlock::Equals(Lzma2ThreadsPerBlock rhs)
{
	return (*this == rhs);
}

//---------------------------------------------------------------------------
// Lzma2ThreadsPerBlock::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool Lzma2ThreadsPerBlock::Equals(Object^ rhs)
{
	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a Lzma2ThreadsPerBlock instance
	Lzma2ThreadsPerBlock^ rhsref = dynamic_cast<Lzma2ThreadsPerBlock^>(rhs);
	if(rhsref == nullptr) return false;

	return (*this == *rhsref);
}

//---------------------------------------------------------------------------
// Lzma2ThreadsPerBlock::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int Lzma2ThreadsPerBlock::GetHashCode(void)
{
	return m_threads.GetHashCode();
}

//---------------------------------------------------------------------------
// Lzma2ThreadsPerBlock::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ Lzma2ThreadsPerBlock::ToString(void)
{
	return m_threads.ToString();
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
