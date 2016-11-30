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
#include "Lzma2BlockSize.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Lzma2BlockSize Constructor
//
// Arguments:
//
//	size		- Block size as a signed integer

Lzma2BlockSize::Lzma2BlockSize(__int64 size) : m_size(size)
{
#ifndef _M_X64
	if((size < 0) || (size > System::UInt32::MaxValue)) throw gcnew ArgumentOutOfRangeException("size");
#else
	if(size < 0) throw gcnew ArgumentOutOfRangeException("size");
#endif
}

//---------------------------------------------------------------------------
// Lzma2BlockSize::operator == (static)

bool Lzma2BlockSize::operator==(Lzma2BlockSize lhs, Lzma2BlockSize rhs)
{
	return lhs.m_size == rhs.m_size;
}

//---------------------------------------------------------------------------
// Lzma2BlockSize::operator != (static)

bool Lzma2BlockSize::operator!=(Lzma2BlockSize lhs, Lzma2BlockSize rhs)
{
	return lhs.m_size != rhs.m_size;
}

//---------------------------------------------------------------------------
// Lzma2BlockSize::operator Lzma2BlockSize (static)

Lzma2BlockSize::operator Lzma2BlockSize(int size)
{
	return Lzma2BlockSize(size);
}

//---------------------------------------------------------------------------
// Lzma2BlockSize::operator Lzma2BlockSize (static)

Lzma2BlockSize::operator Lzma2BlockSize(__int64 size)
{
	return Lzma2BlockSize(size);
}

//---------------------------------------------------------------------------
// Lzma2BlockSize::operator __int64 (static)

Lzma2BlockSize::operator __int64(Lzma2BlockSize rhs)
{
	return rhs.m_size;
}

//---------------------------------------------------------------------------
// Lzma2BlockSize::Equals
//
// Compares this Lzma2BlockSize to another Lzma2BlockSize
//
// Arguments:
//
//	rhs		- Right-hand Lzma2BlockSize to compare against

bool Lzma2BlockSize::Equals(Lzma2BlockSize rhs)
{
	return (*this == rhs);
}

//---------------------------------------------------------------------------
// Lzma2BlockSize::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool Lzma2BlockSize::Equals(Object^ rhs)
{
	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a Lzma2BlockSize instance
	Lzma2BlockSize^ rhsref = dynamic_cast<Lzma2BlockSize^>(rhs);
	if(rhsref == nullptr) return false;

	return (*this == *rhsref);
}

//---------------------------------------------------------------------------
// Lzma2BlockSize::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int Lzma2BlockSize::GetHashCode(void)
{
	return m_size.GetHashCode();
}

//---------------------------------------------------------------------------
// Lzma2BlockSize::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ Lzma2BlockSize::ToString(void)
{
	return m_size.ToString();
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
