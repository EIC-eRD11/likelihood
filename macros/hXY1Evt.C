void hXY1Evt() {
	gROOT->Reset();
	gStyle->SetOptStat(0);
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


	TFile *ff = new TFile("../../output/pi-_1evt.root");
	TH2D *hPiXY = (TH2D*) ff->Get("hPiXY");


	TCanvas *c1 = new TCanvas("c1","c1",700,700);
        c1->cd();
        gPad->SetTopMargin(0.13);
        gPad->SetBottomMargin(0.13);
        gPad->SetLeftMargin(0.13);
        gPad->SetRightMargin(0.13);
        hPiXY->GetXaxis()->SetTitle("X (mm)");
        hPiXY->GetYaxis()->SetTitle("Y (mm)");
        hPiXY->GetZaxis()->SetTitle("n_{e^{-}} per event");
	hPiXY->GetYaxis()->SetTitleColor(kBlack);
	hPiXY->GetZaxis()->SetTitleSize(0.04);
	hPiXY->GetYaxis()->SetTitleSize(0.04);
	hPiXY->GetXaxis()->SetTitleSize(0.035);
	hPiXY->GetZaxis()->SetTitleOffset(1.7);
	hPiXY->GetYaxis()->SetTitleOffset(1.7);
	hPiXY->GetXaxis()->SetTitleOffset(1.7);
	hPiXY->SetTitle();
	hPiXY->Draw("surf");
	TLegend *leg = new TLegend(0.07,0.92,0.88,1.0);
	leg->SetTextSize(0.04);
	leg->SetLineColor(4);
	leg->SetLineStyle(1);
	leg->SetLineWidth(1);
	leg->SetFillColor(0);
	leg->SetMargin(0.1);
	leg->SetFillStyle(0);
	leg->SetTextAlign(12);
	leg->SetBorderSize(0);
	leg->AddEntry(hPiXY,"5 GeV Pion, shoot angle: #theta=5^{o}, #phi=45^{o}","");
	leg->Draw();

	c1->cd();
	c1->SaveAs("figs/XY1EvtPi.pdf");
}
