

/**

// Specifies how the operating system should open a file.
enum class FileMode
{

// Specifies that the operating system should create a new file. This requires System.Security.Permissions.FileIOPermissionAccess.Write
// permission. If the file already exists, an System.IO.IOException exception is
// thrown.
CreateNew = 1,

// Specifies that the operating system should create a new file. If the file already
// exists, it will be overwritten. This requires System.Security.Permissions.FileIOPermissionAccess.Write
// permission. FileMode.Create is equivalent to requesting that if the file does
// not exist, use System.IO.FileMode.CreateNew; otherwise, use System.IO.FileMode.Truncate.
// If the file already exists but is a hidden file, an System.UnauthorizedAccessException
// exception is thrown.
Create = 2,

// Specifies that the operating system should open an existing file. The ability
// to open the file is dependent on the value specified by the System.IO.FileAccess
// enumeration. A System.IO.FileNotFoundException exception is thrown if the file
// does not exist.
Open = 3,

// Specifies that the operating system should open a file if it exists; otherwise,
// a new file should be created. If the file is opened with FileAccess.Read, System.Security.Permissions.FileIOPermissionAccess.Read
// permission is required. If the file access is FileAccess.Write, System.Security.Permissions.FileIOPermissionAccess.Write
// permission is required. If the file is opened with FileAccess.ReadWrite, both
// System.Security.Permissions.FileIOPermissionAccess.Read and System.Security.Permissions.FileIOPermissionAccess.Write
// permissions are required.
OpenOrCreate = 4,

// Specifies that the operating system should open an existing file. When the file
// is opened, it should be truncated so that its size is zero bytes. This requires
// System.Security.Permissions.FileIOPermissionAccess.Write permission. Attempts
// to read from a file opened with FileMode.Truncate cause an System.ArgumentException
// exception.
Truncate = 5,

// Opens the file if it exists and seeks to the end of the file, or creates a new
// file. This requires System.Security.Permissions.FileIOPermissionAccess.Append
// permission. FileMode.Append can be used only in conjunction with FileAccess.Write.
// Trying to seek to a position before the end of the file throws an System.IO.IOException
// exception, and any attempt to read fails and throws a System.NotSupportedException
// exception.
Append = 6
};

// Defines constants for read, write, or read/write access to a file.
class enum FileAccess
{

// Read access to the file. Data can be read from the file. Combine with Write for
// read/write access.
Read = 1,

// Write access to the file. Data can be written to the file. Combine with Read
// for read/write access.
Write = 2,

// Read and write access to the file. Data can be written to and read from the file.
ReadWrite = 3
};

class File
{
public:
    static void AppendAllLines(string path, IEnumerable<string> contents);
    static void AppendAllLines(string path, IEnumerable<string> contents, Encoding encoding);
    static void AppendAllText(string path, string contents);
    static void AppendAllText(string path, string contents, Encoding encoding);
    static void Copy(string sourceFileName, string destFileName);
    static void Copy(string sourceFileName, string destFileName, bool overwrite);
    static FileStream Create(string path);
    static FileStream Create(string path, int bufferSize);
    static FileStream Create(string path, int bufferSize, FileOptions options);
    static FileStream Create(string path, int bufferSize, FileOptions options, FileSecurity fileSecurity);
    static StreamWriter CreateText(string path);
    static void Decrypt(string path);
    static void Delete(string path);
    static void Encrypt(string path);
    static bool Exists(string path);
    static FileSecurity GetAccessControl(string path);
    static FileSecurity GetAccessControl(string path, AccessControlSections includeSections);
    static FileAttributes GetAttributes(string path);
    static DateTime GetCreationTime(string path);
    static DateTime GetCreationTimeUtc(string path);
    static DateTime GetLastAccessTime(string path);
    static DateTime GetLastAccessTimeUtc(string path);
    static DateTime GetLastWriteTime(string path);
    static DateTime GetLastWriteTimeUtc(string path);
    static void Move(string sourceFileName, string destFileName);
    static FileStream Open(string path, FileMode mode);
    static FileStream Open(string path, FileMode mode, FileAccess access);
    static FileStream Open(string path, FileMode mode, FileAccess access, FileShare share);
    static FileStream OpenRead(string path);
    static StreamReader OpenText(string path);
    static FileStream OpenWrite(string path);
    static byte[] ReadAllBytes(string path);
    static string[] ReadAllLines(string path);
    static string[] ReadAllLines(string path, Encoding encoding);
    static string ReadAllText(string path);
    static string ReadAllText(string path, Encoding encoding);
    static IEnumerable<string> ReadLines(string path);
    static IEnumerable<string> ReadLines(string path, Encoding encoding);
    static void Replace(string sourceFileName, string destinationFileName, string destinationBackupFileName);
    static void Replace(string sourceFileName, string destinationFileName, string destinationBackupFileName, bool ignoreMetadataErrors);
    static void SetAccessControl(string path, FileSecurity fileSecurity);
    static void SetAttributes(string path, FileAttributes fileAttributes);
    static void SetCreationTime(string path, DateTime creationTime);
    static void SetCreationTimeUtc(string path, DateTime creationTimeUtc);
    static void SetLastAccessTime(string path, DateTime lastAccessTime);
    static void SetLastAccessTimeUtc(string path, DateTime lastAccessTimeUtc);
    static void SetLastWriteTime(string path, DateTime lastWriteTime);
    static void SetLastWriteTimeUtc(string path, DateTime lastWriteTimeUtc);
    static void WriteAllBytes(string path, byte[] bytes);
    static void WriteAllLines(string path, string[] contents);
    static void WriteAllLines(string path, IEnumerable<string> contents);
    static void WriteAllLines(string path, string[] contents, Encoding encoding);
    static void WriteAllLines(string path, IEnumerable<string> contents, Encoding encoding);
    static void WriteAllText(string path, string contents);
    static void WriteAllText(string path, string contents, Encoding encoding);
};

*/
