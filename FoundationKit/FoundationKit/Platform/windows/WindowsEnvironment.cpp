/****************************************************************************
Copyright (c) 2017 libo All rights reserved.

losemymind.libo@gmail.com

****************************************************************************/
#include "FoundationKit/GenericPlatformMacros.hpp"
#if (PLATFORM_WINDOWS)
#include <sstream>
#include "FoundationKit/Platform/Environment.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"

NS_FK_BEGIN
#define VARIABLE_SIZE	1024
Environment::stringvec Environment::GetEnvironmentVariables()
{
    LPWCH lpvEnv = GetEnvironmentStringsW();
    LPWSTR lpszVariable = (LPWSTR)lpvEnv;
    stringvec  Variables;
    while (*lpszVariable)
    {
        Variables.push_back(StringUtils::wstring2string(lpszVariable));
        lpszVariable += lstrlen(lpszVariable) + 1;
    }
    FreeEnvironmentStringsW(lpvEnv);
    return Variables;
}


std::string Environment::GetEnvironmentVariable(const std::string& variable)
{
    std::wstring VariableName = StringUtils::string2wstring(variable);
    wchar_t VariableValue[VARIABLE_SIZE] = { 0 };
    DWORD VariableLength = ::GetEnvironmentVariableW(VariableName.c_str(), VariableValue, VARIABLE_SIZE);
    if (VariableLength == 0)
        return std::string(); //throw NotFoundException(VariableName);
    if (VariableLength < VARIABLE_SIZE)
        return StringUtils::wstring2string(VariableValue);
    wchar_t* BigVariableValue = new wchar_t[VariableLength + 1];
    memset(BigVariableValue, 0, VariableLength + 1);
    ::GetEnvironmentVariableW(VariableName.c_str(), BigVariableValue, VariableLength);
    std::wstring result = BigVariableValue;
    delete[] BigVariableValue;
    return StringUtils::wstring2string(result);
}

bool Environment::HasEnvironmentVariable(const std::string& variable)
{
    std::wstring VariableName = StringUtils::string2wstring(variable);
    DWORD VariableLength = ::GetEnvironmentVariableW(VariableName.c_str(), 0, 0);
    return VariableLength > 0;
}

bool Environment::SetEnvironmentVariable(const std::string& variable, const std::string& value)
{
    bool ChangeVariableResult = false;
    std::wstring VariableName = StringUtils::string2wstring(variable);
    if (value.empty())
    {
        ChangeVariableResult = (::SetEnvironmentVariableW(VariableName.c_str(), NULL) == TRUE);
    }
    else 
    {
        std::wstring VariableValue = StringUtils::string2wstring(value);
        ChangeVariableResult = (::SetEnvironmentVariableW(VariableName.c_str(), VariableValue.c_str()) == TRUE);
        if (!ChangeVariableResult)
        {
            std::string msg = "cannot set environment variable: ";
            msg.append(variable);
            ASSERTED(false, msg.c_str());
        }
    }
    return ChangeVariableResult;
}

LPWSTR *SegmentCommandLine(LPCWSTR lpCmdLine, DWORD *pNumArgs);
Environment::stringvec Environment::GetCommandLineArgs()
{
    stringvec commandLineArgs;
    do 
    {
        LPWSTR commandLine = GetCommandLineW();
        if (commandLine == nullptr)
            break;
        DWORD numArgs = 0;
        LPWSTR* argv = SegmentCommandLine(commandLine, &numArgs);
        if (!argv || numArgs < 1)
            break;
        // Copy each argument into new Strings.
        for (unsigned int i = 0; i < numArgs; i++)
        {
            commandLineArgs.push_back(StringUtils::wstring2string(argv[i]));
        }
        delete[] argv;

    } while (false);
     return commandLineArgs;
}


LPWSTR *SegmentCommandLine(LPCWSTR lpCmdLine, DWORD *pNumArgs)
{
    *pNumArgs = 0;
    int nch = (int)wcslen(lpCmdLine);

    // Calculate the worstcase storage requirement. (One pointer for
    // each argument, plus storage for the arguments themselves.)
    int cbAlloc = (nch + 1)*sizeof(LPWSTR) + sizeof(WCHAR)*(nch + 1);
    size_t allocCount = cbAlloc / sizeof(WCHAR) +1;
    LPWSTR pAlloc = new (std::nothrow)WCHAR[allocCount];
    if (!pAlloc)
        return NULL;

    memset(pAlloc, 0, allocCount);

    LPWSTR *argv = (LPWSTR*)pAlloc; // We store the argv pointers in the first halt
    LPWSTR pdst = (LPWSTR)(((BYTE*)pAlloc) + sizeof(LPWSTR)*(nch + 1)); // A running pointer to second half to store arguments
    LPCWSTR psrc = lpCmdLine;
    WCHAR   c;
    BOOL    inquote;
    BOOL    copychar;
    int     numslash;

    // First, parse the program name (argv[0]). Argv[0] is parsed under
    // special rules. Anything up to the first whitespace outside a quoted
    // subtring is accepted. Backslashes are treated as normal characters.
    argv[(*pNumArgs)++] = pdst;
    inquote = FALSE;
    do {
        if (*psrc == L'"')
        {
            inquote = !inquote;
            c = *psrc++;
            continue;
        }
        *pdst++ = *psrc;

        c = *psrc++;

    } while ((c != L'\0' && (inquote || (c != L' ' && c != L'\t'))));

    if (c == L'\0') {
        psrc--;
    }
    else {
        *(pdst - 1) = L'\0';
    }

    inquote = FALSE;



    /* loop on each argument */
    for (;;)
    {
        if (*psrc)
        {
            while (*psrc == L' ' || *psrc == L'\t')
            {
                ++psrc;
            }
        }

        if (*psrc == L'\0')
            break;              /* end of args */

        /* scan an argument */
        argv[(*pNumArgs)++] = pdst;

        /* loop through scanning one argument */
        for (;;)
        {
            copychar = 1;
            /* Rules: 2N backslashes + " ==> N backslashes and begin/end quote
            2N+1 backslashes + " ==> N backslashes + literal "
            N backslashes ==> N backslashes */
            numslash = 0;
            while (*psrc == L'\\')
            {
                /* count number of backslashes for use below */
                ++psrc;
                ++numslash;
            }
            if (*psrc == L'"')
            {
                /* if 2N backslashes before, start/end quote, otherwise
                copy literally */
                if (numslash % 2 == 0)
                {
                    if (inquote)
                    {
                        if (psrc[1] == L'"')
                        {
                            psrc++;    /* Double quote inside quoted string */
                        }
                        else
                        {
                            /* skip first quote char and copy second */
                            copychar = 0;
                        }
                    }
                    else
                    {
                        copychar = 0;       /* don't copy quote */
                    }
                    inquote = !inquote;
                }
                numslash /= 2;          /* divide numslash by two */
            }

            /* copy slashes */
            while (numslash--)
            {
                *pdst++ = L'\\';
            }

            /* if at end of arg, break loop */
            if (*psrc == L'\0' || (!inquote && (*psrc == L' ' || *psrc == L'\t')))
                break;

            /* copy character into argument */
            if (copychar)
            {
                *pdst++ = *psrc;
            }
            ++psrc;
        }
        /* null-terminate the argument */
        *pdst++ = L'\0';          /* terminate string */
    }

    /* We put one last argument in -- a null ptr */
    argv[(*pNumArgs)] = NULL;
    _ASSERTE((BYTE*)pdst <= (BYTE*)pAlloc + cbAlloc);
    return argv;
}


NS_FK_END

#endif // OF #if (PLATFORM_WINDOWS)
