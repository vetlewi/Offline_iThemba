#include <iostream>
#include <vector>

#include <cstdint>

#include "FileReader.h"
#include "experimentsetup.h"
#include "Calibration.h"

#include <TFile.h>
#include <TTree.h>

const int buf_size = 32768;

#define NUM_MAX 128

typedef struct {
    int16_t clover_no;
    int16_t crystal_no;
    double energy;
    word_t max_w;
} clover_gamma_t;

typedef struct {
    word_t w_clover[NUM_CLOVER_DETECTORS][NUM_CLOVER_CRYSTALS][NUM_MAX];
    int n_clover[NUM_CLOVER_DETECTORS][NUM_CLOVER_CRYSTALS];
    int tot_clover[NUM_CLOVER_DETECTORS];
    void Reset(){
        for (int i = 0 ; i < NUM_CLOVER_DETECTORS ; ++i){
            tot_clover[i] = 0;
            for (int j = 0 ; j < NUM_CLOVER_CRYSTALS ; ++j){
                n_clover[i][j] = 0;
            }
        }
    }
} clover_events_t;

std::vector<word_t> ReadFileToMemory(const char *filename)
{
    FileReader freader;
    freader.Open(filename);
    int i;
    std::vector<word_t> data(buf_size);

    // We read 32k at the time
    word_t buf[buf_size];
    while (freader.Read(buf,buf_size) > 0){
        for (i = 0 ; i < buf_size ; ++i){
            data.push_back(buf[i]);
        }
    }
    return data;
}

std::vector<clover_gamma_t> Addback_Clover(const clover_events_t &event)
{
    int i, j, k;
    double energy, tdiff;

    std::vector<clover_gamma_t> res(NUM_MAX);
    clover_gamma_t clover_gamma;

    bool have_been_counted[NUM_CLOVER_DETECTORS][NUM_CLOVER_CRYSTALS][NUM_MAX];
        for (i = 0 ; i < NUM_CLOVER_DETECTORS ; ++i){
            for (j = 0 ; j < NUM_CLOVER_CRYSTALS ; ++j){
                for (k = 0 ; k < NUM_MAX ; ++k){
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
                    clover_gamma.clover_no = i;
                    clover_gamma.crystal_no = j;
                    clover_gamma.energy = CalibrateEnergy(event.w_clover[i][j][k]);
                    clover_gamma.max_w = event.w_clover[i][j][k];
                    res.push_back(clover_gamma);

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
                    energy = CalibrateEnergy(event.w_clover[i][j][k]);
                    double max_energy = energy;
                    int max_c = j;
                    word_t max_word = event.w_clover[i][j][k];

                    for ( int n = 0 ; n < NUM_CLOVER_CRYSTALS ; ++n ){
                        if ( n == j )
                            continue;
                        for (  int m = 0 ; m < event.n_clover[i][n] ; ++m ){

                            // Increment the time spectrum!
                            tdiff = event.w_clover[i][n][m].timestamp - event.w_clover[i][j][k].timestamp;
                            tdiff += CalibrateTime(event.w_clover[i][n][m]) - CalibrateTime(event.w_clover[i][j][k]);

                            if ( event.w_clover[i][n][m].adcdata >= 16383 ){
                                have_been_counted[i][n][m] = true;
                                continue;
                            }

                            if ( have_been_counted[i][n][m] ) // We always want to increment the time spectra, but not energies
                                continue; // skip to next!

                            if ( CheckTimeGateAddback(tdiff) ){
                                double this_e = CalibrateEnergy(event.w_clover[i][n][m]);
                                energy += this_e;
                                if (this_e > max_energy){
                                    max_c = n;
                                    max_word = event.w_clover[i][n][m];
                                }
                                have_been_counted[i][n][m] = true;
                            }
                        }
                    }

                    clover_gamma.clover_no = i;
                    clover_gamma.crystal_no = max_c;
                    clover_gamma.energy = energy;
                    clover_gamma.max_w = max_word;
                    res.push_back(clover_gamma);
                }
            }
        }
    }
    return res;
}


