#include "/cvmfs/lz.opensciencegrid.org/BACCARAT/release-3.14.3/tools/BaccRootConverterEvent.hh"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <cfloat> 
//#include <vector>
//#include <functional>   
//#include <numeric>      

#include "TTree.h"
//#include "TBranch.h"
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
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

//using namespace std;

float timebase=1e5;


void ExtractData(TChain* dtrees,TString thisname,TString outdir,TString *processes,TString *cprocesses)
// thisname: name of source volume
// outdir: name of output directory (format should be: "full/path/to/directory")
{

	if(dtrees==NULL){
		std::cout<<"Couldn't find DataTree"<<std::endl;
		exit(1);
	}

	TTreeReader myReader(dtrees);
	TTreeReaderValue<BaccRootConverterEvent> event(myReader,"Event");

	Long64_t numdentries = dtrees->GetEntries();

	TFile* outfile = new TFile(outdir+"/"+thisname+"_SpectrumData.root","RECREATE");
	// Create reduced tree
	std::cout<<"Creating reduced tree"<<std::endl;
	TTree* redtree = new TTree("DTree","DTree");
	//TTree* redtree = (TTree*)outfile->Get("DTree");
	char* tParticleName = new char[1];
	char* svolume = new char[1];
	char* tvolume = new char[1];
	char* tprocess = new char[1];
	char* tcreatorprocess = new char[1];
	double edep=-1;
	double deptime=-1;
	double depPosition[3]={-1,-1,-1};
	double dweight=0.01; //since 100 files per macro
	int particleID=-1;

	redtree->Branch("cParticleName",tParticleName,"tParticleName/C");
	redtree->Branch("iParticleID",&particleID,"particleID/I");
	redtree->Branch("cCreatorProcess",tcreatorprocess,"tcreatorProcess/C");
	//redtree->Branch("dweight",&dweight,"dweight/D");
	redtree->Branch("dEdep_keV",&edep,"edep/D");
	redtree->Branch("dTime_ns",&deptime,"deptime/D");
	redtree->Branch("dPosition_mm",depPosition,"depPosition[3]/D");
	redtree->Branch("cDepVolume",tvolume,"tvolume/C");
	//redtree->Branch("cSourceVolume",svolume,"svolume/C");
	redtree->Branch("cProcess",tprocess,"tprocess/C");
	
	unsigned long int depcount=0;
	unsigned long int depprintfreq=1e7;
	unsigned long int evtprintfreq=1e5;
	//double datafraction=0;
	//double frac80=0;
	std::cout<<"Looping through "<<numdentries<<" events"<<std::endl;
	while(myReader.Next()){
		if(event->primaryParticles.size() == 0) continue;
		//if(iEvt%evtprintfreq==0) std::cout<<"Looping through tracks of event "<<iEvt<<std::endl;
		//if(iEvt%evtprintfreq==0) std::cout<<"tracks size: "<<event->tracks.size()<<std::endl;
		for(unsigned long int iTrk=0; iTrk<event->tracks.size(); iTrk++){
			//if(iTrk%evtprintfreq==0) std::cout<<"steps size: "<<event->tracks[iTrk].steps.size()<<std::endl;
			std::string particlename = event->tracks[iTrk].sParticleName;
			strcpy(tParticleName,particlename.data());
			particleID = event->tracks[iTrk].iParticleID;
			for(unsigned long int iStp=0; iStp<event->tracks[iTrk].steps.size(); iStp++){
				edep = event->tracks[iTrk].steps[iStp].dEnergyDep_keV;

				if(edep>0.1){
					if(depcount%depprintfreq==0) std::cout<<"\nNew deposit found: "<<edep<<std::endl;
					deptime = event->tracks[iTrk].steps[iStp].dTime_ns;

					for(int ipos=0; ipos<3; ipos++)
						depPosition[ipos] = event->tracks[iTrk].steps[iStp].dPosition_mm[ipos];

					if(depcount%depprintfreq==0) std::cout<<"setting strings for deposit "<<depcount<<std::endl;

					std::string volume = event->tracks[iTrk].steps[iStp].sVolumeName;
					std::string process = event->tracks[iTrk].steps[iStp].sProcess;
					std::string creatorprocess = event->tracks[iTrk].sCreatorProcess;

					if(depcount%depprintfreq==0) std::cout<<"Checking volumes"<<std::endl;

					if(volume!="ScintillatorCenter" && volume!="LiquidXenonTarget" && volume!="LiquidSkinXenon" && volume!="LiquidSkinXenonBank" && volume!="ReverseFieldRegion")
						continue;

					strcpy(tvolume,volume.data());
					strcpy(tprocess,process.data());
					strcpy(tcreatorprocess,creatorprocess.data());

					if(depcount%depprintfreq==0) std::cout<<"Filling tree"<<std::endl;
					redtree->Fill();
					if(depcount%depprintfreq==0) std::cout<<"Next!!"<<std::endl;
					depcount++;
					if(*processes!="") *processes+=TString(",");
					if(!processes->Contains(TString(tprocess))) *processes+=TString(tprocess);
					
					if(*cprocesses!="") *cprocesses+=TString(",");
					if(!cprocesses->Contains(TString(tcreatorprocess))) *cprocesses+=TString(tcreatorprocess);
					
					strcpy(tvolume,"");
					strcpy(tprocess,"");
					strcpy(tcreatorprocess,"");
					edep = -1;
					deptime = -1;
					for(int ipos=0; ipos<3; ipos++)
						depPosition[ipos] = -1;
				}
			}// end of step loop
			strcpy(tParticleName,"");
		}// end of track loop
	}// end of event loop

	std::cout<<depcount<<" Deposits found"<<std::endl;
	
	outfile->cd();

	std::cout<<"Writing "<<outdir.Data()<<"/"<<thisname<<"_SpectrumData.root"<<std::endl;

	redtree->Write("",TObject::kOverwrite);
	outfile->Write();

}	
