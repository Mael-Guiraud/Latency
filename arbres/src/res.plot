
 file = "resmult60solo"


plot   file  using 5:(1.) smooth cumulative title  "FIFO" 
replot   file  using 6:(1.) smooth cumulative title  "Deadline" 
replot   file  using 7:(1.) smooth cumulative title  "computed" 
replot   file  using 8:(1.) smooth cumulative title  "Distrib" 



######### Pour une sortie dans un fichier .ps ##################"

 set pointsize 1

set tics font ", 10"
set xlabel font ", 25"
set ylabel font ", 25"






set notitle
set ylabel "Success(%)"
set xlabel "Additional Latency"
set xrange [0:24000]

set key center left
set key font ",10"

set border 3
set term postscript color solid
set output '| ps2pdf - max.pdf'


replot