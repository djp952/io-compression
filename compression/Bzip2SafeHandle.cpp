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
#include "Bzip2SafeHandle.h"

#include "Bzip2Exception.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Bzip2SafeHandle Constructor
//
// Arguments:
//
//	mode		- Mode (compress/decompress) to assign to the bz_stream
//	level		- Indicates whether to emphasize speed or compression efficiency

Bzip2SafeHandle::Bzip2SafeHandle(Compression::CompressionMode mode, Compression::CompressionLevel level) : m_mode(mode), SafeHandle(IntPtr::Zero, true)
{
	try { SetHandle(IntPtr(new bz_stream)); }
	catch(Exception^) { throw gcnew OutOfMemoryException(); }

	// Cast the generated pointer back into a bz_stream to initialize it properly
	bz_stream* unmanaged = reinterpret_cast<bz_stream*>(DangerousGetHandle().ToPointer());
	memset(unmanaged, 0, sizeof(bz_stream));

	// Initialize the bz_stream for either compression or decompression based on the requested mode
	int result = (mode == Compression::CompressionMode::Decompress) ? BZ2_bzDecompressInit(unmanaged, 0, 0) :
		BZ2_bzCompressInit(unmanaged, (level == Compression::CompressionLevel::Optimal) ? 9 : 1, 0, 0);
	if(result != BZ_OK) throw gcnew Bzip2Exception(result);
}

//---------------------------------------------------------------------------
// Bzip2SafeHandle pointer to member operator

bz_stream* Bzip2SafeHandle::operator->(void)
{
	return reinterpret_cast<bz_stream*>(DangerousGetHandle().ToPointer());
}

//---------------------------------------------------------------------------
// Bzip2SafeHandle bz_stream* type conversion operator

Bzip2SafeHandle::operator bz_stream*(void)
{
	return reinterpret_cast<bz_stream*>(DangerousGetHandle().ToPointer());
}

//---------------------------------------------------------------------------
// Bzip2SafeHandle::IsInvalid::get
//
// Gets a value indicating whether the handle/resource value is invalid

bool Bzip2SafeHandle::IsInvalid::get(void)
{ 
	return (handle == IntPtr::Zero);
}

//---------------------------------------------------------------------------
// Bzip2SafeHandle::ReleaseHandle
//
// Releases the contained unmanaged handle/resource
//
// Arguments:
//
//	NONE

bool Bzip2SafeHandle::ReleaseHandle(void)
{
	// Cast the handle back into an unmanaged type pointer
	bz_stream* unmanaged = reinterpret_cast<bz_stream*>(handle.ToPointer());

	// Reset all of the input/output buffer pointers and size information
	unmanaged->next_in = unmanaged->next_out = nullptr;
	unmanaged->avail_in = unmanaged->avail_out = 0;

	// Call either bzDecompressEnd or bzCompressEnd to clean up the bzip data buffers
	if(m_mode == Compression::CompressionMode::Decompress) BZ2_bzDecompressEnd(unmanaged);
	else BZ2_bzCompressEnd(unmanaged);

	delete unmanaged;
	return true;
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
