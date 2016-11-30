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
#include "XzEncoder.h"

#include "LzmaException.h"

// crcinit
//
// Helper function defined in crcinit.cpp; thunks to CrcGenerateTable
extern void crcinit(void);

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// XzEncoder Static Constructor (private)

static XzEncoder::XzEncoder()
{
	crcinit();				// Initialize the CRC table
}

//---------------------------------------------------------------------------
// XzEncoder Constructor
//
// Arguments:
//
//	NONE

XzEncoder::XzEncoder() : m_blocksize(Lzma2BlockSize::Disabled), m_blockthreads(Lzma2ThreadsPerBlock::Default), m_totalthreads(Lzma2MaximumThreads::Default)
{
}

//---------------------------------------------------------------------------
// XzEncoder::BlockSize::get
//
// Indicates the LZMA2 block size

Lzma2BlockSize XzEncoder::BlockSize::get(void)
{
	return m_blocksize;
}

//---------------------------------------------------------------------------
// XzEncoder::BlockSize::set
//
// Indicates the LZMA2 block size

void XzEncoder::BlockSize::set(Lzma2BlockSize value)
{
	m_blocksize = value;
}

//---------------------------------------------------------------------------
// XzEncoder::Encode
//
// Compresses an input stream into an array of bytes
//
// Arguments:
//
//	instream		- Input stream to be compressed

array<unsigned __int8>^ XzEncoder::Encode(Stream^ instream)
{
	if(Object::ReferenceEquals(instream, nullptr)) throw gcnew ArgumentNullException("instream");

	msclr::auto_handle<MemoryStream> outstream(gcnew MemoryStream());
	Encode(instream, System::UInt64::MaxValue, outstream.get());

	return outstream->ToArray();
}

//---------------------------------------------------------------------------
// XzEncoder::Encode
//
// Compresses an input array of bytes
//
// Arguments:
//
//	buffer			- Input buffer of data to be compressed

array<unsigned __int8>^ XzEncoder::Encode(array<unsigned __int8>^ buffer)
{
	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");

	return Encode(buffer, 0, buffer->Length);
}

//---------------------------------------------------------------------------
// XzEncoder::Encode
//
// Compresses an input array of bytes
//
// Arguments:
//
//	buffer			- Input buffer of data to be compressed
//	offset			- Offset within the buffer to begin reading
//	count			- Maximum number of bytes to read from buffer

array<unsigned __int8>^ XzEncoder::Encode(array<unsigned __int8>^ buffer, int offset, int count)
{
	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");

	msclr::auto_handle<MemoryStream> instream(gcnew MemoryStream(buffer, offset, count, false));
	msclr::auto_handle<MemoryStream> outstream(gcnew MemoryStream());

	Encode(instream.get(), count, outstream.get());

	return outstream->ToArray();
}
	
//---------------------------------------------------------------------------
// XzEncoder::Encode
//
// Compresses an input stream into an output stream
//
// Arguments:
//
//	instream		- Input stream to be compressed
//	outstream		- Output stream to received compressed data

void XzEncoder::Encode(Stream^ instream, Stream^ outstream)
{
	if(Object::ReferenceEquals(instream, nullptr)) throw gcnew ArgumentNullException("instream");
	if(Object::ReferenceEquals(instream, nullptr)) throw gcnew ArgumentNullException("outstream");

	// The length of the input stream is not necessarily fixed, do not specify a length
	Encode(instream, System::UInt64::MaxValue, outstream);
}

//---------------------------------------------------------------------------
// XzEncoder::Encode
//
// Compresses an input stream into an output stream
//
// Arguments:
//
//	instream		- Input stream to be compressed
//	insize			- Input stream length, if known and constant
//	outstream		- Output stream to received compressed data

