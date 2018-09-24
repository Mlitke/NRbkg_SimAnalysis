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

float timebase=1e5;

TCanvas *drawhists2(TString name,TH2F* h1,TH1F* h2,TH2F* h4,TH1F* h5){
   TCanvas* c1=new TCanvas(name,name,768*2.5,768*2.5);
   //TPaveLabel* title = new TPaveLabel(0.1,0.96,0.9,0.99,name);
   //title->Draw();
   TPad* graphPad = new TPad("Graphs","Graphs",0.01,0.05,0.95,0.95);
   
   h1->SetStats(false);
   h2->SetStats(false);

   h4->SetStats(false);
   h5->SetStats(false);

   graphPad->cd();
   graphPad->Divide(2,2);

   graphPad->cd(1);
   gPad->SetLogz();
   gPad->SetLogx();
   h1->Draw("colz");
   
   graphPad->cd(2);
   gPad->SetLogy();
   gPad->SetLogx();
   h2->Draw();

   graphPad->cd(3);
   gPad->SetLogz();
   gPad->SetLogx();
   h4->Draw("colz");

   graphPad->cd(4);
   gPad->SetLogy();
   gPad->SetLogx();
   h5->Draw();


   graphPad->cd();
   c1->cd();
   graphPad->Draw();
   gPad->Update();
   //graphPad->SetTitle(name);
   return c1;
}


TCanvas *drawhists3(TString name,TH2F* h1,TH1F* h2,TH1F* h3,TH2F* h4,TH1F* h5,TH1F* h6){
   TCanvas* c1=new TCanvas(name,name,768*3.5,768*2.5);
   //TPaveLabel* title = new TPaveLabel(0.1,0.96,0.9,0.99,name);
   //title->Draw();
   TPad* graphPad = new TPad("Graphs","Graphs",0.01,0.05,0.95,0.95);
   
   h1->SetStats(false);
   h2->SetStats(false);
   h3->SetStats(false);
   h4->SetStats(false);
   h5->SetStats(false);
   h6->SetStats(false);

   graphPad->cd();
   graphPad->Divide(3,2);

   graphPad->cd(1);
   gPad->SetLogz();
   gPad->SetLogx();
   h1->Draw("colz");
   
   graphPad->cd(2);
   gPad->SetLogy();
   gPad->SetLogx();
   h2->Draw();

   graphPad->cd(3);
   gPad->SetLogy();
   gPad->SetLogx();
   h3->Draw();

   graphPad->cd(4);
   gPad->SetLogz();
   gPad->SetLogx();
   h4->Draw("colz");

   graphPad->cd(5);
   gPad->SetLogy();
   gPad->SetLogx();
   h5->Draw();

   graphPad->cd(6);
   gPad->SetLogy();
   gPad->SetLogx();
   h3->Draw();

   graphPad->cd();
   c1->cd();
   graphPad->Draw();
   gPad->Update();
   //graphPad->SetTitle(name);
   return c1;
}

TObject* FindFitRates(TH2F* h_evt, TString name,double timeup=timebase)
//Calculate rate of each energy bin, by fitting to exponential
//Returns hist of rates
{
	//TH1D* h = new TH1D(name+"_"+volume+"_Rate",name+" "+volume+" Recoil Rate",500,0,3000);
	Int_t nBins = h_evt->GetNbinsX();
	TObjArray* aSlices; //stores slices of hist
	TF1* myexpo = new TF1("fr","[0]*exp(-[1]*x)",0,timeup);
	h_evt->FitSlicesY(myexpo,0,-1,0,"QNR",aSlices);
	return (aSlices->At(1));
}

TH1F* FindAvRates(TH2F* h_evt,TString name,double timeup=timebase)
//Calculate average rate (N/T) of each energy bin
//returns histogram of rate vs. Energy
{
	Int_t nBins = h_evt->GetNbinsX();
	Int_t nBinsT = h_evt->GetNbinsY();
	TH1F* h = new TH1F(name+"_AvRate",name+" Mean Recoil Rate ;E [keV]",nBins,0,h_evt->GetXaxis()->GetXmax());
	for(int iBin=0; iBin<=nBins; iBin++){
		double rate = h_evt->Integral(iBin,iBin,0,nBinsT)/timeup;
		h->SetBinContent(iBin,rate);
	}
	return h;
}

