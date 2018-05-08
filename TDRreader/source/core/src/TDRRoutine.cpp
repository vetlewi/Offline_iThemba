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
 * \file TDRRoutine.cpp
 * \brief Implementation of TDRRoutine.
 * \author Vetle W. Ingeberg
 * \version 0.8.0
 * \date 2015-2016
 * \copyright GNU Public License v. 3
 */


#include "TDRRoutine.h"

#include "Event.h"
#include "Histogram1D.h"
#include "Histogram2D.h"

#include <sstream>
#include <iostream>


TDRRoutine::TDRRoutine()
{ }

bool TDRRoutine::Start()
{
	CreateSpectra();
	return true;
}

bool TDRRoutine::End()
{
	return true;
}

bool TDRRoutine::Command(const std::string& cmd)
{
    std::istringstream icmd(cmd.c_str());

    std::string name;

    icmd >> name;

    if (name == "range" || name == "RANGE" ) {
        std::string fname;
        icmd >> fname;
        range.Read(fname);
        return true;
    } else {
        //std::cerr << "TDRRoutine: Unknown command '" << cmd << "'\n";
        return UserCommand(cmd);
    }
    return false;
}


