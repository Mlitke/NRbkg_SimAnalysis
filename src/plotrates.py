import sys,ROOT,glob,os
import string
from ROOT import TH2F,TH1F,TTree,TFile,TChain,TString,THStack

#ROOT.gROOT.LoadMacro("/cvmfs/lz.opensciencegrid.org/BACCARAT/release-3.14.3/tools/BaccRootConverterEvent.hh+")
ROOT.gROOT.LoadMacro("/global/homes/l/lkorley/myprojectdir/HighNR/src/PlotRates.C")

"""
Set of functions for plotting from reduced data root files
"""

def GetProcesses(filelist,appendagename=""):
	"""
	Generates list of processes and creator processes in the dataset for plotting if not already done
	"""

	pname = "/global/homes/l/lkorley/myprojectdir/HighNR/lists/"+appendagename+"processlist.txt"
	cpname = "/global/homes/l/lkorley/myprojectdir/HighNR/lists/"+appendagename+"creatorlist.txt"

	oprocesslist = []
	ocprocesslist = []
	if os.path.exists(pname):
		with open(pname) as f:
			oprocesslist = f.readlines()
	else:
		cmd = "touch " + pname
		os.system(cmd)

	if os.path.exists(cpname):
		with open(cpname) as f:
			ocprocesslist = f.readlines()
	else:
		cmd = "touch " + cpname
		os.system(cmd)

	processes = TString("")
	cprocesses = TString("")
	
	for filename in glob.glob(filelist):
		mychain = TChain("DTree")
		mychain.Add(str(filename))
		ROOT.listprocess(mychain,processes,cprocesses)
		for ip in processes.Data().split(','):
			if ip not in oprocesslist:
				oprocesslist.append(ip)
				with open(pname,"a") as f:
					line = ip+"\n"
					f.write(line)
		for ip in cprocesses.Data().split(','):
			if ip not in ocprocesslist:
				ocprocesslist.append(ip)
				with open(cpname,"a") as f:
					line = ip+"\n"
					f.write(line)

#####################################################################################

def PlotChains(filelist,outdir,sourcename="cluster",makelists=0):
	"""
	Plots a spectra for a comma seperated list of files
	Eventually should include a number of cuts including fiducial region cuts
	Output saved in outdir/<sourcename>_Plot.root
	"""
	indirs = filelist.split(',')

	if sourcename=="cluster" and len(indirs)==1:
		fpath = indirs[0].split('/')[-1]
		sourcename = fpath.rstrip('_SpectrumData.root')

	mychain = TChain("DTree")
	for folder in indirs:
		#folder += "/*_SpectrumData.root"
		for filename in glob.glob(folder):
			mychain.Add(str(filename))

	pname = "/global/homes/l/lkorley/myprojectdir/HighNR/lists/processlist.txt"
	cpname = "/global/homes/l/lkorley/myprojectdir/HighNR/lists/creatorlist.txt"
	if os.path.exists(pname):
		with open(pname) as f:
			proclist = f.readlines()
			for iproc in proclist:
				if iproc=="":
					continue
				print "Creator processes: "+iproc
				ROOT.WriteHists(mychain,TString(outdir),TString(sourcename),TString("process("+icproc+")"))

	if os.path.exists(cpname):
		with open(cpname) as f:
			cproclist = f.readlines()
			for icproc in cproclist:
				if icproc=="":
					continue
				print "Process: "+icproc
				ROOT.WriteHists(mychain,TString(outdir),TString(sourcename),TString(""),TString("creator("+icproc+")"))

	ROOT.WriteHists(mychain,TString(outdir),TString(sourcename))

	if makelists==1:
		GetProcesses(filelist,sourcename+"_")

#####################################################################################

#Mike's thstack function goes here

#####################################################################################

