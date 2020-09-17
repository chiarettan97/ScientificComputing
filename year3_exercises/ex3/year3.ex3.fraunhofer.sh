:

GIF_NAME="./year3.ex3.fraunhofer.gif"
PARAM_FILE=./fraunhofer_slit.params

rm "$GIF_NAME"
#rm fit.log

LAMBDA=`cut -d '|' -f1 $PARAM_FILE`
SCREEN_DISTANCE=`cut -d '|' -f2 $PARAM_FILE`
WIDTH=`cut -d '|' -f3 $PARAM_FILE`


./year3.ex3.fraunhofer $*

echo "LAMBDA = $LAMBDA"
echo "SCREEN_DISTANCE = $SCREEN_DISTANCE"
echo "WIDTH = $WIDTH"

echo "set grid;  set style line 1 lw 0.1 ;" \
'set title "Intensity of diffraction pattern' \
'\\n' \
'in the far field of slit of width d"' \
"; set xlabel 'q' ; set ylabel 'Intensity' ; set term gif; set output '$GIF_NAME'; " \
"plot 'fraunhofer_slit.dat' u 1:5 with lines lt 1 title 'slit', (sin(x * pi * $WIDTH) * (1/(pi * x)))**2" \
"title 'theoretical'; unset multiplot"   | /usr/bin/gnuplot

#echo "set grid;  set style line 1 lw 0.1 ;" \
#'set title "Intensity of diffraction pattern' \
#'\\n' \
#'in the far field of slit of width d"' \
#"; set xlabel 'q' ; set ylabel 'Intensity' ; set term gif; set output '$GIF_NAME'; " \
#"plot 'fraunhofer_slit.dat' u 1:5 with lines lt 1 title 'slit', (sin(x * pi * 100) * (1/(pi * x)))**2" \
#"title 'theoretical'; unset multiplot"   | /usr/bin/gnuplot

#echo "set grid;  set style line 1 lw 0.1 ;" \
#'set title "Intensity of diffraction pattern' \
#'\\n' \
#'in the far field of slit of width d"' \
#"; set xlabel 'y' ; set ylabel 'Intensity' ; set term gif; set output '$GIF_NAME'; " \
#'plot "fraunhofer_slit.dat" u 2:5 with lines lt 1 title "slit", (sin(x * pi * "$WIDTH" * "$SCREEN_DISTANCE" / "$LAMDA") * ("$LAMDA"/(pi * x * "$SCREEN_DISTANCE")))**2' \
#"title 'theoretical'; unset multiplot"   | /usr/bin/gnuplot


/opt/google/chrome/chrome $GIF_NAME
