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

#include <TFile.h>
#include <TTree.h>

typedef struct {
    word_t ring_event;
    word_t sect_event;
} delta_e_event_t;

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
    , clover_time_cuts( GetParameters(), "clover_time_cuts", 2*2  )
    , labr_2x2_ss_time_cuts  ( GetParameters(), "labr_2x2_ss_time_cuts", 2*2  )
    , labr_2x2_fs_time_cuts  ( GetParameters(), "labr_2x2_fs_time_cuts", 2*2  )
    , clover_addback_cuts( GetParameters(), "clover_addback_cuts", 2*2 )
    , particle_ring_cuts ( GetParameters(), "particle_ring_cuts", 2*2 )
    , particle_sect_cuts ( GetParameters(), "particle_sect_cuts", 2*2 )
    , particle_sect_ring_cuts( GetParameters(), "particle_sect_ring_cuts", 2*2 )
{
}


double UserSort::CalibrateE(const word_t &w) const
{
    DetectorInfo_t info = GetDetector(w.address);
    double result = 0;
    switch ( info.type ) {
    case clover : {
        result = gain_clover[info.detectorNum*NUM_CLOVER_CRYSTALS + info.telNum]*(w.adcdata + drand48() - 0.5) + shift_clover[info.detectorNum*NUM_CLOVER_CRYSTALS + info.telNum];
        break;
    }
    case labr_3x8 : {
        result = gain_labr_3x8[info.detectorNum]*(w.adcdata + drand48() - 0.5) + shift_labr_3x8[info.detectorNum];
        break;
    }
    case labr_2x2_ss : {
        result = gain_labr_2x2_ss[info.detectorNum]*(w.adcdata + drand48() - 0.5) + shift_labr_2x2_ss[info.detectorNum];
        break;
    }
    case labr_2x2_fs : {
        result = gain_labr_2x2_fs[info.detectorNum]*(w.adcdata + drand48() - 0.5) + shift_labr_2x2_fs[info.detectorNum];
        break;
    }
    case deDet_ring : {
        result = gain_dE_ring[info.detectorNum]*(w.adcdata + drand48() - 0.5) + shift_dE_ring[info.detectorNum];
        break;
    }
    case deDet_sect : {
        result = gain_dE_sect[info.detectorNum]*(w.adcdata + drand48() - 0.5) + shift_dE_sect[info.detectorNum];
        break;
    }
    case eDet : {
        result = gain_E[info.detectorNum]*(w.adcdata + drand48() - 0.5) + shift_E[info.detectorNum];
        break;
    }
    case rfchan : {
        result = w.adcdata;
        break;
    }
    default:
        result = w.adcdata;
        break;
    }
    return result;
}

