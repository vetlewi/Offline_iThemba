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

#include "UserSort.h"

#include "DefineFile.h"

#include "WordBuffer.h"
#include "Histogram1D.h"
#include "Histogram2D.h"
#include "Histograms.h"
#include "Event.h"


#include <string>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <fstream>

#include <stdint.h>
#include <stdlib.h>

#define FISSION 0

static bool set_par(Parameters& parameters, std::istream& ipar,
                    const std::string& name, int size)
{
    Parameter* p = parameters.Find(name);
    if( !p )
        return false;

    std::vector<Parameter::param_t> values;
    double tmp;
    for(int i=0; i<size; ++i) {
        tmp = 0;
        ipar >> tmp;
        values.push_back(tmp);
    }
    p->Set( values );

    return true;
}

static bool set_gainshift(Parameters& parameters, std::istream& ipar)
{
    bool r = set_par(parameters, ipar, "gain_clover", NUM_CLOVER_DETECTORS*NUM_CLOVER_CRYSTALS );
    r = ( r && set_par(parameters, ipar, "shift_clover",    NUM_CLOVER_DETECTORS*NUM_CLOVER_CRYSTALS ) );
    r = ( r && set_par(parameters, ipar, "gain_labr_3x8",    NUM_LABR_3X8_DETECTORS ) );
    r = ( r && set_par(parameters, ipar, "shift_labr_3x8",   NUM_LABR_3X8_DETECTORS ) );
    r = ( r && set_par(parameters, ipar, "gain_labr_2x2_ss",    NUM_LABR_2X2_DETECTORS ) );
    r = ( r && set_par(parameters, ipar, "shift_labr_2x2_ss",   NUM_LABR_2X2_DETECTORS ) );
    r = ( r && set_par(parameters, ipar, "gain_labr_2x2_fs",    NUM_LABR_2X2_DETECTORS ) );
    r = ( r && set_par(parameters, ipar, "shift_labr_2x2_fs",   NUM_LABR_2X2_DETECTORS ) );
    r = ( r && set_par(parameters, ipar, "gain_de_ring", NUM_SI_DE_RING ) );
    r = ( r && set_par(parameters, ipar, "shift_de_ring", NUM_SI_DE_RING ) );
    r = ( r && set_par(parameters, ipar, "gain_de_sect", NUM_SI_DE_SECT ) );
    r = ( r && set_par(parameters, ipar, "shift_de_sect", NUM_SI_DE_SECT ) );
    r = ( r && set_par(parameters, ipar, "gain_e", NUM_SI_E_DET ) );
    r = ( r && set_par(parameters, ipar, "shift_e", NUM_SI_E_DET ) );
    r = ( r && set_par(parameters, ipar, "shift_time_clover", NUM_CLOVER_DETECTORS*NUM_CLOVER_CRYSTALS) );
    r = ( r && set_par(parameters, ipar, "shift_time_labr_3x8", NUM_LABR_3X8_DETECTORS ) );
    r = ( r && set_par(parameters, ipar, "shift_time_labr_2x2_ss", NUM_LABR_2X2_DETECTORS ) );
    r = ( r && set_par(parameters, ipar, "shift_time_labr_2x2_fs", NUM_LABR_2X2_DETECTORS ) );
    r = ( r && set_par(parameters, ipar, "shift_time_de_ring", NUM_SI_DE_RING ) );
    r = ( r && set_par(parameters, ipar, "shift_time_de_sect", NUM_SI_DE_SECT ) );
    r = ( r && set_par(parameters, ipar, "shift_time_e", NUM_SI_E_DET ) );
    return r;
}

