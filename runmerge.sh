#!/bin/bash
MYDIR=/global/project/projectdirs/lz/users/lkorley/HighNR
INDIR=${MYDIR}/output
source /cvmfs/lz.opensciencegrid.org/BACCARAT/release-3.14.3/setup.sh
FileList=""
inc=""
appen=""
if [ $# -eq 2 ]; then
    inc=$2
fi
if [ $# -eq 1 ]; then
    appen=$1
fi
cd ${INDIR}
for f in *
do
	infile=${INDIR}/${f}/Plots/${inc}*_Plots.root
	if ls ${infile} 1> /dev/null 2>&1; then
		echo "${f} is a good boy"
	else
		echo "${f} is a naughty one"
		continue
	fi

	if [ "${FileList}" != "" ]
	then
		FileList=${FileList},${infile}
	else
		FileList=${infile}
	fi
done
cd ${MYDIR}
echo "List is \n ${FileList}"
sbatch --mem=5G --job-name=merge --output=${MYDIR}/sh_out/merge.%j.out --error=${MYDIR}/sh_out/merge.%j.err ${MYDIR}/NRint.slr ${MYDIR}/src/plotrates.py "MergePlot" "${FileList}" "${INDIR}" "${appen}"
