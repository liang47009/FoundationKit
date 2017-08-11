/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ALGORITHM_HPP
#define FOUNDATIONKIT_ALGORITHM_HPP

#include <algorithm>
#include "FoundationKit/GenericPlatformMacros.hpp"
NS_FK_BEGIN

   /**
    * min_element
    *
    * min_element finds the smallest element in the range [first, last).
    * It returns the first iterator i in [first, last) such that no other
    * iterator in [first, last) points to a value smaller than *i.
    * The return value is last if and only if [first, last) is an empty range.
    *
    * Returns: The first iterator i in the range [first, last) such that
    * for any iterator j in the range [first, last) the following corresponding
    * condition holds: !(*j < *i).
    *
    * Complexity: Exactly 'max((last - first) - 1, 0)' applications of the
    * corresponding comparisons.
    *
    */
    template <typename ForwardIterator>
    ForwardIterator min_element(ForwardIterator first, ForwardIterator last)
    {
        if (first != last)
        {
            ForwardIterator currentMin = first;
            while (++first != last)
            {
                if (*first < *currentMin)
                    currentMin = first;
            }
            return currentMin;
        }
        return first;
    }

   /**
    * min_element
    *
    * min_element finds the smallest element in the range [first, last). 
    * It returns the first iterator i in [first, last) such that no other 
    * iterator in [first, last) points to a value smaller than *i. 
    * The return value is last if and only if [first, last) is an empty range.
    *
    * Returns: The first iterator i in the range [first, last) such that 
    * for any iterator j in the range [first, last) the following corresponding 
    * conditions hold: compare(*j, *i) == false.
    *
    * Complexity: Exactly 'max((last - first) - 1, 0)' applications of the 
    * corresponding comparisons.
    */
    template <typename ForwardIterator, typename Compare>
    ForwardIterator min_element(ForwardIterator first, ForwardIterator last, Compare compare)
    {
        if (first != last)
        {
            ForwardIterator currentMin = first;

            while (++first != last)
            {
                if (compare(*first, *currentMin))
                    currentMin = first;
            }
            return currentMin;
        }
        return first;
    }

   /**
    * max_element
    *
    * max_element finds the largest element in the range [first, last). 
    * It returns the first iterator i in [first, last) such that no other 
    * iterator in [first, last) points to a value greater than *i. 
    * The return value is last if and only if [first, last) is an empty range.
    *
    * Returns: The first iterator i in the range [first, last) such that 
    * for any iterator j in the range [first, last) the following corresponding 
    * condition holds: !(*i < *j).
    *
    * Complexity: Exactly 'max((last - first) - 1, 0)' applications of the 
    * corresponding comparisons.
    */
    template <typename ForwardIterator>
    ForwardIterator max_element(ForwardIterator first, ForwardIterator last)
    {
        if (first != last)
        {
            ForwardIterator currentMax = first;

            while (++first != last)
            {
                if (*currentMax < *first)
                    currentMax = first;
            }
            return currentMax;
        }
        return first;
    }

   /**
    * max_element
    *
    * max_element finds the largest element in the range [first, last). 
    * It returns the first iterator i in [first, last) such that no other 
    * iterator in [first, last) points to a value greater than *i. 
    * The return value is last if and only if [first, last) is an empty range.
    *
    * Returns: The first iterator i in the range [first, last) such that 
    * for any iterator j in the range [first, last) the following corresponding 
    * condition holds: compare(*i, *j) == false.
    *
    * Complexity: Exactly 'max((last - first) - 1, 0)' applications of the 
    * corresponding comparisons.
    */
    template <typename ForwardIterator, typename Compare>
    ForwardIterator max_element(ForwardIterator first, ForwardIterator last, Compare compare)
    {
        if (first != last)
        {
            ForwardIterator currentMax = first;

            while (++first != last)
            {
                if (compare(*currentMax, *first))
                    currentMax = first;
            }
            return currentMax;
        }
        return first;
    }


NS_FK_END

#endif // END OF FOUNDATIONKIT_ALGORITHM_HPP