UserSort::UserSort()
    : gain_clover( GetParameters(), "gain_clover", NUM_CLOVER_DETECTORS*NUM_CLOVER_CRYSTALS, 1)
    , shift_clover(GetParameters(), "shift_clover", NUM_CLOVER_DETECTORS*NUM_CLOVER_CRYSTALS, 0)
    , gain_labr_3x8( GetParameters(), "gain_labr_3x8", NUM_LABR_3X8_DETECTORS, 1)
    , shift_labr_3x8( GetParameters(), "shift_labr_3x8", NUM_LABR_3X8_DETECTORS, 0)
    , gain_labr_2x2_ss( GetParameters(), "gain_labr_2x2_ss", NUM_LABR_2X2_DETECTORS, 1)
    , shift_labr_2x2_ss( GetParameters(), "shift_labr_2x2_ss", NUM_LABR_2X2_DETECTORS, 0)
    , gain_labr_2x2_fs( GetParameters(), "gain_labr_2x2_fs", NUM_LABR_2X2_DETECTORS, 1)
    , shift_labr_2x2_fs( GetParameters(), "shift_labr_2x2_fs", NUM_LABR_2X2_DETECTORS, 0)
    , gain_dE_ring( GetParameters(), "gain_de_ring", NUM_SI_DE_RING, 1)
    , shift_dE_ring( GetParameters(), "shift_de_ring", NUM_SI_DE_RING, 0)
    , gain_dE_sect( GetParameters(), "gain_de_sect", NUM_SI_DE_SECT, 1)
    , shift_dE_sect( GetParameters(), "shift_de_sect", NUM_SI_DE_SECT, 0)
    , gain_E( GetParameters(), "gain_e", NUM_SI_E_DET, 1)
    , shift_E( GetParameters(), "shift_e", NUM_SI_E_DET, 0)
    , shift_time_clover( GetParameters(), "shift_time_clover", NUM_CLOVER_DETECTORS*NUM_CLOVER_CRYSTALS, 0)
    , shift_time_labr_3x8( GetParameters(), "shift_time_labr_3x8", NUM_LABR_3X8_DETECTORS, 0)
    , shift_time_labr_2x2_ss( GetParameters(), "shift_time_labr_2x2_ss", NUM_LABR_2X2_DETECTORS, 0)
    , shift_time_labr_2x2_fs( GetParameters(), "shift_time_labr_2x2_fs", NUM_LABR_2X2_DETECTORS, 0)
    , shift_time_de_ring( GetParameters(), "shift_time_de_ring", NUM_SI_DE_RING, 0)
    , shift_time_de_sect( GetParameters(), "shift_time_de_sect", NUM_SI_DE_SECT, 0)
    , shift_time_e( GetParameters(), "shift_time_e", NUM_SI_E_DET, 0)
    , ex_from_ede    ( GetParameters(), "ex_from_ede", NUM_SI_RINGS*3)
    , thick_range    ( GetParameters(), "thick_range", 2      )
    , labr_3x8_time_cuts  ( GetParameters(), "labr_3x8_time_cuts", 2*2  )
    , labr_2x2_ss_time_cuts  ( GetParameters(), "labr_2x2_ss_time_cuts", 2*2  )
    , labr_2x2_fs_time_cuts  ( GetParameters(), "labr_2x2_fs_time_cuts", 2*2  )
    , particle_ring_cuts ( GetParameters(), "particle_ring_cuts", 2*2 )
    , particle_sect_cuts ( GetParameters(), "particle_sect_cuts", 2*2 )
{
}


double UserSort::CalibrateE(const word_t &w) const
{
    DetectorInfo_t info = GetDetector(w.address);
    switch ( info.type ) {
    case clover : {
        return gain_clover[info.detectorNum*NUM_CLOVER_DETECTORS + info.telNum]*(w.adcdata + drand48() - 0.5) + shift_clover[info.detectorNum*NUM_CLOVER_DETECTORS + info.telNum];
    }
    case labr_3x8 : {
        return gain_labr_3x8[info.detectorNum]*(w.adcdata + drand48() - 0.5) + shift_labr_3x8[info.detectorNum];
    }
    case labr_2x2_ss : {
        return gain_labr_2x2_ss[info.detectorNum]*(w.adcdata + drand48() - 0.5) + shift_labr_2x2_ss[info.detectorNum];
    }
    case labr_2x2_fs : {
        return gain_labr_2x2_fs[info.detectorNum]*(w.adcdata + drand48() - 0.5) + shift_labr_2x2_fs[info.detectorNum];
    }
    case deDet_ring : {
        return gain_dE_ring[info.detectorNum]*(w.adcdata + drand48() - 0.5) + shift_dE_ring[info.detectorNum];
    }
    case deDet_sect : {
        return gain_dE_sect[info.detectorNum]*(w.adcdata + drand48() - 0.5) + shift_dE_sect[info.detectorNum];
    }
    case eDet : {
        return gain_E[info.detectorNum]*(w.adcdata + drand48() - 0.5) + shift_E[info.detectorNum];
    }

    case rfchan : {
        return w.adcdata;
    }
    default:
        return w.adcdata;
    }
}

