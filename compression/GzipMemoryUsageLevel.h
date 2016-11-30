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

#ifndef __GzipMemoryUsageLevelLEVEL_H_
#define __GzipMemoryUsageLevelLEVEL_H_
#pragma once

#include <zlib.h>

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::IO;

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Class GzipMemoryUsageLevel
//
// Indicates the compression level to use with the GZIP encoder
//---------------------------------------------------------------------------

public value class GzipMemoryUsageLevel
{
public:

	// Instance Constructors
	//
	GzipMemoryUsageLevel(int level);

	//-----------------------------------------------------------------------
	// Overloaded Operators

	// operator== (static)
	//
	static bool operator==(GzipMemoryUsageLevel lhs, GzipMemoryUsageLevel rhs);

	// operator!= (static)
	//
	static bool operator!=(GzipMemoryUsageLevel lhs, GzipMemoryUsageLevel rhs);

	// operator GzipMemoryUsageLevel (static)
	//
	static operator GzipMemoryUsageLevel(int level);

	//-----------------------------------------------------------------------
	// Member Functions

	// Equals
	//
	// Overrides Object::Equals()
	virtual bool Equals(Object^ rhs) override;

	// Equals
	//
	// Compares this GzipMemoryUsageLevel to another GzipMemoryUsageLevel
	bool Equals(GzipMemoryUsageLevel rhs);

	// GetHashCode
	//
	// Overrides Object::GetHashCode()
	virtual int GetHashCode(void) override;

	// ToString
	//
	// Overrides Object::ToString()
	virtual String^ ToString(void) override;

	//-----------------------------------------------------------------------
	// Fields

	static initonly GzipMemoryUsageLevel Default		= GzipMemoryUsageLevel(8);
	static initonly GzipMemoryUsageLevel Minimum		= GzipMemoryUsageLevel(1);
	static initonly GzipMemoryUsageLevel Optimal		= GzipMemoryUsageLevel(MAX_MEM_LEVEL);

internal:

	//-----------------------------------------------------------------------
	// Internal Operators

	// operator int
	//
	// Exposes the value as an integer
	static operator int(GzipMemoryUsageLevel rhs);

private:

	//-----------------------------------------------------------------------
	// Member Variables

	int						m_level;		// Underlying memory usage level
};

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)

#endif	// __GzipMemoryUsageLevelLEVEL_H_
