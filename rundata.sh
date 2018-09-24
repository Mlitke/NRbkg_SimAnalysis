#!/bin/bash
DIR=/global/homes/w/wangryan/lz_project_data/wangbtc/G4_Validation_data/ML/batch_job
MYDIR=/global/homes/l/lkorley/myprojectdir/HighNR
OUTDIR=${MYDIR}/output
FileList=""

source /cvmfs/lz.opensciencegrid.org/BACCARAT/release-3.14.3/setup.sh

if [ ! -d ${OUTDIR} ]; then
	mkdir ${OUTDIR}
fi

if [ ! -d ${MYDIR}/sh_out ]; then
	mkdir ${MYDIR}/sh_out
fi
cd ${DIR}
for f in *
do 
	FileListlow=""
	if ls ${DIR}/${f}/*neutron*/out/*.root 1> /dev/null 2>&1; then
		echo "${DIR}/${f} is a good boy"
	else
		continue
	fi
	if [ ! -d ${OUTDIR}/${f} ]; then
		mkdir ${OUTDIR}/${f}
	fi

	echo "Running on $f"

	if [ "${FileList}" != "" ]
	then
		FileList=${FileList},${DIR}/${f}
	else
		FileList=${DIR}/${f}
	fi
	c=0
	for flow in ${DIR}/${f}/*neutron*
	do
		
		if [ "${FileListlow}" != "" ]
		then
			FileListlow=${FileListlow},${flow}
		else
			FileListlow=${flow}
		fi
		let c++
		cd ${MYDIR}
		if [ $1 -ne 0 ]; then
			sbatch --job-name=${f} --output=${MYDIR}/sh_out/${f}.%j.out --error=${MYDIR}/sh_out/${f}.%j.err ${MYDIR}/NRint.slr ${MYDIR}/src/reducedata.py ${flow} ${OUTDIR}/${f} ${c}_
		fi
		cd ${DIR}
	done

	cd ${MYDIR}
	#run python script with (${FileListlow},${f},${OUTDIR}) as arguments
	if [ $1 -eq 0 ]; then
		j=""
		/cvmfs/sft.cern.ch/lcg/external/Python/2.7.4/x86_64-slc6-gcc48-opt/bin/python2.7 ${MYDIR}/src/reducedata.py ${FileListlow} ${OUTDIR}/${f}
	fi
		
	cd ${DIR}
	
	if [ -f ${OUTDIR}/${f}_SpectrumData.root ]; then
		rm ${MYDIR}/old/${f}SpectrumData.root
	fi
done
cd ${MYDIR}
#echo "${FileList}"
