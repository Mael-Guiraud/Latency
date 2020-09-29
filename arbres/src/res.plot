plot 'resmult60solo' using 2:(1.) smooth cumulative with lines t "FIFO"
replot 'resmult60solo' using 3:(1.) smooth cumulative with lines t "DEADLINE"
replot 'resmult60solo' using 4:(1.) smooth cumulative with lines t "computed"
replot 'resmult60solo' using 8:(1.) smooth cumulative with lines t "fpt"

set term postscript color solid
set notitle
set xlabel "margin" 
set key bottom left 

set ylabel "cumulative distribution "
set ytics
set output '| ps2pdf - res.pdf'
replot
