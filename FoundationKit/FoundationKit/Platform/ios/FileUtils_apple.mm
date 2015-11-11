/****************************************************************************
 Copyright (c) 2015 libo All rights reserved.
 
 losemymind.libo@gmail.com
 
 ****************************************************************************/
#include "FoundationKit/FoundationKitDefines.h"
#if (FK_TARGET_PLATFORM == FK_PLATFORM_IOS || FK_TARGET_PLATFORM == FK_PLATFORM_MAC)
#include "FileUtils.h"
#include "FoundationKit/Foundation/Logger.h"
#include <ftw.h>
#import <Foundation/Foundation.h>
NS_FK_BEGIN

void FileUtils::initRootPath()
{
    if (0 == _resourceRootPath.length())
    {
        
    }
}

bool FileUtils::isFileExist(const std::string& filename) const
{
    std::string filePath = filename;
    if (filePath.empty())
        return false;

    bool ret = false;

    if (filePath[0] != '/')
    {
        std::string path;
        std::string file;
        size_t pos = filePath.find_last_of("/");
        if (pos != std::string::npos)
        {
            file = filePath.substr(pos + 1);
            path = filePath.substr(0, pos + 1);
        }
        else
        {
            file = filePath;
        }

        NSString* fullpath = [[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String : file.c_str()]
        ofType : nil
             inDirectory : [NSString stringWithUTF8String : path.c_str()]];
            if (fullpath != nil)
                ret = true;
    }
    else
    {
        // Search path is an absolute path.
        if ([[NSFileManager defaultManager] fileExistsAtPath:[NSString stringWithUTF8String : filePath.c_str()]]) 
            ret = true;
    }

    return ret;
}


std::string FileUtils::getWritablePath() const
{
    if (_writablePath.length())
    {
        return _writablePath;
    }

    // save to document folder
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex : 0];
    std::string strRet = [documentsDirectory UTF8String];
    strRet.append("/");
    return strRet;
}

static int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    auto ret = remove(fpath);
    if (ret)
    {
        LOG_ERROR("Fail to remove: %s ", fpath);
    }

    return ret;
}

bool FileUtils::removeDirectory(const std::string& path)
{
    if (path.size() > 0 && path[path.size() - 1] != '/')
    {
        LOG_ERROR("Fail to remove directory, path must termniate with '/': %s", path.c_str());
        return false;
    }

    if (nftw(path.c_str(), unlink_cb, 64, FTW_DEPTH | FTW_PHYS))
        return false;
    else
        return true;
}

NS_FK_END

#endif