void XzEncoder::Encode(Stream^ instream, unsigned __int64 insize, Stream^ outstream)
{
	CLzma2EncProps				lzma2props;		// LZMA2 encoder properties
	CXzProps					xzprops;		// Encoder properties
	SRes						result;			// LZMA function call result

	if(Object::ReferenceEquals(instream, nullptr)) throw gcnew ArgumentNullException("instream");
	if(Object::ReferenceEquals(outstream, nullptr)) throw gcnew ArgumentNullException("outstream");

	// Initialize the LZMA2 encoder properties
	Lzma2EncProps_Init(&lzma2props);
	
	// Set the LZMA encoder properties for this instance
	lzma2props.lzmaProps.level = m_level;
	lzma2props.lzmaProps.dictSize = m_dictsize;
	lzma2props.lzmaProps.reduceSize = insize;
	lzma2props.lzmaProps.lc = m_litcontextbits;
	lzma2props.lzmaProps.lp = m_litposbits;
	lzma2props.lzmaProps.pb = m_posbits;
	lzma2props.lzmaProps.algo = static_cast<int>(m_compmode);
	lzma2props.lzmaProps.fb = m_fastbytes;
	lzma2props.lzmaProps.btMode = static_cast<int>(m_matchfindmode);
	lzma2props.lzmaProps.numHashBytes = m_hashbytes;
	lzma2props.lzmaProps.mc = m_matchfindpasses;
	lzma2props.lzmaProps.writeEndMark = (m_writeendmark) ? 1 : 0;
	lzma2props.lzmaProps.numThreads = (m_multithreaded) ? 2 : 1;

	// Set the LZMA2 encoder properties for this instance
	lzma2props.blockSize = static_cast<size_t>(static_cast<__int64>(m_blocksize));
	lzma2props.numBlockThreads = m_blockthreads;
	lzma2props.numTotalThreads = m_totalthreads;
  
	// Normalize the LZMA2 encoder properties
	Lzma2EncProps_Normalize(&lzma2props);

	// Initialize the XZ encoder properties
	XzProps_Init(&xzprops);
	xzprops.lzma2Props = &lzma2props;
	xzprops.checkId = XZ_CHECK_CRC64;

	// Create a ReaderWriter instance around the input and output streams
	msclr::auto_handle<ReaderWriter> readerwriter(gcnew ReaderWriter(instream, outstream));

	// Run the encoding operation
	result = Xz_Encode(readerwriter.get(), readerwriter.get(), &xzprops, nullptr);
	if(result != SZ_OK) throw gcnew LzmaException(result);
}

//---------------------------------------------------------------------------
// XzEncoder::Encode
//
// Compresses an input array of bytes into an output stream
//
// Arguments:
//
//	buffer		- Input byte array to be compressed
//	outstream	- Output stream to received compressed data

void XzEncoder::Encode(array<unsigned __int8>^ buffer, Stream^ outstream)
{
	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");
	if(Object::ReferenceEquals(outstream, nullptr)) throw gcnew ArgumentNullException("outstream");

	msclr::auto_handle<MemoryStream> instream(gcnew MemoryStream(buffer, false));
	Encode(instream.get(), buffer->Length, outstream);
}

//---------------------------------------------------------------------------
// XzEncoder::Encode
//
// Compresses an input array of bytes into an output stream
//
// Arguments:
//
//	buffer		- Input byte array to be compressed
//	offset		- Offset within the provided buffer to begin reading
//	count		- Maximum number of bytes from the buffer to be read
//	outstream	- Output stream to received compressed data

void XzEncoder::Encode(array<unsigned __int8>^ buffer, int offset, int count, Stream^ outstream)
{
	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");
	if(Object::ReferenceEquals(outstream, nullptr)) throw gcnew ArgumentNullException("outstream");

	msclr::auto_handle<MemoryStream> instream(gcnew MemoryStream(buffer, offset, count, false));
	Encode(instream.get(), count, outstream);
}

//---------------------------------------------------------------------------
// XzEncoder::MaximumThreads::get
//
// Indicates the maximum number of LZMA2 threads

Lzma2MaximumThreads XzEncoder::MaximumThreads::get(void)
{
	return m_totalthreads;
}

//---------------------------------------------------------------------------
// XzEncoder::MaximumThreads::set
//
// Indicates the maximum number of LZMA2 threads

void XzEncoder::MaximumThreads::set(Lzma2MaximumThreads value)
{
	m_totalthreads = value;
}

//---------------------------------------------------------------------------
// XzEncoder::ThreadsPerBlock::get
//
// Indicates the LZMA2 threads per block

Lzma2ThreadsPerBlock XzEncoder::ThreadsPerBlock::get(void)
{
	return m_blockthreads;
}

//---------------------------------------------------------------------------
// XzEncoder::ThreadsPerBlock::set
//
// Indicates the LZMA2 threads per block

void XzEncoder::ThreadsPerBlock::set(Lzma2ThreadsPerBlock value)
{
	m_blockthreads = value;
}

//
// XZENCODER::READERWRITER IMPLEMENTATION
//
	
//---------------------------------------------------------------------------
// XzEncoder::ReaderWriter Constructor
//
// Arguments:
//
//	instream	- Input stream instance
//	outstream	- Output stream instance