def MergePlot(filelist,outdir,appendagename=""):
	"""
	Merge Plots from the different components and draw the rate histograms,
	eventually should output thstack for various cut-types
	output saved in outdir/<appendagename>MergedPlots.root
	"""
	indirs = filelist.split(',')

	outname = outdir+"/"+appendagename+"MergedPlots.root"
	outfile = TFile(outname,"RECREATE")

	pname = "/global/homes/l/lkorley/myprojectdir/HighNR/lists/processlist.txt"
	cpname = "/global/homes/l/lkorley/myprojectdir/HighNR/lists/creatorlist.txt"

	if os.path.exists(pname):
		with open(pname) as f:
			proclist = f.readlines()
			nrhists = {}
			erhists = {}
			for iproc in proclist:
				print "Processes: "+iproc
				for folder in indirs:
					for filename in glob.glob(folder):
						sourcepath = filename.split('/')[-1]
						sourcename = sourcepath.rstrip('_Plots.root')
						infile = TFile(filename)
						ROOT.MergeHists(outfile,infile,TString(sourcename),TString("process("+icproc+")"))
				
				thisnr = TH1F("hdummy1","dummy",10,0,10)
				thiser = TH1F("hdummy1","dummy",10,0,10)
				checkout = ROOT.DrawMerged(outfile,TString(outdir),thisnr,thiser,TString(iproc))
				if checkout != 0:
					print "ERROR :: One of the merged Hists could NOT be loaded!!!\n"
				else:
					nrhists[iproc] = thisnr
					erhists[iproc] = thiser
			#send dictionaries to mikes function


	if os.path.exists(cpname):
		with open(cpname) as f:
			cproclist = f.readlines()
			nrhists = {}
			erhists = {}
			for icproc in cproclist:
				print "Creator Process: "+icproc
				for folder in indirs:
					for filename in glob.glob(folder):
						sourcepath = filename.split('/')[-1]
						sourcename = sourcepath.rstrip('_Plots.root')
						infile = TFile(filename)
						ROOT.MergeHists(outfile,infile,TString(sourcename),TString(""),TString("creator("+icproc+")"))
				
				thisnr = TH1F("hdummy1","dummy",10,0,10)
				thiser = TH1F("hdummy1","dummy",10,0,10)
				checkout = ROOT.DrawMerged(outfile,TString(outdir),thisnr,thiser,TString(""),TString(icproc))
				if checkout != 0:
					print "ERROR :: One of the merged Hists could NOT be loaded!!!\n"
				else:
					nrhists[iproc] = thisnr
					erhists[iproc] = thiser

	for folder in indirs:
		nrhists = {}
		erhists = {}
		for filename in glob.glob(folder):
			sourcepath = filename.split('/')[-1]
			sourcename = sourcepath.rstrip('_Plots.root')
			infile = TFile(filename)
			ROOT.MergeHists(outfile,infile,TString(sourcename))
		thisnr = TH1F("hdummy1","dummy",10,0,10)
		thiser = TH1F("hdummy1","dummy",10,0,10)
		checkout = ROOT.DrawMerged(outfile,TString(outdir),thisnr,thiser,"","","","LiquidXenonTarget","n")
		if checkout != 0:
			print "ERROR :: One of the merged Hists could NOT be loaded!!!\n"
		else:
			nrhists[iproc] = thisnr
			erhists[iproc] = thiser
	#to stacking function

	dumnr = TH1F("hdummy1","dummy",10,0,10)
	dumer = TH1F("hdummy1","dummy",10,0,10)
	ROOT.DrawMerged(outfile,TString(outdir),dumnr,dumer)

#####################################################################################

if __name__ == '__main__':
	# Map command line arguments to function arguments.
	methodname = sys.argv[1]

	if methodname == "PlotChains":
		PlotChains(*sys.argv[2:])
	if methodname == "GetProcesses":
		GetProcesses(*sys.argv[2:])
	if methodname == "MergePlot":
		MergePlot(*sys.argv[2:])
