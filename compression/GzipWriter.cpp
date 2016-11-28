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
#include "GzipWriter.h"

#include "GzipException.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// GzipWriter Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is written to

GzipWriter::GzipWriter(Stream^ stream) : 
	GzipWriter(stream, GzipCompressionLevel::Default, GzipCompressionStrategy::Default, GzipMemoryUsage::Default, DEFAULT_BUFFER_SIZE, false)
{
}

//---------------------------------------------------------------------------
// GzipWriter Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is written to
//	level		- Indicates whether to emphasize speed or compression efficiency

GzipWriter::GzipWriter(Stream^ stream, Compression::CompressionLevel level) : 
	GzipWriter(stream, GzipCompressionLevel(level), GzipCompressionStrategy::Default, GzipMemoryUsage::Default, DEFAULT_BUFFER_SIZE, false)
{
}

//---------------------------------------------------------------------------
// GzipWriter Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is written to
//	leaveopen	- Flag to leave the base stream open after disposal

GzipWriter::GzipWriter(Stream^ stream, bool leaveopen) : 
	GzipWriter(stream, GzipCompressionLevel::Default, GzipCompressionStrategy::Default, GzipMemoryUsage::Default, DEFAULT_BUFFER_SIZE, leaveopen)
{
}

//---------------------------------------------------------------------------
// GzipWriter Constructor
//
// Arguments:
//
//	stream		- The stream the compressed or decompressed data is written to
//	level		- Indicates the level of compression to use
//	leaveopen	- Flag to leave the base stream open after disposal

GzipWriter::GzipWriter(Stream^ stream, Compression::CompressionLevel level, bool leaveopen) : 
	GzipWriter(stream, GzipCompressionLevel(level), GzipCompressionStrategy::Default, GzipMemoryUsage::Default, DEFAULT_BUFFER_SIZE, leaveopen)
{
}

//---------------------------------------------------------------------------
// GzipWriter Constructor (internal)
//
// Arguments:
//
//	stream			- The stream the compressed or decompressed data is written to
//	level			- Indicates the level of compression to use
//	strategy		- Indicates the compression strategy to use
//	maxmem			- Indicates the maximum memory to use during encoding
//	buffersize		- Indicates the size of the compression buffer
//	leaveopen		- Flag to leave the base stream open after disposal

GzipWriter::GzipWriter(Stream^ stream, GzipCompressionLevel level, GzipCompressionStrategy strategy, GzipMemoryUsage maxmem, int buffersize, bool leaveopen) : 
	m_disposed(false), m_stream(stream), m_leaveopen(leaveopen), m_buffersize(buffersize)
{
	if(Object::ReferenceEquals(stream, nullptr)) throw gcnew ArgumentNullException("stream");
	if(buffersize <= 0) throw gcnew ArgumentOutOfRangeException("buffersize");

	// Allocate and initialize the unmanaged z_stream structure
	try { m_zstream = new z_stream; memset(m_zstream, 0, sizeof(z_stream)); }
	catch(Exception^) { throw gcnew OutOfMemoryException(); }

	// Initialize the z_stream for compression
	int result = deflateInit2(m_zstream, level, Z_DEFLATED, 16 + MAX_WBITS, maxmem, static_cast<int>(strategy));
	if(result != Z_OK) throw gcnew GzipException(result);
}

//---------------------------------------------------------------------------
// GzipWriter Destructor

GzipWriter::~GzipWriter()
{
	int result = Z_OK;					// Result from zlib operation

	if(m_disposed) return;

	msclr::lock lock(m_lock);

	// Create and pin a local compression buffer
	array<unsigned __int8>^ out = gcnew array<unsigned __int8>(m_buffersize);
	pin_ptr<unsigned __int8> pinout = &out[0];

	// Input is not consumed when finishing the zlib stream
	m_zstream->next_in = nullptr;
	m_zstream->avail_in = 0;

	do {

		// Reset the output buffer to point into the managed array
		m_zstream->next_out = reinterpret_cast<Bytef*>(pinout);
		m_zstream->avail_out = m_buffersize;

		// Finish the next block of data in the zlib buffers and write it
		result = deflate(m_zstream, Z_FINISH);
		m_stream->Write(out, 0, m_buffersize - m_zstream->avail_out);

	} while (result == Z_OK);

	// The end result of FINISH should be Z_STREAM_END
	if(result != Z_STREAM_END) throw gcnew GzipException(result);
	
	delete out;								// Dispose of the compression buffer
	if(!m_leaveopen) delete m_stream;		// Optionally dispose of the base stream
	
	this->!GzipWriter();
	m_disposed = true;
}

//---------------------------------------------------------------------------
// GzipWriter Finalizer

GzipWriter::!GzipWriter()
{
	if(m_zstream == nullptr) return;

	// Reset all of the input/output buffer pointers and size information
	m_zstream->next_in = m_zstream->next_out = nullptr;
	m_zstream->avail_in = m_zstream->avail_out = 0;

	deflateEnd(m_zstream);
	delete m_zstream;

	m_zstream = nullptr;
}

//---------------------------------------------------------------------------
// GzipWriter::BaseStream::get
//
// Accesses the underlying base stream instance

Stream^ GzipWriter::BaseStream::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_stream;
}

//---------------------------------------------------------------------------
// GzipWriter::CanRead::get
//
// Gets a value indicating whether the current stream supports reading

bool GzipWriter::CanRead::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return false;
}

//---------------------------------------------------------------------------
// GzipWriter::CanSeek::get
//
// Gets a value indicating whether the current stream supports seeking

