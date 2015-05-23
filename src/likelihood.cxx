//////*********************************//////
//////***** Author: Dr. Liang Xue *****//////
//////******  Date: 01/06/2015  *******//////
//////*********************************//////

// This is to the main program to study the 
// Electron Ion Collider Ring Image Cherenkov
// detector

#include <iostream> 
#include <fstream>
#include <stdlib.h>
#include <cmath> 
#include <math.h> 
#include <TFile.h>
#include <TF1.h>
#include <TMath.h>
#include <TTree.h>
#include <TChain.h>
#include <TRandom.h>
#include "../include/event.h"
#include "../include/hit.h"
#include "../include/material.h"
#include "../include/likelihood.h"

using namespace std;
using namespace TMath;


likelihood::likelihood(TFile *fFile): outputfile(fFile)
{
 cout<<endl;
 cout<<"likelihood::likelihood() ----- Constructor ! ------"<<endl;
 cout<<endl;
 init();
}

likelihood::~likelihood()
{
 cout<<"likelihood::~likelihood() ----- Release memory ! ------"<<endl;
 delete rd;
 delete mat;
 delete outputfile;
 #ifdef doLikelihoodDB
 delete hNEvtvsP;
 delete hPiXY;
 delete hKaonXY;
 delete hProtonXY;
 #else
 delete mTree;
 delete hnHitPerEvtvsMom;
 delete hnHitAeglPerEvtvsMom;
 delete hPhDetXY_bfSmear;
 delete hPhDetXY_afSmear;
 delete hPhDetXY_SbKCs;
 delete hPhDetXY_GaAsP;
 delete hPhDetXY_GaAs;
 delete hPhotonWL;
 delete hPhotonE;
 delete hnPhotonElvsnHits_SbKCs;
 delete hnPhotonElvsnHits_GaAsP;
 delete hnPhotonElvsnHits_GaAs;
 #endif

}

