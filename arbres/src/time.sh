#!/bin/bash
rm resulttime
for i in `seq 8`
do
	
    echo "#define NB_BBU $i" > nb_bbu.h
    make fpt
done
gnuplot timefpt.gplt