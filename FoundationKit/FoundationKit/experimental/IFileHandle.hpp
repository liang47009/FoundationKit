/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_IFILEHANDLE_HPP
#define FOUNDATIONKIT_IFILEHANDLE_HPP

#include "FoundationKit/Base/types.hpp"
NS_FK_BEGIN


//enum class FileMode
//{
//    ReadOnly = 0,
//    ReadWrite,
//
//};
//
//enum class FileType
//{
//    Text = 0,
//    Binary
//};

/**
 * File handle interface.
 **/
class IFileHandle
{
public:
    /** Destructor, also the only way to close the file handle **/
    virtual ~IFileHandle()
    {
    }

   /** Return the current write or read position. **/
    virtual int64		Tell() = 0;

   /**
    * Change the current write or read position.
    * @param NewPosition	new write or read position
    * @return				true if the operation completed successfully.
    **/
    virtual bool		Seek(int64 NewPosition) = 0;

   /**
    * Change the current write or read position, relative to the end of the file.
    * @param NewPositionRelativeToEnd	new write or read position, relative to the end of the file should be <=0!
    * @return							true if the operation completed successfully.
    **/
    virtual bool		SeekFromEnd(int64 NewPositionRelativeToEnd = 0) = 0;
    virtual bool        SeekFromCur(int64 NewPositionRelativeToCur = 0) = 0;

   /**
    * Read bytes from the file.
    * @param Destination	Buffer to holds the results, should be at least BytesToRead in size.
    * @param BytesToRead	Number of bytes to read into the destination.
    * @return				true if the operation completed successfully.
    **/
    virtual bool		Read(uint8* Destination, int64 BytesToRead) = 0;

   /**
    * Write bytes to the file.
    * @param Source		Buffer to write, should be at least BytesToWrite in size.
    * @param BytesToWrite	Number of bytes to write.
    * @return				true if the operation completed successfully.
    **/
    virtual bool		Write(const uint8* Source, int64 BytesToWrite) = 0;

   /**
    * Flushes file.
    **/
    virtual void		Flush() { };

public:
    /////////// Utility Functions. These have a default implementation that uses the pure virtual operations.

    /** Return the total size of the file **/
    virtual int64		Size()
    {
        int64 Current = Tell();
        SeekFromEnd();
        int64 Result = Tell();
        Seek(Current);
        return Result;
    }
};

NS_FK_END


#endif // END OF FOUNDATIONKIT_IFILEHANDLE_HPP
