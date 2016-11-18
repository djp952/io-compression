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
#include "GzipSafeHandle.h"

#include "GzipException.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// GzipSafeHandle Constructor
//
// Arguments:
//
//	mode		- Mode (compress/decompress) to assign to the z_stream
//	level		- Indicates whether to emphasize speed or compression efficiency

GzipSafeHandle::GzipSafeHandle(Compression::CompressionMode mode, Compression::CompressionLevel level) : m_mode(mode), SafeHandle(IntPtr::Zero, true)
{
	try { SetHandle(IntPtr(new z_stream)); }
	catch(Exception^) { throw gcnew OutOfMemoryException(); }

	// Cast the generated pointer back into a z_stream to initialize it properly
	z_stream* unmanaged = reinterpret_cast<z_stream*>(DangerousGetHandle().ToPointer());
	memset(unmanaged, 0, sizeof(z_stream));

	// Choose a zlib compression level
	int compresslevel = Z_DEFAULT_COMPRESSION;
	if(level == Compression::CompressionLevel::NoCompression) compresslevel = 0;
	else if(level == Compression::CompressionLevel::Fastest) compresslevel = 1;
	else if(level == Compression::CompressionLevel::Optimal) compresslevel = 9;

	// Initialize the z_stream for either compression or decompression based on the requested mode
	int result = (mode == Compression::CompressionMode::Decompress) ? inflateInit2(unmanaged, 16 + MAX_WBITS) :
		deflateInit2(unmanaged, compresslevel, Z_DEFLATED, 16 + MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
	if(result != Z_OK) throw gcnew GzipException(result);
}

//---------------------------------------------------------------------------
// GzipSafeHandle pointer to member operator

z_stream* GzipSafeHandle::operator->(void)
{
	return reinterpret_cast<z_stream*>(DangerousGetHandle().ToPointer());
}

//---------------------------------------------------------------------------
// GzipSafeHandle z_stream* type conversion operator

GzipSafeHandle::operator z_stream*(void)
{
	return reinterpret_cast<z_stream*>(DangerousGetHandle().ToPointer());
}

//---------------------------------------------------------------------------
// GzipSafeHandle::IsInvalid::get
//
// Gets a value indicating whether the handle/resource value is invalid

bool GzipSafeHandle::IsInvalid::get(void)
{ 
	return (handle == IntPtr::Zero);
}

//---------------------------------------------------------------------------
// GzipSafeHandle::ReleaseHandle
//
// Releases the contained unmanaged handle/resource
//
// Arguments:
//
//	NONE

bool GzipSafeHandle::ReleaseHandle(void)
{
	// Cast the handle back into an unmanaged type pointer
	z_stream* unmanaged = reinterpret_cast<z_stream*>(handle.ToPointer());

	// Reset all of the input/output buffer pointers and size information
	unmanaged->next_in = unmanaged->next_out = nullptr;
	unmanaged->avail_in = unmanaged->avail_out = 0;

	// Call either bzDecompressEnd or bzCompressEnd to clean up the bzip data buffers
	if(m_mode == Compression::CompressionMode::Decompress) inflateEnd(unmanaged);
	else deflateEnd(unmanaged);

	delete unmanaged;
	return true;
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
