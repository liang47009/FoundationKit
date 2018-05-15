
#include "IFileHandle.hpp"
NS_FK_BEGIN

/**
 * File handle.
 **/
class FileHandle : public IFileHandle
{
public:

    FileHandle(FILE* Handle);

    /** Destructor, also the only way to close the file handle **/
    virtual ~FileHandle();


    /** Return the current write or read position. **/
    virtual int64		Tell() override;

    /**
    * Change the current write or read position.
    * @param NewPosition	new write or read position
    * @return				true if the operation completed successfully.
    **/
    virtual bool		Seek(int64 NewPosition) override;

    /**
    * Change the current write or read position, relative to the end of the file.
    * @param NewPositionRelativeToEnd	new write or read position, relative to the end of the file should be <=0!
    * @return							true if the operation completed successfully.
    **/
    virtual bool		SeekFromEnd(int64 NewPositionRelativeToEnd = 0) override;
    virtual bool        SeekFromCur(int64 NewPositionRelativeToCur = 0) override;

    /**
    * Read bytes from the file.
    * @param Destination	Buffer to holds the results, should be at least BytesToRead in size.
    * @param BytesToRead	Number of bytes to read into the destination.
    * @return				true if the operation completed successfully.
    **/
    virtual bool		Read(uint8* Destination, int64 BytesToRead) override;

    /**
    * Write bytes to the file.
    * @param Source		Buffer to write, should be at least BytesToWrite in size.
    * @param BytesToWrite	Number of bytes to write.
    * @return				true if the operation completed successfully.
    **/
    virtual bool		Write(const uint8* Source, int64 BytesToWrite)override;

    /**
    * Flushes file.
    **/
    virtual void		Flush() override;
protected:
    FILE* NativeFileHandle;
};


NS_FK_END