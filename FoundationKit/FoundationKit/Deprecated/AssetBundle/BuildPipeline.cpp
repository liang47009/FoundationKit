
#include "BuildPipeline.hpp"

#include "FoundationKit/Platform/FileUtils.h"
#include "FoundationKit/Foundation/Logger.h"
#include <map>
NS_FK_BEGIN



void BuildPipeline::buildAssetBundle(const std::string& dir)
{
    uint32_t defaultBufferLength = uint32_t(4) << 20; // 4MiB
    std::string assetRootPath = FileUtils::convertPathFormatToUnixStyle(dir);

    std::string rootDirName = FileUtils::getInstance()->getDirName(assetRootPath);
    std::map<std::string, char>  filesSorted;
    FileUtils::getInstance()->getFilesFromDir(assetRootPath, true, [&](const std::string filePath)
    {
        filesSorted.insert(std::make_pair(filePath, '\0'));
    });

    AssetFileHeader   afh;
    afh.files = new AssetFileHeader::AssetFileInfo[filesSorted.size()];
    afh.fileCount = filesSorted.size();

    size_t fileIndex = 0;
    size_t writeOffset = 0;
    char* buffer = new char[defaultBufferLength];
    std::string assetBundleFile = assetRootPath + "\\AssetBundle.asset";
    FILE* assetFile = fopen(assetBundleFile.c_str(), "wb");
    for (auto& iter:filesSorted)
    {

        std::string subFilePath = iter.first.substr(assetRootPath.size(), iter.first.size() - assetRootPath.size());
        strcpy(afh.files[fileIndex].name, (rootDirName + subFilePath).c_str());
        afh.files[fileIndex].offset = writeOffset;
        afh.files[fileIndex].length = FileUtils::getInstance()->getFileSize(iter.first.c_str());
        writeOffset += afh.files[fileIndex].length;

        size_t readsize = 0;
        size_t totalReadSize = 0;
        FILE* targetfp = fopen(iter.first.c_str(), "rb");
        if (targetfp  == nullptr)
        {
            LOG_ERROR("buildAssetBundle: open file:%s failed.", iter.first.c_str());
            break;
        }
        do
        {
            fseek(targetfp, totalReadSize, SEEK_SET);
            readsize = fread(buffer, 1, defaultBufferLength, targetfp);
            totalReadSize += readsize;
            fwrite(buffer, 1, readsize, assetFile);
        } while (readsize == defaultBufferLength);
        fclose(targetfp);
        fileIndex += 1;
    }

    std::string assetBundleCfg = assetRootPath + "/AssetBundle.manifest";
    FILE* assetCfg = fopen(assetBundleCfg.c_str(), "wb");
    fwrite(&afh, 1, sizeof(afh), assetCfg);
    fwrite(afh.files, 1, sizeof(AssetFileHeader::AssetFileInfo)*afh.fileCount, assetCfg);
    fclose(assetFile);
    fclose(assetCfg);
    delete afh.files;
    delete buffer;
}

void BuildPipeline::releaseAssetBundle(const std::string& assetbundleFile, const std::string& assetbundlecfg, const std::string& destDir)
{
    std::string rootDir = destDir;
    if (rootDir.back() != '/' && rootDir.back() != '\\')
    {
        rootDir.push_back('/');
    }
    FILE* assetCfg = fopen(assetbundlecfg.c_str(), "rb");

    AssetFileHeader* afh = new AssetFileHeader;
    char* assetFileHerderBuffer = new char[sizeof(AssetFileHeader)];
    fread(assetFileHerderBuffer, 1, sizeof(AssetFileHeader), assetCfg);
    memcpy(afh, assetFileHerderBuffer, sizeof(AssetFileHeader));

    delete assetFileHerderBuffer;
    afh->files = new AssetFileHeader::AssetFileInfo[afh->fileCount];
    fseek(assetCfg, sizeof(AssetFileHeader), SEEK_SET);
    char* assetFileHerderData = new char[sizeof(AssetFileHeader::AssetFileInfo)*afh->fileCount];
    fread(assetFileHerderData, 1, sizeof(AssetFileHeader::AssetFileInfo)*afh->fileCount, assetCfg);
    memcpy(afh->files, assetFileHerderData, sizeof(AssetFileHeader::AssetFileInfo)*afh->fileCount);
    fclose(assetCfg);

    FILE* assetFile = fopen(assetbundleFile.c_str(), "rb");
    uint32_t defaultBufferLength = uint32_t(4) << 20; // 4MiB
    char* buffer = new char[defaultBufferLength];
    for (size_t i = 0; i < afh->fileCount; ++i)
    {
        std::string finalFilePath = rootDir + afh->files[i].name;
        FileUtils::getInstance()->createDirectory(finalFilePath.substr(0, finalFilePath.find_last_of('/')));
        FILE* targetFile = fopen(finalFilePath.c_str(), "wb");

        size_t readsize = 0;
        size_t leftReadSize = afh->files[i].length;
        do 
        {
            if (leftReadSize < defaultBufferLength)
            {
                readsize = fread(buffer, 1, leftReadSize, assetFile);
                
            }
            else
            {
                readsize = fread(buffer, 1, defaultBufferLength, assetFile);
            }
            fwrite(buffer, 1, readsize, targetFile);
            leftReadSize -= readsize;
        } while (leftReadSize > 0);
        fclose(targetFile);
    }
    delete buffer;
    delete afh->files;
    delete afh;
    fclose(assetFile);
}



NS_FK_END


