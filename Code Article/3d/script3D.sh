#!/bin/bash

for algo in `seq 3 6`
do
	for depart in `seq 0 4` 
	do

		echo "splot   'gvsgp3D_$depart.data'  using 1:2:$algo notitle with pm3d

		set palette defined (0 \"red\",1 \"green\")
		set pm3d interpolate 10,10
		set view map
		set cblabel \"reussite\"
		set ylabel \"periode\"
		set xlabel \"marge\"
		set term postscript color solid
		set output \"$algo.$depart.ps\"
		set key left top
		replot

		" > /home/merelas/Bureau/ngreen/code/3d/$algo.$depart.gplt
		gnuplot $algo.$depart.gplt
		ps2pdf $algo.$depart.ps
	done
done