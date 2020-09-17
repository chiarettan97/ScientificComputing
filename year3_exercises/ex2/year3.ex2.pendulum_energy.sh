:

GIF_NAME=./year3.ex2.pendulum_energy.gif

rm ./year.ex1.gif
#rm fit.log

./year3.ex2.pendulum $*

echo "set grid;  set style line 1 lw 0.1 ; set xtics pi ; set format x '%.0P'; set title 'Energy vs. time for undamped, unforced pendulum released from rest with amplitude 0.01';  set xlabel 'time/s' ; set ylabel 'Energy/J' ; set term gif; set output '$GIF_NAME'; plot 'pendulum.dat' u 1:4 title 'Energy'" | /usr/bin/gnuplot

/opt/google/chrome/chrome $GIF_NAME