XzEncoder::ReaderWriter::ReaderWriter(Stream^ instream, Stream^ outstream) : m_disposed(false), m_instream(instream), m_outstream(outstream),
	m_onread(gcnew OnReadDelegate(this, &ReaderWriter::OnRead)), m_onwrite(gcnew OnWriteDelegate(this, &ReaderWriter::OnWrite))
{
	if(Object::ReferenceEquals(instream, nullptr)) throw gcnew ArgumentNullException("instream");
	if(Object::ReferenceEquals(outstream, nullptr)) throw gcnew ArgumentNullException("outstream");

	// Allocate and initialize the ISeqInStream interface
	try { m_seqin = new ISeqInStream{ static_cast<OnReadPointer>(Marshal::GetFunctionPointerForDelegate(m_onread).ToPointer()) }; }
	catch(Exception^) { throw gcnew OutOfMemoryException(); }

	// Allocate and initialize the ISeqInStream interface
	try { m_seqout = new ISeqOutStream{ static_cast<OnWritePointer>(Marshal::GetFunctionPointerForDelegate(m_onwrite).ToPointer()) }; }
	catch(Exception^) { throw gcnew OutOfMemoryException(); }
}

//---------------------------------------------------------------------------
// XzEncoder::ReaderWriter ISeqInStream* conversion operator

XzEncoder::ReaderWriter::operator ISeqInStream*(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_seqin;
}

//---------------------------------------------------------------------------
// XzEncoder::ReaderWriter ISeqOutStream* conversion operator

XzEncoder::ReaderWriter::operator ISeqOutStream*(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_seqout;
}

//---------------------------------------------------------------------------
// XzEncoder::ReaderWriter Destructor

XzEncoder::ReaderWriter::~ReaderWriter()
{
	if(m_disposed) return;

	this->!ReaderWriter();
	m_disposed = true;
}

//---------------------------------------------------------------------------
// XzEncoder::ReaderWriter Finalizer

XzEncoder::ReaderWriter::!ReaderWriter()
{
	// Release the unmanaged ISeqInStream and ISeqOutStream structures
	if(m_seqin) { delete m_seqin; m_seqin = nullptr; }
	if(m_seqout) { delete m_seqout; m_seqout = nullptr; }
}

//---------------------------------------------------------------------------
// XzEncoder::ReaderWriter::OnRead
//
// Implements ISeqInStream::Read
//
// Arguments:
//
//	context		- Context pointer (unused)
//	buffer		- Buffer to write the input data into
//	size		- Size of the buffer / number of bytes written

SRes XzEncoder::ReaderWriter::OnRead(void* context, void* buffer, size_t* size)
{
	UNREFERENCED_PARAMETER(context);

	CHECK_DISPOSED(m_disposed);

	if(*size == 0) return SZ_OK;
	if(*size > System::Int32::MaxValue) return SZ_ERROR_PARAM;
	if(buffer == nullptr) return SZ_ERROR_PARAM;

	// Create an intermediate buffer in which to read the data from the managed stream
	array<unsigned __int8>^ intermediate = gcnew array<unsigned __int8>(static_cast<int>(*size));
	pin_ptr<unsigned __int8> pinintermediate = &intermediate[0];

	// Read the data from the managed stream and copy it into the output buffer
	*size = m_instream->Read(intermediate, 0, static_cast<int>(*size));
	if(*size) memcpy(buffer, pinintermediate, *size);

	return SZ_OK;
}

//---------------------------------------------------------------------------
// XzEncoder::ReaderWriter::OnWrite
//
// Implements ISeqOutStream::Write
//
// Arguments:
//
//	context		- Context pointer (unused)
//	buffer		- Buffer to read the input data from
//	size		- Size of the input data buffer

size_t XzEncoder::ReaderWriter::OnWrite(void* context, void const* buffer, size_t size)
{
	UNREFERENCED_PARAMETER(context);

	CHECK_DISPOSED(m_disposed);

	if(size == 0) return 0;
	if(buffer == nullptr) return SZ_ERROR_PARAM;

	// Use an UnmanagedMemoryStream wrapper around the input data and copy it into the output stream
	msclr::auto_handle<UnmanagedMemoryStream> instream(gcnew UnmanagedMemoryStream(const_cast<unsigned __int8*>(reinterpret_cast<const unsigned __int8*>(buffer)), size));
	instream->CopyTo(m_outstream);

	return size;
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