std::pair<TH2F*,TH2F*> Get2DHist(TChain* reducedtree,TString name,TString process="",TString creatorprocess="",TString particlename="",TString volume="LiquidXenonTarget",double timeup=timebase)
// Generate 2D histogram (Energy vs. Time) for given cuts on deposit volume, process, creator process, particle
// to do: implement position cuts for fiducial region rates
{
	TString plotname(name.Data());
	bool bname=false; if(particlename!=""){ bname=true; plotname+="_"+particlename;}
	bool bvol=false; if(volume!=""){ bvol=true; plotname+="_"+volume;}
	bool bproc=false; if(process!=""){ bproc=true; plotname+="_"+process;}
	bool bcproc=false; if(creatorprocess!=""){ bcproc=true; plotname+="_"+creatorprocess;}

	TH2F* hNR = new TH2F("h_NR_"+plotname,plotname+" Time resolved deposits (NR);E [keV];t [ns]",10000,0,3000,2000,0,timeup);
	TH2F* hER = new TH2F("h_ER_"+plotname,plotname+" Time resolved deposits (ER);E [keV];t [ns]",10000,0,3000,2000,0,timeup);

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

	reducedtree->SetBranchAddress("dEdep_keV",&edep);
	reducedtree->SetBranchAddress("dTime_ns",&deptime);
	reducedtree->SetBranchAddress("cDepVolume",tvolume);
	reducedtree->SetBranchAddress("cProcess",tprocess);
	reducedtree->SetBranchAddress("cCreatorProcess",tcreatorprocess);
	reducedtree->SetBranchAddress("cParticleName",tParticleName);
	reducedtree->SetBranchAddress("iParticleID",&particleID);

	Long64_t nEvts=reducedtree->GetEntries();
	std::cout<<"INFO::Looping over "<<nEvts<<" Deposits"<<std::endl;
	for(Long64_t iEvt=0; iEvt<nEvts; iEvt++){
		reducedtree->GetEntry(iEvt);

		//some if statements to handle cuts
		//std::string mvolume(tvolume);
		if((TString(tvolume).Contains(volume) && bvol) || TString(tvolume)=="") continue;
		
		//std::string mproc(tprocess);
		if((TString(tprocess).Contains(process) && bproc) || TString(tprocess)=="") continue;

		//std::string mcproc(tcreatorprocess);
		if((TString(tcreatorprocess).Contains(creatorprocess) && bcproc) || TString(tcreatorprocess)=="") continue;

		//std::string mname(particlename);
		if((TString(tParticleName).Contains(particlename) && bname) || TString(tParticleName)=="") continue;

		
		if(abs(particleID)==2212 || abs(particleID)==2112 || abs(particleID)>10000) // NR
			hNR->Fill(edep,deptime,dweight);
		else if(abs(particleID)==11 || abs(particleID)==22) // ER
			hER->Fill(edep,deptime,dweight);
	}

	return make_pair(hNR,hER);
}

void WriteHists(TChain* reducedtree,TString outdir,TString name,TString process="",TString creatorprocess="",TString particlename="",TString volume="LiquidXenonTarget",double timeup=timebase){
	
	std::cout<<"Getting 2D Hists"<<std::endl;		
	std::pair<TH2F*,TH2F*> hpair = Get2DHist(reducedtree,name,process,creatorprocess,particlename,volume,timeup);
	
	TString plotname(name.Data());
	if(particlename!=""){plotname+="_"+particlename;}
	if(volume!=""){plotname+="_"+volume;}
	if(process!=""){plotname+="_"+process;}
	if(creatorprocess!=""){plotname+="_"+creatorprocess;}

	TFile* fout = new TFile(outdir+"/"+name+"_Plots.root","Update");

	std::cout<<"Calculating N/T"<<std::endl;
	std::pair<TH1F*,TH1F*> havpair;
	havpair.first = FindAvRates(hpair.first,TString("h_NR_")+plotname,hpair.first->GetYaxis()->GetXmax());
	havpair.second = FindAvRates(hpair.second,TString("h_ER_")+plotname,hpair.second->GetYaxis()->GetXmax());
	
	//std::cout<<"Calculating Fit Rate"<<std::endl;
	//std::pair<TH1F*,TH1F*> hfitpair;
	//hfitpair.first = (TH1F*)FindFitRates(hpair.first,TString("NR_")+plotname,hpair.first->GetYaxis()->GetXmax());
	//hfitpair.second = (TH1F*)FindFitRates(hpair.second,TString("ER_")+plotname,hpair.second->GetYaxis()->GetXmax());
	
	std::cout<<"Writing results to file"<<std::endl;
	hpair.first->Write("",TObject::kWriteDelete);
	hpair.second->Write("",TObject::kWriteDelete);
	havpair.first->Write("",TObject::kWriteDelete);
	havpair.second->Write("",TObject::kWriteDelete);
	//hfitpair.first->Write("",TObject::kWriteDelete);
	//hfitpair.second->Write("",TObject::kWriteDelete);

	std::cout<<"Drawing Canvas"<<std::endl;
	//TCanvas *c1 = drawhists3(plotname,hpair.first,havpair.first,hfitpair.first,hpair.second,havpair.second,hfitpair.second);
	TCanvas *c1 = drawhists2(plotname,hpair.first,havpair.first,hpair.second,havpair.second);
	c1->Write("",TObject::kWriteDelete);
	
	fout->Write();
	
	std::stringstream oss;
	oss<<outdir<<"/"<<plotname.Data()<<".png";
	std::string str(oss.str());
	const char *pngname=str.c_str();
	std::cout<<"Saving canvas as "<<pngname<<std::endl;
	gPad->Update();
	c1->SaveAs(pngname);
}

