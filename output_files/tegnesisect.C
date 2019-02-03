#include "TFile.h"
#include "TTree.h"
#include "TColor.h"
void tegnesisect(){
    TFile *f = TFile::Open("Ra226_E_fromr2cal.root");
    if (f==0){
        printf("Error: cannot open file");
        return;
    }
    // f->ls();
    gStyle->SetPalette(1);

    energy_E_00->GetXaxis()->CenterTitle();
	energy_E_00->SetTitle("E-sect");

    energy_E_00->SetLineColor(1);
    energy_E_00->Draw("");
    energy_E_01->SetLineColor(2);
    energy_E_02->SetLineColor(3);
    energy_E_03->SetLineColor(4);
    energy_E_04->SetLineColor(5);
    energy_E_05->SetLineColor(6);
    energy_E_06->SetLineColor(7);
    energy_E_07->SetLineColor(8);
    energy_E_08->SetLineColor(9);
    energy_E_09->SetLineColor(16);
    energy_E_10->SetLineColor(11);
    energy_E_11->SetLineColor(12);
    energy_E_12->SetLineColor(13);
    energy_E_13->SetLineColor(14);
    energy_E_14->SetLineColor(15);
    energy_E_15->SetLineColor(0);

    energy_E_02->Draw("same");
    energy_E_03->Draw("same");
    energy_E_04->Draw("same");
    energy_E_05->Draw("same");
    energy_E_06->Draw("same");
    energy_E_07->Draw("same");
    energy_E_08->Draw("same");
    energy_E_09->Draw("same");
    // energy_E_10->Draw("same");
    // energy_E_11->Draw("same");
    // energy_E_12->Draw("same");
    // energy_E_13->Draw("same");
    // energy_E_14->Draw("same");
    // energy_E_15->Draw("same");

    auto legend = new TLegend(0.6,0.7,0.9,0.9); //x1, y1,x2.y2
    legend->AddEntry(energy_E_00,"energy_E_00","l");
    legend->AddEntry(energy_E_01,"energy_E_01","l");
    legend->AddEntry(energy_E_02,"energy_E_02","l");
    legend->AddEntry(energy_E_03,"energy_E_03","l");
    legend->AddEntry(energy_E_04,"energy_E_04","l");
    legend->AddEntry(energy_E_05,"energy_E_05","l");
    legend->AddEntry(energy_E_06,"energy_E_06","l");
    legend->AddEntry(energy_E_07,"energy_E_07","l");
    legend->AddEntry(energy_E_08,"energy_E_08","l");
    legend->AddEntry(energy_E_09,"energy_E_09","l");
    legend->AddEntry(energy_E_10,"energy_E_10","l");
    legend->AddEntry(energy_E_11,"energy_E_11","l");
    legend->AddEntry(energy_E_12,"energy_E_12","l");
    legend->AddEntry(energy_E_13,"energy_E_13","l");
    legend->AddEntry(energy_E_14,"energy_E_14","l");
    legend->AddEntry(energy_E_15,"energy_E_15","l");

    legend->Draw();

    // energy_clover_01D->SetLineColor(4);
    // energy_clover_01B->Draw("same");
    // energy_clover_01C->Draw("same");
    // energy_clover_01D->Draw("same");



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


    // energy_clover_04A->SetTitle("Clover_04");
    // energy_clover_04A->SetLineColor(1);
    // energy_clover_04A->Draw("");
    // energy_clover_04B->SetLineColor(2);
    // energy_clover_04C->SetLineColor(3);
    // energy_clover_04D->SetLineColor(4);
    // energy_clover_04B->Draw("same");
    // energy_clover_04C->Draw("same");
    // energy_clover_04D->Draw("same");

    // energy_clover_05A->SetTitle("Clover_05");
    // energy_clover_05A->SetLineColor(1);
    // energy_clover_05A->Draw("");
    // energy_clover_05B->SetLineColor(2);
    // energy_clover_05C->SetLineColor(3);
    // energy_clover_05D->SetLineColor(4);
    // energy_clover_05B->Draw("same");
    // energy_clover_05C->Draw("same");
    // energy_clover_05D->Draw("same");

    // energy_clover_06A->SetTitle("Clover_06");
    // energy_clover_06A->SetLineColor(1);
    // energy_clover_06A->Draw("");
    // energy_clover_06B->SetLineColor(2);
    // energy_clover_06C->SetLineColor(3);
    // energy_clover_06D->SetLineColor(4);
    // energy_clover_06B->Draw("same");
    // energy_clover_06C->Draw("same");
    // energy_clover_06D->Draw("same");

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

    // energy_labr_3x8_01->SetTitle("LaBr_3x8");
    // energy_labr_3x8_01->SetLineColor(1);
    // energy_labr_3x8_01->Draw("");
    // energy_labr_3x8_02->SetLineColor(2);
    // energy_labr_3x8_03->SetLineColor(3);
    // energy_labr_3x8_04->SetLineColor(4);
    // energy_labr_3x8_05->SetLineColor(5);
    // energy_labr_3x8_06->SetLineColor(6);
    // energy_labr_3x8_02->Draw("same");
    // energy_labr_3x8_03->Draw("same");
    // energy_labr_3x8_04->Draw("same");
    // energy_labr_3x8_05->Draw("same");
    // energy_labr_3x8_06->Draw("same");

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
