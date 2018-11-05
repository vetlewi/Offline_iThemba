

void GetPeaks_Clover()
{
	TFile *file = TFile::Open("Working.root");

	const int n_det = 10;
	const int n_cry = 4;

	char spec_name[2048], tmp[2048];
	double peak_pos[n_det][n_cry];
	for (int i = 0 ; i < n_det ; ++i){
		sprintf(spec_name, "align_time_clover_%02d", i+1);
		TH2 *m = (TH2 *)file->Get(spec_name);
		TSpectrum spec;
		for (int j = 0 ; j < n_cry ; ++j ){
			sprintf(tmp, "%s_px_%d", spec_name, j+1);
			TH1 *h = m->ProjectionX(tmp, j+1, j+1);
			h->Rebin(5);
			h->Draw();
			spec.Search(h);
			int n_peaks = spec.GetNPeaks();

			double param[3] = {200., spec.GetPositionX()[0], 1.0};
			TF1 *fit = new TF1("total","gaus(0)",spec.GetPositionX()[0]-20.0,spec.GetPositionX()[0]+20.);
		
			for (Int_t k=0; k<3; k++) {
        		fit->SetParameter(k, param[k]);
    		}
			h->Fit("total", "bR");
			peak_pos[i][j] = fit->GetParameter(1);
			fit->Draw("same");
		}
	}

	// Print all the results.
	for (int i = 0 ; i < n_det ; ++i){
		for (int j = 0 ; j < n_cry ; ++j){
			cout << -peak_pos[i][j] << "\t";
		}
		cout << '\\' << endl;
	}

}