void MergeHists(TFile* fmerged,TFile* fadd,TString name,TString process="",TString creatorprocess="",TString particlename="",TString volume="LiquidXenonTarget")
//merge histograms from cluster plotting
{
	TString plotname(name.Data());
	TString mergedname("");
	if(particlename!=""){plotname+="_"+particlename; mergedname+="_"+particlename;}
	if(volume!=""){plotname+="_"+volume; mergedname+="_"+volume;}
	if(process!=""){plotname+="_"+process; mergedname+="_"+process;}
	if(creatorprocess!=""){plotname+="_"+creatorprocess;mergedname+="_"+creatorprocess;}

	TString nr2d = TString("h_NR_")+plotname;
	TString er2d = TString("h_ER_")+plotname;
	TString nr1dav = TString("h_NR_")+plotname+TString("_AvRate");
	TString er1dav = TString("h_ER_")+plotname+TString("_AvRate");

	std::cout<<"Merging in NR 2D"<<std::endl;
	if(fadd->GetListOfKeys()->Contains(nr2d.Data())){
		TH2F *h1,*h2;
		h1 = (TH2F*)fadd->Get(nr2d.Data()); 
		TString mname="h_NR_EvT"+mergedname;
		if( fmerged->GetListOfKeys()->Contains(mname.Data()) ){
			h1->SetName("nr2copy");
			h2 = (TH2F*)fmerged->Get(nr2d.Data());
			h2->Add(h1);
			fmerged->cd();
			h2->Write(mname.Data(),TObject::kWriteDelete);
		}
		else{
			fmerged->cd();
			h1->Write(mname.Data(),TObject::kWriteDelete);
		}
	}
	else
		std::cout<<"WARNING :: "<<nr2d<<" Not found in input file!!!"<<std::endl;

	std::cout<<"Merging in ER 2D"<<std::endl;
	if(fadd->GetListOfKeys()->Contains(er2d.Data())){
		TH2F *h1,*h2;
		h1 = (TH2F*)fadd->Get(er2d.Data()); 
		TString mname="h_ER_EvT"+mergedname;
		if( fmerged->GetListOfKeys()->Contains(mname.Data()) ){
			h1->SetName("er2copy");
			h2 = (TH2F*)fmerged->Get(er2d.Data());
			h2->Add(h1);
			fmerged->cd();
			h2->Write(mname.Data(),TObject::kWriteDelete);
		}
		else{
			fmerged->cd();
			h1->Write(mname.Data(),TObject::kWriteDelete);
		}
	}
	else
		std::cout<<"WARNING :: "<<er2d<<" Not found in input file!!!"<<std::endl;


	std::cout<<"Merging in NR 1D"<<std::endl;
	if(fadd->GetListOfKeys()->Contains(nr1dav.Data())){
		TH1F *h1,*h2;
		h1 = (TH1F*)fadd->Get(nr1dav.Data()); 
		TString mname="h_NR_AvRate"+mergedname;
		if( fmerged->GetListOfKeys()->Contains(mname.Data()) ){
			h1->SetName("nr1copy");
			h2 = (TH1F*)fmerged->Get(nr1dav.Data());
			h2->Add(h1);
			fmerged->cd();
			h2->Write(mname.Data(),TObject::kWriteDelete);
		}
		else{
			fmerged->cd();
			h1->Write(mname.Data(),TObject::kWriteDelete);
		}
	}
	else
		std::cout<<"WARNING :: "<<nr1dav<<" Not found in input file!!!"<<std::endl;

	std::cout<<"Merging in ER 1D"<<std::endl;
	if(fadd->GetListOfKeys()->Contains(er1dav.Data())){
		TH1F *h1,*h2;
		h1 = (TH1F*)fadd->Get(er1dav.Data()); 
		TString mname="h_ER_AvRate"+mergedname;
		if( fmerged->GetListOfKeys()->Contains(mname.Data()) ){
			h1->SetName("er1copy");
			h2 = (TH1F*)fmerged->Get(mname.Data());
			h2->Add(h1);
			fmerged->cd();
			h2->Write(mname.Data(),TObject::kWriteDelete);
		}
		else{
			fmerged->cd();
			h1->Write(mname.Data(),TObject::kWriteDelete);
		}
	}
	else
		std::cout<<"WARNING :: "<<er1dav<<" Not found in input file!!!"<<std::endl;
	
	//fmerged->Write();
}

