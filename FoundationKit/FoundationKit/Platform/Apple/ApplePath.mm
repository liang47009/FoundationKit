/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_APPLE

#import <Foundation/Foundation.h>
#include "FoundationKit/Platform/Path.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
NS_FK_BEGIN

std::string Path::GetTempPath()
{
    NSString* TempDir = NSTemporaryDirectory();
    TemporaryPath = [TempDir UTF8String];
    return TemporaryPath;
}

std::string Path::GetDocumentsPath()
{
    if (DocumentsPath.length())
    {
        return DocumentsPath;
    }
    
    // save to document folder
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex : 0];
    // or
    //NSString *documentsDirectory = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"];
    DocumentsPath = [documentsDirectory UTF8String];
    DocumentsPath.append("/");
    return DocumentsPath;
}

std::string Path::GetApplicationPath()
{
    NSString * bundlPath = [[NSBundle mainBundle] bundlePath];
    return [bundlPath UTF8String];
}

NS_FK_END

#endif //OF #if PLATFORM_APPLE
