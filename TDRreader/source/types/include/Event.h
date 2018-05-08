/*******************************************************************************
 * Copyright (C) 2016-2018 Vetle W. Ingeberg                                   *
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

#ifndef EVENT_H
#define EVENT_H

#include <memory>

#include <cstdint>

#include "experimentsetup.h"
#include "WordBuffer.h"
#include "XIA_CFD.h"

class WordBuffer;


//! Object to contain built events.
//! \author Vetle W. Ingeberg
//! \date 2017-2018
//! \copyright GNU Public License v. 3
struct Event {

    word_t w_clover[NUM_CLOVER_DETECTORS][NUM_CLOVER_CRYSTALS][MAX_WORDS_PER_DET];  //!< Array to contain CLOVER words
    int n_clover[NUM_CLOVER_DETECTORS][NUM_CLOVER_CRYSTALS];                        //!< Number of CLOVER words populated
    int tot_clover;                                                                 //!< Total number of CLOVER words in the event

    word_t w_labr[NUM_LABR_DETECTORS][MAX_WORDS_PER_DET];   //!< Array to contain LaBr words
    int n_labr[NUM_LABR_DETECTORS];                         //!< Number of LaBr words populated
    int tot_labr;                                           //!< Total number of LaBr words in the event

    word_t w_dEdet_ring[NUM_SI_DE_RING][MAX_WORDS_PER_DET];       //!< Array to contain Si words from the dE rings
    int n_dEdet_ring[NUM_SI_DE_RING];                            //!< Number of Si words populated from the dE rings
    int tot_dEdet_ring;                                          //!< Total number of Si words from the dE rings

    word_t w_dEdet_sect[NUM_SI_DE_SECT][MAX_WORDS_PER_DET];       //!< Array to contain Si words from the dE rings
    int n_dEdet_sect[NUM_SI_DE_SECT];                            //!< Number of Si words populated from the dE rings
    int tot_dEdet_sect;                                          //!< Total number of Si words from the dE rings

    word_t w_Edet[NUM_SI_E_DET][MAX_WORDS_PER_DET];         //!< Array to contain Si words from the dE sectors
    int n_Edet[NUM_SI_E_DET];                               //!< Number of Si words populated from the dE sectors
    int tot_Edet;                                           //!< Total number of Si words from the dE sectors

    word_t w_RFpulse[MAX_WORDS_PER_DET];                    //!< Array to contain RF pulse words
    int n_RFpulse;                                          //!< Number of RF pulses populated

    int length;  //! Total length of the event (in no. of words)

    word_t trigger;     //! This is the word that "triggers" the event.

      //! Constructor
    Event() { Reset(); }

    //! Destructor
    ~Event(){ Reset(); }

    //! Pack the event with data.
    void PackEvent(const WordBuffer *buffer, int start, int stop);

    //! Set all counters to zero
    void Reset()
    {
        int i, j;

        // Clearing all Clover counters
        tot_clover = 0;
        for (i = 0 ; i < NUM_CLOVER_DETECTORS ; ++i){
            for (j = 0 ; j < NUM_CLOVER_DETECTORS ; ++j)
                n_clover[i][j] = 0;
        }

        // Clearing all LaBr counters
        tot_labr = 0;
        for (i = 0 ; i < NUM_LABR_DETECTORS ; ++i) {
            n_labr[i] = 0;
        }

        // Clearing dE rings
        tot_dEdet_ring = 0;
        for (i = 0 ; i < NUM_SI_DE_RING ; ++i)
            n_dEdet_ring[i] = 0;

        // Clearing dE sectors
        tot_dEdet_sect = 0;
        for (i = 0 ; i < NUM_SI_DE_SECT ; ++i)
            n_dEdet_sect[i] = 0;

        // Clearing E detectors
        tot_Edet = 0;
        for (i = 0 ; i < NUM_SI_E_DET ; ++i)
            n_Edet[i] = 0;

        // Clearing RF pulses
        n_RFpulse = 0;

        // Resetting event length.
        length = 0;
    }

};



#endif // EVENT_H
