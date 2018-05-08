/*******************************************************************************
 * Copyright (C) 2016 Vetle W. Ingeberg                                        *
 * Author: Vetle Wegner Ingeberg, v.w.ingeberg@fys.uio.no                      *
 *                                                                             *
 * --------------------------------------------------------------------------- *
 * This program is free software; you can redistribute it and/or modify it     *
 * under the terms of the GNU General Public License as published by the       *
 * Free Software Foundation; either version 3 of the license, or (at your      *
 * option) any later version.                                                  *
 *                                                                             *
 * This program is distributed in the hope that it will be useful, but         *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                  *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General   *
 * Public License for more details.                                            *
 *                                                                             *
 * You should have recived a copy of the GNU General Public License along with *
 * the program. If not, see <http://www.gnu.org/licenses/>.                    *
 *                                                                             *
 *******************************************************************************/

/*!
 * \file RateMeter.cpp
 * \brief Implementation of RateMeter.
 * \author Vetle W. Ingeberg
 * \author unknown
 * \date ?-2016
 * \copyright GNU Public License v. 3
 */

#include "RateMeter.h"

#include <algorithm>

RateMeter::RateMeter(int dfl, bool fxd)
    : default_delta( dfl )
    , fixed( fxd )
{
    Reset();
}

// ########################################################################

void RateMeter::Reset()
{
    count = 0;
    delta = default_delta;
    printit = delta;
    gettimeofday(&last, 0);
    start = last;
}

// ########################################################################

float RateMeter::RateCalculation()
{
    if( ++count < printit )
        return -1;

    const float timediff = Timediff(last);
    const float rate = delta / timediff;
    delta = fixed ? default_delta : (int)std::max(1.0f, std::min(10000.0f, 1/timediff));
    printit += delta;

    return rate;
}

// ########################################################################

float RateMeter::TotalRate()
{
    timeval s = start;
    return count / Timediff( s );
}

// ########################################################################

float RateMeter::Timediff(timeval& old)
{
    timeval now;
    gettimeofday(&now, 0);

    const double o = old.tv_sec + old.tv_usec*1e-6;
    const double n = now.tv_sec + now.tv_usec*1e-6;

    old = now;

    // always return a value > 0, even if it is not the correct time
    // difference
    return std::max( n-o, 1e-12 );
}
