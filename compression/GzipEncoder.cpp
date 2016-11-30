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
#include "GzipEncoder.h"

#include "GzipWriter.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// GzipEncoder Constructor
//
// Arguments:
//
//	NONE

GzipEncoder::GzipEncoder() : m_buffersize(GzipWriter::DEFAULT_BUFFER_SIZE), m_level(GzipCompressionLevel::Default),
	m_strategy(GzipCompressionStrategy::Default), m_maxmem(GzipMemoryUsageLevel::Default)
{
}

//---------------------------------------------------------------------------
// GzipEncoder::BufferSize::get
//
// Gets the size of the compression buffer to use

int GzipEncoder::BufferSize::get(void)
{
	return m_buffersize;
}

//---------------------------------------------------------------------------
// GzipEncoder::BufferSize::set
//
// Sets the size of the compression buffer to use

void GzipEncoder::BufferSize::set(int value)
{
	if(value < 0) throw gcnew ArgumentOutOfRangeException("value");
	m_buffersize = value;
}

//---------------------------------------------------------------------------
// GzipEncoder::CompressionLevel::get
//
// Gets the encoder compression level value

GzipCompressionLevel GzipEncoder::CompressionLevel::get(void)
{
	return m_level;
}

//---------------------------------------------------------------------------
// GzipEncoder::CompressionLevel::set
//
// Sets the encoder compression level value

void GzipEncoder::CompressionLevel::set(GzipCompressionLevel value)
{
	m_level = value;
}

//---------------------------------------------------------------------------
// GzipEncoder::CompressionStrategy::get
//
// Gets the encoder compression strategy

GzipCompressionStrategy GzipEncoder::CompressionStrategy::get(void)
{
	return m_strategy;
}

//---------------------------------------------------------------------------
// GzipEncoder::CompressionStrategy::set
//
// Sets the encoder compression strategy

void GzipEncoder::CompressionStrategy::set(GzipCompressionStrategy value)
{
	m_strategy = value;
}

//---------------------------------------------------------------------------
// GzipEncoder::Encode
//
// Compresses an input stream into an array of bytes
//
// Arguments:
//
//	instream		- Input stream to be compressed

array<unsigned __int8>^ GzipEncoder::Encode(Stream^ instream)
{
	if(Object::ReferenceEquals(instream, nullptr)) throw gcnew ArgumentNullException("instream");

	msclr::auto_handle<MemoryStream> outstream(gcnew MemoryStream());
	Encode(instream, outstream.get());
	
	return outstream->ToArray();
}

//---------------------------------------------------------------------------
// GzipEncoder::Encode
//
// Compresses an input array of bytes
//
// Arguments:
//
//	buffer			- Input buffer of data to be compressed

array<unsigned __int8>^ GzipEncoder::Encode(array<unsigned __int8>^ buffer)
{
	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");

	return Encode(buffer, 0, buffer->Length);
}

//---------------------------------------------------------------------------
// GzipEncoder::Encode
//
// Compresses an input array of bytes
//
// Arguments:
//
//	buffer			- Input buffer of data to be compressed
//	offset			- Offset within the buffer to begin reading
//	count			- Maximum number of bytes to read from buffer

array<unsigned __int8>^ GzipEncoder::Encode(array<unsigned __int8>^ buffer, int offset, int count)
{
	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");

	msclr::auto_handle<MemoryStream> instream(gcnew MemoryStream(buffer, offset, count, false));
	msclr::auto_handle<MemoryStream> outstream(gcnew MemoryStream());

	Encode(instream.get(), outstream.get());

	return outstream->ToArray();
}
	
//---------------------------------------------------------------------------
// GzipEncoder::Encode
//
// Compresses an input stream into an output stream
//
// Arguments:
//
//	instream		- Input stream to be compressed
//	outstream		- Output stream to received compressed data

void GzipEncoder::Encode(Stream^ instream, Stream^ outstream)
{
	if(Object::ReferenceEquals(instream, nullptr)) throw gcnew ArgumentNullException("instream");
	if(Object::ReferenceEquals(outstream, nullptr)) throw gcnew ArgumentNullException("outstream");

	msclr::auto_handle<GzipWriter> writer(gcnew GzipWriter(outstream, m_level, m_strategy, m_maxmem, m_buffersize, true));
	instream->CopyTo(writer.get());
}

//---------------------------------------------------------------------------
// GzipEncoder::Encode
//
// Compresses an input array of bytes into an output stream
//
// Arguments:
//
//	buffer		- Input byte array to be compressed
//	outstream	- Output stream to received compressed data

void GzipEncoder::Encode(array<unsigned __int8>^ buffer, Stream^ outstream)
{
	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");
	if(Object::ReferenceEquals(outstream, nullptr)) throw gcnew ArgumentNullException("outstream");

	msclr::auto_handle<GzipWriter> writer(gcnew GzipWriter(outstream, m_level, m_strategy, m_maxmem, m_buffersize, true));
	writer->Write(buffer, 0, buffer->Length);
}

//---------------------------------------------------------------------------
// GzipEncoder::Encode
//
// Compresses an input array of bytes into an output stream
//
// Arguments:
//
//	buffer		- Input byte array to be compressed
//	offset		- Offset within the provided buffer to begin reading
//	count		- Maximum number of bytes from the buffer to be read
//	outstream	- Output stream to received compressed data

void GzipEncoder::Encode(array<unsigned __int8>^ buffer, int offset, int count, Stream^ outstream)
{
	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");
	if(Object::ReferenceEquals(outstream, nullptr)) throw gcnew ArgumentNullException("outstream");

	msclr::auto_handle<GzipWriter> writer(gcnew GzipWriter(outstream, m_level, m_strategy, m_maxmem, m_buffersize, true));
	writer->Write(buffer, offset, count);
}

//---------------------------------------------------------------------------
// GzipEncoder::MemoryUsage::get
//
// Gets the encoder compression strategy

GzipMemoryUsageLevel GzipEncoder::MemoryUsage::get(void)
{
	return m_maxmem;
}

//---------------------------------------------------------------------------
// GzipEncoder::MemoryUsage::set
//
// Sets the encoder compression strategy

void GzipEncoder::MemoryUsage::set(GzipMemoryUsageLevel value)
{
	m_maxmem = value;
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
