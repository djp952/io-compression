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
#include "LzmaEncoder.h"

#include <Alloc.h>
#include "LzmaException.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// LzmaEncoder Constructor
//
// Arguments:
//
//	NONE

LzmaEncoder::LzmaEncoder() : m_level(Compression::CompressionLevel::Optimal), m_writeendmark(true)
{
}

//---------------------------------------------------------------------------
// LzmaEncoder::CompressionLevel::get
//
// Gets the encoder compression level value

Compression::CompressionLevel LzmaEncoder::CompressionLevel::get(void)
{
	return m_level;
}

//---------------------------------------------------------------------------
// LzmaEncoder::CompressionLevel::set
//
// Sets the encoder compression level value

void LzmaEncoder::CompressionLevel::set(Compression::CompressionLevel value)
{
	m_level = value;
}

//---------------------------------------------------------------------------
// LzmaEncoder::Encode
//
// Compresses an input stream into an array of bytes
//
// Arguments:
//
//	instream		- Input stream to be compressed

array<unsigned __int8>^ LzmaEncoder::Encode(Stream^ instream)
{
	if(Object::ReferenceEquals(instream, nullptr)) throw gcnew ArgumentNullException("instream");

	msclr::auto_handle<MemoryStream> outstream(gcnew MemoryStream());
	Encode(instream, outstream.get());

	return outstream->ToArray();
}

//---------------------------------------------------------------------------
// LzmaEncoder::Encode
//
// Compresses an input array of bytes
//
// Arguments:
//
//	buffer			- Input buffer of data to be compressed

array<unsigned __int8>^ LzmaEncoder::Encode(array<unsigned __int8>^ buffer)
{
	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");

	return Encode(buffer, 0, buffer->Length);
}

//---------------------------------------------------------------------------
// LzmaEncoder::Encode
//
// Compresses an input array of bytes
//
// Arguments:
//
//	buffer			- Input buffer of data to be compressed
//	offset			- Offset within the buffer to begin reading
//	count			- Maximum number of bytes to read from buffer

array<unsigned __int8>^ LzmaEncoder::Encode(array<unsigned __int8>^ buffer, int offset, int count)
{
	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");

	msclr::auto_handle<MemoryStream> instream(gcnew MemoryStream(buffer, offset, count, false));
	msclr::auto_handle<MemoryStream> outstream(gcnew MemoryStream());

	Encode(instream.get(), outstream.get());

	return outstream->ToArray();
}
	
//---------------------------------------------------------------------------
// LzmaEncoder::Encode
//
// Compresses an input stream into an output stream
//
// Arguments:
//
//	instream		- Input stream to be compressed
//	outstream		- Output stream to received compressed data

void LzmaEncoder::Encode(Stream^ instream, Stream^ outstream)
{
	CLzmaEncProps				props;			// Encoder properties
	SRes						result;			// LZMA function call result
	unsigned __int64			insize;			// Input stream length

	if(Object::ReferenceEquals(instream, nullptr)) throw gcnew ArgumentNullException("instream");
	if(Object::ReferenceEquals(outstream, nullptr)) throw gcnew ArgumentNullException("outstream");

	// Get the length of the input data, set to 0xFFFFFFFF'FFFFFFFF if unknown
	try { insize = (instream->Length - instream->Position); }
	catch(Exception^) { insize = System::UInt64::MaxValue; }

	// Initialize the encoder properties
	LzmaEncProps_Init(&props);

	// Set the compression level property
	if(m_level == Compression::CompressionLevel::NoCompression) props.level = 0;
	else if(m_level == Compression::CompressionLevel::Fastest) props.level = 1;
	else props.level = 9;

	// Set the write end mark property
	props.writeEndMark = (m_writeendmark) ? 1 : 0;

	// Normalize the properties to ensure nothing is out of range
	LzmaEncProps_Normalize(&props);

	// Create the LZMA encoder instance
	CLzmaEncHandle handle = LzmaEnc_Create(&g_Alloc);
	if(handle == nullptr) throw gcnew OutOfMemoryException();

	try {

		// Apply the encoder properties to the encoder
		result = LzmaEnc_SetProps(handle, &props);
		if(result != SZ_OK) throw gcnew LzmaException(result);

		// Construct a managed byte array to hold the stream properties
		array<unsigned __int8>^ propbits = gcnew array<unsigned __int8>(LZMA_PROPS_SIZE);
		pin_ptr<unsigned __int8> pinpropbits = &propbits[0];

		// Convert the properties into a byte array for the output stream
		size_t outsize = propbits->Length;
		result = LzmaEnc_WriteProperties(handle, pinpropbits, &outsize);
		if(result != SZ_OK) throw gcnew LzmaException(result);

		// Write the properties into the output stream
		outstream->Write(propbits, 0, propbits->Length);

		// Write the length of the input data into the output stream
		array<unsigned __int8>^ sizebits = BitConverter::GetBytes(insize);
		outstream->Write(sizebits, 0, sizebits->Length);

		// Create a ReaderWriter instance around the input and output streams
		msclr::auto_handle<ReaderWriter> readerwriter(gcnew ReaderWriter(instream, outstream));

		// Run the encoding operation
		result = LzmaEnc_Encode(handle, readerwriter.get(), readerwriter.get(), nullptr, &g_Alloc, &g_BigAlloc);
		if(result != SZ_OK) throw gcnew LzmaException(result);
	}

	finally { LzmaEnc_Destroy(handle, &g_Alloc, &g_BigAlloc); }
}

