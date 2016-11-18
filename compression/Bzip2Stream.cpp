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
#include "Bzip2Stream.h"

#include "Bzip2Exception.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Bzip2Stream Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is written to
//	level		- Indicates whether to emphasize speed or compression efficiency

Bzip2Stream::Bzip2Stream(Stream^ stream, Compression::CompressionLevel level) : Bzip2Stream(stream, Compression::CompressionMode::Compress, level, false)
{
}

//---------------------------------------------------------------------------
// Bzip2Stream Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is written to
//	level		- Indicates whether to emphasize speed or compression efficiency
//	leaveopen	- Flag to leave the base stream open after disposal

Bzip2Stream::Bzip2Stream(Stream^ stream, Compression::CompressionLevel level, bool leaveopen) : Bzip2Stream(stream, Compression::CompressionMode::Compress, level, leaveopen)
{
}

//---------------------------------------------------------------------------
// Bzip2Stream Constructor
//
// Arguments:
//
//	stream		- The stream the compressed or decompressed data is written to
//	mode		- Indicates whether to compress or decompress the stream

Bzip2Stream::Bzip2Stream(Stream^ stream, Compression::CompressionMode mode) : Bzip2Stream(stream, mode, Compression::CompressionLevel::Optimal, false)
{
}

//---------------------------------------------------------------------------
// Bzip2Stream Constructor
//
// Arguments:
//
//	stream		- The stream the compressed or decompressed data is written to
//	mode		- Indicates whether to compress or decompress the stream
//	leaveopen	- Flag to leave the base stream open after disposal

Bzip2Stream::Bzip2Stream(Stream^ stream, Compression::CompressionMode mode, bool leaveopen) : Bzip2Stream(stream, mode, Compression::CompressionLevel::Optimal, leaveopen)
{
}

//---------------------------------------------------------------------------
// Bzip2Stream Constructor (private)
//
// Arguments:
//
//	stream		- The stream the compressed or decompressed data is written to
//	mode		- Indicates whether to compress or decompress the stream
//	level		- If compressing, indicates the level of compression to use
//	leaveopen	- Flag to leave the base stream open after disposal

Bzip2Stream::Bzip2Stream(Stream^ stream, Compression::CompressionMode mode, Compression::CompressionLevel level, bool leaveopen) : 
	m_stream(stream), m_mode(mode), m_leaveopen(leaveopen), m_bufferpos(0), m_finished(false)
{
	if(Object::ReferenceEquals(stream, nullptr)) throw gcnew ArgumentNullException("stream");

	// bzip does not provide a 'no compression' option
	if(level == Compression::CompressionLevel::NoCompression) throw gcnew ArgumentOutOfRangeException("level");

	// Allocate the unmanaged bz_stream SafeHandle and the managed input/output buffer
	m_bzstream = gcnew Bzip2SafeHandle(mode, level);
	m_buffer = gcnew array<unsigned __int8>(BUFFER_SIZE);
}

//---------------------------------------------------------------------------
// Bzip2Stream Destructor

Bzip2Stream::~Bzip2Stream()
{
	if(m_disposed) return;

	msclr::lock lock(m_bzstream);

	// Compression streams have to be finished before the bz_stream is destroyed
	if(m_mode == Compression::CompressionMode::Compress) {

		int result = BZ_OK;			// Result from bzip operation

		// Pin the output buffer into memory
		pin_ptr<unsigned __int8> pinout = &m_buffer[0];

		// Input is not consumed when finishing the bzip stream
		m_bzstream->next_in = nullptr;
		m_bzstream->avail_in = 0;

		do {

			// Reset the output buffer to point into the managed array
			m_bzstream->next_out = reinterpret_cast<char*>(pinout);
			m_bzstream->avail_out = BUFFER_SIZE;

			// Finish the next block of data in the bzip buffers and write it
			result = BZ2_bzCompress(m_bzstream, BZ_FINISH);
			m_stream->Write(m_buffer, 0, BUFFER_SIZE - m_bzstream->avail_out);

		} while (result == BZ_FINISH_OK);

		m_stream->Flush();						// Ensure the base stream is flushed
	}
		
	if(!m_leaveopen) delete m_stream;		// Optionally dispose of the base stream
	delete m_bzstream;						// Dispose of the SafeHandle instance
	
	m_disposed = true;
}

