/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_TIMEZONE_HPP
#define FOUNDATIONKIT_TIMEZONE_HPP

#include <string>
#include "FoundationKit/Foundation/DateTime.hpp"
#include "FoundationKit/Foundation/TimeSpan.hpp"
NS_FK_BEGIN

//Defines the period of daylight saving time.
struct DaylightTime
{
   /**
    * Initializes a new instance of the System.Globalization.DaylightTime class with
    * the specified start, end, and time difference information.
    *
    * @param start
    *     The object that represents the date and time when daylight saving time begins.
    *     The value must be in local time.
    *
    * @param end
    *     The object that represents the date and time when daylight saving time ends.
    *     The value must be in local time.
    *
    * @param delta
    *     The object that represents the difference between standard time and daylight
    *     saving time, in ticks.
    */     
    DaylightTime(DateTime start, DateTime end, TimeSpan delta)
        : Start(start)
        , End(end)
        , Delta(delta)
    {

    }


    // The object that represents the date and time when the daylight saving period
    // begins. The value is in local time.
    DateTime Start;

    // The object that represents the date and time when the daylight saving period
    // ends. The value is in local time.
    DateTime End;

    // The time interval that represents the difference between standard time and daylight
    // saving time.
    TimeSpan Delta;
};

class TimeZone
{
public:

    /**
     * Gets the current time zone name.
     */
    static std::string DisplayName();

    /**
     * Gets the daylight saving time zone name.
     */
    static std::string DaylightName();

    /**
     * Gets the standard time zone name.
     */
    static std::string StandardName();

    /**
     * Returns the Coordinated Universal Time (UTC) offset for the specified local time.
     * local time = UTC + UTC Offset + DST Offset
     */
    static TimeSpan GetUTCOffset();

    /**
     * Returns the Coordinated daylight saving time offset for the specified local time.
     * local time = UTC + UTC Offset + DST Offset
     */
    static TimeSpan GetDSTOffset();

    /**
     * Get time zone all info string.
     */
    static std::string ToDebugString();
};


NS_FK_END


#endif // END OF FOUNDATIONKIT_TIMEZONE_HPP
