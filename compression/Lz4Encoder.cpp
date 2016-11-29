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
#include "Lz4Encoder.h"

#include "Lz4Writer.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Lz4Encoder Constructor
//
// Arguments:
//
//	NONE

Lz4Encoder::Lz4Encoder() : m_autoflush(false), m_blockmode(Lz4BlockMode::Default), m_blocksize(Lz4BlockSize::Default), 
	m_level(Lz4CompressionLevel::Default), m_checksum(Lz4ContentChecksum::Default)
{
}

//---------------------------------------------------------------------------
// Lz4Encoder::AutoFlush::get
//
// Gets a flag indicating if the output stream should be auto flushed

bool Lz4Encoder::AutoFlush::get(void)
{
	return m_autoflush;
}

//---------------------------------------------------------------------------
// Lz4Encoder::AutoFlush::set
//
// Sets a flag indicating if the output stream should be auto flushed

void Lz4Encoder::AutoFlush::set(bool value)
{
	m_autoflush = value;
}

//---------------------------------------------------------------------------
// Lz4Encoder::BlockMode::get
//
// Gets the encoder block mode

Lz4BlockMode Lz4Encoder::BlockMode::get(void)
{
	return m_blockmode;
}

//---------------------------------------------------------------------------
// Lz4Encoder::BlockMode::set
//
// Sets the encoder block mode

void Lz4Encoder::BlockMode::set(Lz4BlockMode value)
{
	m_blockmode = value;
}

//---------------------------------------------------------------------------
// Lz4Encoder::BlockSize::get
//
// Gets the encoder block size

Lz4BlockSize Lz4Encoder::BlockSize::get(void)
{
	return m_blocksize;
}

//---------------------------------------------------------------------------
// Lz4Encoder::BlockSize::set
//
// Sets the encoder block size

void Lz4Encoder::BlockSize::set(Lz4BlockSize value)
{
	m_blocksize = value;
}

//---------------------------------------------------------------------------
// Lz4Encoder::CompressionLevel::get
//
// Gets the encoder compression level value

Lz4CompressionLevel Lz4Encoder::CompressionLevel::get(void)
{
	return m_level;
}

//---------------------------------------------------------------------------
// Lz4Encoder::CompressionLevel::set
//
// Sets the encoder compression level value

void Lz4Encoder::CompressionLevel::set(Lz4CompressionLevel value)
{
	m_level = value;
}

//---------------------------------------------------------------------------
// Lz4Encoder::ContentChecksum::get
//
// Gets the encoder content checksum mode

Lz4ContentChecksum Lz4Encoder::ContentChecksum::get(void)
{
	return m_checksum;
}

//---------------------------------------------------------------------------
// Lz4Encoder::ContentChecksum::set
//
// Sets the encoder content checksum mode

void Lz4Encoder::ContentChecksum::set(Lz4ContentChecksum value)
{
	m_checksum = value;
}

//---------------------------------------------------------------------------
// Lz4Encoder::Encode
//
// Compresses an input stream into an array of bytes
//
// Arguments:
//
//	instream		- Input stream to be compressed

array<unsigned __int8>^ Lz4Encoder::Encode(Stream^ instream)
{
	if(Object::ReferenceEquals(instream, nullptr)) throw gcnew ArgumentNullException("instream");

	msclr::auto_handle<MemoryStream> outstream(gcnew MemoryStream());
	Encode(instream, outstream.get());
	
	return outstream->ToArray();
}

//---------------------------------------------------------------------------
// Lz4Encoder::Encode
//
// Compresses an input array of bytes
//
// Arguments:
//
//	buffer			- Input buffer of data to be compressed

array<unsigned __int8>^ Lz4Encoder::Encode(array<unsigned __int8>^ buffer)
{
	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");

	return Encode(buffer, 0, buffer->Length);
}

//---------------------------------------------------------------------------
// Lz4Encoder::Encode
//
// Compresses an input array of bytes
//
// Arguments:
//
//	buffer			- Input buffer of data to be compressed
//	offset			- Offset within the buffer to begin reading
//	count			- Maximum number of bytes to read from buffer

array<unsigned __int8>^ Lz4Encoder::Encode(array<unsigned __int8>^ buffer, int offset, int count)
{
	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");

	msclr::auto_handle<MemoryStream> instream(gcnew MemoryStream(buffer, offset, count, false));
	msclr::auto_handle<MemoryStream> outstream(gcnew MemoryStream());

	// When the input is a byte array, the length is known and can be encoded into the stream
	msclr::auto_handle<Lz4Writer> writer(gcnew Lz4Writer(outstream.get(), m_level, m_autoflush, m_blocksize, m_blockmode, m_checksum, count, true));
	instream->CopyTo(writer.get());

	return outstream->ToArray();
}
	
//---------------------------------------------------------------------------
// Lz4Encoder::Encode
//
// Compresses an input stream into an output stream
//
// Arguments:
//
//	instream		- Input stream to be compressed
//	outstream		- Output stream to received compressed data

void Lz4Encoder::Encode(Stream^ instream, Stream^ outstream)
{
	if(Object::ReferenceEquals(instream, nullptr)) throw gcnew ArgumentNullException("instream");
	if(Object::ReferenceEquals(outstream, nullptr)) throw gcnew ArgumentNullException("outstream");

	// When the input is a stream, the overall length is not immutable, pass a zero to the Lz4Writer for length
	msclr::auto_handle<Lz4Writer> writer(gcnew Lz4Writer(outstream, m_level, m_autoflush, m_blocksize, m_blockmode, m_checksum, 0, true));
	instream->CopyTo(writer.get());
}

//---------------------------------------------------------------------------
// Lz4Encoder::Encode
//
// Compresses an input array of bytes into an output stream
//
// Arguments:
//
//	buffer		- Input byte array to be compressed
//	outstream	- Output stream to received compressed data

void Lz4Encoder::Encode(array<unsigned __int8>^ buffer, Stream^ outstream)
{
	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");
	if(Object::ReferenceEquals(outstream, nullptr)) throw gcnew ArgumentNullException("outstream");

	// When the input is a byte array, the length is known and can be encoded into the stream
	msclr::auto_handle<Lz4Writer> writer(gcnew Lz4Writer(outstream, m_level, m_autoflush, m_blocksize, m_blockmode, m_checksum, buffer->Length, true));
	writer->Write(buffer, 0, buffer->Length);
}

//---------------------------------------------------------------------------
// Lz4Encoder::Encode
//
// Compresses an input array of bytes into an output stream
//
// Arguments:
//
//	buffer		- Input byte array to be compressed
//	offset		- Offset within the provided buffer to begin reading
//	count		- Maximum number of bytes from the buffer to be read
//	outstream	- Output stream to received compressed data

void Lz4Encoder::Encode(array<unsigned __int8>^ buffer, int offset, int count, Stream^ outstream)
{
	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");
	if(Object::ReferenceEquals(outstream, nullptr)) throw gcnew ArgumentNullException("outstream");

	// When the input is a byte array, the length is known and can be encoded into the stream
	msclr::auto_handle<Lz4Writer> writer(gcnew Lz4Writer(outstream, m_level, m_autoflush, m_blocksize, m_blockmode, m_checksum, count, true));
	writer->Write(buffer, offset, count);
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
