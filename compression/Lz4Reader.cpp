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

#include "stdafx.h"
#include "Lz4Reader.h"

#include "Lz4Exception.h"

// LZ4F_dctx_s is an incomplete type; causes LNK4248
//
struct LZ4F_dctx_s {};

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Lz4Reader Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is read from

Lz4Reader::Lz4Reader(Stream^ stream) : Lz4Reader(stream, false)
{
}

//---------------------------------------------------------------------------
// Lz4Reader Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is read from
//	leaveopen	- Flag to leave the base stream open after disposal

Lz4Reader::Lz4Reader(Stream^ stream, bool leaveopen) : m_disposed(false), m_stream(stream), m_leaveopen(leaveopen), m_inpos(0), 
	m_finished(false), m_inavail(0)
{
	if(Object::ReferenceEquals(stream, nullptr)) throw gcnew ArgumentNullException("stream");

	// Allocate and initialize the decompression context structure
	try { m_context = new LZ4F_decompressionContext_t; memset(m_context, 0, sizeof(LZ4F_decompressionContext_t)); }
	catch(Exception^) { throw gcnew OutOfMemoryException(); }

	// Allocate the managed input buffer for this instance
	m_in = gcnew array<unsigned __int8>(BUFFER_SIZE);

	// Create the LZ4 decompression context structure for this instance
	LZ4F_errorCode_t result = LZ4F_createDecompressionContext(m_context, LZ4F_VERSION);
	if(LZ4F_isError(result)) throw gcnew Lz4Exception(result);
}

//---------------------------------------------------------------------------
// Lz4Reader Destructor

Lz4Reader::~Lz4Reader()
{
	if(m_disposed) return;

	// Destroy the managed input buffer
	delete m_in;

	// Optionally dispose of the input stream instance
	if(!m_leaveopen) delete m_stream;
	
	this->!Lz4Reader();
	m_disposed = true;
}

//---------------------------------------------------------------------------
// Lz4Reader Finalizer

Lz4Reader::!Lz4Reader()
{
	if(m_context == nullptr) return;

	// Release the LZ4 compression context structure
	LZ4F_freeDecompressionContext(*m_context);

	delete m_context;
	m_context = nullptr;
}

//---------------------------------------------------------------------------
// Lz4Reader::BaseStream::get
//
// Accesses the underlying base stream instance

Stream^ Lz4Reader::BaseStream::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_stream;
}

//---------------------------------------------------------------------------
// Lz4Reader::CanRead::get
//
// Gets a value indicating whether the current stream supports reading

bool Lz4Reader::CanRead::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_stream->CanRead;
}

//---------------------------------------------------------------------------
// Lz4Reader::CanSeek::get
//
// Gets a value indicating whether the current stream supports seeking

bool Lz4Reader::CanSeek::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return false;
}

//---------------------------------------------------------------------------
// Lz4Reader::CanWrite::get
//
// Gets a value indicating whether the current stream supports writing

bool Lz4Reader::CanWrite::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return false;
}

//---------------------------------------------------------------------------
// Lz4Reader::Flush
//
// Clears all buffers for this stream and causes any buffered data to be written
//
// Arguments:
//
//	NONE

void Lz4Reader::Flush(void)
{
	CHECK_DISPOSED(m_disposed);
	m_stream->Flush();
}

//--------------------------------------------------------------------------
// Lz4Reader::Length::get
//
// Gets the length in bytes of the stream

__int64 Lz4Reader::Length::get(void)
{
	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Lz4Reader::Position::get
//
// Gets the current position within the stream

__int64 Lz4Reader::Position::get(void)
{
	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Lz4Reader::Position::set
//
// Sets the current position within the stream

void Lz4Reader::Position::set(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Lz4Reader::Read
//
// Reads a sequence of bytes from the current stream and advances the position within the stream
//
// Arguments:
//
//	buffer		- Destination data buffer
//	offset		- Offset within buffer to begin copying data
//	count		- Maximum number of bytes to write into the destination buffer

int Lz4Reader::Read(array<unsigned __int8>^ buffer, int offset, int count)
{
	CHECK_DISPOSED(m_disposed);

	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");
	if(offset < 0) throw gcnew ArgumentOutOfRangeException("offset");
	if(count < 0) throw gcnew ArgumentOutOfRangeException("count");
	if((offset + count) > buffer->Length) throw gcnew ArgumentException("The sum of offset and count is larger than the buffer length");

	msclr::lock lock(m_lock);

	// If there is no buffer to read into or the stream is already done, return zero
	if((count == 0) || (m_finished)) return 0;

	// Pin the input and output buffers in memory
	pin_ptr<unsigned __int8> pinin = &m_in[0];
	pin_ptr<unsigned __int8> pinout = &buffer[0];

	// Copy count into a local value to tally the final bytes read from the stream
	int availout = count;

	do {

		// If the input buffer was flushed from a previous iteration, refill it
		if(m_inavail == 0) {

			m_inpos = (m_inavail = m_stream->Read(m_in, 0, BUFFER_SIZE)) - m_inavail;
			if((m_inavail == 0) || (m_inavail > BUFFER_SIZE)) throw gcnew InvalidDataException();
		}

		// Use local input/output size values, they are modified by LZ4F_decompress
		size_t outsize = count - offset;
		size_t insize = m_inavail;

		// Decompress the next chunk of input data
		LZ4F_decompressOptions_t options ={ 0 /* stableSrc */, {0, 0, 0} /* reserved */};
		LZ4F_errorCode_t result = LZ4F_decompress(*m_context, &pinout[offset], &outsize, &pinin[m_inpos], &insize, &options);
		if(LZ4F_isError(result)) throw gcnew Lz4Exception(result);

		// Adjust the input buffer parameters
		m_inavail -= insize;
		m_inpos += insize;

		// Adjust the output buffer parameters
		availout -= static_cast<int>(outsize);
		offset += static_cast<int>(outsize);

		// Zero indicates that the end of the input stream has been reached
		if(result == 0) { m_finished = true; break; }

	} while(availout > 0);

	return (count - availout);
}

//---------------------------------------------------------------------------
// Lz4Reader::Seek
//
// Sets the position within the current stream
//
// Arguments:
//
//	offset		- Byte offset relative to origin
//	origin		- Reference point used to obtain the new position

__int64 Lz4Reader::Seek(__int64 offset, SeekOrigin origin)
{
	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(origin);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Lz4Reader::SetLength
//
// Sets the length of the current stream
//
// Arguments:
//
//	value		- Desired length of the current stream in bytes

void Lz4Reader::SetLength(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Lz4Reader::Write
//
// Writes a sequence of bytes to the current stream and advances the current position
//
// Arguments:
//
//	buffer		- Source data buffer 
//	offset		- Offset within buffer to begin copying from
//	count		- Maximum number of bytes to read from the source buffer

void Lz4Reader::Write(array<unsigned __int8>^ buffer, int offset, int count)
{
	UNREFERENCED_PARAMETER(buffer);
	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(count);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