int likelihood::init()
{
 cout<<"likelihood::init() ----- Initialization ! ------"<<endl;

 rd = new TRandom();    /// random number generator
 mat = new material(); //// initialize the material

 #ifdef doLikelihoodDB
 cout<<"likelihood::init(), initialize database histograms ;"<<endl;
 hNEvtvsP = new TH2D("hNEvtvsP","hNEvtvsP",5,0.,5.,60,3.,15.);
 hPiXY = new TH2D("hPiXY","hPiXY",nPads,-44.,44.,nPads,-44.,44.);
 hKaonXY = new TH2D("hKaonXY","hKaonXY",nPads,-44.,44.,nPads,-44.,44.);
 hProtonXY = new TH2D("hProtonXY","hProtonXY",nPads,-44.,44.,nPads,-44.,44.);
 #else 
 char currdir[256], dbfile[256];
 getcwd(currdir,256);
 sprintf(dbfile,"%s/outputDB/DB_5GeV_Theta5_Phi45.root",currdir);
 cout<<"likelihood::init(), read database file: "<< dbfile <<endl;
 TFile *finput = new TFile(dbfile);
 hNEvtvsPDB = (TH2D*) finput->Get("hNEvtvsP");
 hPiDB = (TH2D *) finput->Get("hPiXY");
 hKaonDB = (TH2D *) finput->Get("hKaonXY");
 hProtonDB = (TH2D *) finput->Get("hProtonXY");

 int NEvtPiBin = hNEvtvsPDB->FindBin(0,5.);
 int NEvtKaonBin = hNEvtvsPDB->FindBin(1,5.);
 int NEvtProtonBin = hNEvtvsPDB->FindBin(2,5.);
 int NEvtPi = hNEvtvsPDB->GetBinContent(NEvtPiBin);
 int NEvtKaon = hNEvtvsPDB->GetBinContent(NEvtKaonBin);
 int NEvtProton = hNEvtvsPDB->GetBinContent(NEvtProtonBin);
 hPiDB->Sumw2();
 hKaonDB->Sumw2();
 hProtonDB->Sumw2();
 hPiDB->Scale(1./NEvtPi);
 hKaonDB->Scale(1./NEvtKaon);
 hProtonDB->Scale(1./NEvtProton);
 
 cout<<"likelihood::init(), initialize tree  ; "<<endl;
 mTree = new TTree("LLTreeDst","Tree for Likelihood Analysis");
 mTree->SetDirectory(outputfile);
 mTree->Branch("pid",&mDst.pid,"pid/I");
 mTree->Branch("px",&mDst.px,"px/F");
 mTree->Branch("py",&mDst.py,"py/F");
 mTree->Branch("pz",&mDst.pz,"pz/F");
 mTree->Branch("vx",&mDst.vx,"vx/F");
 mTree->Branch("vy",&mDst.vy,"vy/F");
 mTree->Branch("vz",&mDst.vz,"vz/F");
 mTree->Branch("theta",&mDst.theta,"theta/F");
 mTree->Branch("phi",&mDst.phi,"phi/F");
 mTree->Branch("nhit",&mDst.nhit,"nhit/I");
 mTree->Branch("nhitAegl",&mDst.nhitAegl,"nhitAegl/I");
 mTree->Branch("nhitPhoDet",&mDst.nhitPhoDet,"nhitPhoDet/I");
 mTree->Branch("nelSbKCs",&mDst.nelSbKCs,"nelSbKCs/I");
 mTree->Branch("nelGaAsP",&mDst.nelGaAsP,"nelGaAsP/I");
 mTree->Branch("nelGaAs",&mDst.nelGaAs,"nelGaAs/I");
 mTree->Branch("Lpion",&mDst.Lpion,"Lpion/D");
 mTree->Branch("LKaon",&mDst.LKaon,"LKaon/D");
 mTree->Branch("Lproton",&mDst.Lproton,"Lproton/D");

 cout<<"likelihood::init(), initialize histogram  ; "<<endl;
 hnHitPerEvtvsMom = new TH2D ("hnHitPerEvtvsMom","hnHitPerEvtvsMom",100,0.,10.,200,0.,200.);
 hnHitAeglPerEvtvsMom = new TH2D ("hnHitAeglPerEvtvsMom","hnHitAeglPerEvtvsMom",100,0.,10.,200,0.,200.);
 hPhDetXY_bfSmear = new TH2D ("hPhDetXY_bfSmear","hPhDetXY_bfSmear",nPads,-44.,44.,nPads,-44.,44.);
 hPhDetXY_afSmear = new TH2D ("hPhDetXY_afSmear","hPhDetXY_afSmear",nPads,-44.,44.,nPads,-44.,44.);
 hPhDetXY_SbKCs = new TH2D ("hPhDetXY_SbKCs","hPhDetXY_SbKCs",nPads,-44.,44.,nPads,-44.,44.);
 hPhDetXY_GaAsP = new TH2D ("hPhDetXY_GaAsP","hPhDetXY_GaAsP",nPads,-44.,44.,nPads,-44.,44.);
 hPhDetXY_GaAs = new TH2D ("hPhDetXY_GaAs","hPhDetXY_GaAs",nPads,-44.,44.,nPads,-44.,44.);
 hPhotonWL = new TH1D ("hPhotonWL","hPhotonWL",100,250.,750.);
 hPhotonE = new TH1D ("hPhotonE","hPhotonE",100,0.,5.);
 hnPhotonElvsnHits_SbKCs = new TH2D ("hnPhotonElvsnHits_SbKCs","hnPhotonElvsnHits_SbKCs",50,0.,50.,25,0.,25.);
 hnPhotonElvsnHits_GaAsP = new TH2D ("hnPhotonElvsnHits_GaAsP","hnPhotonElvsnHits_GaAsP",50,0.,50.,25,0.,25.);
 hnPhotonElvsnHits_GaAs = new TH2D ("hnPhotonElvsnHits_GaAs","hnPhotonElvsnHits_GaAs",50,0.,50.,25,0.,25.);
 #endif
 
 return 0;
}

