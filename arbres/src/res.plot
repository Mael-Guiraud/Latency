
 file = "resmult95solodistrib"



plot   file  using 2:(1.)  lw 10 lt rgb "red"  smooth cnorm with points title "Statistical multiplexing - FIFO" 
#replot   file  using 3:(1.) lw 10 lt rgb "#EE82EE"  smooth cnorm with points title "Statistical multiplexing - Frame preemption" 
replot   file  using 4:(1.)  lw 10 smooth cnorm with points title "Statistical multiplexing - Critical deadline" 
replot   file  using 6:(1.) lw 10 smooth cnorm with points title "Deterministic networking - PMLS" 



set pointsize 0.5

set tics font ", 15"
set xlabel font ", 20"
set ylabel font ", 20"
set key font ", 20"


set xrange[0:25000]

set size 1,0.75
set notitle
set ylabel "Probability of Success"
set xlabel "Margin"

set yrange [0:1]
set border 3
set term postscript color solid
set output '| ps2pdf - stochasticdistrib.pdf'

set key right bot
replot


