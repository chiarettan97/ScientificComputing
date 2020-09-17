:

GIF_NAME="./year3.ex2.pendulum.gif"
PARAM_FILE=./pendulum.params

rm "$GIF_NAME"
#rm fit.log

Q=`cut -d '|' -f1 $PARAM_FILE`
F=`cut -d '|' -f2 $PARAM_FILE`
INIT_VELOCITY=`cut -d '|' -f3 $PARAM_FILE`
INIT_POSITION=`cut -d '|' -f4 $PARAM_FILE`


./year3.ex2.pendulum $*

#echo "set grid;  set style line 1 lw 0.1 ; set xtics pi ; set format x '%.0P';" 'set title "Theta vs. time for damping coefficient' "$Q" '\\nand driving force' "$F," 'released with initial angular speed' "$INIT_VELOCITY" '\\nand initial theta ' "$INIT_POSITION\";"  "set xlabel 'time/s' ; set ylabel 'theta/rad' ; set term gif; set output '$GIF_NAME'; set multiplot; plot 'pendulum.dat' u 1:2 title 'theta', 0.01*cos(x) title 'theoretical'; unset multiplot"  | /usr/bin/gnuplot

echo "set grid;  set style line 1 lw 0.1 ; set xtics pi ; set format x '%.0P';" 'set title "Omega vs. time for damping coefficient' "$Q" '\\nand driving force' "$F," 'released with initial angular speed' "$INIT_VELOCITY" '\\nand initial theta ' "$INIT_POSITION\";"  "set xlabel 'time/s' ; set ylabel 'theta/rad' ; set term gif; set output '$GIF_NAME'; set multiplot; plot 'pendulum.dat' u 1:3 title 'omega', -0.01*sin(x) title 'theoretical'; unset multiplot"  | /usr/bin/gnuplot

#echo "set grid;  set style line 1 lw 0.1 ; set xtics pi ; set format x '%.0P';" 'set title "Energy vs. time for damping coefficient' "$Q" '\\nand driving force' "$F," 'released with initial angular speed' "$INIT_VELOCITY" '\\nand initial theta ' "$INIT_POSITION\";"  "set xlabel 'time/s' ; set ylabel 'theta/rad' ; set term gif; set output '$GIF_NAME'; set multiplot; plot 'pendulum.dat' u 1:4 title 'energy'; unset multiplot"  | /usr/bin/gnuplot

#echo "set grid;  set style line 1 lw 0.1 ; set xtics pi ; set format x '%.0P';" 'set title "Theta vs. time for damping coefficient' "$Q" '\\nand driving force' "$F," 'released with initial angular speed' "$INIT_VELOCITY" '\\nand initial theta ' "$INIT_POSITION\";"  "set xlabel 'time/s' ; set ylabel 'theta/rad' ; set term gif; set output '$GIF_NAME'; set multiplot; set size 1, 1 ; set origin 0.5, 0.5 ; plot 'pendulum.dat' u 1:2 title 'theta', 0.01*cos(x) title 'theoretical';  set size 1, 1; set origin 1.5, 1.5 ; plot 'pendulum.dat' u 1:3 title 'angular speed' ; unset multiplot"  | /usr/bin/gnuplot

/opt/google/chrome/chrome $GIF_NAME
