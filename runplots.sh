#!/bin/bash
MYDIR=/global/homes/l/lkorley/myprojectdir/HighNR
INDIR=${MYDIR}/output
source /cvmfs/lz.opensciencegrid.org/BACCARAT/release-3.14.3/setup.sh
MAKELISTS=0

if ls ${MYDIR}/lists/*list.txt 1> /dev/null 2>&1; then
	if [ ! -f ${MYDIR}/lists/processlist.txt ] && [ ! -f ${MYDIR}/lists/creatorlist ]; then
		/cvmfs/sft.cern.ch/lcg/external/Python/2.7.4/x86_64-slc6-gcc48-opt/bin/python2.7 ${MYDIR}/src/cleanup.py "${MYDIR}/lists"
	fi
	echo "Will plot process rates as well as overall rates"
else
	if [ ! -d  ${MYDIR}/lists ]; then
		mkdir ${MYDIR}/lists
	fi
	MAKELISTS=1
	echo "No Process lists found in ${MYDIR}/lists"
	echo "Will Plot overall rates only and create these lists in ${MYDIR}/lists for you"
	echo "On your next run of this script process rates will be plotted"
fi

cd ${INDIR}
for f in *
do
	if ls ${f}/*SpectrumData.root 1> /dev/null 2>&1; then
		echo "${f} is a good boy"
	else
		continue
	fi
	OUTDIR=${INDIR}/${f}/Plots
	if [ ! -d ${OUTDIR} ]; then
		mkdir ${OUTDIR}
	fi

	for flow in ${f}/*SpectrumData.root
	do
		sbatch --job-name=${f} --output=${MYDIR}/sh_out/${f}.%j.out --error=${MYDIR}/sh_out/${f}.%j.err ${MYDIR}/NRint.slr ${MYDIR}/src/plotrates.py "PlotChains" "${INDIR}/${flow} ${OUTDIR}" "cluster" ${MAKELISTS}
	done
done
cd ${MYDIR}
