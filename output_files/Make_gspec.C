

{
	TFile *file = TFile::Open("Co60_end.root");

	TH2 *m = (TH2 *)file->Get("gam_gam_labr_00");

	TH1 *h = new TH1I("h", "", 2000, 0, 2000);
	double t, E, tmp, tmp2;
	for (int i = 0 ; i < m->GetXaxis()->GetNbins() ; ++i){
		for (int j = 0 ; j < m->GetYaxis()->GetNbins() ; ++j){

			t = m->GetYaxis()->GetBinCenter(j);
			E = m->GetXaxis()->GetBinCenter(i);

			tmp = t - (6.97737673e+01 + 5.0 + 3.25343986e+05*pow(E, -2.04838208e+00));
			tmp2 = t - (6.97737673e+01 + 40.0 + 3.25343986e+05*pow(E, -2.04838208e+00));
			if ( abs(tmp) < 10 && E < 485 ){
				int k = 0;
				while (k < m->GetBinContent(i,j)){
					h->Fill(E);
					++k;
				}
			} else if ( abs(tmp2) < 10 && E < 485 ){
				int k = 0;
				while (k < m->GetBinContent(i,j)){
					h->Fill(E, -1);
					++k;
				}
			} else if ( t > 0 && t < 15 && E > 300 ){
				int k = 0;
				while (k < m->GetBinContent(i,j)){
					h->Fill(E);
					++k;
				}
			} else if (t > -300 && t < -285 && E > 300){
				int k = 0;
				while (k < m->GetBinContent(i,j)){
					h->Fill(E, -1);
					++k;
				}
			}

		}
	}
	h->Draw();
}