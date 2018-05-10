/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.

  losemymind.libo@gmail.com

****************************************************************************/
#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID

#include <android/asset_manager.h>
#include "FoundationKit/Platform/File.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJNI.hpp"
NS_FK_BEGIN

static int AndroidReadAsset(void* asset, char* buf, int size) {
    return AAsset_read((AAsset*)asset, buf, size);
}

static int AndroidWriteAsset(void* asset, const char* buf, int size) {
    return EACCES; // can't provide write access to the apk
}

static fpos_t AndroidSeekAsset(void* asset, fpos_t offset, int whence) {
    return AAsset_seek((AAsset*)asset, offset, whence);
}

static int AndroidCloseAsset(void* asset) {
    AAsset_close((AAsset*)asset);
    return 0;
}

// wrapper fread/fseek/fclose
FILE* AndroidOpenAsset(const char * path)
{
    AAssetManager* AndroidAssetManager = AndroidJNI::GetAAssetManager();
    AAsset* asset = AAssetManager_open(AndroidAssetManager, path, AASSET_MODE_UNKNOWN);
    if (!asset) return nullptr;
    return funopen(asset, AndroidReadAsset, AndroidWriteAsset, AndroidSeekAsset, AndroidCloseAsset);
}

NS_FK_END

#endif //#if PLATFORM_ANDROID