int likelihood::process_event(event *aevt, hit *ahit)
{
	int pid_gen=0;
	float px_gen=0;
	float py_gen=0;
	float pz_gen=0;
	float vx_gen=0;
	float vy_gen=0;
	float vz_gen=0;
	float p_gen=0;
	float theta_gen=0;
	float phi_gen=0;
	// cout<< "tree generated size: "<< aevt->get_pid()->size() <<";    tree flux size:  "<< ahit->get_hitn()->size() <<endl;
	for (unsigned int i=0;i<aevt->get_pid()->size();i++) {
		// cout << aevt->get_pid()->at(i) << " " << aevt->get_px()->at(i) << " " << aevt->get_py()->at(i) << " " << aevt->get_pz()->at(i) << " " << aevt->get_vx()->at(i) << " " << aevt->get_vy()->at(i) << " " << aevt->get_vz()->at(i) << endl; 
		pid_gen=aevt->get_pid()->at(i);
		px_gen=aevt->get_px()->at(i)/1e3;    //in MeV, convert to GeV
		py_gen=aevt->get_py()->at(i)/1e3;	//in MeV, convert to GeV
		pz_gen=aevt->get_pz()->at(i)/1e3;    //in MeV, convert to GeV
		vx_gen=aevt->get_vx()->at(i)/1e1;    //in mm, convert to cm
		vy_gen=aevt->get_vy()->at(i)/1e1;	//in mm, convert to cm
		vz_gen=aevt->get_vz()->at(i)/1e1;    //in mm, convert to cm
		p_gen=sqrt(px_gen*px_gen+py_gen*py_gen+pz_gen*pz_gen);
		theta_gen=acos(pz_gen/p_gen)*DEG;  	//in deg
		phi_gen=atan2(py_gen,px_gen)*DEG;     	//in deg            
	}  

	#ifdef doLikelihoodDB
	if(pid_gen==-211) hNEvtvsP->Fill(0.,p_gen);
	else if(pid_gen==-321) hNEvtvsP->Fill(1.,p_gen);
	else if(pid_gen==2212) hNEvtvsP->Fill(2.,p_gen);

	int nhits = ahit->get_hitn()->size();
	for (int i=0;i<nhits;i++) {
		if(isPhoton(ahit,i) && !isReflection(ahit,i) && isOnPhotonSensor(ahit,i)){
			double out_x(0.), out_y(0.);
			Smearing2D(ahit->get_out_x()->at(i), ahit->get_out_y()->at(i), out_x, out_y);
			if(fabs(out_x)>44.||fabs(out_y)>44.) continue;
			double photonE = ahit->get_trackE()->at(i);   /// in MeV (GEANT4 default)
                        double wavelength = 1240./(photonE*1.e6);  /// MeV->eV,wavelength in "nm"
			double QE_GaAsP=mat->extrapQE_GaAsP(wavelength);
			if(QE_GaAsP>rd->Uniform(0.,1.)){
				if(pid_gen==-211) hPiXY->Fill(out_x,out_y);
				else if(pid_gen==-321) hKaonXY->Fill(out_x,out_y);
				else if(pid_gen==2212) hProtonXY->Fill(out_x,out_y);
			}
		}
	}
	#else
	TH2D *hXY = new TH2D("hXY","hXY",nPads,-44.,44.,nPads,-44.,44.);
	double nhitPhoDet(0.0), nhitAerogel(0.0), nel_SbKCs(0.0), nel_GaAsP(0.0), nel_GaAs(0.0);
	int nhits = ahit->get_hitn()->size();

	for (int i=0;i<nhits;i++) {
		if(isPhoton(ahit,i) && !isReflection(ahit,i) && isOnAerogel(ahit,i)){
			nhitAerogel++;
			double photonE = ahit->get_trackE()->at(i);   /// in MeV (GEANT4 default)
			double wavelength = 1240./(photonE*1.e6);  /// MeV->eV,wavelength in "nm"
			hPhotonWL->Fill(wavelength);
			hPhotonE->Fill(photonE*1.e6);
		}

		if(isPhoton(ahit,i) && !isReflection(ahit,i) && isOnPhotonSensor(ahit,i)){
			nhitPhoDet++;
			hPhDetXY_bfSmear->Fill(ahit->get_out_x()->at(i),ahit->get_out_y()->at(i));
			double out_x(0.), out_y(0.);
			Smearing2D(ahit->get_out_x()->at(i), ahit->get_out_y()->at(i), out_x, out_y);
			if(fabs(out_x)>44.||fabs(out_y)>44.) continue;
			hPhDetXY_afSmear->Fill(out_x,out_y);

			double photonE = ahit->get_trackE()->at(i);   /// in MeV (GEANT4 default)
			double wavelength = 1240./(photonE*1.e6);  /// MeV->eV,wavelength in "nm"

			double QE_SbKCs=mat->extrapQE_SbKCs(wavelength);
			if(QE_SbKCs>rd->Uniform(0.,1.)){
				nel_SbKCs++;
				hPhDetXY_SbKCs->Fill(out_x,out_y);
			}

			double QE_GaAsP=mat->extrapQE_GaAsP(wavelength);
			if(QE_GaAsP>rd->Uniform(0.,1.)){
				nel_GaAsP++; 
				hPhDetXY_GaAsP->Fill(out_x,out_y);
				hXY->Fill(out_x,out_y);
			}

			double QE_GaAs=mat->extrapQE_GaAs(wavelength);
			if(QE_GaAs>rd->Uniform(0.,1.)){
				nel_GaAs++;
				hPhDetXY_GaAs->Fill(out_x,out_y);
			}
		}
	}

	///// randomly throw 2 electrons in each event as noise
	for(int i=0; i<2; i++) {
		double out_x = rd->Uniform(-1.,1.)*44.;
		double out_y = rd->Uniform(-1.,1.)*44.;
		hXY->Fill(out_x,out_y);
	}

	hnHitAeglPerEvtvsMom->Fill(p_gen,nhitAerogel);
	hnPhotonElvsnHits_SbKCs->Fill(nhitPhoDet,nel_SbKCs);
	hnPhotonElvsnHits_GaAsP->Fill(nhitPhoDet,nel_GaAsP);
	hnPhotonElvsnHits_GaAs->Fill(nhitPhoDet,nel_GaAs);

	mDst.pid = pid_gen;
	mDst.px = px_gen;
	mDst.py = py_gen;
	mDst.pz = pz_gen;
	mDst.vx = vx_gen;
	mDst.vy = vy_gen;
	mDst.vz = vz_gen;
	mDst.theta = theta_gen;
	mDst.phi = phi_gen;
	mDst.nhit = nhits;
	mDst.nhitAegl = nhitAerogel;
	mDst.nhitPhoDet = nhitPhoDet;
	mDst.nelSbKCs = nel_SbKCs;
	mDst.nelGaAsP = nel_GaAsP;
	mDst.nelGaAs = nel_GaAs;
	mDst.Lpion = probability(hPiDB, hXY);
	mDst.LKaon = probability(hKaonDB, hXY);
	mDst.Lproton = probability(hProtonDB, hXY);

	if(mTree) mTree->Fill();
	
	delete hXY;
	#endif
	
	return 0;
}

