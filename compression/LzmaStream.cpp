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
#include "LzmaStream.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// LzmaStream Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is written to
//	level		- Indicates whether to emphasize speed or compression efficiency

LzmaStream::LzmaStream(Stream^ stream, Compression::CompressionLevel level) : LzmaStream(stream, level, false)
{
}

//---------------------------------------------------------------------------
// LzmaStream Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is written to
//	level		- Indicates whether to emphasize speed or compression efficiency
//	leaveopen	- Flag to leave the base stream open after disposal

LzmaStream::LzmaStream(Stream^ stream, Compression::CompressionLevel level, bool leaveopen)
{
}

//---------------------------------------------------------------------------
// LzmaStream Constructor
//
// Arguments:
//
//	stream		- The stream the compressed or decompressed data is written to
//	mode		- Indicates whether to compress or decompress the stream

LzmaStream::LzmaStream(Stream^ stream, Compression::CompressionMode mode) : LzmaStream(stream, mode, false)
{
}

//---------------------------------------------------------------------------
// LzmaStream Constructor
//
// Arguments:
//
//	stream		- The stream the compressed or decompressed data is written to
//	mode		- Indicates whether to compress or decompress the stream
//	leaveopen	- Flag to leave the base stream open after disposal

LzmaStream::LzmaStream(Stream^ stream, Compression::CompressionMode mode, bool leaveopen)
{
}

//---------------------------------------------------------------------------
// LzmaStream Destructor

LzmaStream::~LzmaStream()
{
	if(m_disposed) return;
	m_disposed = true;
}

//---------------------------------------------------------------------------
// LzmaStream::CanRead::get
//
// Gets a value indicating whether the current stream supports reading

bool LzmaStream::CanRead::get(void)
{
	CHECK_DISPOSED(m_disposed);

	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------
// LzmaStream::CanSeek::get
//
// Gets a value indicating whether the current stream supports seeking

bool LzmaStream::CanSeek::get(void)
{
	CHECK_DISPOSED(m_disposed);

	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------
// LzmaStream::CanWrite::get
//
// Gets a value indicating whether the current stream supports writing

bool LzmaStream::CanWrite::get(void)
{
	CHECK_DISPOSED(m_disposed);

	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------
// LzmaStream::Flush
//
// Clears all buffers for this stream and causes any buffered data to be written
//
// Arguments:
//
//	NONE

void LzmaStream::Flush(void)
{
	CHECK_DISPOSED(m_disposed);

	throw gcnew NotImplementedException();
}

//--------------------------------------------------------------------------
// LzmaStream::Length::get
//
// Gets the length in bytes of the stream

__int64 LzmaStream::Length::get(void)
{
	CHECK_DISPOSED(m_disposed);

	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------
// LzmaStream::Position::get
//
// Gets the current position within the stream

__int64 LzmaStream::Position::get(void)
{
	CHECK_DISPOSED(m_disposed);

	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------
// LzmaStream::Position::set
//
// Sets the current position within the stream

void LzmaStream::Position::set(__int64 value)
{
	CHECK_DISPOSED(m_disposed);
	
	UNREFERENCED_PARAMETER(value);
	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------
// LzmaStream::Read
//
// Reads a sequence of bytes from the current stream and advances the position within the stream
//
// Arguments:
//
//	buffer		- Destination data buffer
//	offset		- Offset within buffer to begin copying data
//	count		- Maximum number of bytes to write into the destination buffer

int LzmaStream::Read(array<unsigned __int8>^ buffer, int offset, int count)
{
	CHECK_DISPOSED(m_disposed);

	UNREFERENCED_PARAMETER(buffer);
	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(count);
	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------
// LzmaStream::Seek
//
// Sets the position within the current stream
//
// Arguments:
//
//	offset		- Byte offset relative to origin
//	origin		- Reference point used to obtain the new position

__int64 LzmaStream::Seek(__int64 offset, SeekOrigin origin)
{
	CHECK_DISPOSED(m_disposed);

	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(origin);
	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------
// LzmaStream::SetLength
//
// Sets the length of the current stream
//
// Arguments:
//
//	value		- Desired length of the current stream in bytes

void LzmaStream::SetLength(__int64 value)
{
	CHECK_DISPOSED(m_disposed);

	UNREFERENCED_PARAMETER(value);
	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------
// LzmaStream::Write
//
// Writes a sequence of bytes to the current stream and advances the current position
//
// Arguments:
//
//	buffer		- Source data buffer 
//	offset		- Offset within buffer to begin copying from
//	count		- Maximum number of bytes to read from the source buffer

void LzmaStream::Write(array<unsigned __int8>^ buffer, int offset, int count)
{
	CHECK_DISPOSED(m_disposed);

	UNREFERENCED_PARAMETER(buffer);
	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(count);
	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
