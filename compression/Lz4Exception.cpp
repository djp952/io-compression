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
#include "Lz4Exception.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Lz4Exception Constructor (internal)
//
// Arguments:
//
//	code		- The lz4 error code to associate with the exception

Lz4Exception::Lz4Exception(LZ4F_errorCode_t code) : m_code(code), IOException(GetErrorMessage(code))
{
}

//---------------------------------------------------------------------------
// Lz4Exception Constructor (private)
//
// Arguments:
//
//	info		- Serialization information
//	context		- Serialization context

Lz4Exception::Lz4Exception(SerializationInfo^ info, StreamingContext context) : IOException(info, context)
{
	m_code = info->GetInt32("@m_code");
}

//---------------------------------------------------------------------------
// Lz4Exception::ErrorCode::get
//
// Gets the error code associated with this exception

LZ4F_errorCode_t Lz4Exception::ErrorCode::get(void)
{
	return m_code;
}

//---------------------------------------------------------------------------
// Lz4Exception::GetErrorMessage (static, private)
//
// Gets the error message associated with a bzip error code
//
// Arguments:
//
//	code		- lz4 error code to be looked up

String^ Lz4Exception::GetErrorMessage(LZ4F_errorCode_t code)
{
	return gcnew String(LZ4F_getErrorName(code));
}

//---------------------------------------------------------------------------
// Lz4Exception::GetObjectData
//
// Overrides Exception::GetObjectData
//
// Arguments:
//
//	info		- Serialization information
//	context		- Serialization context

void Lz4Exception::GetObjectData(SerializationInfo^ info, StreamingContext context)
{
	if(Object::ReferenceEquals(info, nullptr)) throw gcnew ArgumentNullException("info");
	info->AddValue("@m_code", m_code);
	Exception::GetObjectData(info, context);
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
