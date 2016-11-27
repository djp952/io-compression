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
// Copyright (c) 2011-2015, Yann Collet
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

#ifndef __LZ4WRITER_H_
#define __LZ4WRITER_H_
#pragma once

#include <lz4frame.h>

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::IO;

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Class Lz4Writer
//
// LZ4 stream compressor
//---------------------------------------------------------------------------

public ref class Lz4Writer : public Stream
{
public:

	// Instance Constructors
	//
	Lz4Writer(Stream^ stream);
	Lz4Writer(Stream^ stream, Compression::CompressionLevel level);
	Lz4Writer(Stream^ stream, bool leaveopen);
	Lz4Writer(Stream^ stream, Compression::CompressionLevel level, bool leaveopen);

	//-----------------------------------------------------------------------
	// Member Functions

	// Flush (Stream)
	//
	// Clears all buffers for this stream and causes any buffered data to be written
	virtual void Flush(void) override;

	// Read (Stream)
	//
	// Reads a sequence of bytes from the current stream and advances the position within the stream
	virtual int Read(array<unsigned __int8>^ buffer, int offset, int count) override;

	// Seek (Stream)
	//
	// Sets the position within the current stream
	virtual __int64 Seek(__int64 offset, SeekOrigin origin) override;

	// SetLength (Stream)
	//
	// Sets the length of the current stream
	virtual void SetLength(__int64 value) override;

	// Write (Stream)
	//
	// Writes a sequence of bytes to the current stream and advances the current position
	virtual void Write(array<unsigned __int8>^ buffer, int offset, int count) override;

	//-----------------------------------------------------------------------
	// Properties

	// BaseStream
	//
	// Exposes the underlying base stream instance
	property Stream^ BaseStream
	{
		Stream^ get(void);
	}

	// CanRead (Stream)
	//
	// Gets a value indicating whether the current stream supports reading
	property bool CanRead
	{
		virtual bool get(void) override;
	}

	// CanSeek (Stream)
	//
	// Gets a value indicating whether the current stream supports seeking
	property bool CanSeek
	{
		virtual bool get(void) override;
	}

	// CanWrite (Stream)
	//
	// Gets a value indicating whether the current stream supports writing
	property bool CanWrite
	{
		virtual bool get(void) override;
	}

	// Length (Stream)
	//
	// Gets the length in bytes of the stream
	property __int64 Length
	{
		virtual __int64 get(void) override;
	}

	// Position (Stream)
	//
	// Gets or sets the current position within the stream
	property __int64 Position
	{
		virtual __int64 get(void) override;
		void set(__int64 value) override;
	}

private:

	// Destructor / Finalizer
	//
	~Lz4Writer();
	!Lz4Writer();

	//-----------------------------------------------------------------------
	// Member Variables

	bool							m_disposed;			// Object disposal flag
	Stream^							m_stream;			// Base Stream instance
	bool							m_leaveopen;		// Flag to leave base stream open
	LZ4F_compressionContext_t*		m_context;			// LZ4 compression context
	LZ4F_preferences_t*				m_prefs;			// LZ4 compression preferences

	Object^	m_lock = gcnew Object();		// Synchronization object
};

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)

#endif	// __LZ4WRITER_H_
