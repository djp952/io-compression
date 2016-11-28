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
#include "LzmaReader.h"

#include <Alloc.h>

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// LzmaReader Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is read from

LzmaReader::LzmaReader(Stream^ stream) : LzmaReader(stream, false)
{
}

//---------------------------------------------------------------------------
// LzmaReader Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is read from
//	leaveopen	- Flag to leave the base stream open after disposal

LzmaReader::LzmaReader(Stream^ stream, bool leaveopen) : m_disposed(false), m_stream(stream), m_leaveopen(leaveopen), m_init(false), 
	m_finished(false), m_inpos(0), m_insize(0)
{
	if(Object::ReferenceEquals(stream, nullptr)) throw gcnew ArgumentNullException("stream");

	// Allocate and zero-initialize the unmanaged CLzmaDec structure
	try { m_state = new CLzmaDec(); memset(m_state, 0, sizeof(CLzmaDec)); }
	catch(Exception^) { throw gcnew OutOfMemoryException(); }

	// Allocate the managed input buffer for this instance
	m_in = gcnew array<unsigned __int8>(BUFFER_SIZE);

	LzmaDec_Construct(m_state);			// Construct the LZMA state
}

//---------------------------------------------------------------------------
// LzmaReader Destructor

LzmaReader::~LzmaReader()
{
	if(m_disposed) return;
	
	// Optionally dispose of the base stream
	if(!m_leaveopen) delete m_stream;
	
	this->!LzmaReader();
	m_disposed = true;
}

//---------------------------------------------------------------------------
// LzmaReader Finalizer

LzmaReader::!LzmaReader()
{
	if(m_state == nullptr) return;

	// Release the LZMA decoder resources
	if(m_init) LzmaDec_Free(m_state, &g_Alloc);
	delete m_state;

	m_state = nullptr;
}

//---------------------------------------------------------------------------
// LzmaReader::BaseStream::get
//
// Accesses the underlying base stream instance

Stream^ LzmaReader::BaseStream::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_stream;
}

//---------------------------------------------------------------------------
// LzmaReader::CanRead::get
//
// Gets a value indicating whether the current stream supports reading

bool LzmaReader::CanRead::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_stream->CanRead;
}

//---------------------------------------------------------------------------
// LzmaReader::CanSeek::get
//
// Gets a value indicating whether the current stream supports seeking

bool LzmaReader::CanSeek::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return false;
}

//---------------------------------------------------------------------------
// LzmaReader::CanWrite::get
//
// Gets a value indicating whether the current stream supports writing

bool LzmaReader::CanWrite::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return false;
}

//---------------------------------------------------------------------------
// LzmaReader::Flush
//
// Clears all buffers for this stream and causes any buffered data to be written
//
// Arguments:
//
//	NONE

void LzmaReader::Flush(void)
{
	CHECK_DISPOSED(m_disposed);
	m_stream->Flush();
}

//--------------------------------------------------------------------------
// LzmaReader::Length::get
//
// Gets the length in bytes of the stream