bool GzipWriter::CanSeek::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return false;
}

//---------------------------------------------------------------------------
// GzipWriter::CanWrite::get
//
// Gets a value indicating whether the current stream supports writing

bool GzipWriter::CanWrite::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_stream->CanWrite;
}

//---------------------------------------------------------------------------
// GzipWriter::Flush
//
// Clears all buffers for this stream and causes any buffered data to be written
//
// Arguments:
//
//	NONE

void GzipWriter::Flush(void)
{
	int result = Z_OK;			// Result from zlib operation

	CHECK_DISPOSED(m_disposed);

	msclr::lock lock(m_lock);

	// Create and pin a local compression buffer
	array<unsigned __int8>^ out = gcnew array<unsigned __int8>(m_buffersize);
	pin_ptr<unsigned __int8> pinout = &out[0];

	// Input is not consumed when flushing the zlib stream
	m_zstream->next_in = nullptr;
	m_zstream->avail_in = 0;

	do {

		// Reset the output buffer to point into the managed array
		m_zstream->next_out = reinterpret_cast<Bytef*>(pinout);
		m_zstream->avail_out = m_buffersize;

		// Flush the next block of data in the zlib buffers and write it
		result = deflate(m_zstream, Z_SYNC_FLUSH);
		m_stream->Write(out, 0, m_buffersize - m_zstream->avail_out);
	
	} while(result == Z_OK);

	// The end state of a zlib flush operation will be Z_BUF_ERROR
	if(result != Z_BUF_ERROR) throw gcnew GzipException(result);

	delete out;						// Dispose of the compression buffer
	m_stream->Flush();				// Flush the underlying base stream
}

//--------------------------------------------------------------------------
// GzipWriter::Length::get
//
// Gets the length in bytes of the stream

__int64 GzipWriter::Length::get(void)
{
	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// GzipWriter::Position::get
//
// Gets the current position within the stream

__int64 GzipWriter::Position::get(void)
{
	CHECK_DISPOSED(m_disposed);

	msclr::lock lock(m_lock);
	return static_cast<__int64>(m_zstream->total_in);
}

//---------------------------------------------------------------------------
// GzipWriter::Position::set
//
// Sets the current position within the stream

void GzipWriter::Position::set(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// GzipWriter::Read
//
// Reads a sequence of bytes from the current stream and advances the position within the stream
//
// Arguments:
//
//	buffer		- Destination data buffer
//	offset		- Offset within buffer to begin copying data
//	count		- Maximum number of bytes to write into the destination buffer

int GzipWriter::Read(array<unsigned __int8>^ buffer, int offset, int count)
{
	UNREFERENCED_PARAMETER(buffer);
	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(count);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();}

//---------------------------------------------------------------------------
// GzipWriter::Seek
//
// Sets the position within the current stream
//
// Arguments:
//
//	offset		- Byte offset relative to origin
//	origin		- Reference point used to obtain the new position

__int64 GzipWriter::Seek(__int64 offset, SeekOrigin origin)
{
	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(origin);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// GzipWriter::SetLength
//
// Sets the length of the current stream
//
// Arguments:
//
//	value		- Desired length of the current stream in bytes

void GzipWriter::SetLength(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// GzipWriter::Write
//
// Writes a sequence of bytes to the current stream and advances the current position
//
// Arguments:
//
//	buffer		- Source data buffer 

void GzipWriter::Write(array<unsigned __int8>^ buffer)
{
	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");

	CHECK_DISPOSED(m_disposed);
	Write(buffer, 0, buffer->Length);
}

//---------------------------------------------------------------------------
// GzipWriter::Write
//
// Writes a sequence of bytes to the current stream and advances the current position
//
// Arguments:
//
//	buffer		- Source data buffer 
//	offset		- Offset within buffer to begin copying from
//	count		- Maximum number of bytes to read from the source buffer

void GzipWriter::Write(array<unsigned __int8>^ buffer, int offset, int count)
{
	CHECK_DISPOSED(m_disposed);

	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");
	if(offset < 0) throw gcnew ArgumentOutOfRangeException("offset");
	if(count < 0) throw gcnew ArgumentOutOfRangeException("count");
	if((offset + count) > buffer->Length) throw gcnew ArgumentException("The sum of offset and count is larger than the buffer length");

	if(count == 0) return;

	msclr::lock lock(m_lock);

	// Create a temporary local buffer to hold the compressed data
	array<unsigned __int8>^ out = gcnew array<unsigned __int8>(m_buffersize);
		
	// Pin both the input and output byte arrays in memory
	pin_ptr<unsigned __int8> pinin = &buffer[0];
	pin_ptr<unsigned __int8> pinout = &out[0];

	// Set up the input buffer pointer and available length
	m_zstream->next_in = reinterpret_cast<Bytef*>(&pinin[offset]);
	m_zstream->avail_in = count;

	// Repeatedly compress blocks of data until all input has been consumed
	while(m_zstream->avail_in > 0) {

		// Reset the output buffer pointer and length
		m_zstream->next_out = reinterpret_cast<Bytef*>(pinout);
		m_zstream->avail_out = m_buffersize;

		// Compress the next block of input data into the output buffer
		int result = deflate(m_zstream, Z_NO_FLUSH);
		if(result != Z_OK) throw gcnew GzipException(result);

		// Write the compressed data into the underlying base stream
		m_stream->Write(out, 0, m_buffersize - m_zstream->avail_out);
	};
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
