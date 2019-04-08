#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <cfloat>  

#include "TTree.h"
#include "TChain.h"
#include "TFile.h"
#include "TList.h"
#include "TF1.h"
#include "TObject.h"
#include <TObjArray.h>
#include "TCollection.h"
#include <TH2.h>
#include <TH1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TPad.h>

//using namespace std;

float timebase=1e5;


void ExtractData(TChain* dtrees)
// thisname: name of source volume
// outdir: name of output directory (format should be: "full/path/to/directory")
{

  		if(dtrees==NULL){
		std::cout<<"Couldn't find DataTree"<<std::endl;
		exit(1);
	}

	//gROOT->ProcessLine("LZsoft/BACCARAT/tools/BaccRootConverterEvent.hh+");
	TFile* outfile = new TFile("conduit_feedthrough_Th.root","RECREATE");
	// Create reduced tree
	std::cout<<"Creating reduced tree"<<std::endl;
	TTree* redtree = new TTree("DTree","DTree");
	//TTree* redtree = (TTree*)outfile->Get("DTree");
	double edep=-1;
	double avgedep = -1;
	double numtracks = -1;

	redtree->Branch("dEdep_keV",&edep,"edep/D");
	redtree->Branch("davgEdep_keV",&avgedep,"avgedep/D");
	redtree->Branch("dnumtracks",&numtracks,"numtracks/D");
	

	BaccRootConverterEvent *anEvent = new BaccRootConverterEvent();
	dtrees->SetBranchAddress("Event",&anEvent); 
	Long64_t numdentries = dtrees->GetEntries();
        std::cout<<"Looping through "<<numdentries<<" events"<<std::endl;
	for(int iientr = 0; iientr < numdentries; iientr++){
		//std::cout<<"in events loop"<<std::endl;
		dtrees->GetEntry(iientr);
		//std::cout<<"entry got"<<std::endl;
        	// Get number of tracks in this event + loop
    		int ntrack = anEvent->tracks.size();
		numtracks = ntrack;
		double Etot= 0;
		std::cout<<iientr<<std::endl;
    		for(int j=0; j<ntrack; j++){
			//std::cout<<"in tracks loop"<<std::endl;
      			trackInfo aTrack = anEvent->tracks[j];
      			// Get number of steps in this track + loop
      			int nstep = aTrack.steps.size();
      			for(int k=0; k<nstep; k++){
				stepInfo aStep = aTrack.steps[k];
				TString iV_string(aStep.sVolumeName); // TString easier for comparisons 
	    			// just NR
	    			if (aTrack.iParticleID > 10000 || aTrack.iParticleID == 2212 || aTrack.iParticleID == 2112){
	      				edep = aStep.dEnergyDep_keV; 
					Etot = Etot+edep;        
			}
    		}
		avgedep = Etot/numtracks;
	}

	//std::cout<<depcount<<" Deposits found"<<std::endl;
	
	outfile->cd();

	//std::cout<<"Writing "<<outdir.Data()<<"/"<<thisname<<"_SpectrumData.root"<<std::endl;

	redtree->Write("",TObject::kOverwrite);
	outfile->Write();

} 
}
