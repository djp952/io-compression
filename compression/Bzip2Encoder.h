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

#ifndef __BZIP2ENCODER_H_
#define __BZIP2ENCODER_H_
#pragma once

#include <bzlib.h>
#include "Encoder.h"
#include "Bzip2CompressionLevel.h"
#include "Bzip2WorkFactor.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::IO;

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Class Bzip2Encoder
//
// BZIP2 compression encoder
//---------------------------------------------------------------------------

public ref class Bzip2Encoder : public Encoder
{
public:

	// Instance Constructor
	//
	Bzip2Encoder();

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
	property Bzip2CompressionLevel CompressionLevel
	{
		Bzip2CompressionLevel get(void);
		void set(Bzip2CompressionLevel value);
	} 

	// WorkFactor
	//
	// Gets/sets the bzip2 compression work factor
	property Bzip2WorkFactor WorkFactor
	{
		Bzip2WorkFactor get(void);
		void set(Bzip2WorkFactor value);
	} 

private:

	//-----------------------------------------------------------------------
	// Member Variables

	int							m_buffersize;			// Size of the compression buffer
	Bzip2CompressionLevel		m_level;				// Compression level
	Bzip2WorkFactor				m_workfactor;			// Work factor
};

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)

#endif	// __BZIP2ENCODER_H_
