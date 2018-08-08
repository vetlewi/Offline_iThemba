#include "Event.h"

#include "WordBuffer.h"
#include "experimentsetup.h"

#include <iostream>

void Event::PackEvent(const WordBuffer *buffer, int start, int stop)
{
    length = stop - start;
    DetectorInfo_t dinfo;
    for (int i = start ; i < stop ; ++i){
        dinfo = GetDetector((*buffer)[i].address);

        switch (dinfo.type) {
        case clover: {
            if ( n_clover[dinfo.detectorNum][dinfo.telNum] < MAX_WORDS_PER_DET &&
                 dinfo.telNum < NUM_CLOVER_CRYSTALS && dinfo.detectorNum < NUM_CLOVER_DETECTORS){
                w_clover[dinfo.detectorNum][dinfo.telNum][n_clover[dinfo.detectorNum][dinfo.telNum]++] = (*buffer)[i];
                ++tot_clover;
            } else {
                std::cerr << __PRETTY_FUNCTION__ << ": Could not populate Clover word, run debugger with appropriate break point for more details" << std::endl;
            }
            break;
        }
        case labr_3x8: {
            if ( n_labr_3x8[dinfo.detectorNum] < MAX_WORDS_PER_DET &&
                 dinfo.detectorNum < NUM_LABR_3X8_DETECTORS){
                w_labr_3x8[dinfo.detectorNum][n_labr_3x8[dinfo.detectorNum]++] = (*buffer)[i];
                ++tot_labr_3x8;
            } else {
                std::cerr << __PRETTY_FUNCTION__ << ": Could not populate 3.5x8 LaBr word, run debugger with appropriate break point for more details" << std::endl;
            }
            break;
        }
        case labr_2x2_ss: {
            if ( n_labr_2x2_ss[dinfo.detectorNum] < MAX_WORDS_PER_DET &&
                 dinfo.detectorNum < NUM_LABR_2X2_DETECTORS){
                w_labr_2x2_ss[dinfo.detectorNum][n_labr_2x2_ss[dinfo.detectorNum]++] = (*buffer)[i];
                ++tot_labr_2x2_ss;
            } else {
                std::cerr << __PRETTY_FUNCTION__ << ": Could not populate 2x2 LaBr word (slow), run debugger with appropriate break point for more details" << std::endl;
            }
            break;
        }
        case labr_2x2_fs: {
            if ( n_labr_2x2_fs[dinfo.detectorNum] < MAX_WORDS_PER_DET &&
                 dinfo.detectorNum < NUM_LABR_2X2_DETECTORS){
                w_labr_2x2_fs[dinfo.detectorNum][n_labr_2x2_fs[dinfo.detectorNum]++] = (*buffer)[i];
                ++tot_labr_2x2_fs;
            } else {
                std::cerr << __PRETTY_FUNCTION__ << ": Could not populate 2x2 LaBr word (fast), run debugger with appropriate break point for more details" << std::endl;
            }
            break;
        }
        case deDet_ring: {
            if ( n_dEdet_ring[dinfo.detectorNum] < MAX_WORDS_PER_DET &&
                 dinfo.detectorNum < NUM_SI_DE_RING){
                w_dEdet_ring[dinfo.detectorNum][n_dEdet_ring[dinfo.detectorNum]++] = (*buffer)[i];
                ++tot_dEdet_ring;
            } else {
                std::cerr << __PRETTY_FUNCTION__ << ": Could not populate dEdet word, run debugger with appropriate break point for more details" << std::endl;
            }
            break;
        }
        case deDet_sect: {
            if ( n_dEdet_sect[dinfo.detectorNum] < MAX_WORDS_PER_DET &&
                 dinfo.detectorNum < NUM_SI_DE_SECT){
                w_dEdet_sect[dinfo.detectorNum][n_dEdet_sect[dinfo.detectorNum]++] = (*buffer)[i];
                ++tot_dEdet_sect;
            } else {
                std::cerr << __PRETTY_FUNCTION__ << ": Could not populate dEdet word, run debugger with appropriate break point for more details" << std::endl;
            }
            break;
        }
        case eDet: {
            if ( n_Edet[dinfo.detectorNum] < MAX_WORDS_PER_DET &&
                 dinfo.detectorNum < NUM_SI_E_DET){
                w_Edet[dinfo.detectorNum][n_Edet[dinfo.detectorNum]++] = (*buffer)[i];
                ++tot_Edet;
            } else {
                std::cerr << __PRETTY_FUNCTION__ << ": Could not populate Edet word, run debugger with appropriate break point for more details" << std::endl;
            }
            break;
        }
        case rfchan: {
            if ( n_RFpulse < MAX_WORDS_PER_DET )
                w_RFpulse[n_RFpulse++] = (*buffer)[i];
            else
                std::cerr << __PRETTY_FUNCTION__ << ": Could not populate RF word, run debugger with appropriate break point for more details" << std::endl;
            break;
        }
        default:
            break;
        }
    }
    return;
}
