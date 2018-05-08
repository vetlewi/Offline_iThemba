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
        case labr: {
            if ( n_labr[dinfo.detectorNum] < MAX_WORDS_PER_DET &&
                 dinfo.detectorNum < NUM_LABR_DETECTORS){
                w_labr[dinfo.detectorNum][n_labr[dinfo.detectorNum]++] = (*buffer)[i];
                ++tot_labr;
            } else {
                std::cerr << __PRETTY_FUNCTION__ << ": Could not populate LaBr word, run debugger with appropriate break point for more details" << std::endl;
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
