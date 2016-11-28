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
#include "GzipMemoryUsage.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// GzipMemoryUsage Constructor
//
// Arguments:
//
//	level		- Memory usage level as an integer

GzipMemoryUsage::GzipMemoryUsage(int level) : m_level(level)
{
	if((level < 1) || (level > MAX_MEM_LEVEL)) throw gcnew ArgumentOutOfRangeException("level");
}

//---------------------------------------------------------------------------
// GzipMemoryUsage::operator == (static)

bool GzipMemoryUsage::operator==(GzipMemoryUsage lhs, GzipMemoryUsage rhs)
{
	return lhs.m_level == rhs.m_level;
}

//---------------------------------------------------------------------------
// GzipMemoryUsage::operator != (static)

bool GzipMemoryUsage::operator!=(GzipMemoryUsage lhs, GzipMemoryUsage rhs)
{
	return lhs.m_level != rhs.m_level;
}

//---------------------------------------------------------------------------
// GzipMemoryUsage::operator int (static)

GzipMemoryUsage::operator int(GzipMemoryUsage rhs)
{
	return rhs.m_level;
}

//---------------------------------------------------------------------------
// GzipMemoryUsage::Equals
//
// Compares this GzipMemoryUsage to another GzipMemoryUsage
//
// Arguments:
//
//	rhs		- Right-hand GzipMemoryUsage to compare against

bool GzipMemoryUsage::Equals(GzipMemoryUsage rhs)
{
	return (*this == rhs);
}

//---------------------------------------------------------------------------
// GzipMemoryUsage::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool GzipMemoryUsage::Equals(Object^ rhs)
{
	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a GzipMemoryUsage instance
	GzipMemoryUsage^ rhsref = dynamic_cast<GzipMemoryUsage^>(rhs);
	if(rhsref == nullptr) return false;

	return (*this == *rhsref);
}

//---------------------------------------------------------------------------
// GzipMemoryUsage::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int GzipMemoryUsage::GetHashCode(void)
{
	return m_level.GetHashCode();
}

//---------------------------------------------------------------------------
// GzipMemoryUsage::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ GzipMemoryUsage::ToString(void)
{
	return m_level.ToString();
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
