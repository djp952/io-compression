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
#include "XzReader.h"

#include <Alloc.h>
#include "LzmaException.h"

// crcinit
//
// Helper function defined in crcinit.cpp; thunks to CrcGenerateTable
extern void crcinit(void);

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//--------------------------------------------------------------------------
// XzReader Static Constructor (private)

static XzReader::XzReader()
{
	crcinit();							// Initialize the CRC table
}

//--------------------------------------------------------------------------
// XzReader Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is read from

XzReader::XzReader(Stream^ stream) : XzReader(stream, false)
{
}

//---------------------------------------------------------------------------
// XzReader Constructor (private)
//
// Arguments:
//
//	stream		- The stream the compressed or decompressed data is written to
//	leaveopen	- Flag to leave the base stream open after disposal

XzReader::XzReader(Stream^ stream, bool leaveopen) : m_disposed(false), m_stream(stream), m_leaveopen(leaveopen), m_finished(false),
	m_inpos(0), m_insize(0)
{
	if(Object::ReferenceEquals(stream, nullptr)) throw gcnew ArgumentNullException("stream");

	// Allocate and construct the CXzUnpacker instance
	try { m_unpacker = new CXzUnpacker; XzUnpacker_Construct(m_unpacker, &g_Alloc); }
	catch(Exception^) { throw gcnew OutOfMemoryException(); }

	// Allocate the local input buffer
	m_in = gcnew array<unsigned __int8>(BUFFER_SIZE);

	// Initialize the CXzUnpacker instance
	XzUnpacker_Init(m_unpacker);
}

//---------------------------------------------------------------------------
// XzReader Destructor

XzReader::~XzReader()
{
	if(m_disposed) return;

	// Destroy the managed input buffer
	delete m_in;
	
	// Optionally dispose of the base stream
	if(!m_leaveopen) delete m_stream;
	
	this->!XzReader();
	m_disposed = true;
}

//---------------------------------------------------------------------------
// XzReader Finalizer

XzReader::!XzReader()
{
	if(m_unpacker) { XzUnpacker_Free(m_unpacker); m_unpacker = nullptr; }
}

//---------------------------------------------------------------------------
// XzReader::BaseStream::get
//
// Accesses the underlying base stream instance

Stream^ XzReader::BaseStream::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_stream;
}

//---------------------------------------------------------------------------
// XzReader::CanRead::get
//
// Gets a value indicating whether the current stream supports reading

bool XzReader::CanRead::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_stream->CanRead;
}

//---------------------------------------------------------------------------
// XzReader::CanSeek::get
//
// Gets a value indicating whether the current stream supports seeking

bool XzReader::CanSeek::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return false;
}

//---------------------------------------------------------------------------
// XzReader::CanWrite::get
//
// Gets a value indicating whether the current stream supports writing

bool XzReader::CanWrite::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return false;
}

//---------------------------------------------------------------------------
// XzReader::Flush
//
// Clears all buffers for this stream and causes any buffered data to be written
//
// Arguments:
//
//	NONE

void XzReader::Flush(void)
{
	CHECK_DISPOSED(m_disposed);
	m_stream->Flush();
}

//--------------------------------------------------------------------------
// XzReader::Length::get
//
// Gets the length in bytes of the stream

__int64 XzReader::Length::get(void)
{
	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// XzReader::Position::get
//
// Gets the current position within the stream

__int64 XzReader::Position::get(void)
{
	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// XzReader::Position::set
//
// Sets the current position within the stream

void XzReader::Position::set(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// XzReader::Read
//
// Reads a sequence of bytes from the current stream and advances the position within the stream
//
// Arguments:
//
//	buffer		- Destination data buffer
//	offset		- Offset within buffer to begin copying data
//	count		- Maximum number of bytes to write into the destination buffer

int XzReader::Read(array<unsigned __int8>^ buffer, int offset, int count)
{
	ECoderStatus				encstatus;				// Encoder status flag
		
	CHECK_DISPOSED(m_disposed);

	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");
	if(offset < 0) throw gcnew ArgumentOutOfRangeException("offset");
	if(count < 0) throw gcnew ArgumentOutOfRangeException("count");
	if((offset + count) > buffer->Length) throw gcnew ArgumentException("The sum of offset and count is larger than the buffer length");

	msclr::lock lock(m_lock);

	// If there is no buffer to read into or the stream is already done, return zero
	if((count == 0) || (m_finished)) return 0;

	// Pin both the input and output byte arrays in memory
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

		// Unpack/decompress the next block of data from the input buffer
		SRes result = XzUnpacker_Code(m_unpacker, &pinout[offset], &outsize, &pinin[m_inpos], &insize, 
			(insize == 0) ? CODER_FINISH_END : CODER_FINISH_ANY, &encstatus);
		if(result != SZ_OK) throw gcnew LzmaException(result);

		m_inpos += insize;							// Increment the input buffer offset
		offset += static_cast<int>(outsize);		// Increment the output buffer offset
		availout -= static_cast<int>(outsize);		// Decrement the available output size

		// If no input or output was generated, the stream is finished
		if(m_finished = ((insize == 0) && (outsize == 0))) break;

	} while(availout > 0);

	// If no input or output was generated on the last call, verify that the stream is finished
	if((m_finished) && (!XzUnpacker_IsStreamWasFinished(m_unpacker))) throw gcnew InvalidDataException();

	return (count - availout);
}

//---------------------------------------------------------------------------
// XzReader::Seek
//
// Sets the position within the current stream
//
// Arguments:
//
//	offset		- Byte offset relative to origin
//	origin		- Reference point used to obtain the new position

__int64 XzReader::Seek(__int64 offset, SeekOrigin origin)
{
	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(origin);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// XzReader::SetLength
//
// Sets the length of the current stream
//
// Arguments:
//
//	value		- Desired length of the current stream in bytes

void XzReader::SetLength(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// XzReader::Write
//
// Writes a sequence of bytes to the current stream and advances the current position
//
// Arguments:
//
//	buffer		- Source data buffer 
//	offset		- Offset within buffer to begin copying from
//	count		- Maximum number of bytes to read from the source buffer

void XzReader::Write(array<unsigned __int8>^ buffer, int offset, int count)
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
