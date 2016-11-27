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
#include "Lz4Writer.h"

#include "Lz4Exception.h"

// LZ4F_cctx_s is an incomplete type; causes LNK4248
//
struct LZ4F_cctx_s {};

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Lz4Writer Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is written to

Lz4Writer::Lz4Writer(Stream^ stream) : Lz4Writer(stream, Compression::CompressionLevel::Optimal, false)
{
}

//---------------------------------------------------------------------------
// Lz4Writer Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is written to
//	level		- Indicates whether to emphasize speed or compression efficiency

Lz4Writer::Lz4Writer(Stream^ stream, Compression::CompressionLevel level) : Lz4Writer(stream, level, false)
{
}

//---------------------------------------------------------------------------
// Lz4Writer Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is written to
//	leaveopen	- Flag to leave the base stream open after disposal

Lz4Writer::Lz4Writer(Stream^ stream, bool leaveopen) : Lz4Writer(stream, Compression::CompressionLevel::Optimal, leaveopen)
{
}

//---------------------------------------------------------------------------
// Lz4Writer Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is read from
//	level		- Indicates the level of compression to use
//	leaveopen	- Flag to leave the base stream open after disposal

Lz4Writer::Lz4Writer(Stream^ stream, Compression::CompressionLevel level, bool leaveopen) : m_disposed(false), m_stream(stream), m_leaveopen(leaveopen)
{
	LZ4F_errorCode_t				result;				// Result from LZ4 function call

	if(Object::ReferenceEquals(stream, nullptr)) throw gcnew ArgumentNullException("stream");

	// lz4 does not provide a 'no compression' option
	if(level == Compression::CompressionLevel::NoCompression) throw gcnew ArgumentOutOfRangeException("level");

	// Allocate and initialize the compression context structure
	try { m_context = new LZ4F_compressionContext_t; memset(m_context, 0, sizeof(LZ4F_compressionContext_t)); }
	catch(Exception^) { throw gcnew OutOfMemoryException(); }

	// Allocate and initialize the preferences structure
	try { m_prefs = new LZ4F_preferences_t; memset(m_prefs, 0, sizeof(LZ4F_preferences_t)); }
	catch(Exception^) { throw gcnew OutOfMemoryException(); }

	// Create the LZ4 compression context structure for this instance
	result = LZ4F_createCompressionContext(m_context, LZ4F_VERSION);
	if(LZ4F_isError(result)) throw gcnew Lz4Exception(result);

	// Set up the compression preferences for this instance
	m_prefs->autoFlush = 0;
	m_prefs->compressionLevel = (level == Compression::CompressionLevel::Fastest) ? 1 : 9;
	m_prefs->frameInfo.blockMode = LZ4F_blockIndependent;
	m_prefs->frameInfo.blockSizeID = LZ4F_max4MB;
	m_prefs->frameInfo.contentChecksumFlag = LZ4F_contentChecksumEnabled;

	// Create a temporary buffer to hold the stream header information (max 15 bytes)
	array<unsigned __int8>^ header = gcnew array<unsigned __int8>(15);
	pin_ptr<unsigned __int8> pinheader = &header[0];

	// Initialize the compressed stream
	result = LZ4F_compressBegin(*m_context, pinheader, header->Length, m_prefs);
	if(LZ4F_isError(result)) throw gcnew Lz4Exception(result);
	m_stream->Write(header, 0, result);
}

//---------------------------------------------------------------------------
// Lz4Writer Destructor

Lz4Writer::~Lz4Writer()
{
	if(m_disposed) return;

	// There is no way to know how much data there is in the lz4 buffers, assume
	// the worst case, which is a 4MiB block (LZ4F_blockSizeID_t::LZ4F_max4MB)
	array<unsigned __int8>^ out = gcnew array<unsigned __int8>(4 << 20);
	pin_ptr<unsigned __int8> pinout = &out[0];

	// Complete the compression stream and write out any generated data
	LZ4F_compressOptions_t options ={ 0 /* stableSrc */, {0, 0, 0} /* reserved */};
	LZ4F_errorCode_t result = LZ4F_compressEnd(*m_context, pinout, out->Length, &options);
	if(LZ4F_isError(result)) throw gcnew Lz4Exception(result);
	if(result > 0) m_stream->Write(out, 0, result);

	// Optionally dispose of the input stream instance
	if(!m_leaveopen) delete m_stream;
	
	this->!Lz4Writer();
	m_disposed = true;
}

//---------------------------------------------------------------------------
// Lz4Writer Finalizer

Lz4Writer::!Lz4Writer()
{
	if(m_context == nullptr) return;

	// Release the LZ4 compression context structure
	LZ4F_freeCompressionContext(*m_context);

	delete m_context;
	m_context = nullptr;
}

//---------------------------------------------------------------------------
// Lz4Writer::BaseStream::get
//
// Accesses the underlying base stream instance

Stream^ Lz4Writer::BaseStream::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_stream;
}

