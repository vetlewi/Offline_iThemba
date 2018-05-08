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

#ifndef TDRWORDBUFFER_H
#define TDRWORDBUFFER_H

#include <memory>

#include "BufferType.h"

//! Structure type to contain individual decoded TDR words.
//! \author Vetle W. Ingeberg
//! \date 2015-2016
//! \copyright GNU Public License v. 3
typedef struct {
    uint16_t address;		//!< Holds the address of the ADC.
    uint16_t adcdata;		//!< Data read out from the ADC.
    uint16_t cfddata;       //!< Fractional difference of before/after zero-crossing.
    char cfdfail;           //!< Flag to tell if the CFD was forced or not.
    char finishcode;        //!< Pile-up flag.
    int64_t timestamp;		//!< Timestamp in [ns].
    double cfdcorr;         //!< Correction from the CFD.
} word_t;

class WordBuffer : public Buffer<word_t> {
public:
    WordBuffer() : Buffer<word_t>(BUFSIZE, new word_t[BUFSIZE]) { }
    WordBuffer(int sz, word_t *buf ) : Buffer<word_t>(sz, buf) { }
    ~WordBuffer() { delete[] GetBuffer(); }
    WordBuffer *New() { return new WordBuffer(); }
    enum { BUFSIZE = 1024 };
};

#endif // TDRWORDBUFFER_H
