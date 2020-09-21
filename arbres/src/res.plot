plot 'resmult852' using 2:(1.) smooth cumulative with lines t "60 + BE FIFO"
replot 'resmult852' using 3:(1.) smooth cumulative with lines t "60 + BE DEADLINE"
replot 'resmult60solo' using 2:(1.) smooth cumulative with lines t "6O solo FIFO"
replot 'resmult60solo' using 3:(1.) smooth cumulative with lines t "6O solo deadline"
replot 'resmult81solo' using 2:(1.) smooth cumulative with lines t "80 solo FIFO"
replot 'resmult81solo' using 3:(1.) smooth cumulative with lines t "80 solo deadline"

set term postscript color solid
set notitle
set xlabel "margin" 
set key bottom left 

set ylabel "cumulative distribution "
set ytics
set output '| ps2pdf - res.pdf'
replot