double UserSort::CalcTimediff(const word_t &start, const word_t &stop) const
{
    // First we fetch the correct shift parameters.
    double start_shift, stop_shift;

    DetectorInfo_t info_start = GetDetector(start.address);
    DetectorInfo_t info_stop = GetDetector(stop.address);

    switch ( info_start.type ){
        case clover : {
            start_shift = shift_time_clover[info_start.detectorNum*NUM_CLOVER_DETECTORS + info_start.telNum];
            break;
        }
        case labr_3x8 : {
            start_shift = shift_time_labr_3x8[info_start.detectorNum];
            break;
        }
        case labr_2x2_ss : {
            start_shift = shift_time_labr_2x2_ss[info_start.detectorNum];
            break;
        }
        case labr_2x2_fs : {
            start_shift = shift_time_labr_2x2_fs[info_start.detectorNum];
            break;
        }
        case deDet_ring : {
            start_shift = shift_time_de_ring[info_start.detectorNum];
            break;
        }
        case deDet_sect : {
            start_shift = shift_time_de_sect[info_start.detectorNum];
            break;
        }
        case eDet : {
            start_shift = shift_time_e[info_start.detectorNum];
            break;
        }
        default : {
            start_shift = 0;
            break;
        }
    }

    switch ( info_stop.type ){
        case clover : {
            stop_shift = shift_time_clover[info_start.detectorNum*NUM_CLOVER_DETECTORS + info_start.telNum];
            break;
        }
        case labr_3x8 : {
            stop_shift = shift_time_labr_3x8[info_start.detectorNum];
            break;
        }
        case labr_2x2_ss : {
            stop_shift = shift_time_labr_2x2_ss[info_start.detectorNum];
            break;
        }
        case labr_2x2_fs : {
            stop_shift = shift_time_labr_2x2_fs[info_start.detectorNum];
            break;
        }
        case deDet_ring : {
            stop_shift = shift_time_de_ring[info_stop.detectorNum];
            break;
        }
        case deDet_sect : {
            stop_shift = shift_time_de_sect[info_stop.detectorNum];
            break;
        }
        case eDet : {
            stop_shift = shift_time_e[info_stop.detectorNum];
            break;
        }
        default : {
            stop_shift = 0;
            break;
        }
    }

    // 'Coarse' time difference.
    int64_t diff_coarse = stop.timestamp - start.timestamp;

    // 'Fine' time difference
    double diff_fine = stop.cfdcorr -start.cfdcorr;

    // Actual time difference.
    double diff = diff_coarse + diff_fine;

    // Now we add shift of stop time and subtract shift of start time.
    diff += (stop_shift - start_shift);

    return diff;

}


bool UserSort::UserCommand(const std::string &cmd)
{
    std::istringstream icmd(cmd.c_str());

    std::string name, tmp;
    icmd >> name;

    if ( name == "Gain" ) {
        icmd >> tmp;
        if ( tmp == "file" ) {
            std::string filename;
            icmd >> filename;
            std::ifstream gainfile(filename.c_str());
            if ( !set_gainshift(GetParameters(), gainfile) ){
                std::cerr << __PRETTY_FUNCTION__ << ", gain file: error reading '" << filename << "'" << std::endl;
                return false;
            }
        } else if ( tmp=="data" ){
            if ( !set_gainshift(GetParameters(), icmd) ){
                std::cerr << __PRETTY_FUNCTION__ << ", gain data: error reading calibration data" << std::endl;
                return false;
            }
        } else {
            std::cerr << __PRETTY_FUNCTION__ << ", gain: Expected 'file' or 'data', not '" << tmp << "'" << std::endl;
        }
    } else if ( name == "parameter" ){
        return GetParameters().SetAll(icmd);
    } else {
        return false;
    }
    return true;
}


