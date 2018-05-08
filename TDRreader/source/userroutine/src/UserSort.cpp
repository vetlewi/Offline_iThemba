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
    r = ( r && set_par(parameters, ipar, "gain_labr",    NUM_LABR_DETECTORS ) );
    r = ( r && set_par(parameters, ipar, "shift_labr",   NUM_LABR_DETECTORS ) );
    r = ( r && set_par(parameters, ipar, "gain_de_ring", NUM_SI_DE_RING ) );
    r = ( r && set_par(parameters, ipar, "shift_de_ring", NUM_SI_DE_RING ) );
    r = ( r && set_par(parameters, ipar, "gain_de_sect", NUM_SI_DE_SECT ) );
    r = ( r && set_par(parameters, ipar, "shift_de_sect", NUM_SI_DE_SECT ) );
    r = ( r && set_par(parameters, ipar, "gain_e", NUM_SI_E_DET ) );
    r = ( r && set_par(parameters, ipar, "shift_e", NUM_SI_E_DET ) );
    r = ( r && set_par(parameters, ipar, "shift_time_clover", NUM_CLOVER_DETECTORS*NUM_CLOVER_CRYSTALS) );
    r = ( r && set_par(parameters, ipar, "shift_time_labr", NUM_LABR_DETECTORS ) );
    r = ( r && set_par(parameters, ipar, "shift_time_de_ring", NUM_SI_DE_RING ) );
    r = ( r && set_par(parameters, ipar, "shift_time_de_sect", NUM_SI_DE_SECT ) );
    r = ( r && set_par(parameters, ipar, "shift_time_e", NUM_SI_E_DET ) );
    return r;
}

UserSort::UserSort()
    : gain_clover( GetParameters(), "gain_clover", NUM_CLOVER_DETECTORS*NUM_CLOVER_CRYSTALS, 1)
    , shift_clover(GetParameters(), "shift_clover", NUM_CLOVER_DETECTORS*NUM_CLOVER_CRYSTALS, 0)
    , gain_labr( GetParameters(), "gain_labr", NUM_LABR_DETECTORS, 1)
    , shift_labr( GetParameters(), "shift_labr", NUM_LABR_DETECTORS, 0)
    , gain_dE_ring( GetParameters(), "gain_de_ring", NUM_SI_DE_RING, 1)
    , shift_dE_ring( GetParameters(), "shift_de_ring", NUM_SI_DE_RING, 0)
    , gain_dE_sect( GetParameters(), "gain_de_sect", NUM_SI_DE_SECT, 1)
    , shift_dE_sect( GetParameters(), "shift_de_sect", NUM_SI_DE_SECT, 0)
    , gain_E( GetParameters(), "gain_e", NUM_SI_E_DET, 1)
    , shift_E( GetParameters(), "shift_e", NUM_SI_E_DET, 0)
    , shift_time_clover( GetParameters(), "shift_time_clover", NUM_CLOVER_DETECTORS*NUM_CLOVER_CRYSTALS, 0)
    , shift_time_labr( GetParameters(), "shift_time_labr", NUM_LABR_DETECTORS, 0)
    , shift_time_de_ring( GetParameters(), "shift_time_de_ring", NUM_SI_DE_RING, 0)
    , shift_time_de_sect( GetParameters(), "shift_time_de_sect", NUM_SI_DE_SECT, 0)
    , shift_time_e( GetParameters(), "shift_time_e", NUM_SI_E_DET, 0)
    , ex_from_ede    ( GetParameters(), "ex_from_ede", NUM_SI_RINGS*3)
    , thick_range    ( GetParameters(), "thick_range", 2      )
    , labr_time_cuts  ( GetParameters(), "labr_time_cuts", 2*2  )
    , ppac_time_cuts ( GetParameters(), "ppac_time_cuts", 2*2 )
{
}


