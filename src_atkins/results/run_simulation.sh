#::!/bin/bash
for step in  1000000 10000000
do
	for Nwin in $(seq 10 5 20 ) 
	do
		echo "step=$step Nwin=$Nwin"
		time ./atkins -sim -payfile paytable.dat -Nwin ${Nwin} -N ${step} -out simulation_${step}_${Nwin}.dat
		
	done
done
