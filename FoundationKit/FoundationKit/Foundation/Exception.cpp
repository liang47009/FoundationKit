/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/

#include <typeinfo>
#include "FoundationKit/Foundation/Exception.hpp"

NS_FK_BEGIN

Exception::Exception(int code) 
: _pNested(nullptr)
, _error_code(code)
{
}


Exception::Exception(const std::string& msg, int code) 
    : _error_msg(msg)
    , _pNested(nullptr)
    , _error_code(code)
{
}


Exception::Exception(const std::string& msg, const std::string& arg, int code) 
    : _error_msg(msg)
    , _pNested(nullptr)
    , _error_code(code)
{
	if (!arg.empty())
	{
        _error_msg.append(": ");
        _error_msg.append(arg);
	}
}

Exception::Exception(const std::string& msg, const Exception& nested, int code)
    : _error_msg(msg)
    , _pNested(nested.clone())
    , _error_code(code)
{
}

Exception::Exception(const Exception& exc)
    :std::exception(exc)
    , _error_msg(exc._error_msg)
    , _error_code(exc._error_code)
{
	_pNested = exc._pNested ? exc._pNested->clone() : 0;
}

	
Exception::~Exception() throw()
{
	delete _pNested;
}


Exception& Exception::operator = (const Exception& exc)
{
	if (&exc != this)
	{
		delete _pNested;
        _error_msg  = exc._error_msg;
		_pNested    = exc._pNested ? exc._pNested->clone() : 0;
        _error_code = exc._error_code;
	}
	return *this;
}


const char* Exception::name() const throw()
{
	return "Exception";
}


const char* Exception::className() const throw()
{
	return typeid(*this).name();
}

	
const char* Exception::what() const throw()
{
	return name();
}

	
std::string Exception::displayText() const
{
	std::string txt = name();
    if (!_error_msg.empty())
	{
		txt.append(": ");
        txt.append(_error_msg);
	}
	return txt;
}


void Exception::extendedMessage(const std::string& arg)
{
	if (!arg.empty())
	{
        if (!_error_msg.empty()) _error_msg.append(": ");
        _error_msg.append(arg);
	}
}


Exception* Exception::clone() const
{
	return new Exception(*this);
}


void Exception::rethrow() const
{
	throw *this;
}

IMPLEMENT_EXCEPTION(LogicException, Exception, "Logic exception")
IMPLEMENT_EXCEPTION(AssertionViolationException, LogicException, "Assertion violation")
IMPLEMENT_EXCEPTION(NullPointerException, LogicException, "Null pointer")
IMPLEMENT_EXCEPTION(NullValueException, LogicException, "Null value")
IMPLEMENT_EXCEPTION(BugcheckException, LogicException, "Bugcheck")
IMPLEMENT_EXCEPTION(InvalidArgumentException, LogicException, "Invalid argument")
IMPLEMENT_EXCEPTION(NotImplementedException, LogicException, "Not implemented")
IMPLEMENT_EXCEPTION(RangeException, LogicException, "Out of range")
IMPLEMENT_EXCEPTION(IllegalStateException, LogicException, "Illegal state")
IMPLEMENT_EXCEPTION(InvalidAccessException, LogicException, "Invalid access")
IMPLEMENT_EXCEPTION(SignalException, LogicException, "Signal received")
IMPLEMENT_EXCEPTION(UnhandledException, LogicException, "Unhandled exception")
IMPLEMENT_EXCEPTION(RuntimeException, Exception, "Runtime exception")
IMPLEMENT_EXCEPTION(NotFoundException, RuntimeException, "Not found")
IMPLEMENT_EXCEPTION(ExistsException, RuntimeException, "Exists")
IMPLEMENT_EXCEPTION(TimeoutException, RuntimeException, "Timeout")
IMPLEMENT_EXCEPTION(SystemException, RuntimeException, "System exception")
IMPLEMENT_EXCEPTION(RegularExpressionException, RuntimeException, "Error in regular expression")
IMPLEMENT_EXCEPTION(LibraryLoadException, RuntimeException, "Cannot load library")
IMPLEMENT_EXCEPTION(LibraryAlreadyLoadedException, RuntimeException, "Library already loaded")
IMPLEMENT_EXCEPTION(NoThreadAvailableException, RuntimeException, "No thread available")
IMPLEMENT_EXCEPTION(PropertyNotSupportedException, RuntimeException, "Property not supported")
IMPLEMENT_EXCEPTION(PoolOverflowException, RuntimeException, "Pool overflow")
IMPLEMENT_EXCEPTION(NoPermissionException, RuntimeException, "No permission")
IMPLEMENT_EXCEPTION(OutOfMemoryException, RuntimeException, "Out of memory")
IMPLEMENT_EXCEPTION(DataException, RuntimeException, "Data error")
IMPLEMENT_EXCEPTION(InterruptedException, RuntimeException, "Interrupted")
IMPLEMENT_EXCEPTION(IndexOutOfBoundsException, RuntimeException, "Index out of bounds")
IMPLEMENT_EXCEPTION(UnsupportedOperationException, RuntimeException, "Unsupported operation")
IMPLEMENT_EXCEPTION(EmptyStackException, RuntimeException, "Empty stack")
IMPLEMENT_EXCEPTION(StackOverflowException, RuntimeException, "Stack overflow")
IMPLEMENT_EXCEPTION(ArithmeticException, RuntimeException, "Arithmetic error")
IMPLEMENT_EXCEPTION(DataFormatException, DataException, "Bad data format")
IMPLEMENT_EXCEPTION(SyntaxException, DataException, "Syntax error")
IMPLEMENT_EXCEPTION(CircularReferenceException, DataException, "Circular reference")
IMPLEMENT_EXCEPTION(PathSyntaxException, SyntaxException, "Bad path syntax")
IMPLEMENT_EXCEPTION(IOException, RuntimeException, "I/O error")
IMPLEMENT_EXCEPTION(ProtocolException, IOException, "Protocol error")
IMPLEMENT_EXCEPTION(FileException, IOException, "File access error")
IMPLEMENT_EXCEPTION(FileExistsException, FileException, "File exists")
IMPLEMENT_EXCEPTION(FileNotFoundException, FileException, "File not found")
IMPLEMENT_EXCEPTION(PathNotFoundException, FileException, "Path not found")
IMPLEMENT_EXCEPTION(FileReadOnlyException, FileException, "File is read-only")
IMPLEMENT_EXCEPTION(FileAccessDeniedException, FileException, "Access to file denied")
IMPLEMENT_EXCEPTION(CreateFileException, FileException, "Cannot create file")
IMPLEMENT_EXCEPTION(OpenFileException, FileException, "Cannot open file")
IMPLEMENT_EXCEPTION(WriteFileException, FileException, "Cannot write file")
IMPLEMENT_EXCEPTION(ReadFileException, FileException, "Cannot read file")
IMPLEMENT_EXCEPTION(UnknownURISchemeException, RuntimeException, "Unknown URI scheme")
IMPLEMENT_EXCEPTION(ApplicationException, Exception, "Application exception")
IMPLEMENT_EXCEPTION(BadCastException, RuntimeException, "Bad cast exception")

NS_FK_END