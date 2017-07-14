

#include <string>

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

    // A platform-specific separator character used to separate path strings in environment
    // variables.
    static const char PathSeparator;

    // Provides a platform-specific volume separator character.
    static const char VolumeSeparatorChar;

   /**
    * Changes the extension of a path string.
    * @param[in] path
    *              The path information to modify. The path cannot contain any of the characters
    *              defined in Path::GetInvalidPathChars.
    * @param[in] extension
    *              The new extension (with or without a leading period). Specify an empty string ("") to remove
    *              an existing extension from path.
    * @return
    *              The modified path information.On Windows-based desktop platforms, if path is
    *              an empty string (""), the path information is returned unmodified. If
    *              extension is an empty string (""), the returned string contains the specified
    *              path with its extension removed. If path has no extension, and extension is
    *              not an empty string (""), the returned path string contains extension
    *              appended to the end of path.
    */
    static std::string ChangeExtension(std::string path, std::string extension);

   /**
    * Combines an array of strings into a path.The path cannot contain any of the characters
    * defined in Path::GetInvalidPathChars.
    * @param[in] paths
    *              An array of parts of the path.
    * @return The combined paths.
    */
    static std::string Combine(std::string paths[]);
    //
    // Summary:
    //     Combines two strings into a path.
    //
    // Parameters:
    //   path1:
    //     The first path to combine.
    //
    //   path2:
    //     The second path to combine.
    //
    // Returns:
    //     The combined paths. If one of the specified paths is a zero-length string, this
    //     method returns the other path. If path2 contains an absolute path, this method
    //     returns path2.
    //
    // Exceptions:
    //   T:System.ArgumentException:
    //     path1 or path2 contains one or more of the invalid characters defined in System.IO.Path.GetInvalidPathChars.
    //
    //   T:System.ArgumentNullException:
    //     path1 or path2 is null.
   /**
    * Combines two strings into a path.The path cannot contain any of the characters
    * defined in Path::GetInvalidPathChars.
    * @param[in] path1
    *              The first path to combine.
    * @param[in] path2
    *              The second path to combine.
    * @return The combined paths. If one of the specified paths is a zero-length string, this
    *         method returns the other path. If path2 contains an absolute path, this method
    *         returns path2.
    */
    static std::string Combine(std::string path1, std::string path2);
    //
    // Summary:
    //     Combines three strings into a path.
    //
    // Parameters:
    //   path1:
    //     The first path to combine.
    //
    //   path2:
    //     The second path to combine.
    //
    //   path3:
    //     The third path to combine.
    //
    // Returns:
    //     The combined paths.
    //
    // Exceptions:
    //   T:System.ArgumentException:
    //     path1, path2, or path3 contains one or more of the invalid characters defined
    //     in System.IO.Path.GetInvalidPathChars.
    //
    //   T:System.ArgumentNullException:
    //     path1, path2, or path3 is null.
    static std::string Combine(std::string path1, std::string path2, std::string path3);
    //
    // Summary:
    //     Combines four strings into a path.
    //
    // Parameters:
    //   path1:
    //     The first path to combine.
    //
    //   path2:
    //     The second path to combine.
    //
    //   path3:
    //     The third path to combine.
    //
    //   path4:
    //     The fourth path to combine.
    //
    // Returns:
    //     The combined paths.
    //
    // Exceptions:
    //   T:System.ArgumentException:
    //     path1, path2, path3, or path4 contains one or more of the invalid characters
    //     defined in System.IO.Path.GetInvalidPathChars.
    //
    //   T:System.ArgumentNullException:
    //     path1, path2, path3, or path4 is null.
    static std::string Combine(std::string path1, std::string path2, std::string path3, std::string path4);
    //
    // Summary:
    //     Returns the directory information for the specified path string.
    //
    // Parameters:
    //   path:
    //     The path of a file or directory.
    //
    // Returns:
    //     Directory information for path, or null if path denotes a root directory or is
    //     null. Returns System.String.Empty if path does not contain directory information.
    //
    // Exceptions:
    //   T:System.ArgumentException:
    //     The path parameter contains invalid characters, is empty, or contains only white
    //     spaces.
    //
    //   T:System.IO.PathTooLongException:
    //     In the .NET for Windows Store apps or the Portable Class Library, catch the base
    //     class exception, System.IO.IOException, instead.The path parameter is longer
    //     than the system-defined maximum length.
    static std::string GetDirectoryName(std::string path);
    //
    // Summary:
    //     Returns the extension of the specified path string.
    //
    // Parameters:
    //   path:
    //     The path string from which to get the extension.
    //
    // Returns:
    //     The extension of the specified path (including the period "."), or null, or System.String.Empty.
    //     If path is null, System.IO.Path.GetExtension(System.String) returns null. If
    //     path does not have extension information, System.IO.Path.GetExtension(System.String)
    //     returns System.String.Empty.
    //
    // Exceptions:
    //   T:System.ArgumentException:
    //     path contains one or more of the invalid characters defined in System.IO.Path.GetInvalidPathChars.
    static std::string GetExtension(std::string path);
    //
    // Summary:
    //     Returns the file name and extension of the specified path string.
    //
    // Parameters:
    //   path:
    //     The path string from which to obtain the file name and extension.
    //
    // Returns:
    //     The characters after the last directory character in path. If the last character
    //     of path is a directory or volume separator character, this method returns System.String.Empty.
    //     If path is null, this method returns null.
    //
    // Exceptions:
    //   T:System.ArgumentException:
    //     path contains one or more of the invalid characters defined in System.IO.Path.GetInvalidPathChars.
    static std::string GetFileName(std::string path);
    //
    // Summary:
    //     Returns the file name of the specified path string without the extension.
    //
    // Parameters:
    //   path:
    //     The path of the file.
    //
    // Returns:
    //     The string returned by System.IO.Path.GetFileName(System.String), minus the last
    //     period (.) and all characters following it.
    //
    // Exceptions:
    //   T:System.ArgumentException:
    //     path contains one or more of the invalid characters defined in System.IO.Path.GetInvalidPathChars.
    static std::string GetFileNameWithoutExtension(std::string path);
    //
    // Summary:
    //     Returns the absolute path for the specified path string.
    //
    // Parameters:
    //   path:
    //     The file or directory for which to obtain absolute path information.
    //
    // Returns:
    //     The fully qualified location of path, such as "C:\MyFile.txt".
    //
    // Exceptions:
    //   T:System.ArgumentException:
    //     path is a zero-length string, contains only white space, or contains one or more
    //     of the invalid characters defined in System.IO.Path.GetInvalidPathChars.-or-
    //     The system could not retrieve the absolute path.
    //
    //   T:System.Security.SecurityException:
    //     The caller does not have the required permissions.
    //
    //   T:System.ArgumentNullException:
    //     path is null.
    //
    //   T:System.NotSupportedException:
    //     path contains a colon (":") that is not part of a volume identifier (for example,
    //     "c:\").
    //
    //   T:System.IO.PathTooLongException:
    //     The specified path, file name, or both exceed the system-defined maximum length.
    //     For example, on Windows-based platforms, paths must be less than 248 characters,
    //     and file names must be less than 260 characters.
    static std::string GetFullPath(std::string path);
    //
    // Summary:
    //     Gets an array containing the characters that are not allowed in file names.
    //
    // Returns:
    //     An array containing the characters that are not allowed in file names.
    static char* GetInvalidFileNameChars();
    //
    // Summary:
    //     Gets an array containing the characters that are not allowed in path names.
    //
    // Returns:
    //     An array containing the characters that are not allowed in path names.
    static char* GetInvalidPathChars();
    //
    // Summary:
    //     Gets the root directory information of the specified path.
    //
    // Parameters:
    //   path:
    //     The path from which to obtain root directory information.
    //
    // Returns:
    //     The root directory of path, such as "C:\", or null if path is null, or an empty
    //     string if path does not contain root directory information.
    //
    // Exceptions:
    //   T:System.ArgumentException:
    //     path contains one or more of the invalid characters defined in System.IO.Path.GetInvalidPathChars.-or-
    //     System.String.Empty was passed to path.
    static std::string GetPathRoot(std::string path);
    //
    // Summary:
    //     Returns a random folder name or file name.
    //
    // Returns:
    //     A random folder name or file name.
    static std::string GetRandomFileName();
    //
    // Summary:
    //     Creates a uniquely named, zero-byte temporary file on disk and returns the full
    //     path of that file.
    //
    // Returns:
    //     The full path of the temporary file.
    //
    // Exceptions:
    //   T:System.IO.IOException:
    //     An I/O error occurs, such as no unique temporary file name is available.- or
    //     -This method was unable to create a temporary file.
    static std::string GetTempFileName();
    //
    // Summary:
    //     Returns the path of the current user's temporary folder.
    //
    // Returns:
    //     The path to the temporary folder, ending with a backslash.
    //
    // Exceptions:
    //   T:System.Security.SecurityException:
    //     The caller does not have the required permissions.
    static std::string GetTempPath();
    //
    // Summary:
    //     Determines whether a path includes a file name extension.
    //
    // Parameters:
    //   path:
    //     The path to search for an extension.
    //
    // Returns:
    //     true if the characters that follow the last directory separator (\\ or /) or
    //     volume separator (:) in the path include a period (.) followed by one or more
    //     characters; otherwise, false.
    //
    // Exceptions:
    //   T:System.ArgumentException:
    //     path contains one or more of the invalid characters defined in System.IO.Path.GetInvalidPathChars.
    static bool HasExtension(std::string path);
    //
    // Summary:
    //     Gets a value indicating whether the specified path string contains a root.
    //
    // Parameters:
    //   path:
    //     The path to test.
    //
    // Returns:
    //     true if path contains a root; otherwise, false.
    //
    // Exceptions:
    //   T:System.ArgumentException:
    //     path contains one or more of the invalid characters defined in System.IO.Path.GetInvalidPathChars.
    static bool IsPathRooted(std::string path);
};
