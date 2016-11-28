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
// This program, "bzip2", the associated library "libbzip2", and all
// documentation, are copyright (C) 1996-2010 Julian R Seward.  All
// rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 
// 2. The origin of this software must not be misrepresented; you must 
//    not claim that you wrote the original software.  If you use this 
//    software in a product, an acknowledgment in the product 
//    documentation would be appreciated but is not required.
// 
// 3. Altered source versions must be plainly marked as such, and must
//    not be misrepresented as being the original software.
// 
// 4. The name of the author may not be used to endorse or promote 
//    products derived from this software without specific prior written 
//    permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// Julian Seward, jseward@bzip.org
// bzip2/libbzip2 version 1.0.6 of 6 September 2010
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

Bzip2Writer::Bzip2Writer(Stream^ stream) : 
	Bzip2Writer(stream, Bzip2CompressionLevel::Default, Bzip2WorkFactor::Default, DEFAULT_BUFFER_SIZE, false)
{
}

//---------------------------------------------------------------------------
// Bzip2Writer Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is written to
//	level		- Indicates whether to emphasize speed or compression efficiency

Bzip2Writer::Bzip2Writer(Stream^ stream, Compression::CompressionLevel level) : 
	Bzip2Writer(stream, Bzip2CompressionLevel(level), Bzip2WorkFactor::Default, DEFAULT_BUFFER_SIZE, false)
{
}

//---------------------------------------------------------------------------
// Bzip2Writer Constructor
//
// Arguments:
//
//	stream		- The stream the compressed data is written to
//	leaveopen	- Flag to leave the base stream open after disposal

Bzip2Writer::Bzip2Writer(Stream^ stream, bool leaveopen) : 
	Bzip2Writer(stream, Bzip2CompressionLevel::Default, Bzip2WorkFactor::Default, DEFAULT_BUFFER_SIZE, leaveopen)
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

Bzip2Writer::Bzip2Writer(Stream^ stream, Compression::CompressionLevel level, bool leaveopen) : 
	Bzip2Writer(stream, Bzip2CompressionLevel(level), Bzip2WorkFactor::Default, DEFAULT_BUFFER_SIZE, leaveopen)
{
}

//---------------------------------------------------------------------------
// Bzip2Writer Constructor (internal)
//
// Arguments:
//
//	stream			- The stream the compressed or decompressed data is written to
//	level			- Indicates the level of compression to use
//	workfactor		- Indicates the bzip2 work factor to use
//	buffersize		- Indicates the size of the compression buffer
//	leaveopen		- Flag to leave the base stream open after disposal

Bzip2Writer::Bzip2Writer(Stream^ stream, Bzip2CompressionLevel level, Bzip2WorkFactor workfactor, int buffersize, bool leaveopen) : 
	m_disposed(false), m_stream(stream), m_leaveopen(leaveopen), m_buffersize(buffersize)
{
	if(Object::ReferenceEquals(stream, nullptr)) throw gcnew ArgumentNullException("stream");
	if(buffersize <= 0) throw gcnew ArgumentOutOfRangeException("buffersize");

	// Allocate and initialize the unmanaged bz_stream structure
	try { m_bzstream = new bz_stream; memset(m_bzstream, 0, sizeof(bz_stream)); }
	catch(Exception^) { throw gcnew OutOfMemoryException(); }

	// Initialize the bz_stream for compression
	int result = BZ2_bzCompressInit(m_bzstream, level, 0, workfactor);
	if(result != BZ_OK) throw gcnew Bzip2Exception(result);
}

//---------------------------------------------------------------------------
// Bzip2Writer Destructor

Bzip2Writer::~Bzip2Writer()
{
	int result = BZ_OK;							// Result from bzip operation

	if(m_disposed) return;

	msclr::lock lock(m_lock);

	// Create and pin a local compression buffer
	array<unsigned __int8>^ out = gcnew array<unsigned __int8>(m_buffersize);
	pin_ptr<unsigned __int8> pinout = &out[0];

	// Input is not consumed when finishing the bzip stream
	m_bzstream->next_in = nullptr;
	m_bzstream->avail_in = 0;

	do {

		// Reset the output buffer to point into the managed array
		m_bzstream->next_out = reinterpret_cast<char*>(pinout);
		m_bzstream->avail_out = m_buffersize;

		// Finish the next block of data in the bzip buffers and write it
		result = BZ2_bzCompress(m_bzstream, BZ_FINISH);
		m_stream->Write(out, 0, m_buffersize - m_bzstream->avail_out);

	} while (result == BZ_FINISH_OK);

	delete out;								// Dispose of the local buffer
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

	// Create and pin a local compression buffer
	array<unsigned __int8>^ out = gcnew array<unsigned __int8>(m_buffersize);
	pin_ptr<unsigned __int8> pinout = &out[0];

	// Input is not consumed when flushing the bzip stream
	m_bzstream->next_in = nullptr;
	m_bzstream->avail_in = 0;

	do {

		// Reset the output buffer to point into the managed array
		m_bzstream->next_out = reinterpret_cast<char*>(pinout);
		m_bzstream->avail_out = m_buffersize;

		// Flush the next block of data in the bzip buffers and write it
		result = BZ2_bzCompress(m_bzstream, BZ_FLUSH);
		m_stream->Write(out, 0, m_buffersize - m_bzstream->avail_out);
	
	} while(result == BZ_FLUSH_OK);

	// The end state of a flush operation should be BZ_RUN_OK
	if(result != BZ_RUN_OK) throw gcnew Bzip2Exception(result);

	delete out;						// Dispose of the compression buffer
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

void Bzip2Writer::Write(array<unsigned __int8>^ buffer)
{
	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");

	CHECK_DISPOSED(m_disposed);
	Write(buffer, 0, buffer->Length);
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

	// Create a temporary local buffer to hold the compressed data
	array<unsigned __int8>^ out = gcnew array<unsigned __int8>(m_buffersize);
		
	// Pin both the input and output byte arrays in memory
	pin_ptr<unsigned __int8> pinin = &buffer[0];
	pin_ptr<unsigned __int8> pinout = &out[0];

	// Set up the input buffer pointer and available length
	m_bzstream->next_in = reinterpret_cast<char*>(&pinin[offset]);
	m_bzstream->avail_in = count;

	// Repeatedly compress blocks of data until all input has been consumed
	while(m_bzstream->avail_in > 0) {

		// Reset the output buffer pointer and length
		m_bzstream->next_out = reinterpret_cast<char*>(pinout);
		m_bzstream->avail_out = m_buffersize;

		// Compress the next block of input data into the output buffer
		int result = BZ2_bzCompress(m_bzstream, BZ_RUN);
		if(result != BZ_RUN_OK) throw gcnew Bzip2Exception(result);

		// Write the compressed data into the underlying base stream
		m_stream->Write(out, 0, m_buffersize - m_bzstream->avail_out);
	};

	delete out;
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
