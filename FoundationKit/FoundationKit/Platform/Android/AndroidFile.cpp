#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID

#include <android/asset_manager.h>
#include "FoundationKit/Platform/File.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJNI.hpp"
NS_FK_BEGIN

static int AndroidReadAsset(void* cookie, char* buf, int size) {
    return AAsset_read((AAsset*)cookie, buf, size);
}

static int AndroidWriteAsset(void* cookie, const char* buf, int size) {
    return EACCES; // can't provide write access to the apk
}

static fpos_t AndroidSeekAsset(void* cookie, fpos_t offset, int whence) {
    return AAsset_seek((AAsset*)cookie, offset, whence);
}

static int AndroidCloseAsset(void* cookie) {
    AAsset_close((AAsset*)cookie);
    return 0;
}

FILE* AndroidOpenAsset(const char * path, const char * mode)
{
    if (mode[0] == 'w') return nullptr;
    AAssetManager* AndroidAssetManager = AndroidNode::AndroidJNI::GetAAssetManager();
    AAsset* asset = AAssetManager_open(AndroidAssetManager, path, AASSET_MODE_UNKNOWN);
    if (!asset) return nullptr;
    return funopen(asset, AndroidReadAsset, AndroidWriteAsset, AndroidSeekAsset, AndroidCloseAsset);
}

bool File::Move(const std::string& sourceFileName, const std::string& destFileName)
{
    if (IsExists(destFileName))
        return false;

    if (!IsExists(sourceFileName))
        return false;

    return (rename(sourceFileName.c_str(), destFileName.c_str()) == 0);
}

bool File::Delete(const std::string& path)
{
    return (remove(path.c_str()) == 0);
}

bool File::IsExists(const std::string& path)
{
    struct stat sts;
    if (stat(path.c_str(), &sts) == 0 && S_ISREG(sts.st_mode))
        return true;
    return false;
}

bool File::Rename(const std::string& sourceFileName, const std::string& destFileName)
{
    return (rename(sourceFileName.c_str(), destFileName.c_str()) == 0);
}

NS_FK_END

#endif //#if PLATFORM_ANDROID
