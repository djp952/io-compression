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
#include "LzoStream.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// LzoStream Constructor
//
// Arguments:
//
//	NONE

LzoStream::LzoStream()
{
}

//---------------------------------------------------------------------------
// LzoStream Destructor

LzoStream::~LzoStream()
{
	if(m_disposed) return;
	m_disposed = true;
}

//---------------------------------------------------------------------------
// LzoStream::CanRead::get
//
// Gets a value indicating whether the current stream supports reading

bool LzoStream::CanRead::get(void)
{
	CHECK_DISPOSED(m_disposed);

	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------
// LzoStream::CanSeek::get
//
// Gets a value indicating whether the current stream supports seeking

bool LzoStream::CanSeek::get(void)
{
	CHECK_DISPOSED(m_disposed);

	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------
// LzoStream::CanWrite::get
//
// Gets a value indicating whether the current stream supports writing

bool LzoStream::CanWrite::get(void)
{
	CHECK_DISPOSED(m_disposed);

	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------
// LzoStream::Flush
//
// Clears all buffers for this stream and causes any buffered data to be written
//
// Arguments:
//
//	NONE

void LzoStream::Flush(void)
{
	CHECK_DISPOSED(m_disposed);

	throw gcnew NotImplementedException();
}

//--------------------------------------------------------------------------
// LzoStream::Length::get
//
// Gets the length in bytes of the stream

__int64 LzoStream::Length::get(void)
{
	CHECK_DISPOSED(m_disposed);

	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------
// LzoStream::Position::get
//
// Gets the current position within the stream

__int64 LzoStream::Position::get(void)
{
	CHECK_DISPOSED(m_disposed);

	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------
// LzoStream::Position::set
//
// Sets the current position within the stream

void LzoStream::Position::set(__int64 value)
{
	CHECK_DISPOSED(m_disposed);
	
	UNREFERENCED_PARAMETER(value);
	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------
// LzoStream::Read
//
// Reads a sequence of bytes from the current stream and advances the position within the stream
//
// Arguments:
//
//	buffer		- Destination data buffer
//	offset		- Offset within buffer to begin copying data
//	count		- Maximum number of bytes to write into the destination buffer

int LzoStream::Read(array<unsigned __int8>^ buffer, int offset, int count)
{
	CHECK_DISPOSED(m_disposed);

	UNREFERENCED_PARAMETER(buffer);
	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(count);
	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------
// LzoStream::Seek
//
// Sets the position within the current stream
//
// Arguments:
//
//	offset		- Byte offset relative to origin
//	origin		- Reference point used to obtain the new position

__int64 LzoStream::Seek(__int64 offset, SeekOrigin origin)
{
	CHECK_DISPOSED(m_disposed);

	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(origin);
	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------
// LzoStream::SetLength
//
// Sets the length of the current stream
//
// Arguments:
//
//	value		- Desired length of the current stream in bytes

void LzoStream::SetLength(__int64 value)
{
	CHECK_DISPOSED(m_disposed);

	UNREFERENCED_PARAMETER(value);
	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------
// LzoStream::Write
//
// Writes a sequence of bytes to the current stream and advances the current position
//
// Arguments:
//
//	buffer		- Source data buffer 
//	offset		- Offset within buffer to begin copying from
//	count		- Maximum number of bytes to read from the source buffer

void LzoStream::Write(array<unsigned __int8>^ buffer, int offset, int count)
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
