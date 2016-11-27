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
#include "Lz4LegacyWriter.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// LZ4IO_LZ4_compress (local)
//
// Copy of lz4io::LZ4IO_LZ4_compress; accepts an unused compression level
// argument to allow for pointer compatibility with LZ4_compress_HC

static int LZ4IO_LZ4_compress(char const* source, char* destination, int sourcelen, int destlen, int level)
{
	UNREFERENCED_PARAMETER(level);
    return LZ4_compress_fast(source, destination, sourcelen, destlen, 1);
}

//---------------------------------------------------------------------------
// Lz4LegacyWriter Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is written to

Lz4LegacyWriter::Lz4LegacyWriter(Stream^ stream) : Lz4LegacyWriter(stream, Compression::CompressionLevel::Optimal, false)
{
}

//---------------------------------------------------------------------------
// Lz4LegacyWriter Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is written to
//	level		- Indicates whether to emphasize speed or compression efficiency

Lz4LegacyWriter::Lz4LegacyWriter(Stream^ stream, Compression::CompressionLevel level) : Lz4LegacyWriter(stream, level, false)
{
}

//---------------------------------------------------------------------------
// Lz4LegacyWriter Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is written to
//	leaveopen	- Flag to leave the base stream open after disposal

Lz4LegacyWriter::Lz4LegacyWriter(Stream^ stream, bool leaveopen) : Lz4LegacyWriter(stream, Compression::CompressionLevel::Optimal, leaveopen)
{
}

//---------------------------------------------------------------------------
// Lz4LegacyWriter Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is read from
//	level		- Indicates the level of compression to use
//	leaveopen	- Flag to leave the base stream open after disposal

Lz4LegacyWriter::Lz4LegacyWriter(Stream^ stream, Compression::CompressionLevel level, bool leaveopen) : m_disposed(false), 
	m_stream(stream), m_leaveopen(leaveopen), m_hasmagic(false), m_inpos(0)
{
	if(Object::ReferenceEquals(stream, nullptr)) throw gcnew ArgumentNullException("stream");

	// Convert the level into an integer value that can be passed into LZ4
	if(level == Compression::CompressionLevel::Fastest) m_level = 1;
	else if(level == Compression::CompressionLevel::Optimal) m_level = 9;
	else throw gcnew ArgumentOutOfRangeException("level");

	// Select a compression function based on the requested compression level
	m_compressor = (m_level < 3) ? LZ4IO_LZ4_compress : LZ4_compress_HC;

	// Create the managed input data buffer
	m_in = gcnew array<unsigned __int8>(LEGACY_BLOCKSIZE);
}

//---------------------------------------------------------------------------
// Lz4LegacyWriter Destructor

Lz4LegacyWriter::~Lz4LegacyWriter()
{
	if(m_disposed) return;

	// On disposal, finish compressing any partial block still in the buffer
	if(m_inpos > 0) WriteNextBlock();

	// Destroy the managed input data buffer
	delete m_in;

	// Optionally dispose of the input stream instance
	if(!m_leaveopen) delete m_stream;
	
	m_disposed = true;
}

//---------------------------------------------------------------------------
// Lz4LegacyWriter::BaseStream::get
//
// Accesses the underlying base stream instance

Stream^ Lz4LegacyWriter::BaseStream::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_stream;
}

//---------------------------------------------------------------------------
// Lz4LegacyWriter::CanRead::get
//
// Gets a value indicating whether the current stream supports reading

bool Lz4LegacyWriter::CanRead::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return false;
}

//---------------------------------------------------------------------------
// Lz4LegacyWriter::CanSeek::get
//
// Gets a value indicating whether the current stream supports seeking

bool Lz4LegacyWriter::CanSeek::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return false;
}

//---------------------------------------------------------------------------
// Lz4LegacyWriter::CanWrite::get
//
// Gets a value indicating whether the current stream supports writing

bool Lz4LegacyWriter::CanWrite::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_stream->CanWrite;
}

//---------------------------------------------------------------------------
// Lz4LegacyWriter::Flush
//
// Clears all buffers for this stream and causes any buffered data to be written
//
// Arguments:
//
//	NONE

void Lz4LegacyWriter::Flush(void)
{
	CHECK_DISPOSED(m_disposed);

	msclr::lock lock(m_lock);

	if(m_inpos > 0) WriteNextBlock();		// Flush buffered data
	m_stream->Flush();						// Flush underlying stream
}

//--------------------------------------------------------------------------
// Lz4LegacyWriter::Length::get
//
// Gets the length in bytes of the stream