//---------------------------------------------------------------------------
// Bzip2Stream::BaseStream::get
//
// Accesses the underlying base stream instance

Stream^ Bzip2Stream::BaseStream::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_stream;
}

//---------------------------------------------------------------------------
// Bzip2Stream::CanRead::get
//
// Gets a value indicating whether the current stream supports reading

bool Bzip2Stream::CanRead::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return ((m_mode == Compression::CompressionMode::Decompress) && (m_stream->CanRead));
}

//---------------------------------------------------------------------------
// Bzip2Stream::CanSeek::get
//
// Gets a value indicating whether the current stream supports seeking

bool Bzip2Stream::CanSeek::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return false;
}

//---------------------------------------------------------------------------
// Bzip2Stream::CanWrite::get
//
// Gets a value indicating whether the current stream supports writing

bool Bzip2Stream::CanWrite::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return ((m_mode == Compression::CompressionMode::Compress) && (m_stream->CanWrite));
}

//---------------------------------------------------------------------------
// Bzip2Stream::Flush
//
// Clears all buffers for this stream and causes any buffered data to be written
//
// Arguments:
//
//	NONE

void Bzip2Stream::Flush(void)
{
	int result = BZ_OK;			// Result from bzip operation

	CHECK_DISPOSED(m_disposed);

	// This is a no-operation for decompression streams
	if(m_mode == Compression::CompressionMode::Decompress) return;

	msclr::lock lock(m_bzstream);

	// Pin the output buffer into memory
	pin_ptr<unsigned __int8> pinout = &m_buffer[0];

	// Input is not consumed when flushing the bzip stream
	m_bzstream->next_in = nullptr;
	m_bzstream->avail_in = 0;

	do {

		// Reset the output buffer to point into the managed array
		m_bzstream->next_out = reinterpret_cast<char*>(pinout);
		m_bzstream->avail_out = BUFFER_SIZE;

		// Flush the next block of data in the bzip buffers and write it
		result = BZ2_bzCompress(m_bzstream, BZ_FLUSH);
		m_stream->Write(m_buffer, 0, BUFFER_SIZE - m_bzstream->avail_out);
	
	} while(result == BZ_FLUSH_OK);

	// The end state of a flush operation should be BZ_RUN_OK
	if(result != BZ_RUN_OK) throw gcnew Bzip2Exception(result);

	m_stream->Flush();				// Flush the underlying base stream
}

//--------------------------------------------------------------------------
// Bzip2Stream::Length::get
//
// Gets the length in bytes of the stream

