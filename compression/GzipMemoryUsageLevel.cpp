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
#include "GzipMemoryUsageLevel.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// GzipMemoryUsageLevel Constructor
//
// Arguments:
//
//	level		- Memory usage level as an integer

GzipMemoryUsageLevel::GzipMemoryUsageLevel(int level) : m_level(level)
{
	if((level < 1) || (level > MAX_MEM_LEVEL)) throw gcnew ArgumentOutOfRangeException("level");
}

//---------------------------------------------------------------------------
// GzipMemoryUsageLevel::operator == (static)

bool GzipMemoryUsageLevel::operator==(GzipMemoryUsageLevel lhs, GzipMemoryUsageLevel rhs)
{
	return lhs.m_level == rhs.m_level;
}

//---------------------------------------------------------------------------
// GzipMemoryUsageLevel::operator != (static)

bool GzipMemoryUsageLevel::operator!=(GzipMemoryUsageLevel lhs, GzipMemoryUsageLevel rhs)
{
	return lhs.m_level != rhs.m_level;
}

//---------------------------------------------------------------------------
// GzipMemoryUsageLevel::operator GzipMemoryUsageLevel (static)

GzipMemoryUsageLevel::operator GzipMemoryUsageLevel(int level)
{
	return GzipMemoryUsageLevel(level);
}

//---------------------------------------------------------------------------
// GzipMemoryUsageLevel::operator int (static)

GzipMemoryUsageLevel::operator int(GzipMemoryUsageLevel rhs)
{
	return rhs.m_level;
}

//---------------------------------------------------------------------------
// GzipMemoryUsageLevel::Equals
//
// Compares this GzipMemoryUsageLevel to another GzipMemoryUsageLevel
//
// Arguments:
//
//	rhs		- Right-hand GzipMemoryUsageLevel to compare against

bool GzipMemoryUsageLevel::Equals(GzipMemoryUsageLevel rhs)
{
	return (*this == rhs);
}

//---------------------------------------------------------------------------
// GzipMemoryUsageLevel::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool GzipMemoryUsageLevel::Equals(Object^ rhs)
{
	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a GzipMemoryUsageLevel instance
	GzipMemoryUsageLevel^ rhsref = dynamic_cast<GzipMemoryUsageLevel^>(rhs);
	if(rhsref == nullptr) return false;

	return (*this == *rhsref);
}

//---------------------------------------------------------------------------
// GzipMemoryUsageLevel::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int GzipMemoryUsageLevel::GetHashCode(void)
{
	return m_level.GetHashCode();
}

//---------------------------------------------------------------------------
// GzipMemoryUsageLevel::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ GzipMemoryUsageLevel::ToString(void)
{
	return m_level.ToString();
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
