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

#ifndef __GZIPENCODER_H_
#define __GZIPENCODER_H_
#pragma once

#include <zlib.h>
#include "Encoder.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::IO;

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Class GzipEncoder
//
// GZIP compression encoder
//
// TODO: There are more aspects of the GZIP encoder that can be controlled
// via properties on this class, just the basics are in here right now
//---------------------------------------------------------------------------

public ref class GzipEncoder : public Encoder
{
public:

	// Instance Constructor
	//
	GzipEncoder();

	//-----------------------------------------------------------------------
	// Member Functions

	// Encode (Encoder)
	//
	// Compresses an input stream into an array of bytes
	virtual array<unsigned __int8>^ Encode(Stream^ instream);

	// Encode (Encoder)
	//
	// Compresses an input array of bytes
	virtual array<unsigned __int8>^ Encode(array<unsigned __int8>^ buffer);

	// Encode (Encoder)
	//
	// Compresses an input array of bytes
	virtual array<unsigned __int8>^ Encode(array<unsigned __int8>^ buffer, int offset, int count);

	// Encode (Encoder)
	//
	// Compresses an input stream into an output stream
	virtual void Encode(Stream^ instream, Stream^ outstream);

	// Encode (Encoder)
	//
	// Compresses an input array of bytes into an output stream
	virtual void Encode(array<unsigned __int8>^ buffer, Stream^ outstream);

	// Encode (Encoder)
	//
	// Compresses an input array of bytes into an output stream
	virtual void Encode(array<unsigned __int8>^ buffer, int offset, int count, Stream^ outstream);

	//-----------------------------------------------------------------------
	// Properties

	// BufferSize
	//
	// Gets/sets the size of the compression data buffer
	property int BufferSize
	{
		int get(void);
		void set(int value);
	} 

	// CompressionlLevel
	//
	// Gets/sets the compression level to use
	property int CompressionLevel
	{
		int get(void);
		void set(int value);
	} 

private:

	//-----------------------------------------------------------------------
	// Member Variables

	int					m_buffersize;			// Size of the compression buffer
	int					m_level;				// Compression level
};

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)

#endif	// __GZIPENCODER_H_