__int64 Bzip2Stream::Length::get(void)
{
	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Bzip2Stream::Position::get
//
// Gets the current position within the stream

__int64 Bzip2Stream::Position::get(void)
{
	CHECK_DISPOSED(m_disposed);

	msclr::lock lock(m_bzstream);

	// Decompress --> total_out
	if(m_mode == Compression::CompressionMode::Decompress)
		return static_cast<__int64>(m_bzstream->total_out_hi32) << 32 | m_bzstream->total_out_lo32;

	// Compress --> total_in
	else return static_cast<__int64>(m_bzstream->total_in_hi32) << 32 | m_bzstream->total_in_lo32;
}

//---------------------------------------------------------------------------
// Bzip2Stream::Position::set
//
// Sets the current position within the stream

void Bzip2Stream::Position::set(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Bzip2Stream::Read
//
// Reads a sequence of bytes from the current stream and advances the position within the stream
//
// Arguments:
//
//	buffer		- Destination data buffer
//	offset		- Offset within buffer to begin copying data
//	count		- Maximum number of bytes to write into the destination buffer

int Bzip2Stream::Read(array<unsigned __int8>^ buffer, int offset, int count)
{
	CHECK_DISPOSED(m_disposed);

	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");
	if(offset < 0) throw gcnew ArgumentOutOfRangeException("offset");
	if(count < 0) throw gcnew ArgumentOutOfRangeException("offset");
	if((offset + count) > buffer->Length) throw gcnew ArgumentException("The sum of offset and count is larger than the buffer length");

	msclr::lock lock(m_bzstream);

	// A special flag has to be maintained since BZ_STREAM_END is only returned once
	if(m_finished) return 0;

	// Pin both the input and output byte arrays in memory
	pin_ptr<unsigned __int8> pinin = &m_buffer[0];
	pin_ptr<unsigned __int8> pinout = &buffer[0];

	// Set up the output buffer pointer and available length
	m_bzstream->next_out = reinterpret_cast<char*>(&pinout[offset]);
	m_bzstream->avail_out = count;

	// Repeatedly decompress blocks of data until the output buffer has been filled
	while(m_bzstream->avail_out > 0) {

		// Reset the input pointer based on the current position into the buffer
		m_bzstream->next_in = reinterpret_cast<char*>(&pinin[m_bufferpos]);

		// Attempt to decompress the next block of data
		int result = BZ2_bzDecompress(m_bzstream);
		
		// BZ_STREAM_END indicates that there is no more data to decompress, but bzip
		// will not return it more than once -- set a flag to prevent more attempts
		if(result == BZ_STREAM_END) { m_finished = true; break; }
		else if(result != BZ_OK) throw gcnew Bzip2Exception(result);

		// If the input buffer was exhausted, refill it from the base stream
		if(m_bzstream->avail_in == 0) {

			m_bzstream->avail_in = m_stream->Read(m_buffer, 0, BUFFER_SIZE);
			if((m_bzstream->avail_in == 0) || (m_bzstream->avail_in > BUFFER_SIZE)) throw gcnew InvalidDataException();

			m_bufferpos = 0;			// Reset saved position back to the beginning
		}

		// Input buffer is not exhausted, calculate the new position
		else m_bufferpos = (uintptr_t(m_bzstream->next_in) - uintptr_t(pinin));
	};

	return (count - m_bzstream->avail_out);
}

//---------------------------------------------------------------------------
// Bzip2Stream::Seek
//
// Sets the position within the current stream
//
// Arguments:
//
//	offset		- Byte offset relative to origin
//	origin		- Reference point used to obtain the new position

__int64 Bzip2Stream::Seek(__int64 offset, SeekOrigin origin)
{
	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(origin);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Bzip2Stream::SetLength
//
// Sets the length of the current stream
//
// Arguments:
//
//	value		- Desired length of the current stream in bytes

void Bzip2Stream::SetLength(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Bzip2Stream::Write
//
// Writes a sequence of bytes to the current stream and advances the current position
//
// Arguments:
//
//	buffer		- Source data buffer 
//	offset		- Offset within buffer to begin copying from
//	count		- Maximum number of bytes to read from the source buffer

void Bzip2Stream::Write(array<unsigned __int8>^ buffer, int offset, int count)
{
	CHECK_DISPOSED(m_disposed);

	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");
	if(offset < 0) throw gcnew ArgumentOutOfRangeException("offset");
	if(count < 0) throw gcnew ArgumentOutOfRangeException("offset");
	if((offset + count) > buffer->Length) throw gcnew ArgumentException("The sum of offset and count is larger than the buffer length");

	msclr::lock lock(m_bzstream);

	// Pin both the input and output byte arrays in memory
	pin_ptr<unsigned __int8> pinin = &buffer[0];
	pin_ptr<unsigned __int8> pinout = &m_buffer[0];

	// Set up the input buffer pointer and available length
	m_bzstream->next_in = reinterpret_cast<char*>(&pinin[offset]);
	m_bzstream->avail_in = count;

	// Repeatedly compress blocks of data until all input has been consumed
	while(m_bzstream->avail_in > 0) {

		// Reset the output buffer pointer and length
		m_bzstream->next_out = reinterpret_cast<char*>(&pinout[0]);
		m_bzstream->avail_out = BUFFER_SIZE;

		// Compress the next block of input data into the output buffer
		int result = BZ2_bzCompress(m_bzstream, BZ_RUN);
		if(result != BZ_RUN_OK) throw gcnew Bzip2Exception(result);

		// Write the compressed data into the underlying base stream
		m_stream->Write(m_buffer, 0, BUFFER_SIZE - m_bzstream->avail_out);
	};
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
