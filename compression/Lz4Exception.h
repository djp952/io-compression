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

#ifndef __LZ4EXCEPTION_H_
#define __LZ4EXCEPTION_H_
#pragma once

#include <lz4frame.h>

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::IO;
using namespace System::Reflection;
using namespace System::Resources;
using namespace System::Runtime::Serialization;
using namespace System::Security;
using namespace System::Security::Permissions;
using namespace System::Text;

namespace zuki::io::compression {

//---------------------------------------------------------------------------
// Class Lz4Exception
//
// Represents an lz4 library exception
//---------------------------------------------------------------------------

[SerializableAttribute]
public ref class Lz4Exception sealed : public IOException
{
public:

	//-----------------------------------------------------------------------
	// Member Functions

	// GetObjectData
	//
	// Overrides Exception::GetObjectData
	[SecurityCriticalAttribute]
	[PermissionSetAttribute(SecurityAction::LinkDemand, Unrestricted = true)]
	[PermissionSetAttribute(SecurityAction::InheritanceDemand, Unrestricted = true)]
	[SecurityPermissionAttribute(SecurityAction::Demand, SerializationFormatter = true)]
	virtual void GetObjectData(SerializationInfo^ info, StreamingContext context) override;

	//-----------------------------------------------------------------------
	// Properties

	// ErrorCode
	//
	// Exposes the lz4 error code
	property LZ4F_errorCode_t ErrorCode
	{
		LZ4F_errorCode_t get(void);
	}

internal:

	// Instance Constructor
	//
	Lz4Exception(LZ4F_errorCode_t code);

private:

	// Serialization Constructor
	//
	[SecurityPermissionAttribute(SecurityAction::Demand, SerializationFormatter = true)]
	Lz4Exception(SerializationInfo^ info, StreamingContext context);

	//-----------------------------------------------------------------------
	// Private Member Functions

	// GetErrorMessage
	//
	// Retrieves the error message associated with the error code
	static String^ GetErrorMessage(LZ4F_errorCode_t code);

	//-----------------------------------------------------------------------
	// Member Variables

	LZ4F_errorCode_t				m_code;			// Underlying error code
};

//---------------------------------------------------------------------------

} // zuki::io::compression

#pragma warning(pop)

#endif	// __LZ4EXCEPTION_H_