double UserSort::CalcTimediff(const word_t &start, const word_t &stop) const
{
    // First we fetch the correct shift parameters.
    double start_shift, stop_shift;

    DetectorInfo_t info_start = GetDetector(start.address);
    DetectorInfo_t info_stop = GetDetector(stop.address);

    switch ( info_start.type ){
        case clover : {
            start_shift = shift_time_clover[info_start.detectorNum*NUM_CLOVER_CRYSTALS + info_start.telNum];
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
            stop_shift = shift_time_clover[info_stop.detectorNum*NUM_CLOVER_CRYSTALS + info_stop.telNum];
            break;
        }
        case labr_3x8 : {
            stop_shift = shift_time_labr_3x8[info_stop.detectorNum];
            break;
        }
        case labr_2x2_ss : {
            stop_shift = shift_time_labr_2x2_ss[info_stop.detectorNum];
            break;
        }
        case labr_2x2_fs : {
            stop_shift = shift_time_labr_2x2_fs[info_stop.detectorNum];
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

bool UserSort::SetupTree()
{
    // First we need to check that a file is actually mounted.
    if ( !root_file )
        return false;

    root_tree = new TTree("data","data");

    root_tree->Branch("eDet_ID", &eDet_ID, "eDet_ID/I");
    root_tree->Branch("eDet_energy",&eDet_energy,"eDet_energy/D");
    root_tree->Branch("eDet_timestamp_coarse", &eDet_timestamp_course, "eDet_timestamp_course/L");
    root_tree->Branch("eDet_timestamp_fine", &eDet_timestamp_fine, "eDet_timestamp_fine/D");

    root_tree->Branch("deDet_mult", &deDet_mult, "deDet_mult/I");
    root_tree->Branch("deDet_ringID", &deDet_ringID, "deDet_ringID[deDet_mult]/I");
    root_tree->Branch("deDet_sectID", &deDet_sectID, "deDet_sectID[deDet_mult]/I");
    root_tree->Branch("deDet_ring_energy",&deDet_ring_energy,"deDet_ring_energy[deDet_mult]/D");
    root_tree->Branch("deDet_sect_energy",&deDet_sect_energy,"deDet_sect_energy[deDet_mult]/D");
    root_tree->Branch("deDet_ring_timestamp_coarse",&deDet_ring_timestamp_coarse,"deDet_ring_timestamp_coarse[deDet_mult]/L");
    root_tree->Branch("deDet_sect_timestamp_coarse",&deDet_sect_timestamp_coarse,"deDet_sect_timestamp_coarse[deDet_mult]/L");
    root_tree->Branch("deDet_ring_timestamp_fine",&deDet_ring_timestamp_fine,"deDet_ring_timestamp_fine[deDet_mult]/D");
    root_tree->Branch("deDet_sect_timestamp_fine",&deDet_sect_timestamp_fine,"deDet_sect_timestamp_fine[deDet_mult]/D");

    root_tree->Branch("labr3x8_mult",&labr3x8_mult,"labr3x8_mult/I");
    root_tree->Branch("labr3x8_ID",&labr3x8_ID,"labr3x8_ID[labr3x8_mult]/I");
    root_tree->Branch("labr3x8_energy",&labr3x8_energy,"labr3x8_energy[labr3x8_mult]/D");
    root_tree->Branch("labr3x8_timestamp_coarse",&labr3x8_timestamp_coarse,"labr3x8_timestamp_coarse[labr3x8_mult]/L");
    root_tree->Branch("labr3x8_timestamp_fine",&labr3x8_timestamp_fine,"labr3x8_timestamp_fine[labr3x8_mult]/D");

    root_tree->Branch("labr2x2_mult",&labr2x2_mult,"labr2x2_mult/I");
    root_tree->Branch("labr2x8_ID",&labr2x2_ID,"labr2x2_ID[labr2x2_mult]/I");
    root_tree->Branch("labr2x2_energy",&labr2x2_energy,"labr2x2_energy[labr2x2_mult]/D");
    root_tree->Branch("labr2x2_timestamp_coarse",&labr2x2_timestamp_coarse,"labr2x2_timestamp_coarse[labr2x2_mult]/L");
    root_tree->Branch("labr2x2_timestamp_fine",&labr2x2_timestamp_fine,"labr2x2_timestamp_fine[labr2x2_mult]/D");

    root_tree->Branch("clover_mult",&clover_mult,"clover_mult/I");
    root_tree->Branch("clover_ID",&clover_ID,"clover_ID[clover_mult]/I");
    root_tree->Branch("clover_energy",&clover_energy,"clover_energy[clover_mult]/D");
    root_tree->Branch("clover_timestamp_coarse",&clover_timestamp_coarse,"clover_timestamp_coarse[clover_mult]/L");
    root_tree->Branch("clover_timestamp_fine",&clover_timestamp_fine,"clover_timestamp_fine[clover_mult]/D");

    //root_tree->SetAutoFlush(-3000);
    is_opt = false;
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

        sprintf(tmp, "h_ex_r%02d", i);
        h_ex[i] = Spec(tmp, tmp, 10000, 0, 10000, "Excitation energy [keV]");
    }

    // Allocating the dE sector 'singles' spectra
    for (int i = 0 ; i < NUM_SI_DE_SECT ; ++i){

        // Create energy spectra
        sprintf(tmp, "energy_raw_dE_sect_%02d", i);
        energy_dE_sect_raw[i] = Spec(tmp, tmp, 32768, 0, 32768, "Energy [ch]");

        sprintf(tmp, "energy_dE_sect_%02d", i);
        energy_dE_sect[i] = Spec(tmp, tmp, 10000, 0, 10000, "Energy [keV]");

        sprintf(tmp, "time_ring_sect_%02d", i);
        time_ring_sect[i] = Mat(tmp, tmp, 1000, -500, 500, "Timediff. [ns]", NUM_SI_DE_RING, 0, NUM_SI_DE_RING, "Ring no.");
    }

    // Allocating the LaBr 'singles' spectra
    for (int i = 0 ; i < NUM_SI_E_DET ; ++i){

        // Create energy spectra
        sprintf(tmp, "energy_raw_E_%02d", i);
        energy_E_raw[i] = Spec(tmp, tmp, 32768, 0, 32768, "Energy [ch]");

        sprintf(tmp, "energy_E_%02d", i);
        energy_E[i] = Spec(tmp, tmp, 10000, 0, 10000, "Energy [keV]");
    }

    int num_bin_ede = 50;
    Axis::bin_t e_axis_low = 0;
    Axis::bin_t e_axis_high = 18000;
    Axis::bin_t de_axis_low = 0;
    Axis::bin_t de_axis_high = 10000;

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
            ede_raw[i][j] = Mat(tmp, tmp2, num_bin_ede, e_axis_low, e_axis_high, "Back energy [ch]",
                                           num_bin_ede, de_axis_low, de_axis_high, "Front energy [ch]");

            // Make 'calibrated' ede spectrum.
            sprintf(tmp, "ede_b%02d_f%02d", i, j);
            sprintf(tmp2, "E : DE calibrated, pad %d, ring %d", i, j);
            ede[i][j] = Mat(tmp, tmp2, num_bin_ede, e_axis_low, e_axis_high, "Back energy [keV]",
                                       num_bin_ede, de_axis_low, de_axis_high, "Front energy [keV]");

            // Make 'semi-calibrated' ede spectrum (for calibration of sectors)
            sprintf(tmp, "ede_s%02d_r%02d", i, j);
            ede_raw_sect[i][j] = Mat(tmp, tmp, num_bin_ede, e_axis_low, e_axis_high, "E energy [keV]",
                                               num_bin_ede, de_axis_low, de_axis_high, "#Delta E sector energy [ch]");
        }
    }

    int num_align_bins = 3000;
    Axis::bin_t align_low_bin = -1500.0;
    Axis::bin_t align_hig_bin = 1500.0;

    // Notes to my self:
    // 1) Sector 06 will have to be manually calibrated
    // 2) ---- || ---- for sector 9

    // Alignment spectra (for time alignment of all detector to a common detector)
    for (int i = 0 ; i < NUM_CLOVER_DETECTORS ; ++i){
        sprintf(tmp, "align_time_clover_%02d", i+1);
        align_time_clover[i] = Mat(tmp, tmp, num_align_bins, align_low_bin, align_hig_bin, "t_{CLOVER} - t_{common} [ns]", NUM_CLOVER_CRYSTALS, 0, NUM_CLOVER_CRYSTALS, "CLOVER crystal id.");

        sprintf(tmp, "time_clover_addback_%02d", i+1);
        time_clover_addback[i] = Mat(tmp, tmp, num_align_bins, align_low_bin, align_hig_bin, "t_{CLOVER} - t_{start} [ns]", NUM_CLOVER_CRYSTALS, 0, NUM_CLOVER_CRYSTALS, "CLOVER crystal id.");

        sprintf(tmp, "energy_clover_addback_%02d", i+1);
        energy_clover_addback[i] = Spec(tmp, tmp, 10000, 0, 10000, "Energy [keV]");
    }

    sprintf(tmp, "align_time_labr_3x8");
    align_time_labr_3x8 = Mat(tmp, tmp, num_align_bins, align_low_bin, align_hig_bin, "t_{LaBr 3.5x8} - t_{common} [ns]", NUM_LABR_3X8_DETECTORS, 0, NUM_LABR_3X8_DETECTORS, "LaBr 3.5x8 id.");

    sprintf(tmp, "align_time_labr_2x2_ss");
    align_time_labr_2x2_ss = Mat(tmp, tmp, num_align_bins, align_low_bin, align_hig_bin, "t_{LaBr 2x2 (slow)} - t_{common} [ns]", NUM_LABR_2X2_DETECTORS, 0, NUM_LABR_2X2_DETECTORS, "LaBr 2x2 id.");

    sprintf(tmp, "align_time_labr_2x2_fs");
    align_time_labr_2x2_fs = Mat(tmp, tmp, num_align_bins*10, align_low_bin, align_hig_bin, "t_{LaBr 2x2 (fast)} - t_{common} [ns]", NUM_LABR_2X2_DETECTORS, 0, NUM_LABR_2X2_DETECTORS, "LaBr 2x2 id.");

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

    sprintf(tmp, "prompt_labr_3x8");
    prompt_labr_3x8 = Mat(tmp, tmp, num_align_bins, align_low_bin, align_hig_bin, "t_{LaBr} - t_{Particle} [ns]", NUM_LABR_3X8_DETECTORS, 0, NUM_LABR_3X8_DETECTORS, "LaBr 3.5x8 detector id.");

    sprintf(tmp, "ede_all");
    sprintf(tmp2, "E : DE, all");
    ede_all = Mat(tmp, tmp2, 4000, 0, 20000, "Back energy [keV]", 1000, 0, 10000, "Front energy [keV]");

    sprintf(tmp, "h_thick");
    h_thick = Mat(tmp, tmp, 1000, 0, 1000, "Apparent thickness [um]", 1000, 0, 18000, "E energy [keV]");

    sprintf(tmp, "ede_gate");
    sprintf(tmp2, "E : DE, after particle gate");
    ede_gate = Mat(tmp, tmp2, 1000, 0, 20000, "Back energy [keV]", 1000, 0, 10000, "Front energy [keV]");

    sprintf(tmp, "alfna");
    alfna = Mat(tmp, tmp, 1500, 0, 15000, "LaBr [keV]", 1600, -1000, 15000, "Ex [keV]");

    sprintf(tmp, "alfna_bg");
    alfna_bg = Mat(tmp, tmp, 1500, 0, 15000, "LaBr [keV]", 1600, -1000, 15000, "Ex [keV]");

    sprintf(tmp, "alfna_clover");
    alfna_clover = Mat(tmp, tmp, 1500, 0, 15000, "CLOVER [keV]", 1600, -1000, 15000, "Ex [keV]");

    sprintf(tmp, "alfna_clover_bg");
    alfna_clover_bg = Mat(tmp, tmp, 1500, 0, 15000, "CLOVER [keV]", 1600, -1000, 15000, "Ex [keV]");

    sprintf(tmp, "time_energy_e");
    time_energy_e = Mat(tmp, tmp, 1000, 0, 16000, "Energy [keV]", 2000, -200, 200, "Time t_{#Delta E} - t_{E} [ns]");

    sprintf(tmp, "h_ex_all");
    h_ex_all = Spec(tmp, tmp, 10000, 0, 10000, "Excitation energy [keV]");

    sprintf(tmp, "prompt_clover");
    prompt_clover = Mat(tmp, tmp, 2000, -1000, 1000, "Time [ns]", NUM_CLOVER_DETECTORS, 0, NUM_CLOVER_DETECTORS, "CLOVER detector no.");

    n_fail_e = 0;
    n_fail_de_ring = 0;
    n_fail_de_sect = 0;
    n_tot_e = 0;
    n_tot_de_ring = 0;
    n_tot_de_sect = 0;
    tot = 0;
}

