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

#ifndef __BZIP2READER_H_
#define __BZIP2READER_H_
#pragma once

#include <bzlib.h>

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::IO;

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Class Bzip2Reader
//
// BZIP2-based decompression stream implementation
//---------------------------------------------------------------------------

public ref class Bzip2Reader : public Stream
{
public:

	// Instance Constructors
	//
	Bzip2Reader(Stream^ stream);
	Bzip2Reader(Stream^ stream, bool leaveopen);

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
	static const int BUFFER_SIZE = 65536;

	// Destructor / Finalizer
	//
	~Bzip2Reader();
	!Bzip2Reader();

	//-----------------------------------------------------------------------
	// Member Variables

	bool							m_disposed;		// Object disposal flag
	Stream^							m_stream;		// Base Stream instance
	bool							m_leaveopen;	// Flag to leave base stream open
	array<unsigned __int8>^			m_in;			// BZIP2 stream buffer
	size_t							m_inpos;		// Current position in the buffer
	bool							m_finished;		// Flag if operation is finished
	bz_stream*						m_bzstream;		// BZIP2 stream state information

	Object^	m_lock = gcnew Object();		// Synchronization object
};

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)

#endif	// __BZIP2READER_H_
