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
 * \file Unpacker.cpp
 * \brief Implementation of Unpacker.
 * \author Vetle W. Ingeberg
 * \version 0.8.0
 * \date 2015-2016
 * \copyright GNU Public License v. 3
 */

#include "Unpacker.h"

#include "WordBuffer.h"
#include "Event.h"
#include "experimentsetup.h"
#include "DefineFile.h"

#include <iostream>
#include <string>
#include <cstdlib>

#define GAP_SIZE 3000

Unpacker::Unpacker()
	: buffer( 0 )
	, buffer_idx( 0 )
	, eventlength_sum( 0 )
	, event_count ( 0 )
{
}

void Unpacker::SetBuffer(const WordBuffer* buffr)
{
    buffer = buffr; // Setting internal variable with the new buffer.


    buffer_idx = 0; // Reset buffer position.
    curr_Buf = 0; // Reseting the position in the buffer.

    // Resting the length of the buffer.
	event_count = eventlength_sum = 0;
}

Unpacker::Status Unpacker::Next(Event &event)
{
    if ( curr_Buf >= buffer->GetSize() )
        return END; // End of buffer reached.

	int n_data = 0;
    if ( !UnpackOneEvent(event, n_data) ){
        buffer_idx = n_data + 1;
        return END; // If no event found, end of buffer.
	}

    eventlength_sum += event.length;
	event_count += 1;
    buffer_idx = n_data + 1;

	return OKAY;
}
#if SINGLES
/*bool Unpacker::UnpackOneEvent(Event& event, int& n_data)
{

    event.Reset();

        if ( curr_Buf >= buffer->GetSize() )
            return false;

        int64_t timediff;
        int start = curr_Buf;
        int stop = curr_Buf+1;
        subevent_t sevt1 = subevent_t((*buffer)[curr_Buf]), sevt2;
        for (size_t i = curr_Buf + 1 ; i < buffer->GetSize() ; ++i){
            sevt2 = sevt1;
            sevt1 = subevent_t((*buffer)[i]);
            timediff = sevt1.timestamp - sevt2.timestamp;
            if (timediff > GAP_SIZE){
                stop = i;
                curr_Buf = stop;
                return PackEvent(event, start, stop);
            }
        }

        stop = buffer->GetSize();
        curr_Buf = stop;
        event.PackEvent(event, start, stop);
        return true;
}*/

bool Unpacker::UnpackOneEvent(Event& event, int& n_data)
{

    event.Reset();

        if ( curr_Buf >= buffer->GetSize() )
            return false;

        int64_t timediff;
        int start = curr_Buf;
        int stop = curr_Buf+1;

        for (int i = curr_Buf ; i < buffer->GetSize() ; ++i){
            timediff = (*buffer)[i+1].timestamp - (*buffer)[curr_Buf].timestamp;
            if (timediff > GAP_SIZE){
                stop = i+1;
                break;
            }
        }
        curr_Buf = stop;
        event.PackEvent(buffer, start, stop);
        return true;
}
#else
bool Unpacker::UnpackOneEvent(Event& event, int& n_data)
{
    event.Reset();

    if ( curr_Buf >= buffer->GetSize() )
        return false;

    int64_t timediff;
    DetectorInfo_t dinfo;
    word_t cWord, cword_2;
    for (int i =  curr_Buf ; i < buffer->GetSize() ; ++i){
        cWord = (*buffer)[i];
        dinfo = GetDetector(cWord.address);
        if (GetDetector(cWord.address).type == eDet){
            int start = i;
            int stop = i+1;

            event.trigger = cWord;

            // We will move back a given number of words and check timestamps
            for (int j = i ; j > 0 ; --j){
                cword_2 = (*buffer)[j-1];
                timediff = cword_2.timestamp - cWord.timestamp;
                if (abs(timediff) > 1500){
                    start = j;
                    break;
                }
            }

            for (int j = i ; j < buffer->GetSize() - 1 ; ++j){
                cword_2 = (*buffer)[j+1];
                timediff = cword_2.timestamp - cWord.timestamp;
                if (abs(timediff) > 1500){
                    stop = j+1;
                    break;
                }
            }

            curr_Buf = i+1;
            n_data = stop-start;
            event.PackEvent(buffer, start, stop);
            return true;
        }
    }

    curr_Buf = buffer->GetSize();
    return false;
}
#endif // SINGLES
