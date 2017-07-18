

#include <string>
#include <vector>
#include "FoundationKit/GenericPlatformMacros.hpp"
NS_FK_BEGIN
// Performs operations on System.String instances that contain file or directory
// path information. These operations are performed in a cross-platform manner.To
class Path
{
public:
    // Provides a platform-specific alternate character used to separate directory levels
    // in a path string that reflects a hierarchical file system organization.
    static const char AltDirectorySeparatorChar;

    // Provides a platform-specific character used to separate directory levels in a
    // path string that reflects a hierarchical file system organization.
    static const char DirectorySeparatorChar;

    // Provides a platform-specific array of characters that cannot be specified in
    // path string arguments passed to members of the Path class.
    // A character array of invalid path characters for the current platform.
    static const char InvalidPathChars[];

    static const char InvalidFileNameChars[];

    // A platform-specific separator character used to separate path strings in environment
    // variables.
    static const char PathSeparator;

    // Provides a platform-specific volume separator character.
    static const char VolumeSeparatorChar;

   /**
    * Convert directory separator to unix style if the 
    * path is windows style.The path cannot contain any of the characters
    * defined in Path::GetInvalidPathChars.
    * @param path:
    *         The path of a file or directory.
    * @return The converted path.
    */
    static std::string ConvertPathFormatToUnixStyle(const std::string& path);

   /**
    * Convert directory separator to current platform style.
    * defined in Path::GetInvalidPathChars.
    * @param path:
    *         The path of a file or directory.
    * @return The converted path.
    */
    static std::string ConvertPathFormatToPlatformStyle(const std::string& path);

   /**
    * Changes the extension of a path string.
    * @param path:
    *         The path information to modify. The path cannot contain any of the characters
    *         defined in Path::GetInvalidPathChars.
    * @param extension:
    *         The new extension (with or without a leading period). Specify an empty string ("") to remove
    *         an existing extension from path.
    * @return The modified path information.On Windows-based desktop platforms, if path is
    *         an empty string (""), the path information is returned unmodified. If
    *         extension is an empty string (""), the returned string contains the specified
    *         path with its extension removed. If path has no extension, and extension is
    *         not an empty string (""), the returned path string contains extension
    *         appended to the end of path.
    */
    static std::string ChangeExtension(const std::string& path, const std::string& extension);

   /**
    * Combines an array of strings into a path.The path cannot contain any of the characters
    * defined in Path::GetInvalidPathChars.
    * @param paths
    *         An array of parts of the path.
    * @return The combined paths.
    */
    static std::string Combine(const std::vector<std::string>& paths);

   /**
    * Combines two strings into a path.The path cannot contain any of the characters
    * defined in Path::GetInvalidPathChars.
    * @param path1:
    *         The first path to combine.
    * @param path2:
    *         The second path to combine.
    * @return The combined paths. If one of the specified paths is a zero-length string, this
    *         method returns the other path. If path2 contains an absolute path, this method
    *         returns path2.
    */
    static std::string Combine(const std::string& path1, const std::string& path2);

   /**
    * Combines three strings into a path.The path cannot contain any of the characters
    * defined in Path::GetInvalidPathChars.
    * @param path1:
    *         The first path to combine.
    * @param path2:
    *         The second path to combine.
    * @param path3:
    *         The third path to combine.
    * @return The combined paths. 
    */
    static std::string Combine(const std::string& path1, const std::string& path2, const std::string& path3);

   /**
    * Combines four strings into a path.The path cannot contain any of the characters
    * defined in Path::GetInvalidPathChars.
    * @param path1:
    *         The first path to combine.
    * @param path2:
    *         The second path to combine.
    * @param path3:
    *         The third path to combine.
    * @param path4:
    *         The fourth path to combine.
    * @return The combined paths.
    */
    static std::string Combine(const std::string& path1, const std::string& path2, const std::string& path3, const std::string& path4);

   /**
    * Returns the directory information for the specified path string.
    * @param path:
    *         The path of a file or directory.
    * @return The string returned by this method consists of all characters
    *         between the first and last DirectorySeparatorChar or AltDirectorySeparatorChar
    *         character in the path. The first separator character is included, but the last
    *         separator character is not included in the returned string.
    */
    static std::string GetDirectoryName(const std::string& path);

   /** 
    * Returns the extension of the specified path string.
    * @param path:
    *         The path string from which to get the extension.
    * @return The extension of the specified path (including the period "."), or "".
    */
    static std::string GetExtension(const std::string& path);

   /** 
    * Returns the file name and extension of the specified path string.
    * @param path:
    *         The path string from which to obtain the file name and extension.
    * @return The characters after the last directory character in path. If the last character
    *         of path is a directory or volume separator character, this method returns "".
    *         If path is "", this method returns "".
    */
    static std::string GetFileName(const std::string& path);

   /** 
    * Returns the file name of the specified path string without the extension.
    * @param path:
    *         The path of the file.
    * @return The string returned by Path::GetFileName(System.String), minus the last
    *         period (.) and all characters following it.
    */
    static std::string GetFileNameWithoutExtension(const std::string& path);

   /** 
    * Returns the absolute path for the specified path string.
    * @param path:
    *         The file or directory for which to obtain absolute path information.
    * @return The fully qualified location of path, such as "C:\MyFile.txt".
    */
    //static std::string GetFullPath(std::string path);

   /** 
    * Gets an array containing the characters that are not allowed in file names.
    * @return An array containing the characters that are not allowed in file names.
    */
    static const char* GetInvalidFileNameChars();

   /**
    * Gets an array containing the characters that are not allowed in path names.
    * @return An array containing the characters that are not allowed in path names.
    */
    static const char* GetInvalidPathChars();

   /** 
    * Gets the root directory information of the specified path.
    * @param path:
    *         The path from which to obtain root directory information.
    * @return The root directory of path, such as "C:\", an empty
    *         string if path does not contain root directory information.
    */
    //static std::string GetPathRoot(std::string path);

   /**
    * Creates a uniquely named, zero-byte temporary file on disk and returns the full
    * path of that file.
    * @param path:
    *         The full path of the temporary file.
    */
    static std::string GetTempFileName();

   /** 
    * Returns the path of the current user's temporary folder.
    * @return The path to the temporary folder, ending with a backslash.
    */
    static std::string GetTempPath();

   /** 
    * Determines whether a path includes a file name extension.
    * @param path:
    *         The path to search for an extension.
    * @return true if the characters that follow the last directory separator (\\ or /) or
    *         volume separator (:) in the path include a period (.) followed by one or more
    *         characters; otherwise, false.
    */
    static bool HasExtension(const std::string& path);

   /** 
    * Gets a value indicating whether the specified path string contains a root.
    * @param path:
    *         The path to test.
    * @return true if path contains a root; otherwise, false.
    */
    static bool IsPathRooted(std::string path);

   /**
    *  Checks whether the path is an absolute path.
    *
    *  @param path:
    *          The path that needs to be checked.
    *  @return true if it's an absolute path, false if not.
    */
    static bool IsAbsolutePath(const std::string& path);

    static std::string GetDocumentsPath();

    // On android platform, first must be call this function
    // to set writable path.
    // like:/sdcard/Android/data/com.xxx.xxx/files
    static void SetDocumentsPath(const std::string& path);

protected:
    static std::string DocumentsPath;

};
NS_FK_END

