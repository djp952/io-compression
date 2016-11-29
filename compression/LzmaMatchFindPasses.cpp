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
#include "LzmaMatchFindPasses.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// LzmaMatchFindPasses Constructor
//
// Arguments:
//
//	passes		- Number of match find passes to use during encoding

LzmaMatchFindPasses::LzmaMatchFindPasses(int passes) : m_passes(passes)
{
	if((passes < 1) || (passes > (1 << 30))) throw gcnew ArgumentOutOfRangeException("passes");
}

//---------------------------------------------------------------------------
// LzmaMatchFindPasses::operator == (static)

bool LzmaMatchFindPasses::operator==(LzmaMatchFindPasses lhs, LzmaMatchFindPasses rhs)
{
	return lhs.m_passes == rhs.m_passes;
}

//---------------------------------------------------------------------------
// LzmaMatchFindPasses::operator != (static)

bool LzmaMatchFindPasses::operator!=(LzmaMatchFindPasses lhs, LzmaMatchFindPasses rhs)
{
	return lhs.m_passes != rhs.m_passes;
}

//---------------------------------------------------------------------------
// LzmaMatchFindPasses::operator int (static)

LzmaMatchFindPasses::operator int(LzmaMatchFindPasses rhs)
{
	return rhs.m_passes;
}

//---------------------------------------------------------------------------
// LzmaMatchFindPasses::Equals
//
// Compares this LzmaMatchFindPasses to another LzmaMatchFindPasses
//
// Arguments:
//
//	rhs		- Right-hand LzmaMatchFindPasses to compare against

bool LzmaMatchFindPasses::Equals(LzmaMatchFindPasses rhs)
{
	return (*this == rhs);
}

//---------------------------------------------------------------------------
// LzmaMatchFindPasses::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool LzmaMatchFindPasses::Equals(Object^ rhs)
{
	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a LzmaMatchFindPasses instance
	LzmaMatchFindPasses^ rhsref = dynamic_cast<LzmaMatchFindPasses^>(rhs);
	if(rhsref == nullptr) return false;

	return (*this == *rhsref);
}

//---------------------------------------------------------------------------
// LzmaMatchFindPasses::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int LzmaMatchFindPasses::GetHashCode(void)
{
	return m_passes.GetHashCode();
}

//---------------------------------------------------------------------------
// LzmaMatchFindPasses::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ LzmaMatchFindPasses::ToString(void)
{
	return m_passes.ToString();
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
