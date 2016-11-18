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
#include "GzipException.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// GzipException Constructor (internal)
//
// Arguments:
//
//	code		- The zlib error code to associate with the exception

GzipException::GzipException(int code) : m_code(code), IOException(GetErrorMessage(code))
{
}

//---------------------------------------------------------------------------
// GzipException Constructor (private)
//
// Arguments:
//
//	info		- Serialization information
//	context		- Serialization context

GzipException::GzipException(SerializationInfo^ info, StreamingContext context) : IOException(info, context)
{
	m_code = info->GetInt32("@m_code");
}

//---------------------------------------------------------------------------
// GzipException::ErrorCode::get
//
// Gets the error code associated with this exception

int GzipException::ErrorCode::get(void)
{
	return m_code;
}

//---------------------------------------------------------------------------
// GzipException::GetErrorMessage (static, private)
//
// Gets the error message associated with a zlib error code
//
// Arguments:
//
//	code		- zlib error code to be looked up

String^ GzipException::GetErrorMessage(int code)
{
	try { 

		// The error messages are stored as resources in the format zlib{0}
		Assembly^ assm = Assembly::GetExecutingAssembly();
		ResourceManager^ resmgr = gcnew Resources::ResourceManager(assm->GetName()->Name, assm);

		String^ message = resmgr->GetString(String::Format("zlib{0}", code));
		return String::IsNullOrEmpty(message) ? String::Format("Unknown error {0}", code) : message;
	}

	// Don't allow any exceptions from the exception message lookup
	catch(Exception^) { return String::Format("Unknown error {0}", code); }
}

//---------------------------------------------------------------------------
// GzipException::GetObjectData
//
// Overrides Exception::GetObjectData
//
// Arguments:
//
//	info		- Serialization information
//	context		- Serialization context

void GzipException::GetObjectData(SerializationInfo^ info, StreamingContext context)
{
	if(Object::ReferenceEquals(info, nullptr)) throw gcnew ArgumentNullException("info");
	info->AddValue("@m_code", m_code);
	Exception::GetObjectData(info, context);
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
