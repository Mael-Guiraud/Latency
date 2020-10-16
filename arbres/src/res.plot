
 file = "resmult95solo"
 file2 = "resmult40solo"


plot   file  using 2:(1.) smooth cumulative title  "FIFO 95" 
replot   file  using 3:(1.) smooth cumulative title  "CRAN FIRST - FIFO 95" 
replot   file  using 4:(1.) smooth cumulative title  "CRAN FIRST - DEADLINE 95" 
replot   file  using 6:(1.) smooth cumulative title  "DETERMINISTIC 95" 
replot   file2  using 2:(1.) smooth cumulative title  "FIFO 40" 
replot   file2  using 3:(1.) smooth cumulative title  "CRAN FIRST - FIFO 40" 
replot   file2  using 4:(1.) smooth cumulative title  "CRAN FIRST - DEADLINE 40" 


######### Pour une sortie dans un fichier .ps ##################"

 set pointsize 1

set tics font ", 10"
set xlabel font ", 25"
set ylabel font ", 25"






set notitle
set ylabel "Success(%)"
set xlabel "Additional Latency"
set xrange [0:24000]

set key center right
set key font ",10"

set border 3
set term postscript color solid
set output '| ps2pdf - max.pdf'


replot