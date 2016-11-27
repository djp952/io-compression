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
// This program, "bzip2", the associated library "libbzip2", and all
// documentation, are copyright (C) 1996-2010 Julian R Seward.  All
// rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 
// 2. The origin of this software must not be misrepresented; you must 
//    not claim that you wrote the original software.  If you use this 
//    software in a product, an acknowledgment in the product 
//    documentation would be appreciated but is not required.
// 
// 3. Altered source versions must be plainly marked as such, and must
//    not be misrepresented as being the original software.
// 
// 4. The name of the author may not be used to endorse or promote 
//    products derived from this software without specific prior written 
//    permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// Julian Seward, jseward@bzip.org
// bzip2/libbzip2 version 1.0.6 of 6 September 2010
//---------------------------------------------------------------------------

#include "stdafx.h"
#include "Bzip2Exception.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Bzip2Exception Constructor (internal)
//
// Arguments:
//
//	code		- The bzip error code to associate with the exception

Bzip2Exception::Bzip2Exception(int code) : m_code(code), IOException(GetErrorMessage(code))
{
}

//---------------------------------------------------------------------------
// Bzip2Exception Constructor (private)
//
// Arguments:
//
//	info		- Serialization information
//	context		- Serialization context

Bzip2Exception::Bzip2Exception(SerializationInfo^ info, StreamingContext context) : IOException(info, context)
{
	m_code = info->GetInt32("@m_code");
}

//---------------------------------------------------------------------------
// Bzip2Exception::ErrorCode::get
//
// Gets the error code associated with this exception

int Bzip2Exception::ErrorCode::get(void)
{
	return m_code;
}

//---------------------------------------------------------------------------
// Bzip2Exception::GetErrorMessage (static, private)
//
// Gets the error message associated with a bzip error code
//
// Arguments:
//
//	code		- bzip error code to be looked up

String^ Bzip2Exception::GetErrorMessage(int code)
{
	try { 

		// The error messages are stored as resources in the format bzip{0}
		Assembly^ assm = Assembly::GetExecutingAssembly();
		ResourceManager^ resmgr = gcnew Resources::ResourceManager(assm->GetName()->Name, assm);

		String^ message = resmgr->GetString(String::Format("bzip{0}", code));
		return String::IsNullOrEmpty(message) ? String::Format("Unknown error {0}", code) : message;
	}

	// Don't allow any exceptions from the exception message lookup
	catch(Exception^) { return String::Format("Unknown error {0}", code); }
}

//---------------------------------------------------------------------------
// Bzip2Exception::GetObjectData
//
// Overrides Exception::GetObjectData
//
// Arguments:
//
//	info		- Serialization information
//	context		- Serialization context

void Bzip2Exception::GetObjectData(SerializationInfo^ info, StreamingContext context)
{
	if(Object::ReferenceEquals(info, nullptr)) throw gcnew ArgumentNullException("info");
	info->AddValue("@m_code", m_code);
	Exception::GetObjectData(info, context);
}

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)