//---------------------------------------------------------------------------
// Lz4Writer::CanRead::get
//
// Gets a value indicating whether the current stream supports reading

bool Lz4Writer::CanRead::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return false;
}

//---------------------------------------------------------------------------
// Lz4Writer::CanSeek::get
//
// Gets a value indicating whether the current stream supports seeking

bool Lz4Writer::CanSeek::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return false;
}

//---------------------------------------------------------------------------
// Lz4Writer::CanWrite::get
//
// Gets a value indicating whether the current stream supports writing

bool Lz4Writer::CanWrite::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_stream->CanWrite;
}

//---------------------------------------------------------------------------
// Lz4Writer::Flush
//
// Clears all buffers for this stream and causes any buffered data to be written
//
// Arguments:
//
//	NONE

void Lz4Writer::Flush(void)
{
	CHECK_DISPOSED(m_disposed);

	msclr::lock lock(m_lock);

	// There is no way to know how much data there is in the lz4 buffers, assume
	// the worst case, which is a 4MiB block (LZ4F_blockSizeID_t::LZ4F_max4MB)
	array<unsigned __int8>^ out = gcnew array<unsigned __int8>(4 << 20);
	pin_ptr<unsigned __int8> pinout = &out[0];

	// Compress any buffered block data into the output buffer
	LZ4F_compressOptions_t options ={ 0 /* stableSrc */, {0, 0, 0} /* reserved */};
	LZ4F_errorCode_t result = LZ4F_flush(*m_context, pinout, out->Length, &options);
	if(LZ4F_isError(result)) throw gcnew Lz4Exception(result);

	// Write the compressed data to the output stream and flush it
	if(result > 0) m_stream->Write(out, 0, result);
	m_stream->Flush();
}

//--------------------------------------------------------------------------
// Lz4Writer::Length::get
//
// Gets the length in bytes of the stream

__int64 Lz4Writer::Length::get(void)
{
	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Lz4Writer::Position::get
//
// Gets the current position within the stream

__int64 Lz4Writer::Position::get(void)
{
	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Lz4Writer::Position::set
//
// Sets the current position within the stream

void Lz4Writer::Position::set(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Lz4Writer::Read
//
// Reads a sequence of bytes from the current stream and advances the position within the stream
//
// Arguments:
//
//	buffer		- Destination data buffer
//	offset		- Offset within buffer to begin copying data
//	count		- Maximum number of bytes to write into the destination buffer

int Lz4Writer::Read(array<unsigned __int8>^ buffer, int offset, int count)
{
	UNREFERENCED_PARAMETER(buffer);
	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(count);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Lz4Writer::Seek
//
// Sets the position within the current stream
//
// Arguments:
//
//	offset		- Byte offset relative to origin
//	origin		- Reference point used to obtain the new position

__int64 Lz4Writer::Seek(__int64 offset, SeekOrigin origin)
{
	UNREFERENCED_PARAMETER(offset);
	UNREFERENCED_PARAMETER(origin);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Lz4Writer::SetLength
//
// Sets the length of the current stream
//
// Arguments:
//
//	value		- Desired length of the current stream in bytes

void Lz4Writer::SetLength(__int64 value)
{
	UNREFERENCED_PARAMETER(value);

	CHECK_DISPOSED(m_disposed);
	throw gcnew NotSupportedException();
}

//---------------------------------------------------------------------------
// Lz4Writer::Write
//
// Writes a sequence of bytes to the current stream and advances the current position
//
// Arguments:
//
//	buffer		- Source data buffer 
//	offset		- Offset within buffer to begin copying from
//	count		- Maximum number of bytes to read from the source buffer

void Lz4Writer::Write(array<unsigned __int8>^ buffer, int offset, int count)
{
	CHECK_DISPOSED(m_disposed);

	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");
	if(offset < 0) throw gcnew ArgumentOutOfRangeException("offset");
	if(count < 0) throw gcnew ArgumentOutOfRangeException("count");
	if((offset + count) > buffer->Length) throw gcnew ArgumentException("The sum of offset and count is larger than the buffer length");

	if(count == 0) return;

	msclr::lock lock(m_lock);

	// Create a temporary local buffer to hold the compressed data
	array<unsigned __int8>^ out = gcnew array<unsigned __int8>(LZ4F_compressBound(count, m_prefs));
		
	// Pin both the input and output buffers in memory
	pin_ptr<unsigned __int8> pinin = &buffer[0];
	pin_ptr<unsigned __int8> pinout = &out[0];

	// Compress this block of data; lz4 may return zero if all the data was buffered
	LZ4F_compressOptions_t options ={ 0 /* stableSrc */, {0, 0, 0} /* reserved */};
	LZ4F_errorCode_t result = LZ4F_compressUpdate(*m_context, pinout, out->Length, &pinin[offset], count, &options);
	if(LZ4F_isError(result)) throw gcnew Lz4Exception(result);

	// If any data was written into the output buffer, write it to the underlying stream
	if(result > 0) m_stream->Write(out, 0, result);

	delete out;						// Destroy the local buffer
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