void UserSort::CreateSpectra()
{
    char tmp[1024], tmp2[1024];;

    for (int i = 0 ; i < NUM_CLOVER_DETECTORS ; ++i){
        for (int j = 0 ; j < NUM_CLOVER_CRYSTALS ; ++j){

            sprintf(tmp, "energy_clover_raw_%02d%c", i+1, 'A'+j);
            energy_clover_raw[i][j] = Spec(tmp, tmp, 32768, 0, 32768, "Energy [ch]");

            sprintf(tmp, "energy_clover_%02d%c", i+1, 'A'+j);
            energy_clover[i][j] = Spec(tmp, tmp, 10000, 0, 10000, "Energy [keV]");
        }
    }

    // Allocating the 3.5x8 LaBr 'singles' spectra
    for (int i = 0 ; i < NUM_LABR_3X8_DETECTORS ; ++i){

        // Create energy spectra
        sprintf(tmp, "energy_raw_labr_3x8_%02d", i+1);
        energy_labr_3x8_raw[i] = Spec(tmp, tmp, 32768, 0, 32768, "Energy [ch]");

        sprintf(tmp, "energy_labr_3x8_%02d", i+1);
        energy_labr_3x8[i] = Spec(tmp, tmp, 10000, 0, 10000, "Energy [keV]");
    }

    // Allocating the 2x2 LaBr 'singles' spectra
    for (int i = 0 ; i < NUM_LABR_2X2_DETECTORS ; ++i){

        // Create energy spectra
        sprintf(tmp, "energy_raw_labr_2x2_ss_%02d", i+1);
        energy_labr_2x2_ss_raw[i] = Spec(tmp, tmp, 32768, 0, 32768, "Energy [ch]");
        sprintf(tmp, "energy_raw_labr_2x2_fs_%02d", i+1);
        energy_labr_2x2_fs_raw[i] = Spec(tmp, tmp, 32768, 0, 32768, "Energy [ch]");

        sprintf(tmp, "energy_labr_2x2_ss_%02d", i+1);
        energy_labr_2x2_ss[i] = Spec(tmp, tmp, 10000, 0, 10000, "Energy [keV]");
        sprintf(tmp, "energy_labr_2x2_fs_%02d", i+1);
        energy_labr_2x2_fs[i] = Spec(tmp, tmp, 10000, 0, 10000, "Energy [keV]");
    }

    // Allocating the dE ring 'singles' spectra
    for (int i = 0 ; i < NUM_SI_DE_RING ; ++i){

        // Create energy spectra
        sprintf(tmp, "energy_raw_dE_ring_%02d", i);
        energy_dE_ring_raw[i] = Spec(tmp, tmp, 32768, 0, 32768, "Energy [ch]");

        sprintf(tmp, "energy_dE_ring_%02d", i);
        energy_dE_ring[i] = Spec(tmp, tmp, 10000, 0, 10000, "Energy [keV]");

        sprintf(tmp, "h_ede_ring_%02d", i+1);
        h_ede_ring[i] = Spec(tmp, tmp, 1000, 0, 20000, "Energy [keV]");
    }

    // Allocating the dE sector 'singles' spectra
    for (int i = 0 ; i < NUM_SI_DE_SECT ; ++i){

        // Create energy spectra
        sprintf(tmp, "energy_raw_dE_sect_%02d", i);
        energy_dE_sect_raw[i] = Spec(tmp, tmp, 32768, 0, 32768, "Energy [ch]");

        sprintf(tmp, "energy_dE_sect_%02d", i);
        energy_dE_sect[i] = Spec(tmp, tmp, 10000, 0, 10000, "Energy [keV]");
    }

    // Allocating the LaBr 'singles' spectra
    for (int i = 0 ; i < NUM_SI_E_DET ; ++i){

        // Create energy spectra
        sprintf(tmp, "energy_raw_E_%02d", i);
        energy_E_raw[i] = Spec(tmp, tmp, 32768, 0, 32768, "Energy [ch]");

        sprintf(tmp, "energy_E_%02d", i);
        energy_E[i] = Spec(tmp, tmp, 10000, 0, 10000, "Energy [keV]");
    }

    // Making all spectra that are indexed [NUM_SI_E_DET]
    for (int i = 0 ; i < NUM_SI_E_DET ; ++i){

        // e_de_time spectrum
        //sprintf(tmp, "e_de_time_%02d", i);
        //e_de_time[i] = Mat(tmp, tmp, 3000, -1500, 1500, "Time t_{dE} - t_{E} [ns]", NUM_SI_RINGS, 0, NUM_SI_RINGS, "Ring number");

        // Making all spectra indexed [NUM_SI_E_DET][NUM_SI_RINGS].
        for (int j = 0 ; j < NUM_SI_RINGS ; ++j){

            // Make the 'raw' ede spectrum.
            sprintf(tmp, "ede_raw_b%02d_f%02d", i, j);
            sprintf(tmp2, "E : DE raw, pad %d, ring %d", i, j);
            ede_raw[i][j] = Mat(tmp, tmp2, 500, 0, 16384, "Back energy [ch]", 500, 0, 16384, "Front energy [ch]");

            // Make 'calibrated' ede spectrum.
            sprintf(tmp, "ede_b%02d_f%02d", i, j);
            sprintf(tmp2, "E : DE calibrated, pad %d, ring %d", i, j);
            ede[i][j] = Mat(tmp, tmp2, 500, 0, 20000, "Back energy [keV]", 500, 0, 15000, "Front energy [keV]");

            // Make total energy spectra.
            sprintf(tmp, "h_ede_b%02d_f%02d", i, j);
            sprintf(tmp2, "Total energy deposited, pad %d, ring %d", i, j);
            h_ede[i][j] = Spec(tmp, tmp2, 20000, 0, 20000, "Total energy deposited [keV]");


            // Make excitation spectra.
            sprintf(tmp, "h_ex_b%02d_f%02d", i, j);
            sprintf(tmp2, "Singles excitation spectrum, pad %d, ring %d", i, j);
            h_ex[i][j] = Spec(tmp, tmp2, 20000, 0, 20000, "Excitation energy [keV]");
        }
    }

    int num_align_bins = 3000;
    Axis::bin_t align_low_bin = -1500.0;
    Axis::bin_t align_hig_bin = 1500.0;

    // Alignment spectra (for time alignment of all detector to a common detector)
    for (int i = 0 ; i < NUM_CLOVER_DETECTORS ; ++i){
        sprintf(tmp, "align_time_clover_%02d", i+1);
        align_time_clover[i] = Mat(tmp, tmp, num_align_bins, align_low_bin, align_hig_bin, "t_{CLOVER} - t_{common} [ns]", NUM_CLOVER_CRYSTALS, 0, NUM_CLOVER_CRYSTALS, "CLOVER crystal id.");
    }

    sprintf(tmp, "align_time_labr_3x8");
    align_time_labr_3x8 = Mat(tmp, tmp, num_align_bins, align_low_bin, align_hig_bin, "t_{LaBr 3.5x8} - t_{common} [ns]", NUM_LABR_3X8_DETECTORS, 0, NUM_LABR_3X8_DETECTORS, "LaBr 3.5x8 id.");

    sprintf(tmp, "align_time_labr_2x2_ss");
    align_time_labr_2x2_ss = Mat(tmp, tmp, num_align_bins, align_low_bin, align_hig_bin, "t_{LaBr 2x2 (slow)} - t_{common} [ns]", NUM_LABR_2X2_DETECTORS, 0, NUM_LABR_2X2_DETECTORS, "LaBr 2x2 id.");

    sprintf(tmp, "align_time_labr_2x2_fs");
    align_time_labr_2x2_fs = Mat(tmp, tmp, num_align_bins, align_low_bin, align_hig_bin, "t_{LaBr 2x2 (fast)} - t_{common} [ns]", NUM_LABR_2X2_DETECTORS, 0, NUM_LABR_2X2_DETECTORS, "LaBr 2x2 id.");

    sprintf(tmp, "align_time_de_ring");
    align_time_de_ring = Mat(tmp, tmp, num_align_bins, align_low_bin, align_hig_bin, "t_{#Delta E ring} - t_{common} [ns]", NUM_SI_DE_RING, 0, NUM_SI_DE_RING, "#Delta E ring detector id.");

    sprintf(tmp, "align_time_de_sect");
    align_time_de_sect = Mat(tmp, tmp, num_align_bins, align_low_bin, align_hig_bin, "t_{#Delta E sector} - t_{common} [ns]", NUM_SI_DE_SECT, 0, NUM_SI_DE_SECT, "#Delta E sector detector id.");

    sprintf(tmp, "align_time_e");
    align_time_e = Mat(tmp, tmp, num_align_bins, align_low_bin, align_hig_bin, "t_{E} - t_{common} [ns]", NUM_SI_E_DET, 0, NUM_SI_E_DET, "E detector id.");

    sprintf(tmp, "prompt_de_ring");
    prompt_de_ring = Mat(tmp, tmp, num_align_bins, align_low_bin, align_hig_bin, "t_{#Delta E ring} - t_{E trigger} [ns]", NUM_SI_DE_RING, 0, NUM_SI_DE_RING, "#Delta E ring detector id.");

    sprintf(tmp, "prompt_de_sect");
    prompt_de_sect = Mat(tmp, tmp, num_align_bins, align_low_bin, align_hig_bin, "t_{#Delta E sector} - t_{E trigger} [ns]", NUM_SI_DE_SECT, 0, NUM_SI_DE_SECT, "#Delta E sector detector id.");

    sprintf(tmp, "ede_all");
    sprintf(tmp2, "E : DE, all");
    ede_all = Mat(tmp, tmp2, 4000, 0, 20000, "Back energy [keV]", 1000, 0, 5000, "Front energy [keV]");

    sprintf(tmp, "ede_gate");
    sprintf(tmp2, "E : DE, after particle gate");
    ede_gate = Mat(tmp, tmp2, 1000, 0, 20000, "Back energy [keV]", 250, 0, 5000, "Front energy [keV]");

    sprintf(tmp, "h_thick");
    sprintf(tmp2, "Apparent thickness of #Delta E");
    h_thick = Spec(tmp, tmp2, 3000, 0, 3000, "Apparent thickness [#mu m]");

    sprintf(tmp, "h_ede_all");
    sprintf(tmp2, "Total particle energy, all");
    h_ede_all = Spec(tmp, tmp2, 20000, 0, 20000, "Particle energy [keV]");

    sprintf(tmp, "h_ex_all");
    sprintf(tmp2, "Excitation energy, all");
    h_ex_all = Spec(tmp, tmp2, 20000, 0, 20000, "Excitation energy [keV]");

    sprintf(tmp, "alfna");
    alfna = Mat(tmp, tmp, 1500, 0, 15000, "LaBr [keV]", 1600, -1000, 15000, "Ex [keV]");

    sprintf(tmp, "alfna_bg");
    alfna_bg = Mat(tmp, tmp, 1500, 0, 15000, "LaBr [keV]", 1600, -1000, 15000, "Ex [keV]");

    n_fail_e = 0;
    n_fail_de_ring = 0;
    n_fail_de_sect = 0;
    n_tot_e = 0;
    n_tot_de_ring = 0;
    n_tot_de_sect = 0;
    tot = 0;
}


