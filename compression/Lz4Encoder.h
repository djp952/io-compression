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
// LZ4 Library
// Copyright (c) 2011-2016, Yann Collet
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice, this
//   list of conditions and the following disclaimer in the documentation and/or
//   other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//---------------------------------------------------------------------------

#ifndef __LZ4ENCODER_H_
#define __LZ4ENCODER_H_
#pragma once

#include "Encoder.h"
#include "Lz4BlockMode.h"
#include "Lz4BlockSize.h"
#include "Lz4CompressionLevel.h"
#include "Lz4ContentChecksum.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::IO;

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Class Lz4Encoder
//
// LZ4 compression encoder
//---------------------------------------------------------------------------

public ref class Lz4Encoder : public Encoder
{
public:

	// Instance Constructor
	//
	Lz4Encoder();

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

	// AutoFlush
	//
	// Gets/sets a flag indicating if the stream should be auto-flushed as it's generated
	property bool AutoFlush
	{
		bool get(void);
		void set(bool value);
	}

	// BlockMode
	//
	// Gets/sets the encoder block mode
	property Lz4BlockMode BlockMode
	{
		Lz4BlockMode get(void);
		void set(Lz4BlockMode value);
	}

	// BlockSize
	//
	// Gets/sets the encoder block size
	property Lz4BlockSize BlockSize
	{
		Lz4BlockSize get(void);
		void set(Lz4BlockSize value);
	}

	// CompressionlLevel
	//
	// Gets/sets the compression level to use
	property Lz4CompressionLevel CompressionLevel
	{
		Lz4CompressionLevel get(void);
		void set(Lz4CompressionLevel value);
	}

	// ContentChecksum
	//
	// Gets/sets the content checksum mode
	property Lz4ContentChecksum ContentChecksum
	{
		Lz4ContentChecksum get(void);
		void set(Lz4ContentChecksum value);
	}

private:

	//-----------------------------------------------------------------------
	// Member Variables

	bool						m_autoflush;		// Auto-flush flag
	Lz4BlockMode				m_blockmode;		// Encoder block mode
	Lz4BlockSize				m_blocksize;		// Encoder block size
	Lz4CompressionLevel			m_level;			// Compression level
	Lz4ContentChecksum			m_checksum;			// Content checksum mode
};

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)

#endif	// __LZ4ENCODER_H_
