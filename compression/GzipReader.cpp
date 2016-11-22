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
#include "GzipReader.h"

#include "GzipException.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// GzipReader Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is read from

GzipReader::GzipReader(Stream^ stream) : GzipReader(stream, false)
{
}

//---------------------------------------------------------------------------
// GzipReader Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is read from
//	leaveopen	- Flag to leave the base stream open after disposal

GzipReader::GzipReader(Stream^ stream, bool leaveopen) : m_disposed(false), m_stream(stream), m_leaveopen(leaveopen), 
	m_bufferpos(0), m_finished(false)
{
	if(Object::ReferenceEquals(stream, nullptr)) throw gcnew ArgumentNullException("stream");

	// Allocate and initialize the unmanaged z_stream structure
	try { m_zstream = new z_stream; memset(m_zstream, 0, sizeof(z_stream)); }
	catch(Exception^) { throw gcnew OutOfMemoryException(); }

	// Allocate the managed input/output buffer for this instance
	m_buffer = gcnew array<unsigned __int8>(BUFFER_SIZE);

	// Initialize the z_stream for decompression
	int result = inflateInit2(m_zstream, 16 + MAX_WBITS);
	if(result != Z_OK) throw gcnew GzipException(result);
}

//---------------------------------------------------------------------------
// GzipReader Destructor

GzipReader::~GzipReader()
{
	if(m_disposed) return;
	
	// Optionally dispose of the base stream
	if(!m_leaveopen) delete m_stream;
	
	this->!GzipReader();
	m_disposed = true;
}

//---------------------------------------------------------------------------
// GzipReader Finalizer

GzipReader::!GzipReader()
{
	if(m_zstream == nullptr) return;

	// Reset all of the input/output buffer pointers and size information
	m_zstream->next_in = m_zstream->next_out = nullptr;
	m_zstream->avail_in = m_zstream->avail_out = 0;

	inflateEnd(m_zstream);
	delete m_zstream;

	m_zstream = nullptr;
}

//---------------------------------------------------------------------------
// GzipReader::BaseStream::get
//
// Accesses the underlying base stream instance

Stream^ GzipReader::BaseStream::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_stream;
}

//---------------------------------------------------------------------------
// GzipReader::CanRead::get
//
// Gets a value indicating whether the current stream supports reading

bool GzipReader::CanRead::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_stream->CanRead;
}

//---------------------------------------------------------------------------
// GzipReader::CanSeek::get
//
// Gets a value indicating whether the current stream supports seeking

bool GzipReader::CanSeek::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return false;
}

//---------------------------------------------------------------------------
// GzipReader::CanWrite::get
//
// Gets a value indicating whether the current stream supports writing

bool GzipReader::CanWrite::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return false;
}

//---------------------------------------------------------------------------
// GzipReader::Flush
//
// Clears all buffers for this stream and causes any buffered data to be written
//
// Arguments:
//
//	NONE

void GzipReader::Flush(void)
{
	CHECK_DISPOSED(m_disposed);
	m_stream->Flush();
}

//--------------------------------------------------------------------------
// GzipReader::Length::get
//
// Gets the length in bytes of the stream

__int64 GzipReader::Length::get(void)
{
	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// GzipReader::Position::get
//
// Gets the current position within the stream

__int64 GzipReader::Position::get(void)
{
	CHECK_DISPOSED(m_disposed);

	msclr::lock lock(m_lock);
	return static_cast<__int64>(m_zstream->total_out);
}

//---------------------------------------------------------------------------
// GzipReader::Position::set
//
// Sets the current position within the stream

void GzipReader::Position::set(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// GzipReader::Read
//
// Reads a sequence of bytes from the current stream and advances the position within the stream
//
// Arguments:
//
//	buffer		- Destination data buffer
//	offset		- Offset within buffer to begin copying data
//	count		- Maximum number of bytes to write into the destination buffer

int GzipReader::Read(array<unsigned __int8>^ buffer, int offset, int count)
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
	m_zstream->next_out = reinterpret_cast<Bytef*>(&pinout[offset]);
	m_zstream->avail_out = count;

	do {

		// If the input buffer was flushed from a previous iteration, refill it
		if(m_zstream->avail_in == 0) {

			m_zstream->avail_in = m_stream->Read(m_buffer, 0, BUFFER_SIZE);
			if((m_zstream->avail_in == 0) || (m_zstream->avail_in > BUFFER_SIZE)) throw gcnew InvalidDataException();

			m_bufferpos = 0;			// Reset stored offset to zero
		}

		// Reset the input pointer based on the current position into the buffer, the address
		// of the buffer itself may have changed between calls to Read() due to pinning
		m_zstream->next_in = reinterpret_cast<Bytef*>(&pinin[m_bufferpos]);

		// Attempt to decompress the next block of data and adjust the buffer offset
		int result = inflate(m_zstream, Z_NO_FLUSH);
		m_bufferpos = (uintptr_t(m_zstream->next_in) - uintptr_t(pinin));

		// Z_STREAM_END indicates that there is no more data to decompress, but zlib
		// will not return it more than once -- set a flag to prevent more attempts
		if(result == Z_STREAM_END) { m_finished = true; break; }
		else if(result != Z_OK) throw gcnew GzipException(result);

	} while(m_zstream->avail_out > 0);

	return (count - m_zstream->avail_out);
}

//---------------------------------------------------------------------------
// GzipReader::Seek
//
// Sets the position within the current stream
//
// Arguments:
//
//	offset		- Byte offset relative to origin
//	origin		- Reference point used to obtain the new position

__int64 GzipReader::Seek(__int64 offset, SeekOrigin origin)
{
	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(origin);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// GzipReader::SetLength
//
// Sets the length of the current stream
//
// Arguments:
//
//	value		- Desired length of the current stream in bytes

void GzipReader::SetLength(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// GzipReader::Write
//
// Writes a sequence of bytes to the current stream and advances the current position
//
// Arguments:
//
//	buffer		- Source data buffer 
//	offset		- Offset within buffer to begin copying from
//	count		- Maximum number of bytes to read from the source buffer

void GzipReader::Write(array<unsigned __int8>^ buffer, int offset, int count)
{
	UNREFERENCED_PARAMETER(buffer);
	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(count);

	CHECK_DISPOSED(m_disposed);
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
