#!/bin/bash

# Shell script for running Trojan asteroid project
# Author : Chiara Leadbeater
# Date : April 2018


PNG_DIR=png
MAIN_OUTPUT_DIR=output



prepare_plot_for_latex ()
{
	if [ -z $GENERATE_LATEX ]
	then
		return
	fi

	PLOT_NAME=`basename -s .png $1`
	PLOT_DIR=`dirname $1`
	shift
	CAPTION="$1"
	shift
	FIG_NAME="$1"
	shift
	EXTRA_NAME="$1"
	shift
	LATEX_FILE="$PLOT_DIR/$PLOT_NAME$EXTRA_NAME.fig.tex"
	FIG_NAME="$FIG_NAME$EXTRA_NAME"
	PRODUCE_FOOTNOTE="1"
	FIG_WIDTH=""
	FIG_SIZE="width=\\textwidth"
	FIG_TYPE="figure"
	FIG_SUBTYPE="H"
	FIG_COMMENT=
	for i in 1 2 3
	do
		if [ $# -gt 0 ]
		then
			if [ "$1" = "NO-PRODUCE-FOOTNOTE" ]
			then
				PRODUCE_FOOTNOTE="0"
				shift
			elif [ "$1" = "SUBFIGURE" ]
			then
				FIG_TYPE="subfigure"
				FIG_WIDTH="{0.45\textwidth}"
				FIG_SIZE="width=0.99\linewidth"
				FIG_SUBTYPE="t"
				shift
			fi
		fi
	done

	printf '\\begin{%s}[%s]%s\n' \
		"$FIG_TYPE" "$FIG_SUBTYPE" "$FIG_WIDTH" > $LATEX_FILE
	printf '\t\\centering\n' >> $LATEX_FILE
	printf '\t\\includegraphics[%s]{{%s%s\n' \
			"$FIG_SIZE" "$PLOT_NAME" '}.png}' >> $LATEX_FILE
	printf '\t\\caption{%s%s\n' "$CAPTION" '}' >> $LATEX_FILE
	printf '\t\\label{fig:%s%s\n' "$FIG_NAME" '}' >> $LATEX_FILE
	if [ $PRODUCE_FOOTNOTE = "1" ]
	then
		if [ $# -gt 0 ]
		then
			printf '\t\\footnotesize\\begin{flushleft}\n' >> $LATEX_FILE
			for i
			do
				printf "$i\n" >> $LATEX_FILE
			done
			printf '\t\\end{flushleft}\n' >> $LATEX_FILE
		fi
	fi
	printf '\\end{%s}%s\n' "$FIG_TYPE" "$FIG_COMMENT" >> $LATEX_FILE
}


create_dir ()
{
	if [ ! -d "$1" ]
	then
		mkdir "$1"
	fi
}


for i in 1 2 3 4 5
do
	if [ $# -gt 0 ]
	then
		if [ "$1" = "NORUN" ]
		then
			NORUN=1
			shift
		elif [ "$1" = "GROOMED-ANALYSIS" ]
		then
			GROOMED_ANALYSIS=1
			shift
		elif [ "$1" = "GENERATE-LATEX" ]
		then
			GENERATE_LATEX=1
			shift
		elif [ "$1" = "NO-VIEW-PLOTS" ]
		then
			NO_VIEW_PLOTS=1
			shift
		elif [ "$1" = "CLEANUP" ]
		then
			CLEANUP=1
			shift
		fi
	fi
done




PROG_ARGS=$*

for i
do
	if [ "$i" = "--norun-trajectory" ]; then NORUN_TRAJECTORY=1; fi
	if [ "$i" = "--norun-mass-analysis" ]; then NORUN_MASS_ANALYSIS=1; fi
	if [ "$i" = "--norun-deviation-simulation" ]; then NORUN_DEV_SIMUL=1; fi
	if [ "$i" = "--norun-error-simulation" ]; then NORUN_ERROR_SIMUL=1; fi
	if [ "$i" = "--nocalc-average-wander" ]; then NOCALC_AVERAGE_WANDER=1; fi
done



if [ ! -z $GENERATE_LATEX ]
then
	PNG_DIR=latex/figures/generated
fi


PNG_NAME_TRAJECTORY="$PNG_DIR/year3.project.trajectory.png"
PNG_NAME_VELOCITY="$PNG_DIR/year3.project.velocity_trajectory.png"
PNG_NAME_INERTIAL="$PNG_DIR/year3.project.trajectory_inertial.png"
PNG_NAME_TIME="$PNG_DIR/year3.project.wander_time.png"
PNG_NAME_HAMILTONIAN_ERROR="$PNG_DIR/year3.project.hamiltonian_error.png"
#PNG_NAME_HAMILTONIAN="$PNG_DIR/year3.project.hamiltonian.png"
PNG_NAME_MASS="$PNG_DIR/year3.project.wander_mass.png"
PNG_NAME_MASS_AV="$PNG_DIR/year3.project.average_wander_mass.png"
PNG_NAME_SMALL_MASS_LIBRATION="$PNG_DIR/year3.project.small_mass_libration.png"
PNG_NAME_SMALL_MASS_LIBRATION_AV="$PNG_DIR/year3.project.average_small_mass_libration.png"
PNG_NAME_WANDER_XRDEV="$PNG_DIR/year3.project.wander_xrdev.png"
PNG_NAME_WANDER_XRDEV_AV="$PNG_DIR/year3.project.average_wander_xrdev.png"
PNG_NAME_WANDER_ADEV="$PNG_DIR/year3.project.wander_adev.png"
PNG_NAME_WANDER_ADEV_AV="$PNG_DIR/year3.project.average_wander_adev.png"
PNG_NAME_WANDER_VRDEV="$PNG_DIR/year3.project.wander_vrdev.png"
PNG_NAME_WANDER_VRDEV_AV="$PNG_DIR/year3.project.average_wander_vrdev.png"
PNG_NAME_WANDER_VTDEV="$PNG_DIR/year3.project.wander_vtdev.png"
PNG_NAME_WANDER_VTDEV_AV="$PNG_DIR/year3.project.average_wander_vtdev.png"


GROOMED_ANALYSIS_ZOOMEDWANDER_MASS=analysis_1
GROOMED_ANALYSIS_TRAJECTORY_330=analysis_2
GROOMED_ANALYSIS_TRAJECTORY_HORSESHOE=analysis_3
GROOMED_ANALYSIS_WANDER_MASS_AZIMUTHAL=analysis_4
GROOMED_ANALYSIS_ENHANCED_AVERAGE_WANDER_MASS=analysis_5
GROOMED_ANALYSIS_ENHANCED_SMALL_MASS_LIBRATION=analysis_6
GROOMED_ANALYSIS_ENHANCED_ERROR=analysis_7
GROOMED_ANALYSIS_TRAJECTORY_UNSTABLE=analysis_8

GROOMED_NAME_ZOOMEDWANDER_MASS="year3.project.zoomedwander_mass"
GROOMED_NAME_TRAJECTORY_330="year3.project.trajectory_330"
GROOMED_NAME_TRAJECTORY_HORSESHOE="year3.project.trajectory_horseshoe"
GROOMED_NAME_WANDER_MASS_AZIMUTHAL="year3.project.wander_mass_azimuthal"
GROOMED_NAME_ENHANCED_AVERAGE_WANDER_MASS="year3.project.enhanced_average_wander_mass"
GROOMED_NAME_ENHANCED_SMALL_MASS_LIBRATION="year3.project.enhanced_small_libration"
GROOMED_NAME_ENHANCED_ERROR="year3.project.enhanced_error"
GROOMED_NAME_TRAJECTORY_UNSTABLE="year3.project.trajectory_unstable"


#if [ -z $GENERATE_LATEX -o ! -z $CLEANUP ]
if [ -z $GENERATE_LATEX ]
then
	rm -f "$PNG_NAME_TRAJECTORY" \
		"$PNG_NAME_VELOCITY" \
		"$PNG_NAME_INERTIAL" \
		"$PNG_NAME_TIME" \
		"$PNG_NAME_HAMILTONIAN_ERROR" \
		"$PNG_NAME_MASS" \
		"$PNG_NAME_MASS_AV" \
		"$PNG_NAME_SMALL_MASS_LIBRATION" \
		"$PNG_NAME_SMALL_MASS_LIBRATION_AV" \
		"$PNG_NAME_WANDER_XRDEV" \
		"$PNG_NAME_WANDER_XRDEV_AV" \
		"$PNG_NAME_WANDER_ADEV" \
		"$PNG_NAME_WANDER_ADEV_AV" \
		"$PNG_NAME_WANDER_VRDEV" \
		"$PNG_NAME_WANDER_VRDEV_AV" \
		"$PNG_NAME_WANDER_VTDEV" \
		"$PNG_NAME_WANDER_VTDEV_AV"
fi


if [ $? != 0 ]
then
	echo "Cannot remove one of the PNG files. Perhaps they are being viewed."
	exit 1
fi



if [ $# -gt 0 ]
then
	if [ "$1" = "--help" ]
	then
		echo "$0: [NORUN] [NO-VIEW-PLOTS] [GROOMED-ANALYSIS] [GENERATE-LATEX]"
	fi
fi


# get data for the parameter file created during trajectory mapping

get_main_params ()
{
	DATA_DIR="$1"

	PARAM_FILE=$DATA_DIR/trojan.params

	INIT_X=`cut -d '|' -f1 $PARAM_FILE`
	INIT_Y=`cut -d '|' -f2 $PARAM_FILE`
	INIT_VX=`cut -d '|' -f3 $PARAM_FILE`
	INIT_VY=`cut -d '|' -f4 $PARAM_FILE`
	XR_DEVIATION=`cut -d '|' -f5 $PARAM_FILE`
	A_DEVIATION=`cut -d '|' -f6 $PARAM_FILE`
	VR_DEVIATION=`cut -d '|' -f7 $PARAM_FILE`
	VT_DEVIATION=`cut -d '|' -f8 $PARAM_FILE`
	FIRST_MASS=`cut -d '|' -f9 $PARAM_FILE`
	SECOND_MASS=`cut -d '|' -f10 $PARAM_FILE`
	FIRST_BODY_X=`cut -d '|' -f11 $PARAM_FILE`
	FIRST_BODY_Y=`cut -d '|' -f12 $PARAM_FILE`
	SECOND_BODY_X=`cut -d '|' -f13 $PARAM_FILE`
	SECOND_BODY_Y=`cut -d '|' -f14 $PARAM_FILE`
	L4_X=`cut -d '|' -f15 $PARAM_FILE`
	L4_Y=`cut -d '|' -f16 $PARAM_FILE`
	L5_X=`cut -d '|' -f17 $PARAM_FILE`
	L5_Y=`cut -d '|' -f18 $PARAM_FILE`
	LAGRANGE_POINT=`cut -d '|' -f19 $PARAM_FILE`
}



# Functions for generating the plots with gnuplot

create_main_trajectory_plot ()
{
	DATA_DIR="$1"
	GRAPHICS_DIR="$2"
	GRAPHICS_NAME="$3"
	ALTERNATIVE_AXES_SPEC=$4
	ALTERNATIVE_CAPTION_SPEC=$5
	ALTERNATIVE_TITLE_SPEC=$6
	shift; shift; shift; shift; shift; shift

	GRAPHICS_FILE="$GRAPHICS_DIR/$GRAPHICS_NAME.png"

	AXES_SPEC="set xrange [:6.0]; set yrange [:]; "
	if [ ! "$ALTERNATIVE_AXES_SPEC" = "DEFAULT-AXES" ]
	then
		AXES_SPEC="$ALTERNATIVE_AXES_SPEC"
	fi

	CAPTION_SPEC="Trajectory of Trojan asteroid in corotating frame"
	CAPTION_SPEC="$CAPTION_SPEC for a planet of mass:  $SECOND_MASS"
	if [ ! "$ALTERNATIVE_CAPTION_SPEC" = "DEFAULT-CAPTION" ]
	then
#		CAPTION_SPEC="$CAPTION_SPEC. $ALTERNATIVE_CAPTION_SPEC"
		CAPTION_SPEC="$ALTERNATIVE_CAPTION_SPEC"
	fi

	TITLE_SPEC="Trajectory of Trojan asteroid in corotating frame"
	TITLE_SPEC="$TITLE_SPEC for a planet of mass:  $SECOND_MASS"
	if [ ! "$ALTERNATIVE_TITLE_SPEC" = "DEFAULT-TITLE" ]
	then
		TITLE_SPEC="$TITLE_SPEC $ALTERNATIVE_TITLE_SPEC"
	fi


	#plot trajectory of asteroid about L4/L5 in rotating frame
	echo "set grid; set style line 1 lw 0.1;"\
	"set xlabel 'x / AU' ; set ylabel 'y / AU' ;" \
	"$AXES_SPEC" \
	"unset key; set term png;"\
	"set output '$GRAPHICS_FILE'; set parametric; set trange [0:2*pi];"\
	"r = sqrt($L4_X*$L4_X + $L4_Y*$L4_Y); fx(t) = r * cos(t); fy(t) = r * sin(t);"\
	"set object 1 circle at $FIRST_BODY_X,$FIRST_BODY_Y radius 0.5"\
	"fillcolor rgbcolor 'black' fillstyle solid noborder;"\
	"set label 2 at $FIRST_BODY_X,$FIRST_BODY_Y+1.0 'Sun';"\
	"set object 3 circle at $SECOND_BODY_X,$SECOND_BODY_Y radius 0.05"\
	"fillcolor rgbcolor 'black' fillstyle solid noborder;"\
	"set label 4 at $SECOND_BODY_X,$SECOND_BODY_Y+1.0 'Planet';"\
	"set label 5 at $L4_X,$L4_Y point; set label 6 at $L4_X+0.5,$L4_Y 'L4';"\
	"set label 7 at $L5_X,$L5_Y point; set label 8 at $L5_X+0.5,$L5_Y 'L5';"\
	"plot fx(t),fy(t) ,'$DATA_DIR/trojan.dat' u 2:3 ps 0.5 with lines; "\
	"set linestyle 1 lc 3;"\
	| /usr/bin/gnuplot


	prepare_plot_for_latex $GRAPHICS_FILE \
		"$CAPTION_SPEC" \
		"$GRAPHICS_NAME" \
		"" \
		$* \
		"Initial radial displacement / R (AU) =  $XR_DEVIATION" \
		'\\newline' \
		"Aziumthal displacement / R (AU) = $A_DEVIATION" \
		'\\newline' \
		"Initial radial speed (AU/yrs) = $VR_DEVIATION" \
		'\\newline' \
		"Tangential speed (AU/yrs) = $VT_DEVIATION" \
		'\\newline'
}

create_trajectory_plots ()
{
	DATA_DIR="$1"

	NAME=`basename -s .png $PNG_NAME_TRAJECTORY`
	create_main_trajectory_plot $DATA_DIR $PNG_DIR $NAME \
			"DEFAULT-AXES" "DEFAULT-CAPTION" \
			"DEFAULT-TITLE" \
			"NO-PRODUCE-FOOTNOTE" "SUBFIGURE"

	#plot velocity trajectory of asteroid about L4 in rotating frame
	echo "set grid; set style line 1 lw 0.1 ;"\
	"set xlabel 'vx (AU/yrs)' ; set ylabel 'vy (AU/yrs)' ; unset key; set term png;"\
	"set output '$PNG_NAME_VELOCITY'; "\
	"plot '$DATA_DIR/trojan.dat' u 6:7 ps 0.5 with lines;"\
	"set linestyle 1 lc 3;"	| /usr/bin/gnuplot

	prepare_plot_for_latex $PNG_NAME_VELOCITY \
		"Velocity trajectory of Greek asteroid for a planet of mass:  $SECOND_MASS" \
		"velocity" \
		"" \
		"NO-PRODUCE-FOOTNOTE" \
		"SUBFIGURE" \
		"Initial radial displacement / R (AU) =  $XR_DEVIATION" \
		'\\newline' \
		"Aziumthal displacement / R (AU) = $A_DEVIATION" \
		'\\newline' \
		"Initial radial speed (AU/yrs) = $VR_DEVIATION" \
		'\\newline' \
		"Tangential speed (AU/yrs) = $VT_DEVIATION"


	#plot trajectory of asteroid about L4/L5 in inertial frame
	echo "set grid; set style line 1 lw 0.1 ;"\
	"set xlabel 'x / AU' ; set ylabel 'y / AU' ; unset key; set term png;"\
	"set output '$PNG_NAME_INERTIAL'; "\
	"plot '$DATA_DIR/inertial.dat' u 2:3 ps 0.5 with lines;"\
	"set linestyle 1 lc 3;"	| /usr/bin/gnuplot

	TITLE_INERTIAL="Trajectory of Greek asteroid in inertial frame"
	TITLE_INERTIAL="$TITLE_INERTIAL for a planet of mass:  $SECOND_MASS"

	prepare_plot_for_latex $PNG_NAME_INERTIAL \
		"$TITLE_INERTIAL" \
		"inertial" \
		"" \
		"NO-PRODUCE-FOOTNOTE" \
		"SUBFIGURE" \
		"Initial radial displacement / R (AU) =  $XR_DEVIATION" \
		'\\newline' \
		"Aziumthal displacement / R (AU) = $A_DEVIATION" \
		'\\newline' \
		"Initial radial speed (AU/yrs) = $VR_DEVIATION" \
		'\\newline' \
		"Tangential speed (AU/yrs) = $VT_DEVIATION"

	#plot wander as a function of time
	echo "set grid; set style line 1 lw 0.1 ;"\
	"set xlabel 'Time / yrs' ; set ylabel 'Angular displacement from Lagrange point / deg';"\
	"set term png; unset key; set output '$PNG_NAME_TIME';"\
	"plot '$DATA_DIR/wander_time.dat' u 1:2 ps 0.5 with lines; set linestyle 1 lc 3;" \
	| /usr/bin/gnuplot

	prepare_plot_for_latex $PNG_NAME_TIME \
		"Wander-angle as a function of time for a planet of mass:  $SECOND_MASS" \
		"time_mass" \
		"" \
		"NO-PRODUCE-FOOTNOTE" \
		"SUBFIGURE" \
		"Initial radial displacement / R (AU) =  $XR_DEVIATION" \
		'\\newline' \
		"Aziumthal displacement / R (AU) = $A_DEVIATION" \
		'\\newline' \
		"Initial radial speed (AU/yrs) = $VR_DEVIATION" \
		'\\newline' \
		"Tangential speed (AU/yrs) = $VT_DEVIATION"

}




create_main_error_simulation_plot ()
{
	DATA_DIR="$1"

	ERROR_PARAM_FILE=$DATA_DIR/error.params

	XR_ERROR_0=`cut -d '|' -f1 $ERROR_PARAM_FILE`
	A_ERROR_0=`cut -d '|' -f2 $ERROR_PARAM_FILE`
	VR_ERROR_0=`cut -d '|' -f3 $ERROR_PARAM_FILE`
	VT_ERROR_0=`cut -d '|' -f4 $ERROR_PARAM_FILE`

	XR_ERROR_1=`cut -d '|' -f5 $ERROR_PARAM_FILE`
	A_ERROR_1=`cut -d '|' -f6 $ERROR_PARAM_FILE`
	VR_ERROR_1=`cut -d '|' -f7 $ERROR_PARAM_FILE`
	VT_ERROR_1=`cut -d '|' -f8 $ERROR_PARAM_FILE`

	XR_ERROR_2=`cut -d '|' -f9 $ERROR_PARAM_FILE`
	A_ERROR_2=`cut -d '|' -f10 $ERROR_PARAM_FILE`
	VR_ERROR_2=`cut -d '|' -f11 $ERROR_PARAM_FILE`
	VT_ERROR_2=`cut -d '|' -f12 $ERROR_PARAM_FILE`

	XR_ERROR_3=`cut -d '|' -f13 $ERROR_PARAM_FILE`
	A_ERROR_3=`cut -d '|' -f14 $ERROR_PARAM_FILE`
	VR_ERROR_3=`cut -d '|' -f15 $ERROR_PARAM_FILE`
	VT_ERROR_3=`cut -d '|' -f16 $ERROR_PARAM_FILE`

	GRAPHICS_DIR="$2"
	GRAPHICS_NAME="$3"
	CAPTION="$4"
	shift; shift; shift; shift

	GRAPHICS_FILE="$GRAPHICS_DIR/$GRAPHICS_NAME.png"

	#plot error in Hamiltonian as a function of time for different deviation factors
	echo "set grid; set style line 1 lw 0.1 ; set key below;"\
	"set xlabel 'Time / yrs' ; set ylabel 'Error in Hamiltonian' ;"\
	"set term png; set output '$GRAPHICS_FILE';"\
	"set linestyle 1 lc 3;"\
	'plot '\
	"'$DATA_DIR/trojan.dat'"\
	' u 1:5 ps 0.5 with lines '\
	'title "Line 1",'\
	"'$DATA_DIR/error_0.dat' u 1:3 ps 0.5 with lines "\
	"title 'Line 2',"\
	"'$DATA_DIR/error_1.dat' u 1:3 ps 0.5 with lines "\
	"title 'Line 3',"\
	"'$DATA_DIR/error_2.dat' u 1:3 ps 0.5 with lines "\
	"title 'Line 4',"\
	"'$DATA_DIR/error_3.dat' u 1:3 ps 0.5 with lines "\
	"title 'Line 5'; "\
	| /usr/bin/gnuplot

	TITLE_HAMILTONIAN="Error in Hamiltonian plotted as a function of time"
	TITLE_HAMILTONIAN="$TITLE_HAMILTONIAN for a planet of mass:  $SECOND_MASS"

	if [ ! "$CAPTION" = "DEFAULT-CAPTION" ]
	then
		TITLE_HAMILTONIAN="$CAPTION"
	fi

	LINE_1_HAM="Line 1: $XR_DEVIATION R, a = $A_DEVIATION R, "
	LINE_1_HAM="$LINE_1_HAM vr = $VR_DEVIATION AU/yrs, vt = $VT_DEVIATION AU/yr"

	LINE_2_HAM="Line 2: r = $XR_ERROR_0 R, a = $A_ERROR_0 R,"
	LINE_2_HAM="$LINE_2_HAM vr = $VR_ERROR_0  AU/yrs, vt = $VT_ERROR_0  AU/yrs"

	LINE_3_HAM="Line 3: r = $XR_ERROR_1 R, a = $A_ERROR_1 R,"
	LINE_3_HAM="$LINE_3_HAM vr = $VR_ERROR_1  AU/yrs, vt = $VT_ERROR_1  AU/yrs"

	LINE_4_HAM="Line 4: r = $XR_ERROR_2 R, a = $A_ERROR_2 R,"
	LINE_4_HAM="$LINE_4_HAM vr = $VR_ERROR_2  AU/yrs, vt = $VT_ERROR_2  AU/yrs"

	LINE_5_HAM="Line 5: r = $XR_ERROR_3 R, a = $A_ERROR_3 R,"
	LINE_5_HAM="$LINE_5_HAM vr = $VR_ERROR_3  AU/yrs, vt = $VT_ERROR_3  AU/yrs"

	prepare_plot_for_latex $GRAPHICS_FILE \
		"$TITLE_HAMILTONIAN" \
		"$GRAPHICS_NAME" \
		"" \
		$* \
		"$LINE_1_HAM"\
		'\\newline' \
		"$LINE_2_HAM"\
		'\\newline' \
		"$LINE_3_HAM"\
		'\\newline' \
		"$LINE_4_HAM"\
		'\\newline' \
		"$LINE_5_HAM"

}

create_error_simulation_plots ()
{
	DATA_DIR="$1"

	NAME=`basename -s .png $PNG_NAME_HAMILTONIAN_ERROR`
	create_main_error_simulation_plot $DATA_DIR $PNG_DIR $NAME \
			"DEFAULT-CAPTION" "NO-PRODUCE-FOOTNOTE" "SUBFIGURE"
}

create_zoomed_wander_mass ()
{
	DATA_DIR="$1"
	GRAPHICS_DIR="$2"
	GRAPHICS_NAME="$3"
	shift; shift; shift

	GRAPHICS_FILE="$GRAPHICS_DIR/$GRAPHICS_NAME.png"

	echo "Plotting zoomed wander mass ..."

	#plot average wander for different masses (zoomed in)
	echo "set grid; set style line 1 lw 0.1 ;"\
	"set xlabel 'Reduced mass/ solar mass' ; set ylabel 'Wander-angle / deg';"\
	"set term png; unset key; set output '$GRAPHICS_FILE';"\
	"f(x) = a * ((x + b) ** c) + d ; a = 1.0; b = 0.1; c = -0.5; d = 1.0;"\
	"set fit quiet; "\
	"set fit logfile '$DATA_DIR/fit.wander_mass_average.log'; "\
	"fit f(x) '$DATA_DIR/wander_mass_average.dat' u 1:2 via a, b, c, d;"\
	"plot '$DATA_DIR/wander_mass_average.dat' "\
	"u 1:2 ps 0.5 with lines, f(x); set linestyle 1 lc 3;"\
	| /usr/bin/gnuplot

	prepare_plot_for_latex $GRAPHICS_FILE \
		"Wander-angle as a function of planet mass" \
		"zoomed_wander_mass" \
		"" \
		$* \
		"Initial radial displacement / R (AU) =  $XR_DEVIATION" \
		'\\newline' \
		"Aziumthal displacement / R (AU) = $A_DEVIATION" \
		'\\newline' \
		"Initial radial speed (AU/yrs) = $VR_DEVIATION" \
		'\\newline' \
		"Tangential speed (AU/yrs) = $VT_DEVIATION"
}



create_main_mass_analysis ()
{
	DATA_DIR="$1"
	GRAPHICS_DIR="$2"
	GRAPHICS_NAME="$3"
	GRAPHICS_TITLE="$4"
	shift; shift; shift; shift

	GRAPHICS_FILE="$GRAPHICS_DIR/$GRAPHICS_NAME.png"

	#plot wander about L4 for different masses
	echo "set grid; set style line 1 lw 0.1 ;"\
	"set xlabel 'Reduced mass/ solar mass' ; set ylabel 'Wander-angle / deg';"\
	"set xrange [:0.05]; set term png; unset key; set output '$GRAPHICS_FILE';"\
	"plot '$DATA_DIR/wander_mass.dat' u 1:2 ps 0.5 with lines; set linestyle 1 lc 3;" \
	| /usr/bin/gnuplot

	prepare_plot_for_latex $GRAPHICS_FILE \
		"$GRAPHICS_TITLE" \
		"$GRAPHICS_NAME" \
		"" \
		$* \
		"Initial radial displacement / R (AU) =  $XR_DEVIATION" \
		'\\newline' \
		"Aziumthal displacement / R (AU) = $A_DEVIATION" \
		'\\newline' \
		"Initial radial speed (AU/yrs) = $VR_DEVIATION" \
		'\\newline' \
		"Tangential speed (AU/yrs) = $VT_DEVIATION"
}


create_average_wander_mass_analysis ()
{
	DATA_DIR="$1"
	GRAPHICS_DIR="$2"
	GRAPHICS_NAME="$3"
	CAPTION="$4"
	shift; shift; shift; shift

	GRAPHICS_FILE="$GRAPHICS_DIR/$GRAPHICS_NAME.png"

	echo "set grid; set style line 1 lw 0.1 ;"\
	"set xlabel 'Reduced mass/ solar mass' ; set ylabel 'Wander-angle / deg';"\
	"set xrange [:0.05]; set term png; unset key; set output '$GRAPHICS_FILE';"\
	"plot '$DATA_DIR/wander_mass_average.dat' "\
	"u 1:2 ps 0.5 with lines; set linestyle 1 lc 3;" \
	| /usr/bin/gnuplot

	prepare_plot_for_latex $GRAPHICS_FILE \
		"$CAPTION" \
		"$GRAPHICS_NAME" \
		"" \
		$* \
		"Initial radial displacement / R (AU) =  $XR_DEVIATION" \
		'\\newline' \
		"Aziumthal displacement / R (AU) = $A_DEVIATION" \
		'\\newline' \
		"Initial radial speed (AU/yrs) = $VR_DEVIATION" \
		'\\newline' \
		"Tangential speed (AU/yrs) = $VT_DEVIATION"
}

create_small_libration_analysis ()
{
	DATA_DIR="$1"
	GRAPHICS_DIR="$2"
	GRAPHICS_NAME="$3"
	shift; shift; shift

	GRAPHICS_FILE="$GRAPHICS_DIR/$GRAPHICS_NAME.png"

	#plot small mass libration
	echo "set grid; set style line 1 lw 0.1 ;"\
	"set xlabel 'log (reduced mass/ solar mass)' ; "\
	"set ylabel 'log (radial wander/ angular wander)';"\
	"set term png; unset key; "\
	"set output '$GRAPHICS_FILE';"\
	"f(x) = a * x + b ; a = 0.5; b = 0.01 ;"\
	"set fit quiet; "\
	"set fit logfile '$DATA_DIR/fit.small_mass_libration.log'; "\
	"fit f(x) '$DATA_DIR/small_mass_libration.dat' u 1:2 via a, b ;"\
	"plot '$DATA_DIR/small_mass_libration.dat' "\
	"u 1:2 ps 0.5 with lines, f(x); set linestyle 1 lc 3;" \
	| /usr/bin/gnuplot

	prepare_plot_for_latex $GRAPHICS_FILE \
		"Small mass libration analysis" \
		"$GRAPHICS_NAME" \
		"" \
		$* \
		"Initial radial displacement / R (AU) =  $XR_DEVIATION" \
		'\\newline' \
		"Aziumthal displacement / R (AU) = $A_DEVIATION" \
		'\\newline' \
		"Initial radial speed (AU/yrs) = $VR_DEVIATION" \
		'\\newline' \
		"Tangential speed (AU/yrs) = $VT_DEVIATION"
}

create_mass_analysis_plots ()
{
	DATA_DIR="$1"

	NAME=`basename -s .png $PNG_NAME_MASS`
	create_main_mass_analysis $DATA_DIR $PNG_DIR $NAME \
		"Wander-angle as a function of planet mass" \
		"NO-PRODUCE-FOOTNOTE" "SUBFIGURE"

	#plot average wander for different masses

	if [ -z $NOCALC_AVERAGE_WANDER ]
	then
		NAME=`basename -s .png $PNG_NAME_MASS_AV`
		create_average_wander_mass_analysis $DATA_DIR $PNG_DIR $NAME \
			"Average Wander-angle as a function of planet mass" \
			"NO-PRODUCE-FOOTNOTE" "SUBFIGURE"
	fi

	NAME=`basename -s .png $PNG_NAME_SMALL_MASS_LIBRATION`
	create_small_libration_analysis $DATA_DIR $PNG_DIR $NAME \
			"NO-PRODUCE-FOOTNOTE" "SUBFIGURE"


	if [ -z $NOCALC_AVERAGE_WANDER ]
	then
		#plot small mass libration
		echo "set grid; set style line 1 lw 0.1 ;"\
		"set xlabel 'log (reduced mass/ solar mass)' ;"\
		"set ylabel 'log (radial wander/ angular wander)';"\
		"set term png; "\
		"unset key; set output '$PNG_NAME_SMALL_MASS_LIBRATION_AV';"\
		"f(x) = a * x + b ; a = 0.5; b = 0.01 ;"\
		"set fit quiet; "\
		"set fit logfile '$DATA_DIR/fit.average_small_mass_libration.log'; "\
		"fit f(x) '$DATA_DIR/average_small_mass_libration.dat' u 1:2 via a, b ;"\
		"plot '$DATA_DIR/average_small_mass_libration.dat' "\
		"u 1:2 ps 0.5 with lines, f(x); set linestyle 1 lc 3;" \
		| /usr/bin/gnuplot

		prepare_plot_for_latex $PNG_NAME_SMALL_MASS_LIBRATION_AV \
			"Average small mass libration analysis" \
			"average_small_mass_libration" \
			"" \
			"NO-PRODUCE-FOOTNOTE" \
			"SUBFIGURE" \
			"Initial radial displacement / R (AU) =  $XR_DEVIATION" \
			'\\newline' \
			"Aziumthal displacement / R (AU) = $A_DEVIATION" \
			'\\newline' \
			"Initial radial speed (AU/yrs) = $VR_DEVIATION" \
			'\\newline' \
			"Tangential speed (AU/yrs) = $VT_DEVIATION"
	fi

}


create_deviation_simulation_plots ()
{
	DATA_DIR="$1"
	EXTRA_NAME=""
	SUBFIG="SUBFIGURE"
	if [ $# -gt 1 ]
	then
		EXTRA_NAME="$2"
		SUBFIG=
	fi

	#plot wander about L4 for different xr deviations
	echo "set grid; set style line 1 lw 0.1 ;"\
	"set xlabel 'Radial displacement / R' ; set ylabel 'Wander-angle / deg';"\
	"unset key; set term png; set output '$PNG_NAME_WANDER_XRDEV';"\
	"plot '$DATA_DIR/xr_wander.dat' u 1:2 ps 0.5 with lines; set linestyle 1 lc 3;" \
	| /usr/bin/gnuplot

	prepare_plot_for_latex $PNG_NAME_WANDER_XRDEV \
		"Wander-angle as a function of radial displacement from $LAGRANGE_POINT" \
		"wander_xrdev" \
		"$EXTRA_NAME" \
		"NO-PRODUCE-FOOTNOTE" \
		"$SUBFIG"


	if [ -z $NOCALC_AVERAGE_WANDER ]
	then
		#plot average wander for different xr deviations
		echo "set grid; set style line 1 lw 0.1 ;"\
		"set xlabel 'Radial displacement / R' ; set ylabel 'Wander-angle / deg';"\
		"unset key; set term png; set output '$PNG_NAME_WANDER_XRDEV_AV';"\
		"plot '$DATA_DIR/xr_wander_average.dat' "\
		"u 1:2 ps 0.5 with lines; set linestyle 1 lc 3;" \
		| /usr/bin/gnuplot

		CAP_TEXT="Average Wander-angle as a function of radial displacement"
		if [ -n "$EXTRA_NAME" ]
		then
			CAP_TEXT="Angular wander plotted as a function of the radial displacement."
		fi

		prepare_plot_for_latex $PNG_NAME_WANDER_XRDEV_AV \
			"$CAP_TEXT" \
			"wander_xrdev_av" \
			"$EXTRA_NAME" \
			"NO-PRODUCE-FOOTNOTE" \
			"$SUBFIG"
	fi

	#plot wander about L4 for different a deviations
	echo "set grid; set style line 1 lw 0.1 ;"\
	"set xlabel 'Azimuthal displacement/R' ; set ylabel 'Wander-angle (deg)';"\
	"unset key; set term png; set output '$PNG_NAME_WANDER_ADEV';"\
	"plot '$DATA_DIR/a_wander.dat' u 1:2 ps 0.5 with lines; set linestyle 1 lc 3;" \
	| /usr/bin/gnuplot

	prepare_plot_for_latex $PNG_NAME_WANDER_ADEV \
		"Wander-angle as a function of azimuthal displacement from $LAGRANGE_POINT" \
		"wander_adev" \
		"$EXTRA_NAME" \
		"NO-PRODUCE-FOOTNOTE" \
		"$SUBFIG"


	if [ -z $NOCALC_AVERAGE_WANDER ]
	then
		#plot average wander for different a deviations
		echo "set grid; set style line 1 lw 0.1 ;"\
		"set xlabel 'Azimuthal displacement/R' ; set ylabel 'Wander-angle (deg)';"\
		"unset key; set term png; set output '$PNG_NAME_WANDER_ADEV_AV';"\
		"plot '$DATA_DIR/a_wander_average.dat' "\
		"u 1:2 ps 0.5 with lines; set linestyle 1 lc 3;" \
		| /usr/bin/gnuplot

		CAP_TEXT="Average Wander-angle as a function of azimuthal displacement"
		if [ -n "$EXTRA_NAME" ]
		then
			CAP_TEXT="Angular wander plotted as a function of the azimuthal"
			CAP_TEXT="$CAP_TEXT displacement. The graph displays a high level of"
			CAP_TEXT="$CAP_TEXT symmetry about the L3 point because the asteroid"
			CAP_TEXT="$CAP_TEXT is able to transition between Lagrange points."
		fi

		prepare_plot_for_latex $PNG_NAME_WANDER_ADEV_AV \
			"$CAP_TEXT" \
			"wander_adev_av" \
			"$EXTRA_NAME" \
			"NO-PRODUCE-FOOTNOTE" \
			"$SUBFIG"
	fi


	#plot wander about L4 for different vr deviations
	echo "set grid; set style line 1 lw 0.1 ;"\
	"set xlabel 'Radial speed (AU/yrs)' ; set ylabel 'Wander-angle / deg';"\
	"unset key; set term png; set output '$PNG_NAME_WANDER_VRDEV';"\
	"plot '$DATA_DIR/vr_wander.dat' u 1:2 ps 0.5 with lines; set linestyle 1 lc 3;" \
	| /usr/bin/gnuplot

	prepare_plot_for_latex $PNG_NAME_WANDER_VRDEV \
		"Wander-angle as a function of initial radial speed at $LAGRANGE_POINT" \
		"wander_vrdev" \
		"$EXTRA_NAME" \
		"NO-PRODUCE-FOOTNOTE" \
		"$SUBFIG"


	if [ -z $NOCALC_AVERAGE_WANDER ]
	then
		#plot wander for different vr deviations
		echo "set grid; set style line 1 lw 0.1 ;"\
		"set xlabel 'Radial speed (AU/yrs)' ; set ylabel 'Wander-angle / deg';"\
		"unset key; set term png; set output '$PNG_NAME_WANDER_VRDEV_AV';"\
		"plot '$DATA_DIR/vr_wander_average.dat' "\
		"u 1:2 ps 0.5 with lines; set linestyle 1 lc 3;" \
		| /usr/bin/gnuplot

		CAP_TEXT="Average Wander-angle as a function of initial radial speed"
		if [ -n "$EXTRA_NAME" ]
		then
			CAP_TEXT="Angular wander plotted as a function of the initial radial speed."
		fi

		prepare_plot_for_latex $PNG_NAME_WANDER_VRDEV_AV \
			"$CAP_TEXT" \
			"wander_vrdev_av" \
			"$EXTRA_NAME" \
			"NO-PRODUCE-FOOTNOTE" \
			"$SUBFIG"
	fi


	#plot wander about L4 for different vt deviations
	echo "set grid; set style line 1 lw 0.1 ;"\
	"set xlabel 'Tangential speed (AU/yrs)' ; set ylabel 'Wander-angle / deg';"\
	"unset key; set term png; set output '$PNG_NAME_WANDER_VTDEV';"\
	"plot '$DATA_DIR/vt_wander.dat' u 1:2 ps 0.5 with lines; set linestyle 1 lc 3;" \
	| /usr/bin/gnuplot

	prepare_plot_for_latex $PNG_NAME_WANDER_VTDEV \
		"Wander-angle as a function of initial tangential speed at $LAGRANGE_POINT" \
		"wander_vtdev" \
		"$EXTRA_NAME" \
		"NO-PRODUCE-FOOTNOTE" \
		"$SUBFIG"


	if [ -z $NOCALC_AVERAGE_WANDER ]
	then
		#plot average wander for different vt deviations
		echo "set grid; set style line 1 lw 0.1 ;"\
		"set xlabel 'Tangential speed (AU/yrs)' ; set ylabel 'Wander-angle / deg';"\
		"unset key; set term png; set output '$PNG_NAME_WANDER_VTDEV_AV';"\
		"plot '$DATA_DIR/vt_wander_average.dat' "\
		"u 1:2 ps 0.5 with lines; set linestyle 1 lc 3;" \
		| /usr/bin/gnuplot

		CAP_TEXT="Average Wander-angle as a function of initial tangential speed"
		if [ -n "$EXTRA_NAME" ]
		then
			CAP_TEXT="Angular wander plotted as a function of the initial tangential speed."
		fi

		prepare_plot_for_latex $PNG_NAME_WANDER_VTDEV_AV \
			"$CAP_TEXT" \
			"wander_vtdev_av" \
			"$EXTRA_NAME" \
			"NO-PRODUCE-FOOTNOTE" \
			"$SUBFIG"
	fi

}



if [ -z $GROOMED_ANALYSIS ]
then
	OUTPUT_DIR="$MAIN_OUTPUT_DIR"
	create_dir "$MAIN_OUTPUT_DIR"
else
	GROOMED_DIR="$MAIN_OUTPUT_DIR/groomed"
	create_dir "$MAIN_OUTPUT_DIR"
	create_dir "$MAIN_OUTPUT_DIR/groomed"
fi


# Run the program

if [ -z $NORUN ]
then
	if [ -z $GROOMED_ANALYSIS ]
	then
		./year3.project.trajectory $PROG_ARGS --output-dir "$OUTPUT_DIR"
		if [ $? != 0 ]; then exit 1; fi
	else
		OUTPUT_DIR="$GROOMED_DIR/$GROOMED_ANALYSIS_ZOOMEDWANDER_MASS"
		create_dir "$OUTPUT_DIR"
		echo "Doing " $GROOMED_NAME_ZOOMEDWANDER_MASS
		./year3.project.trajectory $PROG_ARGS \
			--max-simulation-mass 0.005 \
			--mass-increment 0.00001 \
			--norun-trajectory \
			--norun-deviation-simulation \
			--norun-error-simulation \
			--output-dir "$OUTPUT_DIR"
		if [ $? != 0 ]; then exit 1; fi

		OUTPUT_DIR="$GROOMED_DIR/$GROOMED_ANALYSIS_TRAJECTORY_330"
		create_dir "$OUTPUT_DIR"
		echo "Doing " $GROOMED_NAME_TRAJECTORY_330
		./year3.project.trajectory $PROG_ARGS \
			--t-final 330 \
			--norun-mass-analysis \
			--norun-deviation-simulation \
			--norun-error-simulation \
			--output-dir "$OUTPUT_DIR"
		if [ $? != 0 ]; then exit 1; fi

		OUTPUT_DIR="$GROOMED_DIR/$GROOMED_ANALYSIS_TRAJECTORY_HORSESHOE"
		create_dir "$OUTPUT_DIR"
		echo "Doing " $GROOMED_NAME_TRAJECTORY_HORSESHOE
		./year3.project.trajectory $PROG_ARGS \
			--t-final 430 \
			--xr-deviation 0.013 \
			--norun-mass-analysis \
			--norun-deviation-simulation \
			--norun-error-simulation \
			--output-dir "$OUTPUT_DIR"
		if [ $? != 0 ]; then exit 1; fi

		OUTPUT_DIR="$GROOMED_DIR/$GROOMED_ANALYSIS_TRAJECTORY_UNSTABLE"
		create_dir "$OUTPUT_DIR"
		echo "Doing " $GROOMED_NAME_TRAJECTORY_UNSTABLE
		./year3.project.trajectory $PROG_ARGS \
			--t-final 200 \
			--xr-deviation 0.03 \
			--norun-mass-analysis \
			--norun-deviation-simulation \
			--norun-error-simulation \
			--output-dir "$OUTPUT_DIR"
		if [ $? != 0 ]; then exit 1; fi

		OUTPUT_DIR="$GROOMED_DIR/$GROOMED_ANALYSIS_WANDER_MASS_AZIMUTHAL"
		create_dir "$OUTPUT_DIR"
		echo "Doing " $GROOMED_NAME_WANDER_MASS_AZIMUTHAL
		./year3.project.trajectory $PROG_ARGS \
			--xr-deviation 0.0 \
			--a-deviation 0.05 \
			--max-simulation-mass 0.045 \
			--mass-increment 0.0001 \
			--norun-trajectory \
			--norun-deviation-simulation \
			--norun-error-simulation \
			--output-dir "$OUTPUT_DIR"
		if [ $? != 0 ]; then exit 1; fi

		OUTPUT_DIR="$GROOMED_DIR/$GROOMED_ANALYSIS_ENHANCED_AVERAGE_WANDER_MASS"
		create_dir "$OUTPUT_DIR"
		echo "Doing " $GROOMED_NAME_ENHANCED_AVERAGE_WANDER_MASS
		./year3.project.trajectory $PROG_ARGS \
			--max-simulation-mass 0.045 \
			--mass-increment 0.0001 \
			--norun-trajectory \
			--norun-deviation-simulation \
			--norun-error-simulation \
			--output-dir "$OUTPUT_DIR"
		if [ $? != 0 ]; then exit 1; fi

		OUTPUT_DIR="$GROOMED_DIR/$GROOMED_ANALYSIS_ENHANCED_SMALL_MASS_LIBRATION"
		create_dir "$OUTPUT_DIR"
		echo "Doing " $GROOMED_NAME_ENHANCED_SMALL_MASS_LIBRATION
		./year3.project.trajectory $PROG_ARGS \
			--xr-deviation 0.0001 \
			--max-simulation-mass 0.001 \
			--mass-increment 0.0001 \
			--norun-trajectory \
			--norun-deviation-simulation \
			--norun-error-simulation \
			--output-dir "$OUTPUT_DIR"
		if [ $? != 0 ]; then exit 1; fi

		OUTPUT_DIR="$GROOMED_DIR/$GROOMED_ANALYSIS_ENHANCED_ERROR"
		create_dir "$OUTPUT_DIR"
		echo "Doing " $GROOMED_NAME_ENHANCED_ERROR
		./year3.project.trajectory $PROG_ARGS \
			--t-final 100000 \
			--norun-mass-analysis \
			--norun-deviation-simulation \
			--output-dir "$OUTPUT_DIR"
		if [ $? != 0 ]; then exit 1; fi
	fi
fi


# Generate all of the plots

create_dir "$PNG_DIR"
if [ ! -d "$PNG_DIR" ]; then PNG_DIR=.; fi


if [ -z $GROOMED_ANALYSIS ]
then
	get_main_params "$MAIN_OUTPUT_DIR"
	if [ -z $NORUN_TRAJECTORY ]; then \
		create_trajectory_plots "$MAIN_OUTPUT_DIR" "$PNG_DIR" ; fi
	if [ -z $NORUN_MASS_ANALYSIS ]; then \
		create_mass_analysis_plots "$MAIN_OUTPUT_DIR" "$PNG_DIR"; fi
	if [ -z $NORUN_DEV_SIMUL ]; then \
		create_deviation_simulation_plots "$MAIN_OUTPUT_DIR"; fi
	if [ -z $NORUN_DEV_SIMUL ]; then \
		create_deviation_simulation_plots "$MAIN_OUTPUT_DIR" "_repeat"; fi
	if [ -z $NORUN_ERROR_SIMUL ]; then \
		create_error_simulation_plots "$MAIN_OUTPUT_DIR" ; fi
else
	GROOMED_PNG_DIR="$PNG_DIR/groomed"
	if [ ! -d "$GROOMED_PNG_DIR" ]; then GROOMED_PNG_DIR="$PNG_DIR"; fi

	OUTPUT_DIR="$GROOMED_DIR/$GROOMED_ANALYSIS_ZOOMEDWANDER_MASS"
	get_main_params "$OUTPUT_DIR"
	create_zoomed_wander_mass "$OUTPUT_DIR" "$GROOMED_PNG_DIR" \
			"$GROOMED_NAME_ZOOMEDWANDER_MASS"

	OUTPUT_DIR="$GROOMED_DIR/$GROOMED_ANALYSIS_TRAJECTORY_330"
	get_main_params "$OUTPUT_DIR"
	CAPTION_330="Tadpole orbit. The epicyclic motion (small arcs) and the"
	CAPTION_330="$CAPTION_330 libration (overall shape) are clearly demonstrated."
	CAPTION_330="$CAPTION_330 The epicyclic motion of the Jupiter trojans"
	CAPTION_330="$CAPTION_330 is small because the eccentricity is zero"
	create_main_trajectory_plot "$OUTPUT_DIR" "$GROOMED_PNG_DIR" \
			"$GROOMED_NAME_TRAJECTORY_330" \
			"set xrange [0:5.0]; set yrange [2.0:6.0]; " \
			"$CAPTION_330" "DEFAULT-TITLE"

	OUTPUT_DIR="$GROOMED_DIR/$GROOMED_ANALYSIS_TRAJECTORY_HORSESHOE"
	get_main_params "$OUTPUT_DIR"

	CAP_TEXT="Horseshoe orbit. An asteroid that is travelling along the"
	CAP_TEXT="$CAP_TEXT exterior orbit encounters Jupiter."
	CAP_TEXT="$CAP_TEXT The asteroid loses angular momentum and enters"
	CAP_TEXT="$CAP_TEXT the interior orbit. When it encounters Jupiter"
	CAP_TEXT="$CAP_TEXT again, it gains angular momentum and re-enters"
	CAP_TEXT="$CAP_TEXT the exterior orbit."

	create_main_trajectory_plot "$OUTPUT_DIR" "$GROOMED_PNG_DIR" \
			"$GROOMED_NAME_TRAJECTORY_HORSESHOE" \
			"DEFAULT-AXES" \
			"$CAP_TEXT" "DEFAULT-TITLE"

	OUTPUT_DIR="$GROOMED_DIR/$GROOMED_ANALYSIS_TRAJECTORY_UNSTABLE"
	get_main_params "$OUTPUT_DIR"
	CAPTION_UNSTABLE="Unstable orbit. The asteroid is seen to circulate."
	create_main_trajectory_plot "$OUTPUT_DIR" "$GROOMED_PNG_DIR" \
			"$GROOMED_NAME_TRAJECTORY_UNSTABLE" \
			"set xrange [-15.0:15.0]; set yrange [:]; " \
			"$CAPTION_UNSTABLE" "DEFAULT-TITLE"

	OUTPUT_DIR="$GROOMED_DIR/$GROOMED_ANALYSIS_WANDER_MASS_AZIMUTHAL"
	get_main_params "$OUTPUT_DIR"
	CAP_TEXT="Angular wander plotted as the reduced mass the of system is varied for a"
	CAP_TEXT="$CAP_TEXT constant azimuthal displacement."
	CAP_TEXT="$CAP_TEXT There is a 2:1 resonance about ~0.025."
	create_main_mass_analysis "$OUTPUT_DIR" "$GROOMED_PNG_DIR" \
		"$GROOMED_NAME_WANDER_MASS_AZIMUTHAL" \
		"$CAP_TEXT"

	OUTPUT_DIR="$GROOMED_DIR/$GROOMED_ANALYSIS_ENHANCED_AVERAGE_WANDER_MASS"
	get_main_params "$OUTPUT_DIR"
	CAP_TEXT="Angular wander plotted as the reduced mass the of system is varied for"
	CAP_TEXT="$CAP_TEXT a constant radial displacement. The unstable orbits for reduced"
	CAP_TEXT="$CAP_TEXT  masses of ~0.025 are due to 2:1 resonances. The sharper peak"
	CAP_TEXT="$CAP_TEXT about ~0.014 is due to a 3:1 resonance, which is less destabilising."
	create_average_wander_mass_analysis "$OUTPUT_DIR" "$GROOMED_PNG_DIR" \
			"$GROOMED_NAME_ENHANCED_AVERAGE_WANDER_MASS" \
			"$CAP_TEXT"

	OUTPUT_DIR="$GROOMED_DIR/$GROOMED_ANALYSIS_ENHANCED_SMALL_MASS_LIBRATION"
	get_main_params "$OUTPUT_DIR"
	create_small_libration_analysis "$OUTPUT_DIR" "$GROOMED_PNG_DIR" \
			"$GROOMED_NAME_ENHANCED_SMALL_MASS_LIBRATION"

	CAP_TEXT=" The error in the Hamiltonian plotted for long times using a tolerance of"
	CAP_TEXT="$CAP_TEXT 1e(-12). Azimuthal displacements give rise to more stable"
	CAP_TEXT="$CAP_TEXT orbits than radial displacements."
	CAP_TEXT="$CAP_TEXT The errors are small, and they only increase linearly in time."

	OUTPUT_DIR="$GROOMED_DIR/$GROOMED_ANALYSIS_ENHANCED_ERROR"
	create_main_error_simulation_plot "$OUTPUT_DIR" "$GROOMED_PNG_DIR" \
			"$GROOMED_NAME_ENHANCED_ERROR" \
			"$CAP_TEXT"
fi


# View all the generated plots

if [ -z ${NO_VIEW_PLOTS} ]
then

	if [ -z ${PNG_VIEWER} ]; then PNG_VIEWER="eog"; fi


	if [ -z $GROOMED_ANALYSIS ]
	then
		if [ -z $NORUN_TRAJECTORY ]
		then
			$PNG_VIEWER $PNG_NAME_TRAJECTORY &
			$PNG_VIEWER $PNG_NAME_VELOCITY &
			$PNG_VIEWER $PNG_NAME_INERTIAL &
			$PNG_VIEWER $PNG_NAME_TIME & 
		fi
		if [ -z $NORUN_MASS_ANALYSIS ]
		then
			$PNG_VIEWER $PNG_NAME_MASS &
			if [ -z $NOCALC_AVERAGE_WANDER ]; then \
				$PNG_VIEWER $PNG_NAME_MASS_AV & fi
			$PNG_VIEWER $PNG_NAME_SMALL_MASS_LIBRATION &
			if [ -z $NOCALC_AVERAGE_WANDER ]; then \
				$PNG_VIEWER $PNG_NAME_SMALL_MASS_LIBRATION_AV & fi
		fi
		if [ -z $NORUN_DEV_SIMUL ]
		then
			$PNG_VIEWER $PNG_NAME_WANDER_XRDEV &
			if [ -z $NOCALC_AVERAGE_WANDER ]; then \
				$PNG_VIEWER $PNG_NAME_WANDER_XRDEV_AV & fi
			$PNG_VIEWER $PNG_NAME_WANDER_ADEV &
			if [ -z $NOCALC_AVERAGE_WANDER ]; then \
				$PNG_VIEWER $PNG_NAME_WANDER_ADEV_AV & fi
			$PNG_VIEWER $PNG_NAME_WANDER_VRDEV &
			if [ -z $NOCALC_AVERAGE_WANDER ]; then \
				$PNG_VIEWER $PNG_NAME_WANDER_VRDEV_AV & fi
			$PNG_VIEWER $PNG_NAME_WANDER_VTDEV &
			if [ -z $NOCALC_AVERAGE_WANDER ]; then \
				$PNG_VIEWER $PNG_NAME_WANDER_VTDEV_AV & fi
		fi
		if [ -z $NORUN_ERROR_SIMUL ]; then \
				$PNG_VIEWER $PNG_NAME_HAMILTONIAN_ERROR & fi
	else
		for i in $GROOMED_NAME_ZOOMEDWANDER_MASS \
				$GROOMED_NAME_TRAJECTORY_330 \
				$GROOMED_NAME_TRAJECTORY_HORSESHOE \
				$GROOMED_NAME_TRAJECTORY_UNSTABLE \
				$GROOMED_NAME_WANDER_MASS_AZIMUTHAL \
				$GROOMED_NAME_ENHANCED_AVERAGE_WANDER_MASS \
				$GROOMED_NAME_ENHANCED_SMALL_MASS_LIBRATION \
				$GROOMED_NAME_ENHANCED_ERROR
		do
			GROOMED_PNG="$GROOMED_PNG_DIR/$i.png"
			$PNG_VIEWER $GROOMED_PNG &
		done
	fi
fi
