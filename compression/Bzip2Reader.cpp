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
#include "Bzip2Reader.h"

#include "Bzip2Exception.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Bzip2Reader Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is read from

Bzip2Reader::Bzip2Reader(Stream^ stream) : Bzip2Reader(stream, false)
{
}

//---------------------------------------------------------------------------
// Bzip2Reader Constructor (private)
//
// Arguments:
//
//	stream		- The stream the compressed or decompressed data is written to
//	leaveopen	- Flag to leave the base stream open after disposal

Bzip2Reader::Bzip2Reader(Stream^ stream, bool leaveopen) : m_disposed(false), m_stream(stream), m_leaveopen(leaveopen), m_bufferpos(0), m_finished(false)
{
	if(Object::ReferenceEquals(stream, nullptr)) throw gcnew ArgumentNullException("stream");

	// Allocate and initialize the unmanaged bz_stream structure
	try { m_bzstream = new bz_stream; memset(m_bzstream, 0, sizeof(bz_stream)); }
	catch(Exception^) { throw gcnew OutOfMemoryException(); }

	// Allocate the managed input/output buffer for this instance
	m_buffer = gcnew array<unsigned __int8>(BUFFER_SIZE);

	// Initialize the bz_stream for decompression
	int result = BZ2_bzDecompressInit(m_bzstream, 0, 0);
	if(result != BZ_OK) throw gcnew Bzip2Exception(result);
}

//---------------------------------------------------------------------------
// Bzip2Reader Destructor

Bzip2Reader::~Bzip2Reader()
{
	if(m_disposed) return;
	
	// Optionally dispose of the base stream
	if(!m_leaveopen) delete m_stream;
	
	this->!Bzip2Reader();
	m_disposed = true;
}

//---------------------------------------------------------------------------
// Bzip2Reader Finalizer

Bzip2Reader::!Bzip2Reader()
{
	if(m_bzstream == nullptr) return;

	// Reset all of the input/output buffer pointers and size information
	m_bzstream->next_in = m_bzstream->next_out = nullptr;
	m_bzstream->avail_in = m_bzstream->avail_out = 0;

	BZ2_bzDecompressEnd(m_bzstream);
	delete m_bzstream;

	m_bzstream = nullptr;
}

//---------------------------------------------------------------------------
// Bzip2Reader::BaseStream::get
//
// Accesses the underlying base stream instance

Stream^ Bzip2Reader::BaseStream::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_stream;
}

//---------------------------------------------------------------------------
// Bzip2Reader::CanRead::get
//
// Gets a value indicating whether the current stream supports reading

bool Bzip2Reader::CanRead::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_stream->CanRead;
}

//---------------------------------------------------------------------------
// Bzip2Reader::CanSeek::get
//
// Gets a value indicating whether the current stream supports seeking

bool Bzip2Reader::CanSeek::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return false;
}

//---------------------------------------------------------------------------
// Bzip2Reader::CanWrite::get
//
// Gets a value indicating whether the current stream supports writing

bool Bzip2Reader::CanWrite::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return false;
}

//---------------------------------------------------------------------------
// Bzip2Reader::Flush
//
// Clears all buffers for this stream and causes any buffered data to be written
//
// Arguments:
//
//	NONE

void Bzip2Reader::Flush(void)
{
	CHECK_DISPOSED(m_disposed);
	m_stream->Flush();
}

//--------------------------------------------------------------------------
// Bzip2Reader::Length::get
//
// Gets the length in bytes of the stream

__int64 Bzip2Reader::Length::get(void)
{
	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Bzip2Reader::Position::get
//
// Gets the current position within the stream

__int64 Bzip2Reader::Position::get(void)
{
	CHECK_DISPOSED(m_disposed);

	msclr::lock lock(m_lock);
	return static_cast<__int64>(m_bzstream->total_out_hi32) << 32 | m_bzstream->total_out_lo32;
}

//---------------------------------------------------------------------------
// Bzip2Reader::Position::set
//
// Sets the current position within the stream

void Bzip2Reader::Position::set(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Bzip2Reader::Read
//
// Reads a sequence of bytes from the current stream and advances the position within the stream
//
// Arguments:
//
//	buffer		- Destination data buffer
//	offset		- Offset within buffer to begin copying data
//	count		- Maximum number of bytes to write into the destination buffer

int Bzip2Reader::Read(array<unsigned __int8>^ buffer, int offset, int count)
{
	CHECK_DISPOSED(m_disposed);

	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");
	if(offset < 0) throw gcnew ArgumentOutOfRangeException("offset");
	if(count < 0) throw gcnew ArgumentOutOfRangeException("count");
	if((offset + count) > buffer->Length) throw gcnew ArgumentException("The sum of offset and count is larger than the buffer length");

	msclr::lock lock(m_lock);

	// If there is no buffer to read into or the stream is already done, return zero
	if((count == 0) || (m_finished)) return 0;

	// Pin both the input and output byte arrays in memory
	pin_ptr<unsigned __int8> pinin = &m_buffer[0];
	pin_ptr<unsigned __int8> pinout = &buffer[0];

	// Set up the output buffer pointer and available length
	m_bzstream->next_out = reinterpret_cast<char*>(&pinout[offset]);
	m_bzstream->avail_out = count;

	do {

		// If the input buffer was flushed from a previous iteration, refill it
		if(m_bzstream->avail_in == 0) {

			m_bzstream->avail_in = m_stream->Read(m_buffer, 0, BUFFER_SIZE);
			if((m_bzstream->avail_in == 0) || (m_bzstream->avail_in > BUFFER_SIZE)) throw gcnew InvalidDataException();

			m_bufferpos = 0;			// Reset stored offset to zero
		}

		// Reset the input pointer based on the current position into the buffer, the address
		// of the buffer itself may have changed between calls to Read() due to pinning
		m_bzstream->next_in = reinterpret_cast<char*>(&pinin[m_bufferpos]);

		// Attempt to decompress the next block of data and adjust the buffer offset
		int result = BZ2_bzDecompress(m_bzstream);
		m_bufferpos = (uintptr_t(m_bzstream->next_in) - uintptr_t(pinin));

		// BZ_STREAM_END indicates that there is no more data to decompress, but bzip
		// will not return it more than once -- set a flag to prevent more attempts
		if(result == BZ_STREAM_END) { m_finished = true; break; }
		else if(result != BZ_OK) throw gcnew Bzip2Exception(result);

	} while(m_bzstream->avail_out > 0);

	return (count - m_bzstream->avail_out);
}

//---------------------------------------------------------------------------
// Bzip2Reader::Seek
//
// Sets the position within the current stream
//
// Arguments:
//
//	offset		- Byte offset relative to origin
//	origin		- Reference point used to obtain the new position

__int64 Bzip2Reader::Seek(__int64 offset, SeekOrigin origin)
{
	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(origin);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Bzip2Reader::SetLength
//
// Sets the length of the current stream
//
// Arguments:
//
//	value		- Desired length of the current stream in bytes

void Bzip2Reader::SetLength(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Bzip2Reader::Write
//
// Writes a sequence of bytes to the current stream and advances the current position
//
// Arguments:
//
//	buffer		- Source data buffer 
//	offset		- Offset within buffer to begin copying from
//	count		- Maximum number of bytes to read from the source buffer

void Bzip2Reader::Write(array<unsigned __int8>^ buffer, int offset, int count)
{
	UNREFERENCED_PARAMETER(buffer);
	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(count);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
