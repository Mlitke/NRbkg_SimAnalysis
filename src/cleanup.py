import sys,ROOT,glob,os
import string
from ROOT import TTree,TBranch,TFile,TChain,TString

def cleanup(mdir):
	wcardname = mdir + "/*processlist.txt"
	processlist = []
	for filename in glob.glob(wcardname):
		oprocesslist = []
		with open(filename) as f:
			oprocesslist = f.readlines()
			for ip in oprocesslist:
				if ip not in processlist:
					processlist.append(ip)
		cmd = "rm " + filename
		os.system(cmd)

	with open('/global/homes/l/lkorley/myprojectdir/HighNR/lists/processlist.txt','w') as f:
		for ip in processlist:
			ip = re.sub(r'[^\x00-\x7f]',r'', ip)
			if ip=="":
				continue
			line = ip+"\n"
			f.write(line)

	wcardname = mdir + "/*creatorlist.txt"
	processlist = []
	for filename in glob.glob(wcardname):
		oprocesslist = []
		with open(filename) as f:
			oprocesslist = f.readlines()
			for ip in oprocesslist:
				if ip not in processlist:
					processlist.append(ip)
		cmd = "rm " + filename
		os.system(cmd)

	with open('/global/homes/l/lkorley/myprojectdir/HighNR/lists/creatorlist.txt','w') as f:
		for ip in processlist:
			ip = re.sub(r'[^\x00-\x7f]',r'', ip)
			if ip=="":
				continue
			line = ip+"\n"
			f.write(line)

if __name__ == '__main__':
    # Map command line arguments to function arguments.
    cleanup(*sys.argv[1:])
