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

std::vector<word_t> ReadFileToMemory(char *filename)
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


int main(int argc, char *argv[])
{
    char file[] = "/Volumes/PR282/PR282/R73_0";

    std::vector<word_t> full_file = ReadFileToMemory(file);
    std::cout << "Done reading file!" << std::endl;
    size_t start=0, stop=0;
    int64_t timediff;

    // Now we loop over all the words and check if we have a dE event.
    // Next we check all timestamps 1.5 us before and after. This we will
    // commit to the tree.

    TFile *fout = new TFile("output.root", "RECREATE", "", 9);
    TTree *tree = new TTree("data", "data");

    int deDet_ringID;
    double deDet_ringE, deDet_ring_time_fine;
    int64_t deDet_ring_time_course;
    tree->Branch("deDet_ringID", &deDet_ringID, "deDet_ringID/I");
    tree->Branch("deDet_ringE", &deDet_ringE, "deDet_ringE/D");
    tree->Branch("deDet_ring_time_fine", &deDet_ring_time_fine, "deDet_ring_time_fine/D");
    tree->Branch("deDet_ring_time_course",&deDet_ring_time_course, "deDet_ring_time_course/L");


    int deDet_sect_mult, deDet_sectID[NUM_MAX];
    double deDet_sectE[NUM_MAX], deDet_sect_time_fine[NUM_MAX];
    int64_t deDet_sect_time_course[NUM_MAX];
    tree->Branch("deDet_sect_mult", &deDet_sect_mult, "deDet_sect_mult/I");
    tree->Branch("deDet_sectID", &deDet_sectID, "deDet_sectID[deDet_sect_mult]/I");
    tree->Branch("deDet_sectE", &deDet_sectE, "deDet_sectE[deDet_sect_mult]/D");
    tree->Branch("deDet_sect_time_fine", &deDet_sect_time_fine, "deDet_sect_time_fine[deDet_sect_mult]/D");
    tree->Branch("deDet_sect_time_course", &deDet_sect_time_course, "deDet_sect_time_course[deDet_sect_mult]/L");

    int eDet_mult, eDet_ID[NUM_MAX];
    double eDet_E[NUM_MAX], eDet_time_fine[NUM_MAX];
    int64_t eDet_time_course[NUM_MAX];
    tree->Branch("eDet_mult", &eDet_mult, "eDet_mult/I");
    tree->Branch("eDet_ID", &eDet_ID, "eDet_ID[eDet_mult]/I");
    tree->Branch("eDet_E", &eDet_E, "eDet_E[eDet_mult]/D");
    tree->Branch("eDet_time_fine", &eDet_time_fine, "eDet_time_fine[eDet_mult]/D");
    tree->Branch("eDet_time_course", &eDet_time_course, "eDet_time_course[eDet_mult]/L");

    int labr3x8_mult, labr3x8_ID[NUM_MAX];
    double labr3x8_E[NUM_MAX], labr3x8_time_fine[NUM_MAX];
    int64_t labr3x8_time_course[NUM_MAX];
    tree->Branch("labr3x8_mult", &labr3x8_mult, "labr3x8_mult/I");
    tree->Branch("labr3x8_ID", &labr3x8_ID, "labr3x8_ID[labr3x8_mult]/I");
    tree->Branch("labr3x8_E", &labr3x8_E, "labr3x8_E[labr3x8_mult]/D");
    tree->Branch("labr3x8_time_fine", &labr3x8_time_fine, "labr3x8_time_fine[labr3x8_mult]/D");
    tree->Branch("labr3x8_time_course", &labr3x8_time_course, "labr3x8_time_course[labr3x8_mult]/L");

    int labr2x2_mult, labr2x2_ID[NUM_MAX];
    double labr2x2_E[NUM_MAX], labr2x2_time_fine[NUM_MAX];
    int64_t labr2x2_time_course[NUM_MAX];
    tree->Branch("labr2x2_mult", &labr2x2_mult, "labr2x2_mult/I");
    tree->Branch("labr2x2_ID", &labr2x2_ID, "labr2x2_ID[labr2x2_mult]/I");
    tree->Branch("labr2x2_E", &labr2x2_E, "labr2x2_E[labr2x2_mult]/D");
    tree->Branch("labr2x2_time_fine", &labr2x2_time_fine, "labr2x2_time_fine[labr2x2_mult]/D");
    tree->Branch("labr2x2_time_course", &labr2x2_time_course, "labr2x2_time_course[labr2x2_mult]/L");

    /*int clover_mult, clover_ID[NUM_MAX];
    double clover_E[NUM_MAX], clover_time_fine[NUM_MAX];
    int64_t clover_time_course[NUM_MAX];*/



    for (size_t i = 0 ; i < full_file.size() ; ++i){
        DetectorInfo_t dinfo = GetDetector(full_file[i].address);

        if ( dinfo.type != deDet_ring ) // Skip to next word.
            continue;

        for (size_t j = i ; j > 0 ; --j){
            timediff = abs(full_file[i].timestamp - full_file[j-1].timestamp);
            if (timediff > 1500){
                start = j;
                break;
            }
        }

        for (size_t j = i ; j < full_file.size() - 1 ; ++j){
            timediff = abs(full_file[i].timestamp - full_file[j+1].timestamp);
            if (timediff > 1500){
                stop = j+1;
                break;
            }
        }

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
                default :
                    break;
            }

        }
        if(i % (full_file.size()/100) == 0){
            tree->OptimizeBaskets();
            std::cout << ".";
            std::cout.flush();
        }
        tree->Fill();

    }

    fout->Write(0,TObject::kOverwrite);
    fout->Close();
    return 0;
}

