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
#include "Bzip2Writer.h"

#include "Bzip2Exception.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Bzip2Writer Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is written to

Bzip2Writer::Bzip2Writer(Stream^ stream) : Bzip2Writer(stream, Compression::CompressionLevel::Optimal, false)
{
}

//---------------------------------------------------------------------------
// Bzip2Writer Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is written to
//	level		- Indicates whether to emphasize speed or compression efficiency

Bzip2Writer::Bzip2Writer(Stream^ stream, Compression::CompressionLevel level) : Bzip2Writer(stream, level, false)
{
}

//---------------------------------------------------------------------------
// Bzip2Writer Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is written to
//	leaveopen	- Flag to leave the base stream open after disposal

Bzip2Writer::Bzip2Writer(Stream^ stream, bool leaveopen) : Bzip2Writer(stream, Compression::CompressionLevel::Optimal, leaveopen)
{
}

//---------------------------------------------------------------------------
// Bzip2Writer Constructor
//
// Arguments:
//
//	stream		- The stream the compressed or decompressed data is written to
//	level		- Indicates the level of compression to use
//	leaveopen	- Flag to leave the base stream open after disposal

Bzip2Writer::Bzip2Writer(Stream^ stream, Compression::CompressionLevel level, bool leaveopen) : m_disposed(false), m_stream(stream), m_leaveopen(leaveopen)
{
	if(Object::ReferenceEquals(stream, nullptr)) throw gcnew ArgumentNullException("stream");

	// bzip does not provide a 'no compression' option
	if(level == Compression::CompressionLevel::NoCompression) throw gcnew ArgumentOutOfRangeException("level");

	// Allocate and initialize the unmanaged bz_stream structure
	try { m_bzstream = new bz_stream; memset(m_bzstream, 0, sizeof(bz_stream)); }
	catch(Exception^) { throw gcnew OutOfMemoryException(); }

	// Allocate the managed input/output buffer for this instance
	m_buffer = gcnew array<unsigned __int8>(BUFFER_SIZE);

	// Initialize the bz_stream for compression
	int result = BZ2_bzCompressInit(m_bzstream, (level == Compression::CompressionLevel::Optimal) ? 9 : 1, 0, 0);
	if(result != BZ_OK) throw gcnew Bzip2Exception(result);
}

//---------------------------------------------------------------------------
// Bzip2Writer Destructor

Bzip2Writer::~Bzip2Writer()
{
	int result = BZ_OK;							// Result from bzip operation

	if(m_disposed) return;

	msclr::lock lock(m_lock);

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

	if(!m_leaveopen) delete m_stream;		// Optionally dispose of the base stream
	
	this->!Bzip2Writer();
	m_disposed = true;
}

//---------------------------------------------------------------------------
// Bzip2Writer Finalizer

Bzip2Writer::!Bzip2Writer()
{
	if(m_bzstream == nullptr) return;

	// Reset all of the input/output buffer pointers and size information
	m_bzstream->next_in = m_bzstream->next_out = nullptr;
	m_bzstream->avail_in = m_bzstream->avail_out = 0;

	BZ2_bzCompressEnd(m_bzstream);
	delete m_bzstream;

	m_bzstream = nullptr;
}

//---------------------------------------------------------------------------
// Bzip2Writer::BaseStream::get
//
// Accesses the underlying base stream instance

Stream^ Bzip2Writer::BaseStream::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_stream;
}

//---------------------------------------------------------------------------
// Bzip2Writer::CanRead::get
//
// Gets a value indicating whether the current stream supports reading

bool Bzip2Writer::CanRead::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return false;
}

//---------------------------------------------------------------------------
// Bzip2Writer::CanSeek::get
//
// Gets a value indicating whether the current stream supports seeking

bool Bzip2Writer::CanSeek::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return false;
}

//---------------------------------------------------------------------------
// Bzip2Writer::CanWrite::get
//
// Gets a value indicating whether the current stream supports writing

bool Bzip2Writer::CanWrite::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_stream->CanWrite;
}

//---------------------------------------------------------------------------
// Bzip2Writer::Flush
//
// Clears all buffers for this stream and causes any buffered data to be written
//
// Arguments:
//
//	NONE

void Bzip2Writer::Flush(void)
{
	int result = BZ_OK;			// Result from bzip operation

	CHECK_DISPOSED(m_disposed);

	msclr::lock lock(m_lock);

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
// Bzip2Writer::Length::get
//
// Gets the length in bytes of the stream

__int64 Bzip2Writer::Length::get(void)
{
	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Bzip2Writer::Position::get
//
// Gets the current position within the stream

__int64 Bzip2Writer::Position::get(void)
{
	CHECK_DISPOSED(m_disposed);

	msclr::lock lock(m_lock);
	return static_cast<__int64>(m_bzstream->total_in_hi32) << 32 | m_bzstream->total_in_lo32;
}

//---------------------------------------------------------------------------
// Bzip2Writer::Position::set
//
// Sets the current position within the stream

void Bzip2Writer::Position::set(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Bzip2Writer::Read
//
// Reads a sequence of bytes from the current stream and advances the position within the stream
//
// Arguments:
//
//	buffer		- Destination data buffer
//	offset		- Offset within buffer to begin copying data
//	count		- Maximum number of bytes to write into the destination buffer

int Bzip2Writer::Read(array<unsigned __int8>^ buffer, int offset, int count)
{
	UNREFERENCED_PARAMETER(buffer);
	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(count);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------
// Bzip2Writer::Seek
//
// Sets the position within the current stream
//
// Arguments:
//
//	offset		- Byte offset relative to origin
//	origin		- Reference point used to obtain the new position

__int64 Bzip2Writer::Seek(__int64 offset, SeekOrigin origin)
{
	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(origin);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Bzip2Writer::SetLength
//
// Sets the length of the current stream
//
// Arguments:
//
//	value		- Desired length of the current stream in bytes

void Bzip2Writer::SetLength(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Bzip2Writer::Write
//
// Writes a sequence of bytes to the current stream and advances the current position
//
// Arguments:
//
//	buffer		- Source data buffer 
//	offset		- Offset within buffer to begin copying from
//	count		- Maximum number of bytes to read from the source buffer

void Bzip2Writer::Write(array<unsigned __int8>^ buffer, int offset, int count)
{
	CHECK_DISPOSED(m_disposed);

	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");
	if(offset < 0) throw gcnew ArgumentOutOfRangeException("offset");
	if(count < 0) throw gcnew ArgumentOutOfRangeException("count");
	if((offset + count) > buffer->Length) throw gcnew ArgumentException("The sum of offset and count is larger than the buffer length");

	if(count == 0) return;

	msclr::lock lock(m_lock);
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
