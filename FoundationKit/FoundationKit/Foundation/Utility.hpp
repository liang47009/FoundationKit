/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_UTILITY_HPP
#define FOUNDATIONKIT_UTILITY_HPP

template<typename dst_type, typename src_type>
dst_type any_pointer_cast(src_type src)
{
    return *static_cast<dst_type*>(static_cast<void*>(&src));
}

#endif // END OF FOUNDATIONKIT_UTILITY_HPP