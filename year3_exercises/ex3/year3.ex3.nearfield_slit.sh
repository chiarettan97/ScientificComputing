:

GIF_NAME="./year3.ex3.nearfield_slit.gif"
#PARAM_FILE=./pendulum.params

rm "$GIF_NAME"
#rm fit.log

#Q=`cut -d '|' -f1 $PARAM_FILE`
#F=`cut -d '|' -f2 $PARAM_FILE`
#INIT_VELOCITY=`cut -d '|' -f3 $PARAM_FILE`
#INIT_POSITION=`cut -d '|' -f4 $PARAM_FILE`


./year3.ex3.nearfield $*

echo "set grid;  set style line 1 lw 0.1 ;" \
'set title "Intensity of diffraction pattern' \
'\\n' \
'in the near field of slit of width d"' \
"; set xlabel 'y' ; set ylabel 'Intensity' ; set term gif; set output '$GIF_NAME'; " \
"plot 'nearfield_slit.dat' u 2:5 with lines lt 1 title 'slit'" \
"; unset multiplot"   | /usr/bin/gnuplot


#echo "set grid;  set style line 1 lw 0.1 ;" \
#'set title "Intensity of diffraction pattern' \
#'\\n' \
#'in the near field of slit of width d"' \
#"; set xlabel 'q' ; set ylabel 'Intensity' ; set term gif; set output '$GIF_NAME'; " \
#"plot 'nearfield_slit.dat' u 1:5 with lines lt 1 title 'slit'" \
#"; unset multiplot"   | /usr/bin/gnuplot

/opt/google/chrome/chrome $GIF_NAME
