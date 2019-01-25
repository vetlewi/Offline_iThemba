#include "TFile.h"
#include "TTree.h"
#include "TColor.h"
void tegnelabr3x8(){
    TFile *f = TFile::Open("eu152_first.root");
    if (f==0){
        printf("Error: cannot open file");
        return;
    }
    // f->ls();
    gStyle->SetPalette(1);
 //    energy_clover_01A->GetXaxis()->CenterTitle();
	// energy_clover_01A->SetTitle("Clover_01");
 //    energy_clover_01A->SetLineColor(1);
 //    energy_clover_01A->Draw("");
 //    energy_clover_01B->SetLineColor(2);
 //    energy_clover_01C->SetLineColor(3);
 //    energy_clover_01D->SetLineColor(4);
 //    energy_clover_01B->Draw("same");
 //    energy_clover_01C->Draw("same");
 //    energy_clover_01D->Draw("same");



    // energy_clover_02A->SetTitle("Clover_02");
    // energy_clover_02A->SetLineColor(1);
    // energy_clover_02A->Draw("");
    // energy_clover_02B->SetLineColor(2);
    // energy_clover_02C->SetLineColor(3);
    // energy_clover_02D->SetLineColor(7);
    // energy_clover_02B->Draw("same");
    // energy_clover_02C->Draw("same");
    // energy_clover_02D->Draw("same");
//
//
    // energy_clover_03A->SetTitle("Clover_03");
    // energy_clover_03A->SetLineColor(1);
    // energy_clover_03A->Draw("");
    // energy_clover_03B->SetLineColor(2);
    // energy_clover_03C->SetLineColor(3);
    // energy_clover_03D->SetLineColor(4);
    // energy_clover_03B->Draw("same");
    // energy_clover_03C->Draw("same");
    // energy_clover_03D->Draw("same");
//
//     // energy_clover_02A->Draw("same");
//     // energy_clover_02B->Draw("same");
//     // energy_clover_02C->Draw("same");
//     // energy_clover_02D->Draw("same");
// LaBr fast
    // energy_labr_2x2_fs_01->SetTitle("LaBr_2");
    // energy_labr_2x2_fs_01->SetLineColor(1);
    // energy_labr_2x2_fs_01->Draw("");
    // energy_labr_2x2_fs_02->SetLineColor(2);
    // energy_labr_2x2_fs_03->SetLineColor(3);
    // energy_labr_2x2_fs_04->SetLineColor(4);
    // energy_labr_2x2_fs_05->SetLineColor(5);
    // energy_labr_2x2_fs_06->SetLineColor(6);
    // energy_labr_2x2_fs_02->Draw("same");
    // energy_labr_2x2_fs_03->Draw("same");
    // energy_labr_2x2_fs_04->Draw("same");
    // energy_labr_2x2_fs_05->Draw("same");
    // energy_labr_2x2_fs_06->Draw("same");

    energy_labr_3x8_01->SetTitle("LaBr_3x8");
    energy_labr_3x8_01->SetLineColor(1);
    energy_labr_3x8_01->Draw("");
    energy_labr_3x8_02->SetLineColor(2);
    energy_labr_3x8_03->SetLineColor(3);
    energy_labr_3x8_04->SetLineColor(4);
    energy_labr_3x8_05->SetLineColor(5);
    energy_labr_3x8_06->SetLineColor(6);
    energy_labr_3x8_02->Draw("same");
    energy_labr_3x8_03->Draw("same");
    energy_labr_3x8_04->Draw("same");
    energy_labr_3x8_05->Draw("same");
    energy_labr_3x8_06->Draw("same");

}


// energy_clover_01A->Draw("");
// energy_clover_01B->Draw("same");
// energy_clover_01C->Draw("same");
// energy_clover_01D->Draw("same");
// energy_clover_02A->Draw("same");
// energy_clover_02B->Draw("same");
// energy_clover_02C->Draw("same");
// energy_clover_02D->Draw("same");

// energy_labr_2x2_fs_01->Draw("");energy_clover_01A->SetLineColor(4);
// energy_labr_2x2_fs_02->Draw("same");
// energy_labr_2x2_fs_03->Draw("same");
// energy_labr_2x2_fs_04->Draw("same");
// energy_labr_2x2_fs_05->Draw("same");
// energy_labr_2x2_fs_06->Draw("same");
// energy_labr_2x2_fs_07->Draw("same");
//
// energy_labr_3x8_01->Draw("");
// energy_labr_3x8_02->Draw("same");
// energy_labr_3x8_03->Draw("same");
// energy_labr_3x8_04->Draw("same");
// energy_labr_3x8_05->Draw("same");
// energy_labr_3x8_06->Draw("same");