bool UserSort::Sort(const Event &event)
{
    int i, j, k;
    double energy;
    double tdiff;

    n_tot_e += event.tot_Edet;
    n_tot_de_ring += event.tot_dEdet_ring;
    n_tot_de_sect += event.tot_dEdet_sect;
    tot += 1;

    // For time alignments.
    bool have_alginment_data = ( event.n_labr_2x2_fs[0] == 1 );
    word_t start_alignment_word = event.w_labr_2x2_fs[0][0];

    word_t de_words[256]; // List of dE hits from pads in front of the trigger E word.
    int n_de_words=0;

    for ( i = 0 ; i < NUM_CLOVER_DETECTORS ; ++i ){
        for ( j = 0 ; j < NUM_CLOVER_CRYSTALS ; ++j ){
            for (k = 0 ; k < event.n_clover[i][j] ; ++k){
                energy_clover_raw[i][j]->Fill(event.w_clover[i][j][k].adcdata);
                energy = CalibrateE(event.w_clover[i][j][k]);
                energy_clover[i][j]->Fill(energy);

                if (have_alginment_data){
                    tdiff = CalcTimediff(start_alignment_word, event.w_clover[i][j][k]);
                    align_time_clover[i]->Fill(tdiff, j);
                }
            }
        }
    }

    // First fill some 'singles' spectra.
    for ( i = 0 ; i < NUM_LABR_3X8_DETECTORS ; ++i ){
        for ( j = 0 ; j < event.n_labr_3x8[i] ; ++j ){
            energy_labr_3x8_raw[i]->Fill(event.w_labr_3x8[i][j].adcdata);
            energy = CalibrateE(event.w_labr_3x8[i][j]);
            energy_labr_3x8[i]->Fill(energy);

            // We align times
            if (have_alginment_data){
                tdiff = CalcTimediff(start_alignment_word, event.w_labr_3x8[i][j]);
                align_time_labr_3x8->Fill(tdiff, i);
            }
        }
    }

    for ( i = 0 ; i < NUM_LABR_2X2_DETECTORS ; ++i ){
        for ( j = 0 ; j < event.n_labr_2x2_ss[i] ; ++j ){
            energy_labr_2x2_ss_raw[i]->Fill(event.w_labr_2x2_ss[i][j].adcdata);
            energy = CalibrateE(event.w_labr_2x2_ss[i][j]);
            energy_labr_2x2_ss[i]->Fill(energy);
            // We align times
            if (have_alginment_data){
                tdiff = CalcTimediff(start_alignment_word, event.w_labr_2x2_ss[i][j]);
                align_time_labr_2x2_ss->Fill(tdiff, i);
            }
        }
        for ( j = 0 ; j < event.n_labr_2x2_fs[i] ; ++j ){
            energy_labr_2x2_fs_raw[i]->Fill(event.w_labr_2x2_fs[i][j].adcdata);
            energy = CalibrateE(event.w_labr_2x2_fs[i][j]);
            energy_labr_2x2_fs[i]->Fill(energy);
            // We align times
            if (have_alginment_data){
                tdiff = CalcTimediff(start_alignment_word, event.w_labr_2x2_fs[i][j]);
                align_time_labr_2x2_fs->Fill(tdiff, i);
            }
        }
    }

    for ( i = 0 ; i < NUM_SI_DE_RING ; ++i ){
        for ( j = 0 ; j < event.n_dEdet_ring[i] ; ++j ){
            energy_dE_ring_raw[i]->Fill(event.w_dEdet_ring[i][j].adcdata);
            energy = CalibrateE(event.w_dEdet_ring[i][j]);
            energy_dE_ring[i]->Fill(energy);

            // We align times
            if (have_alginment_data){
                tdiff = CalcTimediff(start_alignment_word, event.w_dEdet_ring[i][j]);
                align_time_de_ring->Fill(tdiff, i);
            }

            // Check if bellongs to the 'trigger' event
            tdiff = CalcTimediff(event.w_dEdet_ring[i][j], event.trigger);
            prompt_de_ring->Fill(tdiff, i);
            if ( CheckTimeStatus(tdiff, particle_ring_cuts) ){
                de_words[n_de_words++] = event.w_dEdet_ring[i][j];
            }

            if (event.w_dEdet_ring[i][j].cfdfail > 0) // For 'statistical' purposes!
                ++n_fail_de_ring;
        }
    }

    for ( i = 0 ; i < NUM_SI_DE_SECT ; ++i ){
        for ( j = 0 ; j < event.n_dEdet_sect[i] ; ++j ){
            energy_dE_sect_raw[i]->Fill(event.w_dEdet_sect[i][j].adcdata);
            energy = CalibrateE(event.w_dEdet_sect[i][j]);
            energy_dE_sect[i]->Fill(energy);

            // We align times
            if (have_alginment_data){
                tdiff = CalcTimediff(start_alignment_word, event.w_dEdet_sect[i][j]);
                align_time_de_sect->Fill(tdiff, i);
            }

            // Check if bellongs to the 'trigger' event
            tdiff = CalcTimediff(event.w_dEdet_sect[i][j], event.trigger);
            prompt_de_sect->Fill(tdiff, i);
            if ( CheckTimeStatus(tdiff, particle_sect_cuts) ){
                de_words[n_de_words++] = event.w_dEdet_sect[i][j];
            }

            if (event.w_dEdet_sect[i][j].cfdfail > 0) // For 'statistical' purposes!
                ++n_fail_de_sect;
        }
    }

    for ( i = 0 ; i < NUM_SI_E_DET ; ++i ){
        for ( j = 0 ; j < event.n_Edet[i] ; ++j ){
            energy_E_raw[i]->Fill(event.w_Edet[i][j].adcdata);
            energy = CalibrateE(event.w_Edet[i][j]);
            energy_E[i]->Fill(energy);

            // We align times
            if (have_alginment_data){
                tdiff = CalcTimediff(start_alignment_word, event.w_Edet[i][j]);
                align_time_e->Fill(tdiff, i);
            }

            if (event.w_Edet[i][j].cfdfail > 0) // For 'statistical' purposes!
                ++n_fail_e;
        }
    }

    // The total number of ring events have to be one or has to be correlated in
    // time with the E detector... Maybe later... For now only one!
    /*if ( event.tot_dEdet_ring == 1 && event.tot_dEdet_sect == 1 ){
        for (int i = 0 ; i < NUM_SI_DE_RING ; ++i){
            for (int j = 0 ; j < event.n_dEdet_ring[i] ; ++j){
                de_words[0] = event.w_dEdet_ring[i][j];
            }
        }
        for (int i = 0 ; i < NUM_SI_DE_SECT ; ++i){
            for (int j = 0 ; j < event.n_dEdet_sect[i] ; ++j){
                de_words[1] = event.w_dEdet_sect[i][j];
            }
        }
        n_de_words = 1;
    }*/



    // Check if only one dE detector of the trapezoidal has fired.
    if ( n_de_words == 2 ){

        word_t e_word = event.trigger;
        word_t de_word_r = de_words[0];
        word_t de_word_s = de_words[1];

        int ring = GetDetector(de_word_r.address).detectorNum;
        int sect = GetDetector(de_word_s.address).detectorNum;
        int sect_e = GetDetector(e_word.address).detectorNum;

        double e_energy = CalibrateE(e_word);
        double de_energy = CalibrateE(de_word_s);

        ede_raw[sect_e][ring]->Fill(e_word.adcdata, de_word_s.adcdata);

        if (!(ring < 16 || ring > 27))
            return true;

        ede[sect_e][ring]->Fill(e_energy, de_energy);
        ede_all->Fill(e_energy, de_energy);

        double thick = range.GetRange(e_energy + de_energy) - range.GetRange(e_energy);

        h_thick->Fill(thick);

        if (thick >= thick_range[0] && thick <= thick_range[1]){
            h_ede[sect_e][ring]->Fill(e_energy + de_energy);
            h_ede_ring[ring]->Fill(e_energy+de_energy);
        }


    }

    return true;

}


