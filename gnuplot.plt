set terminal png
set output "goodput.png"
set title "title"
set xlabel "xlabel"
set ylabel "ylabel"
set key bottom right
plot "throughput2.txt" using 1:2 with lines title "With Title" lw 2
