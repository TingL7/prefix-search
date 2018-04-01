reset
set xlabel 'numerical distribution'
set ylabel 'time(sec)'
set title 'perfomance comparison'
set term png enhanced font 'Verdana,10'
set output 'runtime3.png'

plot [:][:0.00001] 'cpy.txt' using 1:2 with points title 'cpy'
#'ref.txt' using 1:2 with points title 'ref'