//---------------------------------------------------------------------------
// LzmaEncoder::Encode
//
// Compresses an input array of bytes into an output stream
//
// Arguments:
//
//	buffer		- Input byte array to be compressed
//	outstream	- Output stream to received compressed data

void LzmaEncoder::Encode(array<unsigned __int8>^ buffer, Stream^ outstream)
{
	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");
	if(Object::ReferenceEquals(outstream, nullptr)) throw gcnew ArgumentNullException("outstream");

	msclr::auto_handle<MemoryStream> instream(gcnew MemoryStream(buffer, false));
	Encode(instream.get(), outstream);
}

//---------------------------------------------------------------------------
// LzmaEncoder::Encode
//
// Compresses an input array of bytes into an output stream
//
// Arguments:
//
//	buffer		- Input byte array to be compressed
//	offset		- Offset within the provided buffer to begin reading
//	count		- Maximum number of bytes from the buffer to be read
//	outstream	- Output stream to received compressed data

void LzmaEncoder::Encode(array<unsigned __int8>^ buffer, int offset, int count, Stream^ outstream)
{
	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");
	if(Object::ReferenceEquals(outstream, nullptr)) throw gcnew ArgumentNullException("outstream");

	msclr::auto_handle<MemoryStream> instream(gcnew MemoryStream(buffer, offset, count, false));
	Encode(instream.get(), outstream);
}

//---------------------------------------------------------------------------
// LzmaEncoder::WriteEndMark::get
//
// Gets the flag to write the end of stream marker

bool LzmaEncoder::WriteEndMark::get(void)
{
	return m_writeendmark;
}

//---------------------------------------------------------------------------
// LzmaEncoder::WriteEndMark::set
//
// Sets the flag to write the end of stream marker

void LzmaEncoder::WriteEndMark::set(bool value)
{
	m_writeendmark = value;
}
	
//
// LZMAENCODER::READERWRITER IMPLEMENTATION
//
	
//---------------------------------------------------------------------------
// LzmaEncoder::ReaderWriter Constructor
//
// Arguments:
//
//	instream	- Input stream instance
//	outstream	- Output stream instance

LzmaEncoder::ReaderWriter::ReaderWriter(Stream^ instream, Stream^ outstream) : m_disposed(false), m_instream(instream), m_outstream(outstream),
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
// LzmaEncoder::ReaderWriter ISeqInStream* conversion operator

LzmaEncoder::ReaderWriter::operator ISeqInStream*(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_seqin;
}

//---------------------------------------------------------------------------
// LzmaEncoder::ReaderWriter ISeqOutStream* conversion operator

LzmaEncoder::ReaderWriter::operator ISeqOutStream*(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_seqout;
}

//---------------------------------------------------------------------------
// LzmaEncoder::ReaderWriter Destructor

LzmaEncoder::ReaderWriter::~ReaderWriter()
{
	if(m_disposed) return;

	this->!ReaderWriter();
	m_disposed = true;
}

//---------------------------------------------------------------------------
// LzmaEncoder::ReaderWriter Finalizer

LzmaEncoder::ReaderWriter::!ReaderWriter()
{
	// Release the unmanaged ISeqInStream and ISeqOutStream structures
	if(m_seqin) { delete m_seqin; m_seqin = nullptr; }
	if(m_seqout) { delete m_seqout; m_seqout = nullptr; }
}

//---------------------------------------------------------------------------
// LzmaEncoder::ReaderWriter::OnRead
//
// Implements ISeqInStream::Read
//
// Arguments:
//
//	context		- Context pointer (unused)
//	buffer		- Buffer to write the input data into
//	size		- Size of the buffer / number of bytes written

SRes LzmaEncoder::ReaderWriter::OnRead(void* context, void* buffer, size_t* size)
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
// LzmaEncoder::ReaderWriter::OnWrite
//
// Implements ISeqOutStream::Write
//
// Arguments:
//
//	context		- Context pointer (unused)
//	buffer		- Buffer to read the input data from
//	size		- Size of the input data buffer

size_t LzmaEncoder::ReaderWriter::OnWrite(void* context, void const* buffer, size_t size)
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
