

#include "FoundationKit/GenericPlatformMacros.h"
#include <string>
NS_FK_BEGIN

struct AssetFileHeader 
{
    struct AssetFileInfo 
    {
        AssetFileInfo()
        {
            memset(name, 0, FILEPATH_MAX+1);
        }
        char name[FILEPATH_MAX+1];
        size_t offset;
        size_t length;
    };
    size_t            fileCount;
    AssetFileInfo*    files;
};

class BuildPipeline
{
public:
    static void buildAssetBundle(const std::string& dir);

    static void releaseAssetBundle(const std::string& assetbundleFile
        , const std::string& assetbundlecfg
        , const std::string& destDir);

};

NS_FK_END