void UserSort::AnalyzeGamma(const word_t &de_word, const double &excitation,const Event &event)
{
/*
    // We will loop over all gamma-rays.
    for (int i = 0 ; i < NUM_LABR_DETECTORS ; ++i){
        for (int j = 0 ; j < event.n_labr[i] ; ++j){

            // Get energy and time of the gamma-ray.

            double energy = CalibrateE(event.w_labr[i][j]);
            double tdiff = CalcTimediff(de_word, event.w_labr[i][j]);

            // Fill time spectra.
            labr_align_time->Fill(tdiff, i);
            energy_time_labr[i]->Fill(energy, tdiff);
            energy_time_labr_all->Fill(energy, tdiff);


            // Check time gate.
            switch ( CheckTimeStatus(tdiff, labr_time_cuts) ) {
                case is_prompt : {
                    alfna->Fill(energy, excitation);
                    break;
                }
                case is_background : {
                    alfna->Fill(energy, excitation, -1);
                    alfna_bg->Fill(energy, excitation);
                    break;
                }
                case ignore : {
                    break;
                }
            }
        }
    }
*/
    return;
}





UserSort::prompt_status_t UserSort::CheckTimeStatus(const double &time, const Parameter &parameter) const
{
    if ( time > parameter[0] && time < parameter[1])
        return is_prompt;
    else if (time > parameter[2] && time < parameter[3])
        return is_background;
    else
        return ignore;
}

bool UserSort::End()
{
    std::cout << "Stats info: " << std::endl;
    std::cout << "CFD fails in E - detectors: " << n_fail_e << std::endl;
    std::cout << "CFD fails in dE ring - detectors: " << n_fail_de_ring << std::endl;
    std::cout << "CFD fails in dE sector - detectors: " << n_fail_de_sect << std::endl;
    std::cout << "Average number of dE ring words: " << n_tot_de_ring/double(tot) << std::endl;
    std::cout << "Average number of dE sector words: " << n_tot_de_sect/double(tot) << std::endl;
    std::cout << "Average number of E words: " << n_tot_e/double(tot) << std::endl;
    return true;
}