//! Function that performs the actual event building and writing to root files.
void Convert_to_ROOT(std::vector<std::string> files,    /*!< Raw binary files to read from  */
                     std::string rfile                  /*!< Root file to write to          */)
{
    // Defining the required variables (file, tree, etc.)
    TFile *fout = new TFile(rfile.c_str(), "RECREATE");
    TTree *tree = new TTree("data","data");

    int16_t deDet_ringID;
    double deDet_ringE, deDet_ring_time_fine;
    int64_t deDet_ring_time_course;
    tree->Branch("deDet_ringID", &deDet_ringID, "deDet_ringID/S");
    tree->Branch("deDet_ringE", &deDet_ringE, "deDet_ringE/D");
    tree->Branch("deDet_ring_time_fine", &deDet_ring_time_fine, "deDet_ring_time_fine/D");
    tree->Branch("deDet_ring_time_course",&deDet_ring_time_course, "deDet_ring_time_course/L");

    int16_t deDet_sect_mult, deDet_sectID[NUM_MAX];
    double deDet_sectE[NUM_MAX], deDet_sect_time_fine[NUM_MAX];
    int64_t deDet_sect_time_course[NUM_MAX];
    tree->Branch("deDet_sect_mult", &deDet_sect_mult, "deDet_sect_mult/S");
    tree->Branch("deDet_sectID", &deDet_sectID, "deDet_sectID[deDet_sect_mult]/S");
    tree->Branch("deDet_sectE", &deDet_sectE, "deDet_sectE[deDet_sect_mult]/D");
    tree->Branch("deDet_sect_time_fine", &deDet_sect_time_fine, "deDet_sect_time_fine[deDet_sect_mult]/D");
    tree->Branch("deDet_sect_time_course", &deDet_sect_time_course, "deDet_sect_time_course[deDet_sect_mult]/L");

    int16_t eDet_mult, eDet_ID[NUM_MAX];
    double eDet_E[NUM_MAX], eDet_time_fine[NUM_MAX];
    int64_t eDet_time_course[NUM_MAX];
    tree->Branch("eDet_mult", &eDet_mult, "eDet_mult/S");
    tree->Branch("eDet_ID", &eDet_ID, "eDet_ID[eDet_mult]/S");
    tree->Branch("eDet_E", &eDet_E, "eDet_E[eDet_mult]/D");
    tree->Branch("eDet_time_fine", &eDet_time_fine, "eDet_time_fine[eDet_mult]/D");
    tree->Branch("eDet_time_course", &eDet_time_course, "eDet_time_course[eDet_mult]/L");

    int16_t labr3x8_mult, labr3x8_ID[NUM_MAX];
    double labr3x8_E[NUM_MAX], labr3x8_time_fine[NUM_MAX];
    int64_t labr3x8_time_course[NUM_MAX];
    tree->Branch("labr3x8_mult", &labr3x8_mult, "labr3x8_mult/S");
    tree->Branch("labr3x8_ID", &labr3x8_ID, "labr3x8_ID[labr3x8_mult]/S");
    tree->Branch("labr3x8_E", &labr3x8_E, "labr3x8_E[labr3x8_mult]/D");
    tree->Branch("labr3x8_time_fine", &labr3x8_time_fine, "labr3x8_time_fine[labr3x8_mult]/D");
    tree->Branch("labr3x8_time_course", &labr3x8_time_course, "labr3x8_time_course[labr3x8_mult]/L");

    int16_t labr2x2_mult, labr2x2_ID[NUM_MAX];
    double labr2x2_E[NUM_MAX], labr2x2_time_fine[NUM_MAX];
    int64_t labr2x2_time_course[NUM_MAX];
    tree->Branch("labr2x2_mult", &labr2x2_mult, "labr2x2_mult/S");
    tree->Branch("labr2x2_ID", &labr2x2_ID, "labr2x2_ID[labr2x2_mult]/S");
    tree->Branch("labr2x2_E", &labr2x2_E, "labr2x2_E[labr2x2_mult]/D");
    tree->Branch("labr2x2_time_fine", &labr2x2_time_fine, "labr2x2_time_fine[labr2x2_mult]/D");
    tree->Branch("labr2x2_time_course", &labr2x2_time_course, "labr2x2_time_course[labr2x2_mult]/L");

    int16_t clover_mult, clover_ID[NUM_MAX];
    char clover_crystal[NUM_MAX]; // We need to know which crystal is hit first.
    double clover_E[NUM_MAX], clover_time_fine[NUM_MAX];
    int64_t clover_time_course[NUM_MAX];
    tree->Branch("clover_mult",&clover_mult, "clover_mult/S");
    tree->Branch("clover_ID",&clover_ID, "clover_ID[clover_mult]/S");
    tree->Branch("clover_crystal",&clover_crystal, "clover_crystal[clover_mult]/B");
    tree->Branch("clover_E", &clover_E, "clover_E[clover_mult]/D");
    tree->Branch("clover_time_fine", &clover_time_fine, "clover_time_fine[clover_mult]/D");
    tree->Branch("clover_time_course", &clover_time_course, "clover_time_course[clover_mult]/L");


    // Misc. variables.
    int64_t timediff;
    size_t start=0,stop=0;
    size_t i, j;

    // Clover stuff
    std::vector<clover_gamma_t> clover_events;
    clover_events_t c_events;


    // Now the actual conversion begins!
    // This part should be paralized. How? I don't know... Yet...
    for (size_t f = 0 ; f < files.size() ; ++f){

        // First we read the ENTIRE binary file from disc.
        std::cout << "Reading from file '" << files[f] << "'" << std::endl;
        std::vector<word_t> full_file = ReadFileToMemory(files[f].c_str());
        std::cout << "Building events from file '" << files[f] << "'" << std::endl;

        int counted = 0;

        // Build events
        for (i = 0 ; i < full_file.size() ; ++i){
            DetectorInfo_t dinfo = GetDetector(full_file[i].address);

            if ( dinfo.type != deDet_ring ) // Skip to next word.
                continue;

            for (j = i ; j > 0 ; --j){
                timediff = abs(full_file[i].timestamp - full_file[j-1].timestamp);
                if (timediff > 1500){
                    start = j;
                    break;
                }
            }

            for (j = i ; j < full_file.size() - 1 ; ++j){
                timediff = abs(full_file[i].timestamp - full_file[j+1].timestamp);
                if (timediff > 1500){
                    stop = j+1;
                    break;
                }
            }

            // Cleaning up clover stuff
            clover_events.clear();
            c_events.Reset();


            // Now we work our way through all the stuff...
            // First we will set everything to zero!
            deDet_ringID = dinfo.detectorNum;
            deDet_ringE = CalibrateEnergy(full_file[i]);
            deDet_ring_time_course = full_file[i].timestamp;
            deDet_ring_time_fine = CalibrateTime(full_file[i]);
            deDet_sect_mult = 0;
            eDet_mult = 0;
            labr3x8_mult = 0;
            labr2x2_mult = 0;
            for (size_t j = start ; j < stop ; ++j){
                if (j == i)
                    continue; // Skip trigger.
                DetectorInfo_t dinf = GetDetector(full_file[j].address);
                switch (dinf.type) {
                    case deDet_sect: {
                        deDet_sectID[deDet_sect_mult] = dinf.detectorNum;
                        deDet_sectE[deDet_sect_mult] = CalibrateEnergy(full_file[j]);
                        deDet_sect_time_course[deDet_sect_mult] = full_file[j].timestamp;
                        deDet_sect_time_fine[deDet_sect_mult++] = CalibrateTime(full_file[j]);
                        break;
                    }
                    case eDet : {
                        eDet_ID[eDet_mult] = dinf.detectorNum;
                        eDet_E[eDet_mult] = CalibrateEnergy(full_file[j]);
                        eDet_time_course[eDet_mult] = full_file[j].timestamp;
                        eDet_time_fine[eDet_mult++] = CalibrateTime(full_file[j]);
                        break;
                    }
                    case labr_3x8 : {
                        labr3x8_ID[labr3x8_mult] = dinf.detectorNum;
                        labr3x8_E[labr3x8_mult] = CalibrateEnergy(full_file[j]);
                        labr3x8_time_course[labr3x8_mult] = full_file[j].timestamp;
                        labr3x8_time_fine[labr3x8_mult++] = CalibrateTime(full_file[j]);
                        break;
                    }
                    case labr_2x2_fs : {
                        labr2x2_ID[labr2x2_mult] = dinf.detectorNum;
                        labr2x2_E[labr2x2_mult] = CalibrateEnergy(full_file[j]);
                        labr2x2_time_course[labr2x2_mult] = full_file[j].timestamp;
                        labr2x2_time_fine[labr2x2_mult++] = CalibrateTime(full_file[j]);
                        break;
                    }
                    case clover : {
                        c_events.w_clover[dinf.detectorNum][dinf.telNum][c_events.n_clover[dinf.detectorNum][dinf.telNum]++] = full_file[j];
                        ++c_events.tot_clover[dinf.detectorNum];
                        break;
                    }
                    default :
                        break;
                }

            }

            clover_events = Addback_Clover(c_events);
            clover_mult = clover_events.size();
            for (j = 0 ; j < clover_events.size() ; ++j){
                clover_ID[j] = clover_events[j].clover_no;
                clover_crystal[j] = 'A' + clover_events[j].crystal_no;
                clover_E[j] = clover_events[j].energy;
                clover_time_course[j] = clover_events[j].max_w.timestamp;
                clover_time_fine[j] = CalibrateTime(clover_events[j].max_w);
            }

            if(i % (full_file.size()/100) == 0){
                ++counted;
                tree->OptimizeBaskets();
                std::cout << counted*100./10. << "% ";
                std::cout.flush();
            }
            tree->Fill();
        }
        std::cout << std::endl;
    }
    fout->Write(0,TObject::kOverwrite);
    fout->Close();
    return;
}


int main(int argc, const char *argv[])
{
    if ( argc < 3 ){
        std::cout << "Usage: <file1> <file2> ... <output file>" << std::endl;
        return 1;
    }
    std::vector<std::string> filenames;
    std::cout << "Following files will be converted:" << std::endl;
    for (int i = 1 ; i < argc - 1 ; ++i){
        std::cout << argv[i] << std::endl;
        filenames.push_back(argv[i]);
    }
    std::cout << "Into root file: '" << argv[argc-1] << std::endl;
    Convert_to_ROOT(filenames, argv[argc-1]);
    std::cout << "Done converting to ROOT" << std::endl;
    return 0;

}

