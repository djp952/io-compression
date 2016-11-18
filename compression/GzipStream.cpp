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
#include "GzipStream.h"

#include "GzipException.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// GzipStream Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is written to
//	level		- Indicates whether to emphasize speed or compression efficiency

GzipStream::GzipStream(Stream^ stream, Compression::CompressionLevel level) : GzipStream(stream, Compression::CompressionMode::Compress, level, false)
{
}

//---------------------------------------------------------------------------
// GzipStream Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is written to
//	level		- Indicates whether to emphasize speed or compression efficiency
//	leaveopen	- Flag to leave the base stream open after disposal

GzipStream::GzipStream(Stream^ stream, Compression::CompressionLevel level, bool leaveopen) : GzipStream(stream, Compression::CompressionMode::Compress, level, leaveopen)
{
}

//---------------------------------------------------------------------------
// GzipStream Constructor
//
// Arguments:
//
//	stream		- The stream the compressed or decompressed data is written to
//	mode		- Indicates whether to compress or decompress the stream

GzipStream::GzipStream(Stream^ stream, Compression::CompressionMode mode) : GzipStream(stream, mode, Compression::CompressionLevel::Optimal, false)
{
}

//---------------------------------------------------------------------------
// GzipStream Constructor
//
// Arguments:
//
//	stream		- The stream the compressed or decompressed data is written to
//	mode		- Indicates whether to compress or decompress the stream
//	leaveopen	- Flag to leave the base stream open after disposal

GzipStream::GzipStream(Stream^ stream, Compression::CompressionMode mode, bool leaveopen) : GzipStream(stream, mode, Compression::CompressionLevel::Optimal, leaveopen)
{
}

//---------------------------------------------------------------------------
// GzipStream Constructor (private)
//
// Arguments:
//
//	stream		- The stream the compressed or decompressed data is written to
//	mode		- Indicates whether to compress or decompress the stream
//	level		- If compressing, indicates the level of compression to use
//	leaveopen	- Flag to leave the base stream open after disposal

GzipStream::GzipStream(Stream^ stream, Compression::CompressionMode mode, Compression::CompressionLevel level, bool leaveopen) : 
	m_stream(stream), m_mode(mode), m_leaveopen(leaveopen), m_bufferpos(0), m_finished(false)
{
	if(Object::ReferenceEquals(stream, nullptr)) throw gcnew ArgumentNullException("stream");

	// Allocate the unmanaged z_stream SafeHandle and the managed input/output buffer
	m_zstream = gcnew GzipSafeHandle(mode, level);
	m_buffer = gcnew array<unsigned __int8>(BUFFER_SIZE);
}

//---------------------------------------------------------------------------
// GzipStream Destructor

GzipStream::~GzipStream()
{
	if(m_disposed) return;

	msclr::lock lock(m_zstream);

	// Compression streams have to be finished before the z_stream is destroyed
	if(m_mode == Compression::CompressionMode::Compress) {

		int result = Z_OK;			// Result from zlib operation

		// Pin the output buffer into memory
		pin_ptr<unsigned __int8> pinout = &m_buffer[0];

		// Input is not consumed when finishing the zlib stream
		m_zstream->next_in = nullptr;
		m_zstream->avail_in = 0;

		do {

			// Reset the output buffer to point into the managed array
			m_zstream->next_out = reinterpret_cast<Bytef*>(pinout);
			m_zstream->avail_out = BUFFER_SIZE;

			// Finish the next block of data in the zlib buffers and write it
			result = deflate(m_zstream, Z_FINISH);
			m_stream->Write(m_buffer, 0, BUFFER_SIZE - m_zstream->avail_out);

		} while (result == Z_OK);

		// The end result of FINISH should be Z_STREAM_END
		if(result != Z_STREAM_END) throw gcnew GzipException(result);

		m_stream->Flush();						// Ensure the base stream is flushed
	}
		
	if(!m_leaveopen) delete m_stream;		// Optionally dispose of the base stream
	delete m_zstream;						// Dispose of the SafeHandle instance
	
	m_disposed = true;
}

//---------------------------------------------------------------------------
// GzipStream::BaseStream::get
//
// Accesses the underlying base stream instance

Stream^ GzipStream::BaseStream::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_stream;
}

//---------------------------------------------------------------------------
// GzipStream::CanRead::get
//
// Gets a value indicating whether the current stream supports reading

bool GzipStream::CanRead::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return ((m_mode == Compression::CompressionMode::Decompress) && (m_stream->CanRead));
}

//---------------------------------------------------------------------------
// GzipStream::CanSeek::get
//
// Gets a value indicating whether the current stream supports seeking

bool GzipStream::CanSeek::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return false;
}

//---------------------------------------------------------------------------
// GzipStream::CanWrite::get
//
// Gets a value indicating whether the current stream supports writing

bool GzipStream::CanWrite::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return ((m_mode == Compression::CompressionMode::Compress) && (m_stream->CanWrite));
}

