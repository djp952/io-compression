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
#include "LzmaException.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// LzmaException Constructor (internal)
//
// Arguments:
//
//	code		- The lzma error code to associate with the exception

LzmaException::LzmaException(int code) : m_code(code), IOException(GetErrorMessage(code))
{
}

//---------------------------------------------------------------------------
// LzmaException Constructor (private)
//
// Arguments:
//
//	info		- Serialization information
//	context		- Serialization context

LzmaException::LzmaException(SerializationInfo^ info, StreamingContext context) : IOException(info, context)
{
	m_code = info->GetInt32("@m_code");
}

//---------------------------------------------------------------------------
// LzmaException::ErrorCode::get
//
// Gets the error code associated with this exception

int LzmaException::ErrorCode::get(void)
{
	return m_code;
}

//---------------------------------------------------------------------------
// LzmaException::GetErrorMessage (static, private)
//
// Gets the error message associated with an lzma error code
//
// Arguments:
//
//	code		- lzma error code to be looked up

String^ LzmaException::GetErrorMessage(int code)
{
	try { 

		// The error messages are stored as resources in the format lzma{0}
		Assembly^ assm = Assembly::GetExecutingAssembly();
		ResourceManager^ resmgr = gcnew Resources::ResourceManager(assm->GetName()->Name, assm);

		String^ message = resmgr->GetString(String::Format("lzma{0}", code));
		return String::IsNullOrEmpty(message) ? String::Format("Unknown error {0}", code) : message;
	}

	// Don't allow any exceptions from the exception message lookup
	catch(Exception^) { return String::Format("Unknown error {0}", code); }
}

//---------------------------------------------------------------------------
// LzmaException::GetObjectData
//
// Overrides Exception::GetObjectData
//
// Arguments:
//
//	info		- Serialization information
//	context		- Serialization context

void LzmaException::GetObjectData(SerializationInfo^ info, StreamingContext context)
{
	if(Object::ReferenceEquals(info, nullptr)) throw gcnew ArgumentNullException("info");
	info->AddValue("@m_code", m_code);
	Exception::GetObjectData(info, context);
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
