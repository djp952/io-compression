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

#ifndef __XZENCODER_H_
#define __XZENCODER_H_
#pragma once

#include <XzEnc.h>
#include "Encoder.h"
#include "LzmaEncoder.h"
#include "Lzma2BlockSize.h"
#include "Lzma2MaximumThreads.h"
#include "Lzma2ThreadsPerBlock.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::IO;
using namespace System::Runtime::InteropServices;

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Class XzEncoder
//
// XZ compression encoder
//---------------------------------------------------------------------------

public ref class XzEncoder : public LzmaEncoder
{
public:

	// Instance Constructor
	//
	XzEncoder();

	//-----------------------------------------------------------------------
	// Member Functions

	// Encode (Encoder)
	//
	// Compresses an input stream into an array of bytes
	virtual array<unsigned __int8>^ Encode(Stream^ instream) override;

	// Encode (Encoder)
	//
	// Compresses an input array of bytes
	virtual array<unsigned __int8>^ Encode(array<unsigned __int8>^ buffer) override;

	// Encode (Encoder)
	//
	// Compresses an input array of bytes
	virtual array<unsigned __int8>^ Encode(array<unsigned __int8>^ buffer, int offset, int count) override;

	// Encode (Encoder)
	//
	// Compresses an input stream into an output stream
	virtual void Encode(Stream^ instream, Stream^ outstream) override;

	// Encode (Encoder)
	//
	// Compresses an input array of bytes into an output stream
	virtual void Encode(array<unsigned __int8>^ buffer, Stream^ outstream) override;

	// Encode (Encoder)
	//
	// Compresses an input array of bytes into an output stream
	virtual void Encode(array<unsigned __int8>^ buffer, int offset, int count, Stream^ outstream) override;

	//-----------------------------------------------------------------------
	// Properties

	// BlockSize
	//
	// Indicates the LZMA2 block size
	property Lzma2BlockSize BlockSize
	{
		Lzma2BlockSize get(void);
		void set(Lzma2BlockSize value);
	}

	// MaximumThreads
	//
	// Indicates the maximum number of LZMA2 threads
	property Lzma2MaximumThreads MaximumThreads
	{
		Lzma2MaximumThreads get(void);
		void set(Lzma2MaximumThreads value);
	}

	// ThreadsPerBlock
	//
	// Indicates the LZMA2 threads per block
	property Lzma2ThreadsPerBlock ThreadsPerBlock
	{
		Lzma2ThreadsPerBlock get(void);
		void set(Lzma2ThreadsPerBlock value);
	}

private:

	// Static Constructor
	//
	static XzEncoder();

	// ReaderWriter
	//
	// Helper class that wraps the LZMA ISeqInStream / ISeqOutStream callbacks
	ref class ReaderWriter
	{
	public:

		// Instance Constructor
		//
		ReaderWriter(Stream^ instream, Stream^ outstream);

		// ISeqInStream* conversion operator
		//
		operator ISeqInStream*();

		// ISeqOutStream* conversion operator
		//
		operator ISeqOutStream*();

	private:

		// Destructor / Finalizer
		//
		~ReaderWriter();
		!ReaderWriter();
	
		// OnReadDelegate
		//
		// Delegate to provide access to OnRead as an unmanaged __cdecl function
		[UnmanagedFunctionPointer(CallingConvention::Cdecl)] delegate SRes OnReadDelegate(void*, void*, size_t*);

		// OnReadPointer
		//
		// ISeqInStream::Read compatible function declaration
		using OnReadPointer = SRes(__cdecl*)(void*, void*, size_t*);

		// OnWriteDelegate
		//
		// Delegate to provide access to OnRead as an unmanaged __cdecl function
		[UnmanagedFunctionPointer(CallingConvention::Cdecl)] delegate size_t OnWriteDelegate(void*, void const*, size_t);

		// OnWritePointer
		//
		// ISeqOutStream::Write compatible function pointer
		using OnWritePointer = size_t(__cdecl*)(void*, void const*, size_t);

		//-------------------------------------------------------------------
		// Private Member Functions

		// OnRead
		//
		// Implements ISeqInStream::Read
		SRes OnRead(void* context, void* buffer, size_t* size);

		// OnWrite
		//
		// Implements ISeqOutStream::Write
		size_t OnWrite(void* context, void const* buffer, size_t size);

		//-------------------------------------------------------------------
		// Member Variables

		bool						m_disposed;			// Object disposal flag
		initonly Stream^			m_instream;			// The input stream
		initonly Stream^			m_outstream;		// The output stream
		ISeqInStream*				m_seqin;			// ISeqInStream instance
		ISeqOutStream*				m_seqout;			// ISeqOutStream instance
		initonly OnReadDelegate^	m_onread;			// OnRead delegate instance
		initonly OnWriteDelegate^	m_onwrite;			// OnWrite delegate instance
	};
	
	//-----------------------------------------------------------------------
	// Private Member Functions

	// Encode
	//
	// Compresses an input stream into an output stream
	void Encode(Stream^ instream, unsigned __int64 insize, Stream^ outstream);

	//-----------------------------------------------------------------------
	// Member Variables

	Lzma2BlockSize				m_blocksize;			// LZMA2 block size
	Lzma2ThreadsPerBlock		m_blockthreads;			// Threads per block
	Lzma2MaximumThreads			m_totalthreads;			// Total LZMA2 threads
};

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)

#endif	// __XZENCODER_H_