int UserSort::Addback_Clover(const Event &event, const word_t &align_time, clover_gamma_t *clover_gamma)
{
    int i, j, k, n_clover_gamma=0;
    double energy, tdiff;

    bool have_been_counted[NUM_CLOVER_DETECTORS][NUM_CLOVER_CRYSTALS][MAX_WORDS_PER_DET];
        for (i = 0 ; i < NUM_CLOVER_DETECTORS ; ++i){
            for (j = 0 ; j < NUM_CLOVER_CRYSTALS ; ++j){
                for (k = 0 ; k < MAX_WORDS_PER_DET ; ++k){
                    have_been_counted[i][j][k] = false;
                }
            }
        }

    for ( i = 0 ; i < NUM_CLOVER_DETECTORS ; ++i ){

        if (event.tot_clover[i] == 1){ // Only single, fill time and energy spectra as usual.

            for (j = 0 ; j < NUM_CLOVER_CRYSTALS ; ++j ){
                for ( k = 0 ; k < event.n_clover[i][j] ; ++k ){
                    if ( have_been_counted[i][j][k] ) // This should never be true, but check anyway
                        continue; // Skip to next word.
                    have_been_counted[i][j][k] = true;
                    if ( event.w_clover[i][j][k].adcdata >= 16383 )
                        continue;
                    clover_gamma[n_clover_gamma].clover_no = i;
                    clover_gamma[n_clover_gamma].crystal_no = j;
                    clover_gamma[n_clover_gamma].energy = CalibrateE(event.w_clover[i][j][k]);
                    clover_gamma[n_clover_gamma++].max_w = event.w_clover[i][j][k];
                    energy_clover_addback[i]->Fill(CalibrateE(event.w_clover[i][j][k]));

                }
            }

        } else { // Addback! We fill 'singles' spectra for the individual crystals and the 'addback' spectra!

            for ( j = 0 ; j < NUM_CLOVER_CRYSTALS ; ++j ){
                for ( k = 0 ; k < event.n_clover[i][j] ; ++k ){

                    if ( event.w_clover[i][j][k].adcdata >= 16383 ){
                        have_been_counted[i][j][k] = true;
                        continue;
                    }

                    // First we check if we have analyzed this particular word previously.
                    if ( have_been_counted[i][j][k] )
                        continue; // Skip to next word.
                    have_been_counted[i][j][k] = true;
                    energy = CalibrateE(event.w_clover[i][j][k]);
                    double max_energy = energy;
                    int max_c = j;
                    word_t max_word = event.w_clover[i][j][k];

                    for ( int n = 0 ; n < NUM_CLOVER_CRYSTALS ; ++n ){
                        if ( n == j )
                            continue;
                        for (  int m = 0 ; m < event.n_clover[i][n] ; ++m ){

                            // Increment the time spectrum!
                            tdiff = CalcTimediff(event.w_clover[i][j][k], event.w_clover[i][n][m]);
                            time_clover_addback[i]->Fill(tdiff, n);

                            if ( event.w_clover[i][n][m].adcdata >= 16383 ){
                                have_been_counted[i][n][m] = true;
                                continue;
                            }

                            if ( have_been_counted[i][n][m] ) // We always want to increment the time spectra, but not energies
                                continue; // skip to next!

                            if ( CheckTimeStatus(tdiff, clover_addback_cuts) == is_prompt ){
                                double this_e = CalibrateE(event.w_clover[i][n][m]);
                                energy += this_e;
                                if (this_e > max_energy){
                                    max_c = n;
                                    max_word = event.w_clover[i][n][m];
                                }
                                have_been_counted[i][n][m] = true;
                            }
                        }
                    }

                    energy_clover_addback[i]->Fill(energy);
                    clover_gamma[n_clover_gamma].clover_no = i;
                    clover_gamma[n_clover_gamma].crystal_no = max_c;
                    clover_gamma[n_clover_gamma].energy = energy;
                    clover_gamma[n_clover_gamma++].max_w = max_word;
                }
            }
        }
    }
    return n_clover_gamma;
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

    eDet_ID = GetDetector(event.trigger.address).detectorNum;
    eDet_energy = CalibrateE(event.trigger);
    eDet_timestamp_course = event.trigger.timestamp;
    eDet_timestamp_fine = event.trigger.timestamp + shift_time_e[eDet_ID];
    deDet_mult = 0;
    labr3x8_mult = 0;
    clover_mult = 0;

    // For time alignments.
    bool have_alginment_data = ( event.n_labr_2x2_fs[0] == 1 );
    word_t start_alignment_word = event.w_labr_2x2_fs[0][0];
    //bool have_alginment_data = true;//( GetDetector(event.trigger.address).detectorNum == 0 );
    //word_t start_alignment_word = event.trigger;
    //if (have_alginment_data == false)
    //    start_alignment_word.address = 0;

    delta_e_event_t de_events[256];
    int n_de_events = 0;

    clover_gamma_t clover_gamma[256];
    int n_clover_gamma = 0;

    n_clover_gamma = Addback_Clover(event, start_alignment_word, clover_gamma);
    clover_mult = n_clover_gamma;
    for (i = 0 ; i < clover_mult ; ++i){
        clover_energy[i] = clover_gamma[i].energy;
        clover_ID[i] = clover_gamma[i].clover_no;
        clover_timestamp_coarse[i] = clover_gamma[i].max_w.timestamp;
        clover_timestamp_fine[i] = clover_gamma[i].max_w.cfdcorr + shift_time_clover[clover_gamma[i].clover_no*NUM_CLOVER_DETECTORS + clover_gamma[i].crystal_no];
    }

    for ( i = 0 ; i < NUM_CLOVER_DETECTORS ; ++i ){
        for ( j = 0 ; j < NUM_CLOVER_CRYSTALS ; ++j ){
            for (k = 0 ; k < event.n_clover[i][j] ; ++k){
                energy_clover_raw[i][j]->Fill(event.w_clover[i][j][k].adcdata);
                energy = CalibrateE(event.w_clover[i][j][k]);
                energy_clover[i][j]->Fill(energy);

                if ( have_alginment_data ){
                    tdiff = CalcTimediff(start_alignment_word, event.w_clover[i][j][k]);
                    align_time_clover[i]->Fill(tdiff, j);
                }
            }
        }
    }

    // First fill some 'singles' spectra.
    for ( i = 0 ; i < NUM_LABR_3X8_DETECTORS ; ++i ){
        for ( j = 0 ; j < event.n_labr_3x8[i] ; ++j ){
            labr3x8_ID[labr3x8_mult] = i;
            energy_labr_3x8_raw[i]->Fill(event.w_labr_3x8[i][j].adcdata);
            energy = CalibrateE(event.w_labr_3x8[i][j]);
            labr3x8_energy[labr3x8_mult] = energy;
            energy_labr_3x8[i]->Fill(energy);
            labr3x8_timestamp_coarse[labr3x8_mult] = event.w_labr_3x8[i][j].timestamp;
            labr3x8_timestamp_fine[labr3x8_mult++] = event.w_labr_3x8[i][j].cfdcorr + shift_time_labr_3x8[i];


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
            labr2x2_ID[labr2x2_mult] = i;
            energy_labr_2x2_fs_raw[i]->Fill(event.w_labr_2x2_fs[i][j].adcdata);
            energy = CalibrateE(event.w_labr_2x2_fs[i][j]);
            labr2x2_energy[labr2x2_mult] = energy;
            energy_labr_2x2_fs[i]->Fill(energy);
            labr2x2_timestamp_coarse[labr2x2_mult] = event.w_labr_2x2_fs[i][j].timestamp;
            labr2x2_timestamp_fine[labr2x2_mult++] = event.w_labr_2x2_fs[i][j].cfdcorr + shift_time_labr_2x2_fs[i];


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
            tdiff = CalcTimediff(event.trigger, event.w_dEdet_ring[i][j]);
            prompt_de_ring->Fill(tdiff, i);

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
            tdiff = CalcTimediff(event.trigger, event.w_dEdet_sect[i][j]);
            prompt_de_sect->Fill(tdiff, i);

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


    for ( i = 0 ; i < NUM_SI_DE_RING ; ++i){
        for ( j = 0 ; j < event.n_dEdet_ring[i] ; ++j){
            for (int m = 0 ; m < NUM_SI_DE_SECT ; ++m){
                for (int n = 0 ; n < event.n_dEdet_sect[m] ; ++n){
                    tdiff = CalcTimediff(event.w_dEdet_ring[i][j], event.w_dEdet_sect[m][n]);
                    time_ring_sect[m]->Fill(tdiff, i);

                    // check if is in 'coincidense'
                    if ( CheckTimeStatus(tdiff, particle_sect_ring_cuts) == is_prompt && n_de_events < 256 ){
                            deDet_ringID[deDet_mult] = i;
                            deDet_sectID[deDet_mult] = m;
                            deDet_ring_energy[deDet_mult] = CalibrateE(event.w_dEdet_ring[i][j]);
                            deDet_sect_energy[deDet_mult] = CalibrateE(event.w_dEdet_sect[m][n]);
                            deDet_ring_timestamp_coarse[deDet_mult] = event.w_dEdet_ring[i][j].timestamp;
                            deDet_sect_timestamp_coarse[deDet_mult] = event.w_dEdet_sect[m][n].timestamp;
                            deDet_ring_timestamp_fine[deDet_mult] = event.w_dEdet_ring[i][j].cfdcorr + shift_time_de_ring[GetDetector(event.w_dEdet_ring[i][j].address).detectorNum];
                            deDet_sect_timestamp_fine[deDet_mult++] = event.w_dEdet_sect[m][n].cfdcorr + shift_time_de_sect[GetDetector(event.w_dEdet_sect[m][n].address).detectorNum];

                           de_events[n_de_events].ring_event = event.w_dEdet_ring[i][j];
                           de_events[n_de_events++].sect_event = event.w_dEdet_sect[m][n];
                    }
                }
            }
        }
    }

    // Now we may write to file, if and only if the tree is actually set.
    if (root_tree)
        root_tree->Fill();

    if ( root_tree &&  n_fail_e > 5000 && !is_opt ){
        root_tree->OptimizeBaskets();
        is_opt = true;
    }

    // Loop over all dE events and plot particles.
    /*for ( i = 0 ; i < n_de_events ; ++i){

        word_t de_word = de_events[i].ring_event;
        word_t de_word_sect = de_events[i].sect_event;
        word_t e_word = event.trigger;


        int ring = GetDetector(de_word.address).detectorNum;
        int sect = GetDetector(de_events[i].sect_event.address).detectorNum;
        int sect_e = GetDetector(e_word.address).detectorNum;

        double e_energy = CalibrateE(e_word);
        double de_energy = CalibrateE(de_word_sect);

        double time_ring = CalcTimediff(de_word, e_word);
        double time_sect = CalcTimediff(de_word_sect, e_word);

        prompt_de_ring->Fill(time_ring, ring);
        prompt_de_sect->Fill(time_sect, sect);




        // Check if we are prompt
        if ( CheckTimeStatus(time_sect, particle_sect_cuts) != is_prompt ) // Skip if not prompt
            continue;

        ede_raw[sect_e][ring]->Fill(e_word.adcdata, de_word.adcdata);
        ede[sect_e][ring]->Fill(e_energy, de_energy);
        ede_raw_sect[sect][ring]->Fill(e_energy, de_word_sect.adcdata);
        ede_all->Fill(e_energy, de_energy);

        // For now, we exclude ring 16 up to 27.
        if (ring >= 16 && ring <= 27)
            continue;

        // Chalculate the apparent thickness
        double thick = range.GetRange(e_energy + de_energy) - range.GetRange(e_energy);

        if (ring == 14)
            h_thick->Fill(thick, e_energy);

        if (thick >= thick_range[0] && thick <= thick_range[1]){
            // Now we will look at the time-difference vs. e-energy
            time_energy_e->Fill(e_energy, time_sect);
            ede_gate->Fill(e_energy, de_energy);

            double excitation = ex_from_ede[ring*3] + ex_from_ede[ring*3 + 1]*(e_energy + de_energy)*1e-3 + ex_from_ede[ring*3+2]*pow((e_energy + de_energy)*1e-3, 2);
            excitation *= 1e3;
            h_ex[ring]->Fill(excitation);
            h_ex_all->Fill(excitation);

            // Wait for addback...


            AnalyzeGamma(de_word, excitation, event, clover_gamma, n_clover_gamma);
        }
    }*/



    return true;

}


void UserSort::AnalyzeGamma(const word_t &de_word, const double &excitation, const Event &event, const clover_gamma_t *c_gamma, const int n_clover)
{

    // We will loop over all gamma-rays.
    for (int i = 0 ; i < NUM_LABR_3X8_DETECTORS ; ++i){
        for (int j = 0 ; j < event.n_labr_3x8[i] ; ++j){

            // Get energy and time of the gamma-ray.

            double energy = CalibrateE(event.w_labr_3x8[i][j]);
            double tdiff = CalcTimediff(event.trigger, event.w_labr_3x8[i][j]);

            // Fill time spectra.
            prompt_labr_3x8->Fill(tdiff, i);


            // Check time gate.
            switch ( CheckTimeStatus(tdiff, labr_3x8_time_cuts) ) {
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


    for (int i = 0 ; i < n_clover ; ++i){

        double energy = c_gamma[i].energy;
        double tdiff = CalcTimediff(event.trigger, c_gamma[i].max_w);

        // Fill time spectra
        prompt_clover->Fill(tdiff, c_gamma[i].clover_no);


        // Check if prompt or not.
        switch ( CheckTimeStatus(tdiff, clover_time_cuts) ) {

            case is_prompt : {
                alfna_clover->Fill(energy, excitation);
                break;
            }

            case is_background : {
                alfna_clover->Fill(energy, excitation, -1);
                alfna_clover_bg->Fill(energy, excitation);
                break;
            }

            case ignore :
                break;

        }

    }

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
    if (root_file){
        root_file->Write(0,TObject::kOverwrite);
        root_file->Close();
    }
    std::cout << "Stats info: " << std::endl;
    std::cout << "CFD fails in E - detectors: " << n_fail_e << std::endl;
    std::cout << "CFD fails in dE ring - detectors: " << n_fail_de_ring << std::endl;
    std::cout << "CFD fails in dE sector - detectors: " << n_fail_de_sect << std::endl;
    std::cout << "Average number of dE ring words: " << n_tot_de_ring/double(tot) << std::endl;
    std::cout << "Average number of dE sector words: " << n_tot_de_sect/double(tot) << std::endl;
    std::cout << "Average number of E words: " << n_tot_e/double(tot) << std::endl;
    return true;
}