int likelihood::end()
{
 cout<<endl;
 cout<<"likelihood::end() ----- Write out tree and histogram to files !------"<<endl;
 cout<<"This is the end of this program !"<<endl;
	if(outputfile != NULL){
		outputfile->cd();
#ifdef doLikelihoodDB
		hNEvtvsP->Write();
		hPiXY->Write();
		hKaonXY->Write();
		hProtonXY->Write();
#else
		mTree->Write();
		hnHitPerEvtvsMom->Write();
		hnHitAeglPerEvtvsMom->Write();
		hPhDetXY_bfSmear->Write();
		hPhDetXY_afSmear->Write();
		hPhDetXY_SbKCs->Write();
		hPhDetXY_GaAsP->Write();
		hPhDetXY_GaAs->Write();
		hPhotonWL->Write();
		hPhotonE->Write();
		hnPhotonElvsnHits_SbKCs->Write();
		hnPhotonElvsnHits_GaAsP->Write();
		hnPhotonElvsnHits_GaAs->Write();
#endif
		outputfile->Close();
	}
        return 0;
}

bool likelihood::isPhoton(hit *ahit, int i)
{
	if(ahit->get_pid()->at(i)==0) return true;
	else return false;
}

bool likelihood::isReflection(hit *ahit, int i)
{
	if(ahit->get_out_pz()->at(i)<0.) return true;
	else return false;
}

bool likelihood::isOnAerogel(hit *ahit, int i)
{
	if(ahit->get_out_z()->at(i)>=50.5 && ahit->get_out_z()->at(i)<=70.5) return true;
	else return false;
}

bool likelihood::isOnPhotonSensor(hit *ahit, int i)
{
	if(ahit->get_out_z()->at(i)>143 && ahit->get_out_z()->at(i)<147) return true;
	else return false;
}

