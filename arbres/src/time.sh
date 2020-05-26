#!/bin/bash
rm resulttime
for i in `seq 6`
do
	
    echo "#define NB_BBU $i" > nb_bbu.h
    make tests
done
gnuplot timefpt.gplt