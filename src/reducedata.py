import sys,ROOT,glob,os
import string
from ROOT import TH2D,TH1D,TTree,TBranch,TFile,TChain,TString

ROOT.gROOT.LoadMacro("/cvmfs/lz.opensciencegrid.org/BACCARAT/release-3.14.3/tools/BaccRootConverterEvent.hh+")
ROOT.gROOT.LoadMacro("/global/homes/l/lkorley/myprojectdir/HighNR/src/ReduceData.C+")


def reduce(dirlist,outdir,listname=""):
	indirs = dirlist.split(',')
	pname_wa = 'w'
	cpname_wa = 'w'
	oprocesslist = []
	ocprocesslist = []
	pname = outdir+"/"+listname+"processlist.txt"
	cpname = outdir+"/"+listname+"creatorlist.txt"
	if os.path.exists(pname):
		pname_wa ='a'
		with open(pname) as f:
			oprocesslist = f.readlines()
	if os.path.exists(cpname):
		cpname_wa = 'a'
		with open(cpname) as f:
			ocprocesslist = f.readlines()

	processlist = []
	cprocesslist = []

	processes = TString("")
	cprocesses = TString("")
	for folder in indirs:
		mychain = TChain("DataTree")
		print "\nAdding: " + folder
		outnames = folder.rsplit('/')[-1]
		sourcename = outnames.split('_',1)[-1]
		folder += "/out/*.root"	
		mychain.Add(str(folder))
		print "Processing " + sourcename

		ROOT.ExtractData(mychain,TString(sourcename),TString(outdir),processes,cprocesses)

		for ip in processes.Data().split(','):
			if ip not in oprocesslist and ip not in processlist and ip!="":
				processlist.append(ip)
		for ip in cprocesses.Data().split(','):
			if ip not in ocprocesslist and ip not in cprocesslist and ip!="":
				cprocesslist.append(ip)

	with open(pname,pname_wa) as f:
		for ip in processlist:
			line = ip+"\n"
			f.write(line)
	with open(cpname,cpname_wa) as f:
		for ip in cprocesslist:
			line = ip+"\n"
			f.write(line)

if __name__ == '__main__':
    # Map command line arguments to function arguments.
    reduce(*sys.argv[1:])