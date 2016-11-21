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
#include "Lz4LegacyReader.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Lz4LegacyReader Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is read from

Lz4LegacyReader::Lz4LegacyReader(Stream^ stream) : Lz4LegacyReader(stream, false)
{
}

//---------------------------------------------------------------------------
// Lz4LegacyReader Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is read from
//	leaveopen	- Flag to leave the base stream open after disposal

Lz4LegacyReader::Lz4LegacyReader(Stream^ stream, bool leaveopen) : m_disposed(false), m_stream(stream), m_leaveopen(leaveopen),
	m_hasmagic(false), m_buffer(gcnew array<unsigned __int8>(LEGACY_BLOCKSIZE)), m_bufferavail(0), m_bufferpos(0)
{
	if(Object::ReferenceEquals(stream, nullptr)) throw gcnew ArgumentNullException("stream");
}

//---------------------------------------------------------------------------
// Lz4LegacyReader Destructor

Lz4LegacyReader::~Lz4LegacyReader()
{
	if(m_disposed) return;

	// Destroy the managed decompressed data buffer
	delete m_buffer;

	// Optionally dispose of the input stream instance
	if(!m_leaveopen) delete m_stream;
	
	m_disposed = true;
}

//---------------------------------------------------------------------------
// Lz4LegacyReader::BaseStream::get
//
// Accesses the underlying base stream instance

Stream^ Lz4LegacyReader::BaseStream::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_stream;
}

//---------------------------------------------------------------------------
// Lz4LegacyReader::CanRead::get
//
// Gets a value indicating whether the current stream supports reading

bool Lz4LegacyReader::CanRead::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return (m_stream->CanRead);
}

//---------------------------------------------------------------------------
// Lz4LegacyReader::CanSeek::get
//
// Gets a value indicating whether the current stream supports seeking

bool Lz4LegacyReader::CanSeek::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return false;
}

//---------------------------------------------------------------------------
// Lz4LegacyReader::CanWrite::get
//
// Gets a value indicating whether the current stream supports writing

bool Lz4LegacyReader::CanWrite::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return false;
}

//---------------------------------------------------------------------------
// Lz4LegacyReader::Flush
//
// Clears all buffers for this stream and causes any buffered data to be written
//
// Arguments:
//
//	NONE

void Lz4LegacyReader::Flush(void)
{
	CHECK_DISPOSED(m_disposed);
	m_stream->Flush();
}

//--------------------------------------------------------------------------
// Lz4LegacyReader::Length::get
//
// Gets the length in bytes of the stream

__int64 Lz4LegacyReader::Length::get(void)
{
	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Lz4LegacyReader::Position::get
//
// Gets the current position within the stream

__int64 Lz4LegacyReader::Position::get(void)
{
	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Lz4LegacyReader::Position::set
//
// Sets the current position within the stream

void Lz4LegacyReader::Position::set(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Lz4LegacyReader::Read
//
// Reads a sequence of bytes from the current stream and advances the position within the stream
//
// Arguments:
//
//	buffer		- Destination data buffer
//	offset		- Offset within buffer to begin copying data
//	count		- Maximum number of bytes to write into the destination buffer

int Lz4LegacyReader::Read(array<unsigned __int8>^ buffer, int offset, int count)
{
	unsigned int				nextblock;			// Next compressed block size
	int							out = 0;			// Total bytes read from the stream

	CHECK_DISPOSED(m_disposed);

	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");
	if(offset < 0) throw gcnew ArgumentOutOfRangeException("offset");
	if(count < 0) throw gcnew ArgumentOutOfRangeException("count");
	if((offset + count) > buffer->Length) throw gcnew ArgumentException("The sum of offset and count is larger than the buffer length");

	if(count == 0) return 0;				// No output buffer to read into

	msclr::lock lock(m_lock);				// Serialize access to the buffer

	// Wait to check the magic number of the input stream until the first Read() attempt
	if(!m_hasmagic) {

		// If there is insufficient input for the magic number, there is no input
		if(!ReadLE32(m_stream, nextblock)) return 0;

		// Verify that LEGACY_MAGICNUMBER has been returned
		if(nextblock != LEGACY_MAGICNUMBER) throw gcnew InvalidDataException();
		m_hasmagic = true;
	}

	do {

		// If there is no more decompressed data available, go get some
		if(m_bufferavail == 0) {

			// Get the length of the next block of data to be decompressed; if there
			// is insufficient data left in the stream, decompression is finished
			if(!ReadLE32(m_stream, nextblock)) break;
			if((nextblock == 0) || (nextblock > Int32::MaxValue)) throw gcnew InvalidDataException();

			// Read the next entire block of compressed data from the input stream
			array<unsigned __int8>^ in = gcnew array<unsigned __int8>(nextblock);
			if(m_stream->Read(in, 0, nextblock) != (int)nextblock) throw gcnew InvalidDataException();

			// Pin both the input and output buffers so the LZ4 API can access them
			pin_ptr<unsigned __int8> pinin = &in[0];
			pin_ptr<unsigned __int8> pinout = &m_buffer[0];

			// Decompress the next block of data into the output buffer
			m_bufferavail = LZ4_decompress_safe(reinterpret_cast<char const*>(pinin), reinterpret_cast<char*>(pinout), nextblock, m_buffer->Length);
			if(m_bufferavail <= 0) throw gcnew InvalidDataException();

			m_bufferpos = 0;				// Reset the stored buffer offset
			delete in;						// Dispose of the input buffer
		}

		// Copy data from the decompression buffer into the output buffer
		int next = Math::Min(m_bufferavail, count);
		Array::Copy(m_buffer, m_bufferpos, buffer, offset, next);

		m_bufferpos += next;				// Move offset into the decompression buffer
		m_bufferavail -= next;				// Reduce length of the decompression buffer
		out += next;						// Increment the amount of data written to the caller
		count -= next;						// Decrement the amount of data still to read
	
	} while(count > 0);

	return out;
}

//---------------------------------------------------------------------------
// Lz4LegacyReader::ReadLE32 (static, private)
//
// Reads an unsigned 32 bit value from an input stream
//
// Arguments:
//
//	stream		- Stream instance from which to read the value
//	value		- Value read from the stream

bool Lz4LegacyReader::ReadLE32(Stream^ stream, unsigned int% value)
{
	if(Object::ReferenceEquals(stream, nullptr)) throw gcnew ArgumentNullException();

	array<unsigned __int8>^ buffer = gcnew array<unsigned __int8>(4);
	if(stream->Read(buffer, 0, 4) != 4) return false;

	// Convert the 4 individual bytes into a single unsigned 32-bit value
	value = (buffer[0] << 0) | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);

	return true;
}

//---------------------------------------------------------------------------
// Lz4LegacyReader::Seek
//
// Sets the position within the current stream
//
// Arguments:
//
//	offset		- Byte offset relative to origin
//	origin		- Reference point used to obtain the new position

__int64 Lz4LegacyReader::Seek(__int64 offset, SeekOrigin origin)
{
	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(origin);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Lz4LegacyReader::SetLength
//
// Sets the length of the current stream
//
// Arguments:
//
//	value		- Desired length of the current stream in bytes

void Lz4LegacyReader::SetLength(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Lz4LegacyReader::Write
//
// Writes a sequence of bytes to the current stream and advances the current position
//
// Arguments:
//
//	buffer		- Source data buffer 
//	offset		- Offset within buffer to begin copying from
//	count		- Maximum number of bytes to read from the source buffer

void Lz4LegacyReader::Write(array<unsigned __int8>^ buffer, int offset, int count)
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
