:

rm ./year.ex1.gif
rm fit.log

./year3.ex1 $*

echo "set xrange [0:8]; set yrange [0:]; set grid;  set title 'Errors in the Monte carlo estimation vs the number of points used, N';  set xlabel 'log( N )' ; set ylabel 'log( Error )' ; set term gif; set output './year3.ex1.a.gif'; set multiplot ;  f(x) = a*x + b ; a = -0.5; b = 3.5; fit f(x) 'monte_carlo_sine.error_analysis.dat' using (log(\$1)):(log(\$2)) via a,b ;  plot 'monte_carlo_sine.dat' u (log(\$1)):(log(\$2)) title 'Errors in Monte Carlo estimation', a*x + b title 'Fit' ; unset multiplot" | /usr/bin/gnuplot

echo "set grid;  set title 'Errors in the Monte carlo estimation vs theoretical errors';  set xlabel 'Theoretical Errors' ; set ylabel 'Errors' ; set term gif; set output './year3.ex1.b.gif';  plot 'monte_carlo_sine.theoretical_error_analysis.dat' title 'Error cross-analysis in Monte Carlo estimation'" | /usr/bin/gnuplot


/opt/google/chrome/chrome ./year3.ex1.a.gif
/opt/google/chrome/chrome ./year3.ex1.b.gif