double UserSort::CalibrateE(const word_t &w) const
{
    DetectorInfo_t info = GetDetector(w.address);
    switch ( info.type ) {
    case clover : {
        return gain_clover[info.detectorNum*NUM_CLOVER_DETECTORS + info.telNum]*(w.adcdata + drand48() - 0.5) + shift_clover[info.detectorNum*NUM_CLOVER_DETECTORS + info.telNum];
    }
    case labr : {
        return gain_labr[info.detectorNum]*(w.adcdata + drand48() - 0.5) + shift_labr[info.detectorNum];
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
        case labr : {
            start_shift = shift_time_labr[info_start.detectorNum];
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
        case labr : {
            stop_shift = shift_time_labr[info_stop.detectorNum];
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

    // Allocating the LaBr 'singles' spectra
    for (int i = 0 ; i < NUM_LABR_DETECTORS ; ++i){

        // Create energy spectra
        sprintf(tmp, "energy_raw_labr_%02d", i+1);
        energy_labr_raw[i] = Spec(tmp, tmp, 32768, 0, 32768, "Energy [ch]");

        sprintf(tmp, "energy_labr_%02d", i+1);
        energy_labr[i] = Spec(tmp, tmp, 10000, 0, 10000, "Energy [keV]");

        sprintf(tmp, "energy_time_labr_%02d", i+1);
        energy_time_labr[i] = Mat(tmp, tmp, 1000, 0, 16000, "LaBr energy [keV]", 2000, -100, 100, "Time difference [ns]");
    }

    // Allocating the dE ring 'singles' spectra
    for (int i = 0 ; i < NUM_SI_DE_RING ; ++i){

        // Create energy spectra
        sprintf(tmp, "energy_raw_dE_ring_%02d", i);
        energy_dE_ring_raw[i] = Spec(tmp, tmp, 32768, 0, 32768, "Energy [ch]");

        sprintf(tmp, "energy_dE_ring_%02d", i);
        energy_dE_ring[i] = Spec(tmp, tmp, 10000, 0, 10000, "Energy [keV]");
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
        sprintf(tmp, "e_de_time_%02d", i);
        e_de_time[i] = Mat(tmp, tmp, 3000, -1500, 1500, "Time t_{dE} - t_{E} [ns]", NUM_SI_RINGS, 0, NUM_SI_RINGS, "Ring number");

        // Making all spectra indexed [NUM_SI_E_DET][NUM_SI_RINGS].
        for (int j = 0 ; j < NUM_SI_RINGS ; ++j){

            // Make the 'raw' ede spectrum.
            sprintf(tmp, "ede_raw_b%02d_f%02d", i, j);
            sprintf(tmp2, "E : DE raw, pad %d, ring %d", i, j);
            ede_raw[i][j] = Mat(tmp, tmp2, 500, 0, 16384, "Back energy [ch]", 500, 0, 16384, "Front energy [ch]");

            // Make 'calibrated' ede spectrum.
            sprintf(tmp, "ede_b%02d_f%02d", i, j);
            sprintf(tmp2, "E : DE calibrated, pad %d, ring %d", i, j);
            ede[i][j] = Mat(tmp, tmp2, 500, 0, 20000, "Back energy [keV]", 500, 0, 5000, "Front energy [keV]");

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

    // Time spectra (except those 'listed')
    sprintf(tmp, "de_ring_align_time");
    sprintf(tmp2, "t_{dE} - t_{LaBr nr. 1}");
    de_ring_align_time = Mat(tmp, tmp2, 3000, -1500, 1500, "t_{dE} - t_{LaBr nr. 1} [ns]", NUM_SI_DE_RING, 0, NUM_SI_DE_RING, "#Delta E ring detector id.");

    sprintf(tmp, "de_sect_align_time");
    sprintf(tmp2, "t_{dE} - t_{LaBr nr. 1}");
    de_sect_align_time = Mat(tmp, tmp2, 3000, -1500, 1500, "t_{dE} - t_{LaBr nr. 1} [ns]", NUM_SI_DE_SECT, 0, NUM_SI_DE_SECT, "#Delta E sector detector id.");

    sprintf(tmp, "e_align_time");
    sprintf(tmp2, "t_{E} - t_{LaBr nr. 1}");
    e_align_time = Mat(tmp, tmp2, 3000, -1500, 1500, "t_{E} - t_{LaBr nr. 1} [ns]", NUM_SI_E_DET, 0, NUM_SI_E_DET, "E sector detector id.");

    sprintf(tmp, "de_ring_e_time");
    sprintf(tmp2, "t_{#Delta E ring} - t_{E ANY}");
    de_ring_e_time = Mat(tmp, tmp2, 3000, -1500, 1500, "t_{#Delta E} - t_{E ANY} [ns]", NUM_SI_DE_RING, 0, NUM_SI_DE_RING, "#Delta E ring detector id.");

    sprintf(tmp, "de_sect_e_time");
    sprintf(tmp2, "t_{#Delta E sect} - t_{E ANY}");
    de_sect_e_time = Mat(tmp, tmp2, 3000, -1500, 1500, "t_{#Delta E} - t_{E ANY} [ns]", NUM_SI_DE_SECT, 0, NUM_SI_DE_SECT, "#Delta E sector detector id.");

    sprintf(tmp, "labr_align_time");
    sprintf(tmp2, "t_{LaBr} - t_{dE ANY}");
    labr_align_time = Mat(tmp, tmp2, 3000, -1500, 1500, "t_{LaBr} - t_{dE ANY} [ns]", NUM_LABR_DETECTORS, 0, NUM_LABR_DETECTORS, "LaBr detector id.");

    sprintf(tmp, "energy_time_labr_all");
    sprintf(tmp2, "E_{LaBr} : t_{LaBr} - t_{dE ANY}, all");
    energy_time_labr_all = Mat(tmp, tmp2, 2000, 0, 16000, "Energy LaBr [keV]", 2000, -50, 50, "t_{LaBr} - t_{DE} [ns]");

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
    int i, j;
    double energy;
    double tdiff;

    n_tot_e += event.tot_Edet;
    n_tot_de_ring += event.tot_dEdet_ring;
    n_tot_de_sect += event.tot_dEdet_sect;
    tot += 1;


    word_t de_words[256]; // List of dE hits from pads in front of the trigger E word.
    int n_de_words=0;

    // First fill some 'singles' spectra.
    for ( i = 0 ; i < NUM_LABR_DETECTORS ; ++i ){
        for ( j = 0 ; j < event.n_labr[i] ; ++j ){
            energy_labr_raw[i]->Fill(event.w_labr[i][j].adcdata);
            energy = CalibrateE(event.w_labr[i][j]);
            energy_labr[i]->Fill(energy);
        }
    }

    for ( i = 0 ; i < NUM_SI_DE_RING ; ++i ){
        for ( j = 0 ; j < event.n_dEdet_ring[i] ; ++j ){
            energy_dE_ring_raw[i]->Fill(event.w_dEdet_ring[i][j].adcdata);
            energy = CalibrateE(event.w_dEdet_ring[i][j]);
            energy_dE_ring[i]->Fill(energy);
            de_ring_e_time->Fill(CalcTimediff(event.trigger, event.w_dEdet_ring[i][j]), i);
            if (event.w_dEdet_ring[i][j].cfdfail > 0) // For 'statistical' purposes!
                ++n_fail_de_ring;
        }
    }

    for ( i = 0 ; i < NUM_SI_DE_SECT ; ++i ){
        for ( j = 0 ; j < event.n_dEdet_sect[i] ; ++j ){
            energy_dE_sect_raw[i]->Fill(event.w_dEdet_sect[i][j].adcdata);
            energy = CalibrateE(event.w_dEdet_sect[i][j]);
            energy_dE_sect[i]->Fill(energy);
            de_sect_e_time->Fill(CalcTimediff(event.trigger, event.w_dEdet_sect[i][j]), i);
            if (event.w_dEdet_sect[i][j].cfdfail > 0) // For 'statistical' purposes!
                ++n_fail_de_sect;
        }
    }

    for ( i = 0 ; i < NUM_SI_E_DET ; ++i ){
        for ( j = 0 ; j < event.n_Edet[i] ; ++j ){
            energy_E_raw[i]->Fill(event.w_Edet[i][j].adcdata);
            energy = CalibrateE(event.w_Edet[i][j]);
            energy_E[i]->Fill(energy);
            if (event.w_Edet[i][j].cfdfail > 0) // For 'statistical' purposes!
                ++n_fail_e;
        }
    }


    // The total number of ring events have to be one or has to be correlated in
    // time with the E detector... Maybe later... For now only one!
    if ( event.tot_dEdet_ring == 1 && event.tot_dEdet_sect == 1 ){
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
    }



    // Check if only one dE detector of the trapezoidal has fired.
    if ( n_de_words == 1){

        word_t e_word = event.trigger;
        word_t de_word_r = de_words[0];
        word_t de_word_s = de_words[1];

        int ring = GetDetector(de_word_r.address).detectorNum;
        int sect = GetDetector(de_word_s.address).detectorNum;
        int sect_e = GetDetector(e_word.address).detectorNum;

        // First we check times. These should all be align to the LaBr # 1 detector.
        if ( event.n_labr[7] == 1 && event.w_labr[7][0].adcdata > 2500 ){ // This is a large volume LaBr detector

            de_ring_align_time->Fill(CalcTimediff(event.w_labr[7][0], de_word_r), ring);
            de_sect_align_time->Fill(CalcTimediff(event.w_labr[7][0], de_word_s), sect);
            e_align_time->Fill(CalcTimediff(event.w_labr[7][0], e_word), sect_e);

        }

        ede_raw[sect_e][ring]->Fill(e_word.adcdata, de_word_r.adcdata);

    }

    return true;

}


void UserSort::AnalyzeGamma(const word_t &de_word, const double &excitation,const Event &event)
{

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