int DrawMerged(TFile* fmerged,TString outdir,TH1F* hnr,TH1F* her,TString process="",TString creatorprocess="",TString particlename="",TString volume="LiquidXenonTarget",TString savepng="y"){
	TString mergedname("");
	if(particlename!=""){mergedname+="_"+particlename;}
	if(volume!=""){ mergedname+="_"+volume;}
	if(process!=""){ mergedname+="_"+process;}
	if(creatorprocess!=""){mergedname+="_"+creatorprocess;}
	
	TH2F *nr2d,*er2d;
	//TH1F* nr1dav,er1dav;
	TString nr2dname = TString("h_NR_EvT")+mergedname;
	TString er2dname = TString("h_ER_EvT")+mergedname;
	TString nr1davname = TString("h_NR_AvRate")+mergedname+TString("_AvRate");
	TString er1davname = TString("h_ER_AvRate")+mergedname+TString("_AvRate");

	if(fmerged->GetListOfKeys()->Contains(nr2dname.Data()))
		nr2d = (TH2F*)fmerged->Get(nr2dname);
	else
		return 1;

	if(fmerged->GetListOfKeys()->Contains(er2dname.Data()))
		er2d = (TH2F*)fmerged->Get(er2dname);
	else
		return 1;

	if(fmerged->GetListOfKeys()->Contains(nr1davname.Data()))
		hnr = (TH1F*)fmerged->Get(nr1davname);
	else
		return 1;

	if(fmerged->GetListOfKeys()->Contains(er1davname.Data()))
		her = (TH1F*)fmerged->Get(er1davname);
	else
		return 1;

	fmerged->cd();

	TCanvas *c1 = drawhists2(TString("Merged")+mergedname,nr2d,hnr,er2d,her);
	c1->Write("",TObject::kWriteDelete);

	fmerged->Write();
	if(savepng=="y"){
		std::stringstream oss;
		oss<<outdir<<"/Merged"<<mergedname<<".png";
		std::string str(oss.str());
		const char *pngname=str.c_str();
		std::cout<<"Saving canvas as "<<pngname<<std::endl;
		gPad->Update();
		c1->SaveAs(pngname);
	}

	return 0;
}

void listprocess(TChain* trees,TString* processes,TString* cprocesses){
	char* tprocess = new char[1];
	char* tcreatorprocess = new char[1];
	Long64_t nEvts=trees->GetEntries();
	trees->SetBranchAddress("cProcess",tprocess);
	trees->SetBranchAddress("cCreatorProcess",tcreatorprocess);

	for(Long64_t iEvt=0; iEvt<nEvts; iEvt++){
		trees->GetEntry(iEvt);
		if(*processes!="") *processes+=TString(",");
		if(!processes->Contains(TString(tprocess))) *processes+=TString(tprocess);
					
		if(*cprocesses!="") *cprocesses+=TString(",");
		if(!cprocesses->Contains(TString(tcreatorprocess))) *cprocesses+=TString(tcreatorprocess);
	}
}
