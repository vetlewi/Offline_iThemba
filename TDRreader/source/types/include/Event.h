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
    int tot_clover[NUM_CLOVER_DETECTORS];                                           //!< Total number of CLOVER words in the event

    word_t w_labr_3x8[NUM_LABR_3X8_DETECTORS][MAX_WORDS_PER_DET];   //!< Array to contain 3.5x8 LaBr words
    int n_labr_3x8[NUM_LABR_3X8_DETECTORS];                         //!< Number of 3.5x8 LaBr words populated
    int tot_labr_3x8;                                               //!< Total number of 3.5x8 LaBr words in the event

    word_t w_labr_2x2_ss[NUM_LABR_2X2_DETECTORS][MAX_WORDS_PER_DET];    //!< Array to contain 2x2 LaBr words, slow signal
    int n_labr_2x2_ss[NUM_LABR_2X2_DETECTORS];                          //!< Number of 2x2 LaBr words populated, slow signal
    int tot_labr_2x2_ss;                                                //!< Total number of 2x2 LaBr words in the event, slow signal

    word_t w_labr_2x2_fs[NUM_LABR_2X2_DETECTORS][MAX_WORDS_PER_DET];    //!< Array to contain 2x2 LaBr words, fast signal
    int n_labr_2x2_fs[NUM_LABR_2X2_DETECTORS];                          //!< Number of 2x2 LaBr words populated, fast signal
    int tot_labr_2x2_fs;                                                //!< Total number of 2x2 LaBr words in the event, fast signal

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
        for (i = 0 ; i < NUM_CLOVER_DETECTORS ; ++i){
            tot_clover[i] = 0;
            for (j = 0 ; j < NUM_CLOVER_DETECTORS ; ++j)
                n_clover[i][j] = 0;
        }

        // Clearing all 3.5x8 LaBr counters
        tot_labr_3x8 = 0;
        for (i = 0 ; i < NUM_LABR_3X8_DETECTORS ; ++i) {
            n_labr_3x8[i] = 0;
        }

        // Clearing all 2x2 LaBr counters, slow signal
        tot_labr_2x2_ss = 0;
        for (i = 0 ; i < NUM_LABR_2X2_DETECTORS ; ++i) {
            n_labr_2x2_ss[i] = 0;
        }

        // Clearing all 2x2 LaBr counters, fast signal
        tot_labr_2x2_fs = 0;
        for (i = 0 ; i < NUM_LABR_2X2_DETECTORS ; ++i) {
            n_labr_2x2_fs[i] = 0;
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