__int64 Lz4LegacyWriter::Length::get(void)
{
	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Lz4LegacyWriter::Position::get
//
// Gets the current position within the stream

__int64 Lz4LegacyWriter::Position::get(void)
{
	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Lz4LegacyWriter::Position::set
//
// Sets the current position within the stream

void Lz4LegacyWriter::Position::set(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Lz4LegacyWriter::Read
//
// Reads a sequence of bytes from the current stream and advances the position within the stream
//
// Arguments:
//
//	buffer		- Destination data buffer
//	offset		- Offset within buffer to begin copying data
//	count		- Maximum number of bytes to write into the destination buffer

int Lz4LegacyWriter::Read(array<unsigned __int8>^ buffer, int offset, int count)
{
	UNREFERENCED_PARAMETER(buffer);
	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(count);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Lz4LegacyWriter::Seek
//
// Sets the position within the current stream
//
// Arguments:
//
//	offset		- Byte offset relative to origin
//	origin		- Reference point used to obtain the new position

__int64 Lz4LegacyWriter::Seek(__int64 offset, SeekOrigin origin)
{
	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(origin);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Lz4LegacyWriter::SetLength
//
// Sets the length of the current stream
//
// Arguments:
//
//	value		- Desired length of the current stream in bytes

void Lz4LegacyWriter::SetLength(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Lz4LegacyWriter::Write
//
// Writes a sequence of bytes to the current stream and advances the current position
//
// Arguments:
//
//	buffer		- Source data buffer 
//	offset		- Offset within buffer to begin copying from
//	count		- Maximum number of bytes to read from the source buffer

void Lz4LegacyWriter::Write(array<unsigned __int8>^ buffer, int offset, int count)
{
	CHECK_DISPOSED(m_disposed);

	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");
	if(offset < 0) throw gcnew ArgumentOutOfRangeException("offset");
	if(count < 0) throw gcnew ArgumentOutOfRangeException("count");
	if((offset + count) > buffer->Length) throw gcnew ArgumentException("The sum of offset and count is larger than the buffer length");

	if(count == 0) return;

	msclr::lock lock(m_lock);

	// Wait to write the magic number into the output stream until the first Write() attempt
	if(!m_hasmagic) { WriteLE32(m_stream, LEGACY_MAGICNUMBER); m_hasmagic = true; }

	while(count > 0) {

		// Copy the next chunk of input data into the buffer
		int next = Math::Min(m_in->Length - m_inpos, count);
		Array::Copy(buffer, offset, m_in, m_inpos, next);

		m_inpos += next;				// Increment length of buffer
		count -= next;					// Decrement bytes remaining

		// If the input buffer has been filled, write the next block to output
		if(m_inpos == m_in->Length) WriteNextBlock();
	}
}

//---------------------------------------------------------------------------
// Lz4LegacyWriter::WriteLE32 (static, private)
//
// Writes an unsigned 32 bit value into an output stream
//
// Arguments:
//
//	stream		- Stream instance from which to read the value
//	value		- Value read from the stream

void Lz4LegacyWriter::WriteLE32(Stream^ stream, unsigned int value)
{
	if(Object::ReferenceEquals(stream, nullptr)) throw gcnew ArgumentNullException();

	// Convert the 32 bit unsigned value into an array of 4 little endian bytes
	array<unsigned __int8>^ buffer = gcnew array<unsigned __int8>{(unsigned __int8)((value & 0xFF) >> 0), 
		(unsigned __int8)((value & 0xFF00) >> 8), (unsigned __int8)((value & 0xFF0000) >> 16), (unsigned __int8)((value & 0xFF000000) >> 24)};

	stream->Write(buffer, 0, 4);
}

//---------------------------------------------------------------------------
// Lz4LegacyWriter::WriteNextBlock (private)
//
// Writes the contents of the buffer to output as a single block
//
// Arguments:
//
//	NONE

int Lz4LegacyWriter::WriteNextBlock(void)
{
	msclr::lock lock(m_lock);

	// If there is nothing in the buffer, there is no work to do
	if(m_inpos == 0) return 0;

	// Generate a temporary buffer large enough for LZ4 to compress into
	array<unsigned __int8>^ out = gcnew array<unsigned __int8>(LZ4_compressBound(m_inpos));

	// Pin both the input and output buffers in memory
	pin_ptr<unsigned __int8> pinin = &m_in[0];
	pin_ptr<unsigned __int8> pinout = &out[0];

	// Compress the data using the specified compressor and level
	int outlen = m_compressor(reinterpret_cast<char const*>(pinin), reinterpret_cast<char*>(pinout), m_inpos, out->Length, m_level);

	WriteLE32(m_stream, outlen);			// Write the length prefix
	m_stream->Write(out, 0, outlen);		// Write the compressed data

	delete out;								// Get rid of the temporary buffer
	m_inpos = 0;							// Reset buffer position to zero

	return outlen + 4;						// Include the LE32 length prefix
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
