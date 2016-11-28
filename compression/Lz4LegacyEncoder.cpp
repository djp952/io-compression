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
#include "Lz4LegacyEncoder.h"

#include "Lz4LegacyWriter.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Lz4LegacyEncoder Constructor
//
// Arguments:
//
//	NONE

Lz4LegacyEncoder::Lz4LegacyEncoder() : m_level(Lz4CompressionLevel::Default)
{
}

//---------------------------------------------------------------------------
// Lz4LegacyEncoder::CompressionLevel::get
//
// Gets the encoder compression level value

Lz4CompressionLevel Lz4LegacyEncoder::CompressionLevel::get(void)
{
	return m_level;
}

//---------------------------------------------------------------------------
// Lz4LegacyEncoder::CompressionLevel::set
//
// Sets the encoder compression level value

void Lz4LegacyEncoder::CompressionLevel::set(Lz4CompressionLevel value)
{
	m_level = value;
}

//---------------------------------------------------------------------------
// Lz4LegacyEncoder::Encode
//
// Compresses an input stream into an array of bytes
//
// Arguments:
//
//	instream		- Input stream to be compressed

array<unsigned __int8>^ Lz4LegacyEncoder::Encode(Stream^ instream)
{
	if(Object::ReferenceEquals(instream, nullptr)) throw gcnew ArgumentNullException("instream");

	msclr::auto_handle<MemoryStream> outstream(gcnew MemoryStream());
	Encode(instream, outstream.get());
	
	return outstream->ToArray();
}

//---------------------------------------------------------------------------
// Lz4LegacyEncoder::Encode
//
// Compresses an input array of bytes
//
// Arguments:
//
//	buffer			- Input buffer of data to be compressed

array<unsigned __int8>^ Lz4LegacyEncoder::Encode(array<unsigned __int8>^ buffer)
{
	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");

	return Encode(buffer, 0, buffer->Length);
}

//---------------------------------------------------------------------------
// Lz4LegacyEncoder::Encode
//
// Compresses an input array of bytes
//
// Arguments:
//
//	buffer			- Input buffer of data to be compressed
//	offset			- Offset within the buffer to begin reading
//	count			- Maximum number of bytes to read from buffer

array<unsigned __int8>^ Lz4LegacyEncoder::Encode(array<unsigned __int8>^ buffer, int offset, int count)
{
	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");

	msclr::auto_handle<MemoryStream> instream(gcnew MemoryStream(buffer, offset, count, false));
	msclr::auto_handle<MemoryStream> outstream(gcnew MemoryStream());

	Encode(instream.get(), outstream.get());

	return outstream->ToArray();
}
	
//---------------------------------------------------------------------------
// Lz4LegacyEncoder::Encode
//
// Compresses an input stream into an output stream
//
// Arguments:
//
//	instream		- Input stream to be compressed
//	outstream		- Output stream to received compressed data

void Lz4LegacyEncoder::Encode(Stream^ instream, Stream^ outstream)
{
	if(Object::ReferenceEquals(instream, nullptr)) throw gcnew ArgumentNullException("instream");
	if(Object::ReferenceEquals(outstream, nullptr)) throw gcnew ArgumentNullException("outstream");

	msclr::auto_handle<Lz4LegacyWriter> writer(gcnew Lz4LegacyWriter(outstream, m_level, true));
	instream->CopyTo(writer.get());
}

//---------------------------------------------------------------------------
// Lz4LegacyEncoder::Encode
//
// Compresses an input array of bytes into an output stream
//
// Arguments:
//
//	buffer		- Input byte array to be compressed
//	outstream	- Output stream to received compressed data

void Lz4LegacyEncoder::Encode(array<unsigned __int8>^ buffer, Stream^ outstream)
{
	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");
	if(Object::ReferenceEquals(outstream, nullptr)) throw gcnew ArgumentNullException("outstream");

	msclr::auto_handle<Lz4LegacyWriter> writer(gcnew Lz4LegacyWriter(outstream, m_level, true));
	writer->Write(buffer, 0, buffer->Length);
}

//---------------------------------------------------------------------------
// Lz4LegacyEncoder::Encode
//
// Compresses an input array of bytes into an output stream
//
// Arguments:
//
//	buffer		- Input byte array to be compressed
//	offset		- Offset within the provided buffer to begin reading
//	count		- Maximum number of bytes from the buffer to be read
//	outstream	- Output stream to received compressed data

void Lz4LegacyEncoder::Encode(array<unsigned __int8>^ buffer, int offset, int count, Stream^ outstream)
{
	if(Object::ReferenceEquals(buffer, nullptr)) throw gcnew ArgumentNullException("buffer");
	if(Object::ReferenceEquals(outstream, nullptr)) throw gcnew ArgumentNullException("outstream");

	msclr::auto_handle<Lz4LegacyWriter> writer(gcnew Lz4LegacyWriter(outstream, m_level, true));
	writer->Write(buffer, offset, count);
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