double likelihood::probability(TH2D *db, TH2D *hXY)
{
	double prob=1.0;
	const double noise = 2./nPads/nPads; ///// noise level is set to be 2 electrons on photonsensor

	for(unsigned int i=1; i<=nPads; i++){   ///// photonsensor pad loop in X
		for(unsigned int j=1; j<=nPads; j++)   ///// photonsensot pad loop in Y
		{
			double k = hXY->GetBinContent(i,j); ///// should be integer (mostly 0 or 1)
			double lambda = db->GetBinContent(i,j); ///// get the expected value of Poisson distribution from DB
			if(lambda==0.) prob *= PoissonI(k,noise);
			else if(lambda>0.) prob *= PoissonI(k,lambda);
			else cout<<"DB histogram bin content should not be < 0. !"<<endl;
		}
	}
	return log(prob);
}

void likelihood::Smearing2D(double inx, double iny, double& outx, double& outy)
{
	TF1 *fx = new TF1("fx","1/([1]*sqrt(2*3.1415926))*exp(-1*pow(x-[0],2)/(2.*[1]*[1]))",inx-20.,inx+20.);
	fx->SetParameter(0,inx);
	fx->SetParameter(1,1.); //// 1 mm smearing in photon position x
	outx = fx->GetRandom();

	TF1 *fy = new TF1("fy","1/([1]*sqrt(2*3.1415926))*exp(-1*pow(x-[0],2)/(2.*[1]*[1]))",iny-20.,iny+20.);
	fy->SetParameter(0,iny);
	fy->SetParameter(1,1.); //// 1 mm smearing in photon position y
	outy = fy->GetRandom();

	delete fx;
	delete fy;
	return ;
}


////// This is the main function 
int main(int argc, char **argv)
{
	cout<<endl;
	cout<<"//////////////////////////////////////////////"<<endl;
	cout<<"This program start at: "<<endl;
	time_t now = time(0);
	char *dt = ctime(&now);
	cout<<dt<<endl;
	char currdir[256];
	cout<<"Current directory is: "<< getcwd(currdir,256)  <<endl;
	cout<<"//////////////////////////////////////////////"<<endl;
	cout<<endl;

	if(argc!=3 && argc!=1) return 0;
	char *FileInput=0;
	char *FileOutput=0;

	if(argc==1){
		FileInput  = "example.list";
		FileOutput = "example.root";
	}
	else if(argc==3){
		FileInput = argv[1];
		FileOutput = argv[2];
	}

	char fspace[256];
	sprintf(fspace,"%s/%s",currdir,FileOutput);
	TFile *fFile = new TFile(fspace,"RECREATE");
	TChain *fevt  = new TChain("generated");
	TChain *fhit  = new TChain("eic_rich");

	int fileNumber = 0;
	char FileList[512];
	ifstream* inputStream = new ifstream;
	inputStream->open(FileInput);
	if (!(inputStream)) {
		printf("can not open list file\n");
		return 0;
	}
	for (;inputStream->good();) {
		inputStream->getline(FileList,512);

		if(strstr(FileList,".root")==NULL) {
			printf("%s is not a root-file address!!!\n",FileList);
			continue;
		}
		if  ( inputStream->good()) {
			TFile *ftmp = new TFile(FileList);
			if(!ftmp||!(ftmp->IsOpen())||!(ftmp->GetNkeys())) {
				printf(" file %s error in opening!!!\n",FileList);
			}
			else {
				printf(" read in file %s\n",FileList);
				fevt->Add(FileList);
				fhit->Add(FileList);
				fileNumber++;
			}
			delete ftmp;
		}
	}
	delete inputStream;
	printf(" files read in %d\n",fileNumber);

	event *aevt = new event(fevt);  /// declear and save info to branchs for event
	hit *ahit = new hit(fhit);	/// declear and save info to branchs for track

	likelihood *lk = new likelihood(fFile);

	int nevent = (int)fevt->GetEntries();
	cout << "total number of events:  " << nevent << endl;
	for (Int_t i=0;i<nevent;i++) { 
		if(i%100==0) cout << "processing event:  " << i << " ;"<<endl;
		fevt->GetEntry(i);  
		fhit->GetEntry(i);
		lk->process_event(aevt, ahit);
	}

	lk->end();
	delete ahit;
	delete aevt;
	delete fhit;
	delete fevt;
	delete fFile;

	return 0;
}
