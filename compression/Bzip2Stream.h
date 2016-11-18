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

#ifndef __BZIP2STREAM_H_
#define __BZIP2STREAM_H_
#pragma once

#include <bzlib.h>

#include "Bzip2SafeHandle.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::IO;

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Class Bzip2Stream
//
// BZIP2-based compression/decompression stream implementation
//---------------------------------------------------------------------------

public ref class Bzip2Stream : public Stream
{
public:

	// Instance Constructors
	//
	Bzip2Stream(Stream^ stream, Compression::CompressionLevel level);
	Bzip2Stream(Stream^ stream, Compression::CompressionLevel level, bool leaveopen);
	Bzip2Stream(Stream^ stream, Compression::CompressionMode mode);
	Bzip2Stream(Stream^ stream, Compression::CompressionMode mode, bool leaveopen);

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

	// BUFFER_SIZE
	//
	// Size of the local input/output buffer, in bytes
	static const int BUFFER_SIZE = 8192;

	// Instance Constructor
	//
	Bzip2Stream(Stream^ stream, Compression::CompressionMode mode, Compression::CompressionLevel level, bool leaveopen);

	// Destructor
	//
	~Bzip2Stream();

	//-----------------------------------------------------------------------
	// Member Variables

	bool							m_disposed;		// Object disposal flag
	Stream^							m_stream;		// Base Stream instance
	Compression::CompressionMode	m_mode;			// Compression mode
	bool							m_leaveopen;	// Flag to leave base stream open
	array<unsigned __int8>^			m_buffer;		// BZIP2 stream buffer
	size_t							m_bufferpos;	// Current position in the buffer
	bool							m_finished;		// Flag if operation is finished
	Bzip2SafeHandle^				m_bzstream;		// BZIP2 stream safe handle
};

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)

#endif	// __BZIP2STREAM_H_
