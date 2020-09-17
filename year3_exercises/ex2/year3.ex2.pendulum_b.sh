:

GIF_NAME="./year3.ex2.pendulum_b.gif"
GIF_NAME_2="./year3.ex2.pendulum_b_omega_t.gif"
GIF_NAME_3="./year3.ex2.pendulum_b_theta_omega.gif"
GIF_NAME_4="./year3.ex2.pendulum_b_energy.gif"
GIF_NAME_ANAL="./year3.ex2.pendulum_b.period_analysis.gif"
PARAM_FILE=./pendulum.params

rm "$GIF_NAME"
rm "$GIF_NAME_ANAL"
#rm fit.log


./year3.ex2.pendulum_b $*

Q=`cut -d '|' -f1 $PARAM_FILE`
F=`cut -d '|' -f2 $PARAM_FILE`
INIT_VELOCITY=`cut -d '|' -f3 $PARAM_FILE`
INIT_POSITION=`cut -d '|' -f4 $PARAM_FILE`



#echo "INIT_POS = $INIT_POSITION"
#echo "INIT_VELOCITY = $INIT_VELOCITY"
#echo "Q = $Q"
#echo "F = $F"

echo "set grid;  set style line 1 lw 0.1 ; set xtics pi ; set format x '%.0P';" 'set title "Theta vs. time for damping coefficient' "$Q" '\\nand driving force' "$F," 'released with initial angular speed' "$INIT_VELOCITY" '\\nand initial theta ' "$INIT_POSITION\";"  "set xlabel 'time/s' ; set ylabel 'theta/rad' ; set term gif; set output '$GIF_NAME'; set multiplot; plot 'pendulum.dat' u 1:2 title 'theta', 0.01*cos(x) title 'theoretical'; unset multiplot"  | /usr/bin/gnuplot

echo "set grid;  set style line 1 lw 0.1 ; set xtics pi ; set format x '%.0P';" 'set title "Omega vs. time for damping coefficient' "$Q" '\\nand driving force' "$F," 'released with initial angular speed' "$INIT_VELOCITY" '\\nand initial theta ' "$INIT_POSITION\";"  "set xlabel 'time/s' ; set ylabel 'theta/rad' ; set term gif; set output '$GIF_NAME_2'; set multiplot; plot 'pendulum.dat' u 1:3 title 'omega', -0.01*sin(x) title 'theoretical'; unset multiplot"  | /usr/bin/gnuplot

echo "set grid; set xtics pi ; set format x '%.0P';" 'set title "Energy vs. time for damping coefficient' "$Q" '\\nand driving force' "$F," 'released with initial angular speed' "$INIT_VELOCITY" '\\nand initial theta ' "$INIT_POSITION\";"  "set xlabel 'time/s' ; set ylabel 'theta/rad' ; set term gif; set output '$GIF_NAME_3'; set multiplot; plot 'pendulum.dat' u 1:4 title 'energy'; unset multiplot"  | /usr/bin/gnuplot

echo "set grid; set xtics pi ; set format x '%.0P';" 'set title "Omega vs. theta for damping coefficient' "$Q" '\\nand driving force' "$F," 'released with initial angular speed' "$INIT_VELOCITY" '\\nand initial theta ' "$INIT_POSITION\";"  "set xlabel 'angle/rad' ; set ylabel 'angular speed/rad s-1' ; set term gif; set output '$GIF_NAME_4'; plot 'pendulum.dat' u 2:3 "  | /usr/bin/gnuplot

echo "set grid; set xtics pi ; set xtics pi ; set format x '%.0P'; set title 'Period vs. initial angle for damping coefficient "$Q" and driving force "$F"' ; set xlabel 'initial theta/rad' ; set ylabel 'time period/s' ; set term gif; set output '$GIF_NAME_ANAL'; set multiplot; plot 'year3.ex2.pendulum_period_analysis.dat' title 'period'; unset multiplot"  | /usr/bin/gnuplot

eog $GIF_NAME &
eog $GIF_NAME_2 &
eog $GIF_NAME_3 &
eog $GIF_NAME_4 &
eog $GIF_NAME_ANAL
