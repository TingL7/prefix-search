reset
set xlabel 'numerical distribution'
set ylabel 'time(sec)'
set title 'perfomance comparison'
set term png enhanced font 'Verdana,10'
set output 'runtime2.png'

stats "cpy.txt" using 2
stats "ref.txt" using 2

plot [:][:0.00001] 'cpy.txt' using 1:2 with points title 'cpy', \
'ref.txt' using 1:2 with points title 'ref'
