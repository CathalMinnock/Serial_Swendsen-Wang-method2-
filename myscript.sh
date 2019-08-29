#!/bin/sh
#SBATCH -n 1
#SBATCH -t 6:00:00
#SBATCH -p compute
#SBATCH -J q3s8serial

samples=10000
skip_steps=1
q=3
size=8
temps=40
beta_start=0
beta_end=1
beta_diff=`echo "scale=4; $beta_end-$beta_start" | bc`
beta_increment=`echo "scale=4; $beta_diff/$temps" | bc`
beta=$beta_start

directory_name="Serial_SW_q_${q}_size_${size}"
rm -r $directory_name
mkdir $directory_name
make
for ((i = 0; i <= $temps; i++ ))
  do
    echo "HELLO" 
    filename="${directory_name}/${directory_name}_beta_${beta}.txt"
    ./swprog -x $size -y $size -z $size -q $q -b $beta -f $filename -s $samples -a $skip_steps
    beta=`echo "scale=4; $beta+$beta_increment" | bc`
 done
