void LLAnaKaon() {
	gROOT->Reset();
	gStyle->SetOptStat(111);
	gStyle->SetOptFit(111);
	gStyle->SetPalette(1);
	gStyle->SetOptDate(0);

	gStyle->SetCanvasColor(kWhite);
	gStyle->SetFrameFillStyle(1001);
	gStyle->SetFrameFillColor(kWhite);
	gStyle->SetPadBorderMode(0);
	gStyle->SetPadColor(kWhite);
	gStyle->SetStatColor(kWhite);
	gStyle->SetPadTickX(1);
	gStyle->SetPadTickY(1);
	// gStyle->SetLabelSize(.05,"X");
	// gStyle->SetLabelSize(.05,"Y");
	gStyle->SetLabelColor(1,"X");
	gStyle->SetLabelColor(1,"Y");


	TFile *ff = new TFile("../../output/Kaon_5GeV_Theta5_Phi45.root");
	TTree *mTree=(TTree*) ff->Get("LLTreeDst");

	TCanvas *c1 = new TCanvas("c1","c1",1400,1500);
	c1->Divide(2,3);

	TH1 *h[5];

	h[0] = new TH1D("hLPion","",100,0.,2e-6);
	h[0]->GetXaxis()->SetTitle("prob(K #rightarrow #pi^{-})");
	h[0]->GetYaxis()->SetTitle("Counts");
	h[0]->GetXaxis()->SetTitleSize(0.060);
	h[0]->GetYaxis()->SetTitleSize(0.060);
	mTree->Project("hLPion","Lpion");

	h[1] = new TH1D("hLKaon","",100,0.,2.e-6);
	h[1]->GetXaxis()->SetTitle("prob(K #rightarrow K)");
	h[1]->GetYaxis()->SetTitle("Counts");
	h[1]->GetXaxis()->SetTitleSize(0.060);
	h[1]->GetYaxis()->SetTitleSize(0.060);
	mTree->Project("hLKaon","LKaon");

	h[2] = new TH1D("hLProton","",500,0.,2.e-8);
	h[2]->GetXaxis()->SetTitle("prob(K #rightarrow proton)");
	h[2]->GetYaxis()->SetTitle("Counts");
	h[2]->GetXaxis()->SetTitleSize(0.060);
	h[2]->GetYaxis()->SetTitleSize(0.060);
	mTree->Project("hLProton","Lproton");

	h[3] = new TH1D("hLKaonovPion","",100,0.,1.e9);
	h[3]->GetXaxis()->SetTitle("prob(K)/prob(#pi^{-})");
	h[3]->GetYaxis()->SetTitle("Counts");
	h[3]->GetXaxis()->SetTitleSize(0.060);
	h[3]->GetYaxis()->SetTitleSize(0.060);
	mTree->Project("hLKaonovPion","LKaon/Lpion");

	h[4] = new TH1D("hLKaonovProton","",100,0.,1.e30);
	h[4]->GetXaxis()->SetTitle("prob(K)/prob(proton)");
	h[4]->GetYaxis()->SetTitle("Counts");
	h[4]->GetXaxis()->SetTitleSize(0.060);
	h[4]->GetYaxis()->SetTitleSize(0.060);
	mTree->Project("hLKaonovProton","LKaon/Lproton");
	
	
	for(int i=1; i<6; i++){
		c1->cd(i);
		gPad->SetTopMargin(0.12);
		gPad->SetBottomMargin(0.12);
		gPad->SetLeftMargin(0.12);
		gPad->SetRightMargin(0.12);
		gPad->SetLogy(1);

		h[i-1]->Draw("");

		TLegend *leg = new TLegend(0.26,0.92,0.93,1.0);
		leg->SetTextSize(0.05);
		leg->SetLineColor(4);
		leg->SetLineStyle(1);
		leg->SetLineWidth(1);
		leg->SetFillColor(0);
		leg->SetMargin(0.1);
		leg->SetFillStyle(0);
		leg->SetTextAlign(12);
		leg->SetBorderSize(0);
		leg->AddEntry(h[i],"5 GeV Kaon, #theta=5^{o}, #phi=45^{o}","");
		leg->Draw();
	}

	char outfile[256];
	sprintf(outfile,"figs/Kaon_5GeV_Theta5_Phi45.pdf");
	c1->cd();
	c1->SaveAs(outfile);
}