__int64 LzmaReader::Length::get(void)
{
	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// LzmaReader::Position::get
//
// Gets the current position within the stream

__int64 LzmaReader::Position::get(void)
{
	CHECK_DISPOSED(m_disposed);
	
	msclr::lock lock(m_lock);
	return static_cast<__int64>(m_state->processedPos);
}

//---------------------------------------------------------------------------
// LzmaReader::Position::set
//
// Sets the current position within the stream

void LzmaReader::Position::set(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// LzmaReader::Read
//
// Reads a sequence of bytes from the current stream and advances the position within the stream
//
// Arguments:
//
//	buffer		- Destination data buffer
//	offset		- Offset within buffer to begin copying data
//	count		- Maximum number of bytes to write into the destination buffer

int LzmaReader::Read(array<unsigned __int8>^ buffer, int offset, int count)
{
	ELzmaStatus				status;			// Status from LZMA decode operation

	CHECK_DISPOSED(m_disposed);

	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");
	if(offset < 0) throw gcnew ArgumentOutOfRangeException("offset");
	if(count < 0) throw gcnew ArgumentOutOfRangeException("count");
	if((offset + count) > buffer->Length) throw gcnew ArgumentException("The sum of offset and count is larger than the buffer length");

	msclr::lock lock(m_lock);

	// If there is no buffer to read into or the stream is already done, return zero
	if((count == 0) || (m_finished)) return 0;

	// Wait to initialize the LZMA decoder until the first call to Read()
	if(!m_init) {

		// Read the properties and stream length from the input stream
		array<unsigned __int8>^ props = gcnew array<unsigned __int8>(LZMA_PROPS_SIZE + sizeof(uint64_t));
		if(m_stream->Read(props, 0, LZMA_PROPS_SIZE + sizeof(uint64_t)) != LZMA_PROPS_SIZE + sizeof(uint64_t)) throw gcnew InvalidDataException();

		// Allocate the LZMA decoder state
		pin_ptr<unsigned __int8> pinprops = &props[0];
		SRes result = LzmaDec_Allocate(m_state, pinprops, LZMA_PROPS_SIZE, &g_Alloc);
		if(result == SZ_ERROR_MEM) throw gcnew OutOfMemoryException();
		else if(result == SZ_ERROR_UNSUPPORTED) throw gcnew InvalidDataException();

		LzmaDec_Init(m_state);				// Initialize the LZMA decoder
		m_init = true;						// Ready to decompress the stream
	}

	// Pin the input/output buffers and the available input length
	pin_ptr<unsigned __int8> pinin = &m_in[0];
	pin_ptr<unsigned __int8> pinout = &buffer[0];

	// Copy count into a local value to tally the final bytes read from the stream
	int availout = count;

	do {

		// If the input buffer was flushed from a previous iteration, refill it
		if(m_inpos == m_insize) {

			m_inpos = (m_insize = m_stream->Read(m_in, 0, BUFFER_SIZE)) - m_insize;
			if(m_insize > BUFFER_SIZE) throw gcnew InvalidDataException();
		}

		// Use local input/output size values, they are modified by XzUnpacker_Code
		size_t insize = m_insize - m_inpos;
		size_t outsize = count - offset;

		// Attempt to decode the next block of compressed data into the output buffer
		SRes result = LzmaDec_DecodeToBuf(m_state, &pinout[offset], &outsize, &pinin[m_inpos], &insize, LZMA_FINISH_ANY, &status);
 		if(result == SZ_ERROR_DATA) throw gcnew InvalidDataException();

		m_inpos += insize;							// Increment the input buffer offset
		offset += static_cast<int>(outsize);		// Increment the output buffer offset
		availout -= static_cast<int>(outsize);		// Decrement the available output size

		// LZMA_STATUS_FINISHED_WITH_MARK indicates that there is no more data
		if(m_finished = (status == LZMA_STATUS_FINISHED_WITH_MARK)) break;

	} while(availout > 0);

	return (count - availout);
}

//---------------------------------------------------------------------------
// LzmaReader::Seek
//
// Sets the position within the current stream
//
// Arguments:
//
//	offset		- Byte offset relative to origin
//	origin		- Reference point used to obtain the new position

__int64 LzmaReader::Seek(__int64 offset, SeekOrigin origin)
{
	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(origin);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// LzmaReader::SetLength
//
// Sets the length of the current stream
//
// Arguments:
//
//	value		- Desired length of the current stream in bytes

void LzmaReader::SetLength(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// LzmaReader::Write
//
// Writes a sequence of bytes to the current stream and advances the current position
//
// Arguments:
//
//	buffer		- Source data buffer 
//	offset		- Offset within buffer to begin copying from
//	count		- Maximum number of bytes to read from the source buffer

void LzmaReader::Write(array<unsigned __int8>^ buffer, int offset, int count)
{
	UNREFERENCED_PARAMETER(buffer);
	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(count);

	CHECK_DISPOSED(m_disposed);
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
