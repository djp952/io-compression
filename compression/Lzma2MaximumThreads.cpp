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
#include "Lzma2MaximumThreads.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Lzma2MaximumThreads Constructor
//
// Arguments:
//
//	threads		- Maximum number of threads to use

Lzma2MaximumThreads::Lzma2MaximumThreads(int threads) : m_threads(threads)
{
	if(threads < 0) throw gcnew ArgumentOutOfRangeException("threads");
}

//---------------------------------------------------------------------------
// Lzma2MaximumThreads::operator == (static)

bool Lzma2MaximumThreads::operator==(Lzma2MaximumThreads lhs, Lzma2MaximumThreads rhs)
{
	return lhs.m_threads == rhs.m_threads;
}

//---------------------------------------------------------------------------
// Lzma2MaximumThreads::operator != (static)

bool Lzma2MaximumThreads::operator!=(Lzma2MaximumThreads lhs, Lzma2MaximumThreads rhs)
{
	return lhs.m_threads != rhs.m_threads;
}

//---------------------------------------------------------------------------
// Lzma2MaximumThreads::operator Lzma2MaximumThreads (static)

Lzma2MaximumThreads::operator Lzma2MaximumThreads(int size)
{
	return Lzma2MaximumThreads(size);
}

//---------------------------------------------------------------------------
// Lzma2MaximumThreads::operator int (static)

Lzma2MaximumThreads::operator int(Lzma2MaximumThreads rhs)
{
	return rhs.m_threads;
}

//---------------------------------------------------------------------------
// Lzma2MaximumThreads::Equals
//
// Compares this Lzma2MaximumThreads to another Lzma2MaximumThreads
//
// Arguments:
//
//	rhs		- Right-hand Lzma2MaximumThreads to compare against

bool Lzma2MaximumThreads::Equals(Lzma2MaximumThreads rhs)
{
	return (*this == rhs);
}

//---------------------------------------------------------------------------
// Lzma2MaximumThreads::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool Lzma2MaximumThreads::Equals(Object^ rhs)
{
	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a Lzma2MaximumThreads instance
	Lzma2MaximumThreads^ rhsref = dynamic_cast<Lzma2MaximumThreads^>(rhs);
	if(rhsref == nullptr) return false;

	return (*this == *rhsref);
}

//---------------------------------------------------------------------------
// Lzma2MaximumThreads::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int Lzma2MaximumThreads::GetHashCode(void)
{
	return m_threads.GetHashCode();
}

//---------------------------------------------------------------------------
// Lzma2MaximumThreads::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ Lzma2MaximumThreads::ToString(void)
{
	return m_threads.ToString();
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
