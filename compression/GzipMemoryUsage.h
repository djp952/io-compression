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

#ifndef __GZIPMEMORYUSAGE_H_
#define __GZIPMEMORYUSAGE_H_
#pragma once

#include <zlib.h>

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::IO;

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Class GzipMemoryUsage
//
// Indicates the compression level to use with the GZIP encoder
//---------------------------------------------------------------------------

public value class GzipMemoryUsage
{
public:

	// Instance Constructors
	//
	GzipMemoryUsage(int level);

	//-----------------------------------------------------------------------
	// Overloaded Operators

	// operator== (static)
	//
	static bool operator==(GzipMemoryUsage lhs, GzipMemoryUsage rhs);

	// operator!= (static)
	//
	static bool operator!=(GzipMemoryUsage lhs, GzipMemoryUsage rhs);

	//-----------------------------------------------------------------------
	// Member Functions

	// Equals
	//
	// Overrides Object::Equals()
	virtual bool Equals(Object^ rhs) override;

	// Equals
	//
	// Compares this GzipMemoryUsage to another GzipMemoryUsage
	bool Equals(GzipMemoryUsage rhs);

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

	static initonly GzipMemoryUsage Default		= GzipMemoryUsage(8);
	static initonly GzipMemoryUsage Minimum		= GzipMemoryUsage(1);
	static initonly GzipMemoryUsage Optimal		= GzipMemoryUsage(MAX_MEM_LEVEL);

internal:

	//-----------------------------------------------------------------------
	// Internal Operators

	// operator int
	//
	// Exposes the value as an integer
	static operator int(GzipMemoryUsage rhs);

private:

	//-----------------------------------------------------------------------
	// Member Variables

	int						m_level;		// Underlying memory usage level
};

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)

#endif	// __GZIPMEMORYUSAGE_H_
