/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_FOUNDATIONKITVERSION_HPP
#define FOUNDATIONKIT_FOUNDATIONKITVERSION_HPP

#define FOUNDATIONKIT_VERSION_MAJOR 1
#define FOUNDATIONKIT_VERSION_MINOR 0
#define FOUNDATIONKIT_VERSION_BUILD 0
#define FOUNDATIONKIT_VERSION_REVISION 0
#define FOUNDATIONKIT_VERSION_FULL "1.0.0.0"

// Version as a single hex number, e.g. 0x01000300 == 1.0.3
#define FOUNDATIONKIT_VERSION_HEX ((FOUNDATIONKIT_VERSION_MAJOR << 24) | \
                                   (FOUNDATIONKIT_VERSION_MINOR << 16) | \
                                   (FOUNDATIONKIT_VERSION_BUILD <<  8))

#define FOUNDATIONKIT_VERSION_STRING "Foundation Version:" FOUNDATIONKIT_VERSION_FULL

#endif // END OF FOUNDATIONKIT_FOUNDATIONKITVERSION_HPP