//---------------------------------------------------------------------------
// GzipStream::Flush
//
// Clears all buffers for this stream and causes any buffered data to be written
//
// Arguments:
//
//	NONE

void GzipStream::Flush(void)
{
	int result = Z_OK;			// Result from zlib operation

	CHECK_DISPOSED(m_disposed);

	// This is a no-operation for decompression streams
	if(m_mode == Compression::CompressionMode::Decompress) return;

	msclr::lock lock(m_zstream);

	// Pin the output buffer into memory
	pin_ptr<unsigned __int8> pinout = &m_buffer[0];

	// Input is not consumed when flushing the zlib stream
	m_zstream->next_in = nullptr;
	m_zstream->avail_in = 0;

	do {

		// Reset the output buffer to point into the managed array
		m_zstream->next_out = reinterpret_cast<Bytef*>(pinout);
		m_zstream->avail_out = BUFFER_SIZE;

		// Flush the next block of data in the zlib buffers and write it
		result = deflate(m_zstream, Z_SYNC_FLUSH);
		m_stream->Write(m_buffer, 0, BUFFER_SIZE - m_zstream->avail_out);
	
	} while(result == Z_OK);

	// The end state of a zlib flush operation will be Z_BUF_ERROR
	if(result != Z_BUF_ERROR) throw gcnew GzipException(result);

	m_stream->Flush();				// Flush the underlying base stream
}

//--------------------------------------------------------------------------
// GzipStream::Length::get
//
// Gets the length in bytes of the stream

__int64 GzipStream::Length::get(void)
{
	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// GzipStream::Position::get
//
// Gets the current position within the stream

__int64 GzipStream::Position::get(void)
{
	CHECK_DISPOSED(m_disposed);

	msclr::lock lock(m_zstream);

	// Decompress --> total_out / Compress --> total_in
	if(m_mode == Compression::CompressionMode::Decompress) return static_cast<__int64>(m_zstream->total_out);
	else return static_cast<__int64>(m_zstream->total_in);
}

//---------------------------------------------------------------------------
// GzipStream::Position::set
//
// Sets the current position within the stream

void GzipStream::Position::set(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// GzipStream::Read
//
// Reads a sequence of bytes from the current stream and advances the position within the stream
//
// Arguments:
//
//	buffer		- Destination data buffer
//	offset		- Offset within buffer to begin copying data
//	count		- Maximum number of bytes to write into the destination buffer

int GzipStream::Read(array<unsigned __int8>^ buffer, int offset, int count)
{
	CHECK_DISPOSED(m_disposed);

	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");
	if(offset < 0) throw gcnew ArgumentOutOfRangeException("offset");
	if(count < 0) throw gcnew ArgumentOutOfRangeException("offset");
	if((offset + count) > buffer->Length) throw gcnew ArgumentException("The sum of offset and count is larger than the buffer length");

	msclr::lock lock(m_zstream);

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
// GzipStream::Seek
//
// Sets the position within the current stream
//
// Arguments:
//
//	offset		- Byte offset relative to origin
//	origin		- Reference point used to obtain the new position

__int64 GzipStream::Seek(__int64 offset, SeekOrigin origin)
{
	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(origin);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// GzipStream::SetLength
//
// Sets the length of the current stream
//
// Arguments:
//
//	value		- Desired length of the current stream in bytes

void GzipStream::SetLength(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// GzipStream::Write
//
// Writes a sequence of bytes to the current stream and advances the current position
//
// Arguments:
//
//	buffer		- Source data buffer 
//	offset		- Offset within buffer to begin copying from
//	count		- Maximum number of bytes to read from the source buffer

void GzipStream::Write(array<unsigned __int8>^ buffer, int offset, int count)
{
	CHECK_DISPOSED(m_disposed);

	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");
	if(offset < 0) throw gcnew ArgumentOutOfRangeException("offset");
	if(count < 0) throw gcnew ArgumentOutOfRangeException("offset");
	if((offset + count) > buffer->Length) throw gcnew ArgumentException("The sum of offset and count is larger than the buffer length");

	msclr::lock lock(m_zstream);

	// Pin both the input and output byte arrays in memory
	pin_ptr<unsigned __int8> pinin = &buffer[0];
	pin_ptr<unsigned __int8> pinout = &m_buffer[0];

	// Set up the input buffer pointer and available length
	m_zstream->next_in = reinterpret_cast<Bytef*>(&pinin[offset]);
	m_zstream->avail_in = count;

	// Repeatedly compress blocks of data until all input has been consumed
	while(m_zstream->avail_in > 0) {

		// Reset the output buffer pointer and length
		m_zstream->next_out = reinterpret_cast<Bytef*>(&pinout[0]);
		m_zstream->avail_out = BUFFER_SIZE;

		// Compress the next block of input data into the output buffer
		int result = deflate(m_zstream, Z_NO_FLUSH);
		if(result != Z_OK) throw gcnew GzipException(result);

		// Write the compressed data into the underlying base stream
		m_stream->Write(m_buffer, 0, BUFFER_SIZE - m_zstream->avail_out);
	};
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
