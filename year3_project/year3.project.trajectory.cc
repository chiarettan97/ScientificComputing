/*
Project: Trojan asteroids
Candidate number:
Date: April 2018

Runge-Kutta-Fehlberg method is employed to simulate the motion of trojans 
in the co-rotating frame of two primary masses

the motion is confined to the x-y plane
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>

#include <getopt.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <limits.h>

#include "gsl/gsl_errno.h"
#include "gsl/gsl_odeiv2.h"

using namespace std ;


// masses measured in solar masses, distances in AU

#define DEBUG

#define DIMENSIONS		2			// dimensions (x,y,z)
#define G			(4.0 * M_PI * M_PI)	// gravitational constant
#define R_DISTANCE		5.2	// average distance between Jupiter and Sun
#define BILLION 1E9


enum LAGRANGE_POINT
{
	LAGRANGE_4 = 0,
	LAGRANGE_5 = 1
} ;

struct body
{
	double m ;		// mass
	double x [DIMENSIONS] ;		// position
} ;

struct derivative_params
{
	double U ;
	double omega ;
	body * bodies [2] ;
} ;

// Implementation of a timer for measuring CPU clock cycles consumed by
// portions of the code
struct cpu_timer
{
	struct timespec timeStart, timeEnd;
	clock_t clkStart, clkEnd ;
	struct tms tmsStart, tmsEnd ;

	cpu_timer () : clkStart ((clock_t) 0) {}

	void start ()
	{
		clock_gettime (CLOCK_REALTIME, &timeStart);
		clkStart = times (&tmsStart) ;
	}
	void end (const char * title)
	{
		clock_gettime (CLOCK_REALTIME, &timeEnd);
		clkEnd = times (&tmsEnd) ;

		if ( (clock_t) 0 == clkStart )
		{
			cerr << "CPU timer not started" << endl ;
			return ;
		}

		double accum = ( timeEnd.tv_sec - timeStart.tv_sec )
			+ ( timeEnd.tv_nsec - timeStart.tv_nsec )
  			/ BILLION;

		cout << "Time for " << title << " :" << endl ;
		cout << "\t" << accum << " secs" << endl ;
		cout << "\t" << tmsEnd.tms_utime - tmsStart.tms_utime <<
				" user clock ticks" << endl ;
		cout << "\t" << tmsEnd.tms_stime - tmsStart.tms_stime <<
				" system clock ticks" << endl ;
		cout << "\t" << sysconf (_SC_CLK_TCK) <<
				" clock ticks per second" << endl ;
	}
} ;


struct program_control
{
	bool debug ;
	bool restrict_data_output ;

	double t_final ;
	double step_size ;
	bool constant_step_size ;
	long max_data_points ;

	double max_simulation_mass ;
	double mvt_simulation_mass ;
	double mass_increment ;

	bool run_trajectory ;
	bool run_mass_analysis ;
	bool run_deviation_simulation ;
	bool run_error_simulation ;

	bool calc_average_wander ;

	cpu_timer timer ;

	string output_dir ;
} ;

struct program_params
{
	double xr_deviation ;
	double a_deviation ;
	double vr_deviation ;
	double vt_deviation ;

	LAGRANGE_POINT lagrange_point ;
} ;

struct initial_conditions
{
	double L_4 [DIMENSIONS] ;
	double L_5 [DIMENSIONS] ;
	double *L_point ;

	double initial_position [DIMENSIONS] ;
	double initial_speed [DIMENSIONS] ;
} ;


struct deviation_iteration
{
	const char * filename ;
	program_params params ;
	double from, to, step ;
	double * deviation ;
} ;



static
const char *
make_output_file (const program_control &prog_control,
		const char *name, const char *suffix = NULL)
{
static char fname [PATH_MAX] ;

	sprintf (fname, "%s/%s.%s", prog_control.output_dir.c_str (), name,
			suffix != NULL ? suffix : "dat") ;

	return fname ;
}


static
double
calc_reduced_mass (const double m1, const double m2)
{
double reduced_mass = (m1 * m2) / (m1 + m2) ;

	return reduced_mass ;
}

static
double
calc_reduced_mass (const body &first_body, const body &second_body)
{
	return calc_reduced_mass (first_body.m, second_body.m) ;
}

static
void
calc_distances ( body * first_body , body * second_body , const double *position,
				double r_squared [] )
{
	r_squared [0] = 0.0 ;
	r_squared [1] = 0.0 ;

	for ( int i = 0 ; i < DIMENSIONS ; i ++ )		// or D?
	{
		r_squared [0] += pow ( (position[i] - first_body->x[i]), 2.0 ) ;
		r_squared [1] += pow ( (position[i] - second_body->x[i]), 2.0 ) ;
	}
}

// user-defined function used by GSL
// set up the ODEs to be solved
// x[0] = x, x[1] = y, x[2] = dx/dt, x[3] = dy/dt
static
int
calc_derivatives ( double t, const double x[], double dxdt[], void *params )
{
	derivative_params * d_params = (derivative_params *) params ;
	double f [DIMENSIONS] ;
	double r_squared [2] ;

#define FIRST_BODY d_params->bodies [0]
#define SECOND_BODY d_params->bodies [1]

	calc_distances ( FIRST_BODY, SECOND_BODY, x, r_squared ) ;

	for ( int i = 0 ; i < DIMENSIONS ; i ++ )
	{
		f[i] = -( G * FIRST_BODY->m * ( x[i] - FIRST_BODY->x[i] ) /
				pow (r_squared [0], 3.0/2.0) )
			-( G * SECOND_BODY->m * ( x[i] - SECOND_BODY->x[i] ) /
				pow (r_squared [1], 3.0/2.0) ) ;
	}

	//	cout << "f = " << f [0] << "\t" << f [1] << endl ;

	d_params->U = - (G * FIRST_BODY->m / sqrt(r_squared [0]))
		- (G * SECOND_BODY->m / sqrt(r_squared [1])) ;

	dxdt[0] = x[2] ;
	dxdt[1] = x[3] ;

	dxdt[2] = f[0] + (2.0 * d_params->omega * x[3]) + (pow (d_params->omega, 2.0) * x[0]) ;
	dxdt[3] = f[1] - (2.0 * d_params->omega * x[2]) + (pow (d_params->omega, 2.0) * x[1]) ;

#undef FIRST_BODY
#undef SECOND_BODY

	return GSL_SUCCESS ;
}

// calculate the Hamiltonian per unit mass, which is a constant of motion
static
double
calc_hamiltonian ( const double *position, const double *speed, double omega, double U)
{
	double hamiltonian =
		( -0.5 * pow (omega, 2.0) * (pow (position[0], 2.0) + pow (position[1], 2.0)) )
		+ ( 0.5 * (pow (speed[0], 2.0) + pow (speed[1], 2.0)) )
		+ U ;

	return hamiltonian ;
}

static
bool
position_is_collinear (const double x [], const double L_point [])
{
	if ( fabs (x [0]) < 0.00000001 )   return false ;

	double slope = (x [1] / x [0]) - (L_point [1] / L_point [0]) ;

	return ( fabs (slope) < 0.0001 ) ;
}

static
double
calc_wander (const double x [], const double L_point [],
			double &radial_wander, bool *collinear)
{
	double angle = atan2 (x[1], x[0]) ;

	double L_angle = atan2 (L_point [1], L_point [0]) ;

	if ( angle < 0.0 )
	{
		angle += (2.0 * M_PI) ;
	}

	if ( L_angle < 0.0 )
	{
		L_angle += (2.0 * M_PI) ;
	}

	double angular_wander = (angle - L_angle) ;

	radial_wander = ((*collinear = position_is_collinear (x, L_point)) ?
			sqrt (pow (x[0], 2.0) + pow (x[1], 2.0)) :
			0.0) ;

	return angular_wander ;
}

static
double
convert_inertial (ofstream & f, const double *x, double omega, double &t)
{
	double theta = omega * t ;
	double inertial_position [DIMENSIONS] ;

	inertial_position [0] = (x [0] * cos (theta)) - (x [1] * sin (theta)) ;
	inertial_position [1] = (x [1] * cos (theta)) + (x [0] * sin (theta)) ;

	f << t << "\t" << inertial_position [0] << "\t" << inertial_position [1] << endl ;
}

/*
calc_mvt is the 'engine' room for the program. As the GSL iterates through a
time loop, the Hamiltonian is calculated for the error analysis, wander values are
calculated and data is output to the files used for plotting
*/
static
int
calc_mvt (bool analyseTrajectory,
		double &angular_wander,
		double &radial_wander,
		program_control & prog_control,
		const initial_conditions & conditions,
		const program_params & prog_params,
		body & first_body, body & second_body,
		ofstream * errorfile)
{
	ofstream outfile ;
	ofstream inertialfile ;
	ofstream wander_time ;
	derivative_params d_params ;
	double x [4] =
		{
			conditions.initial_position[0],
			conditions.initial_position[1],
			conditions.initial_speed[0],
			conditions.initial_speed[1]
		} ;

	double h_step = prog_control.step_size ;


	//const gsl_odeiv2_step_type *type = gsl_odeiv2_step_rk4 ;
	const gsl_odeiv2_step_type *type = gsl_odeiv2_step_rkf45 ;
	//const gsl_odeiv2_step_type *type = gsl_odeiv2_step_rkck ;

	gsl_odeiv2_step *step = gsl_odeiv2_step_alloc (type, 2 * DIMENSIONS) ;
	gsl_odeiv2_control * control = prog_control.constant_step_size ?
			NULL : gsl_odeiv2_control_y_new (1e-12, 0.0) ;		
	gsl_odeiv2_evolve *evolve = gsl_odeiv2_evolve_alloc ( 2 * DIMENSIONS ) ;

	gsl_odeiv2_system sys =
		{ calc_derivatives, NULL, 2 * DIMENSIONS, (void *) &d_params } ;

	double min_hamiltonian = 999999999999999.99 ;
	double max_hamiltonian = -999999999999999.99 ;
	double max_angleFromLpoint = prog_params.a_deviation *
		((LAGRANGE_4 == prog_params.lagrange_point) ? 1.0 : -1.0) ;
	double min_angleFromLpoint = prog_params.a_deviation *
		((LAGRANGE_4 == prog_params.lagrange_point) ? 1.0 : -1.0) ;
	double min_radialFromOrigin = R_DISTANCE ;
	double max_radialFromOrigin = R_DISTANCE ;


	d_params.omega = sqrt ( G * (first_body.m + second_body.m) / pow (R_DISTANCE, 3.0) ) ;
	d_params.bodies [0] = &first_body ;
	d_params.bodies [1] = &second_body ;

	//trajectory-related data files	
	if ( analyseTrajectory )
	{
		if ( prog_control.debug )
		{
			cout << "Doing trajectory analysis for mass " << second_body.m << endl ;
		}

		outfile.open (make_output_file (prog_control, "trojan"), ios::trunc) ;
		inertialfile.open (make_output_file (prog_control, "inertial"), ios::trunc) ;
		wander_time.open (make_output_file (prog_control, "wander_time"), ios::trunc) ;
	}

	// advance solution until reach t_final to find orbit

	int n_steps = 0 ;

	for ( double t = 0.0; t < prog_control.t_final; n_steps++ )
	{
		double t_before = t ;
		int status ;
		double hamiltonian ;
		double angleFromLpoint ;
		double radialFromOrigin ;

		status = gsl_odeiv2_evolve_apply ( evolve, control, step,
				&sys, &t, prog_control.t_final,
				&h_step, x ) ;

		if ( status != GSL_SUCCESS )
		{
			cerr << "ERROR: GSL evolve has returned unsuccessfully after "
				<< n_steps << " steps" << endl ;
			break ;
		}
		hamiltonian  = calc_hamiltonian ( x,
			&x[DIMENSIONS], d_params.omega, d_params.U ) ;

		if ( hamiltonian > max_hamiltonian )   max_hamiltonian = hamiltonian ;
		if ( hamiltonian < min_hamiltonian )   min_hamiltonian = hamiltonian ;

		bool collinear ;

		angleFromLpoint = calc_wander (x, conditions.L_point,
				radialFromOrigin, &collinear) ;

		if ( angleFromLpoint > max_angleFromLpoint )
		{
			max_angleFromLpoint = angleFromLpoint ;
		}
		if ( angleFromLpoint < min_angleFromLpoint )
		{
			min_angleFromLpoint = angleFromLpoint ;
		}

		if ( collinear )
		{
			if ( prog_control.debug )
			{
				cout << "calc_mvt radial wander: " <<
					t << "\t" << (radialFromOrigin - R_DISTANCE) << endl ;
			}

			if ( radialFromOrigin > max_radialFromOrigin )
			{
				max_radialFromOrigin = radialFromOrigin ;
			}
			if ( radialFromOrigin < min_radialFromOrigin )
			{
				min_radialFromOrigin = radialFromOrigin ;
			}
		}


		if ( analyseTrajectory )
		{
			if ( prog_control.debug )
			{
				cout << "calc_mvt: " << t << "\t" <<
					fmod (t_before, prog_control.t_final/10000.0) <<
					endl ;
			}

			// limit the amount of data output for plotting to avoid
			// very large data files, unless overridden by the user
			if ( !prog_control.restrict_data_output ||
				fmod (t_before, prog_control.t_final/10000.0) < 0.01 )
			{

				outfile << t_before << "\t" << x [0] << "\t" << x [1] << "\t"
					<< hamiltonian << "\t" <<
					(max_hamiltonian - min_hamiltonian) << "\t"
					<< x[DIMENSIONS] << "\t" << x[DIMENSIONS+1] << endl ;
				
				convert_inertial (inertialfile, x, d_params.omega, t_before) ;
				wander_time << t_before << "\t" <<
					(angleFromLpoint * 180.0 / M_PI) <<
					"\t" << ((radialFromOrigin - 5.1974) / 5.1974) <<
					endl ;
			}

		}

		if ( errorfile != NULL )
		{
			(*errorfile) << t_before << "\t" <<
				hamiltonian << "\t" <<
				(max_hamiltonian - min_hamiltonian) << endl ;
		}
	}

	if ( prog_control.debug )
	{
		cout << "calc_mvt integration performed in " <<
			n_steps << " time steps" << endl ;
	}


	angular_wander = max_angleFromLpoint - min_angleFromLpoint ;
	radial_wander = max_radialFromOrigin - min_radialFromOrigin ;


	// free workspaces
	gsl_odeiv2_evolve_free ( evolve ) ;
	if ( control != NULL )
	{
		gsl_odeiv2_control_free ( control ) ;
	}
	gsl_odeiv2_step_free ( step ) ;


	if ( analyseTrajectory )
	{
		outfile.close () ;
		inertialfile.close () ;
		wander_time.close () ;

		// write a param file of program constants for gnuplot etc. to use
		ofstream paramfile ;

		paramfile.open (make_output_file (prog_control, "trojan", "params"), ios::trunc) ;
		paramfile <<
				conditions.initial_position[0] <<
				'|' << conditions.initial_position[1] <<
				'|' << conditions.initial_speed[0] <<
				'|' << conditions.initial_speed[1] <<
				'|' << prog_params.xr_deviation <<
				'|' << prog_params.a_deviation <<
				'|' << prog_params.vr_deviation <<
				'|' << prog_params.vt_deviation <<
				'|' << first_body.m <<
				'|' << second_body.m <<
				'|' << first_body.x[0] <<
				'|' << first_body.x[1] <<
				'|' << second_body.x[0] <<
				'|' << second_body.x[1] <<
				'|' << conditions.L_4 [0] <<
				'|' << conditions.L_4 [1] <<
				'|' << conditions.L_5 [0] <<
				'|' << conditions.L_5 [1] <<
				'|' << ((LAGRANGE_4 == prog_params.lagrange_point)
					? "L4" : "L5") <<
				endl ;
		paramfile.close () ;
	}

	return 0 ;
}

// command-line passing of user-supplied parameters
static
int
get_args (int argc, char **argv,
		program_control *prog_control, program_params *prog_params)
{
	bool bArgs = false ;

#define NORUN_TRAJECTORY 1000
#define NORUN_MASS_ANALYSIS 1001
#define NORUN_DEVIATION_SIMULATION 1002
#define NORUN_ERROR_SIMULATION 1003
#define NOCALC_AVERAGE_WANDER 1004
#define CONSTANT_STEP_SIZE 1005
#define MAX_DATA_POINTS 1006

	while (1)
	{
		int c;
		int option_index = 0;
		struct option long_options[] = {
			{"help", no_argument, 0, 'h' },
			{"debug", no_argument, 0, 'd' },
			{"norestrict-data-output", no_argument, 0, 'n' },
			{"output-dir", required_argument, 0, 'o' },
			{"xr-deviation", required_argument, 0, 'x' },
			{"a-deviation", required_argument, 0, 'a' },
			{"vr-deviation", required_argument, 0, 'r' },
			{"vt-deviation", required_argument, 0, 'f' },
			{"t-final", required_argument, 0, 't' },
			{"step-size", required_argument, 0, 's' },
			{"max-data-points", required_argument, 0, MAX_DATA_POINTS },
			{"constant-step-size", no_argument, 0, CONSTANT_STEP_SIZE },
			{"max-simulation-mass", required_argument, 0, 'c' },
			{"mvt-simulation-mass", required_argument, 0, 'm' },
			{"mass-increment", required_argument, 0, 'i' },
			{"lagrange-point", required_argument, 0, 'l' },
			{"norun-trajectory", no_argument, 0, NORUN_TRAJECTORY },
			{"norun-mass-analysis", no_argument, 0, NORUN_MASS_ANALYSIS },
			{"norun-deviation-simulation", no_argument, 0, NORUN_DEVIATION_SIMULATION },
			{"norun-error-simulation", no_argument, 0, NORUN_ERROR_SIMULATION },
			{"nocalc-average-wander", no_argument, 0, NOCALC_AVERAGE_WANDER },
			{0, 0, 0, 0 }
		};
		double dummy ;

		if ( (c = getopt_long(argc, argv,
			"", long_options, &option_index)) < 0 )   break ;

		switch (c)
		{
		case 0:
			printf("option %s", long_options[option_index].name);
			if (optarg)
				printf(" with arg %s", optarg);
			printf("\n");
			break;
		case 'h':
			cout << "usage: " << argv [0] << " [--help]"
					//[--initial-position[0] num] [--initial-position[1] num]
					//[--initial-speed[0] num] [--initial-speed[1] num]
					"[--xr-deviation num] [--a-deviation num]"
					"[--vr-deviation num] [--vt-deviation num]"
					"[--t-final num]" <<
					"[--step-size num]" <<
					"[--constant-step-size]" <<
					"[--max-simulation-mass num]" <<
					"[--mvt-simulation-mass num]" <<
					"[--mass-increment num]" <<
					"[--lagrange-point {L_4|L_5}" <<
					"[--norestrict-data-output]" <<
					"[--norun-trajectory]" <<
					"[--norun-mass-analysis]" <<
					"[--norun-deviation-simulation]" <<
					"[--norun-error-simulation]" <<
					"[--nocalc-average-wander]" <<
					"[--output-dir dir]" <<
					"[--debug]" <<
					endl ;
			return -1 ;
			break;
		case 'x':
			dummy = atof (optarg) ;
			if ( dummy < 0.0 )
			{
				cerr << "Radial deviation is not valid : " << dummy << endl ;
				return -1 ;
			}
			prog_params->xr_deviation = dummy ;
			bArgs = true ;
			break;
		case 'a':
			dummy = atof (optarg) ;
			if ( dummy < 0.0 )
			{
				cerr << "Azimuthal deviation is not valid : " << dummy << endl ;
				return -1 ;
			}
			prog_params->a_deviation = dummy ;
			bArgs = true ;
			break;
		case 'r':
			dummy = atof (optarg) ;
			if ( dummy < 0.0 )
			{
				cerr << "Radial speed is not valid : " << dummy << endl ;
				return -1 ;
			}
			prog_params->vr_deviation = dummy ;
			bArgs = true ;
			break;
		case 'f':
			dummy = atof (optarg) ;
			if ( dummy < 0.0 )
			{
				cerr << "Tangential speed is not valid : " << dummy << endl ;
				return -1 ;
			}
			prog_params->vt_deviation = dummy ;
			bArgs = true ;
			break;
		case 't':
			dummy = atof (optarg) ;
			if ( dummy < 0.0 )
			{
				cerr << "t-final is not valid : " << dummy << endl ;
				return -1 ;
			}
			prog_control->t_final = dummy ;
			bArgs = true ;
			break;
		case 's':
			dummy = atof (optarg) ;
			if ( dummy < 0.0 )
			{
				cerr << "Initial step size is not valid : " << dummy << endl ;
				return -1 ;
			}
			prog_control->step_size = dummy ;
			bArgs = true ;
			break;
		case 'c':
			dummy = atof (optarg) ;
			if ( dummy < 0.0 )
			{
				cerr << "Max. simulation mass is not valid : " << dummy << endl ;
				return -1 ;
			}
			prog_control->max_simulation_mass = dummy ;
			bArgs = true ;
			break;
		case 'i':
			dummy = atof (optarg) ;
			if ( dummy < 0.0 )
			{
				cerr << "Mass increment is not valid : " << dummy << endl ;
				return -1 ;
			}
			prog_control->mass_increment = dummy ;
			bArgs = true ;
			break;
		case 'm':
			dummy = atof (optarg) ;
			if ( dummy < 0.0 )
			{
				cerr << "Mvt. simulation mass is not valid : " << dummy << endl ;
				return -1 ;
			}
			prog_control->mvt_simulation_mass = dummy ;
			bArgs = true ;
			break;
		case 'l':
			if ( strcmp (optarg, "L_4") != 0 && strcmp (optarg, "L_5") != 0 )
			{
				cerr << "Lagrange point is not L_4 or L_5 : " << optarg << endl ;
				return -1 ;
			}
			prog_params->lagrange_point =
				(0 == (strcmp (optarg, "L_4")) ? LAGRANGE_4 : LAGRANGE_5) ;
			bArgs = true ;
			break;
		case 'd':
			prog_control->debug = true ;
			bArgs = true ;
			break;
		case 'n':
			prog_control->restrict_data_output = false ;
			bArgs = true ;
			break;
		case 'o':
			prog_control->output_dir = optarg ;
			bArgs = true ;
			break ;
		case NORUN_TRAJECTORY:
			prog_control->run_trajectory = false ;
			bArgs = true ;
			break;
		case NORUN_MASS_ANALYSIS:
			prog_control->run_mass_analysis = false ;
			bArgs = true ;
			break;
		case NORUN_DEVIATION_SIMULATION:
			prog_control->run_deviation_simulation = false ;
			bArgs = true ;
			break;
		case NORUN_ERROR_SIMULATION:
			prog_control->run_error_simulation = false ;
			bArgs = true ;
			break;
		case NOCALC_AVERAGE_WANDER:
			prog_control->calc_average_wander = false ;
			bArgs = true ;
			break;
		case CONSTANT_STEP_SIZE:
			prog_control->constant_step_size = true ;
			bArgs = true ;
			break;
		case MAX_DATA_POINTS:
			{
				long l_dummy = atol (optarg) ;
				if ( l_dummy < 0 )
				{
					cerr << "Max data points is not valid : " <<
						l_dummy << endl ;
					return -1 ;
				}
				prog_control->max_data_points = l_dummy ;
				bArgs = true ;
			}
			break;
		case '?':
			return -1 ;
			break;
		default:
			printf("?? getopt returned character code 0%o ??\n", c);
			return -1 ;
			break ;
		}
	}

	// check for errors and discrepancies in program parameters applied by user
	if ( prog_control->mvt_simulation_mass >=
			(prog_control->max_simulation_mass + 0.000001) )
	{
		cerr << "Mvt simulation mass (" << prog_control->mvt_simulation_mass
				<< ") is greater than max simulation mass ("
				<< prog_control->max_simulation_mass << ")" << endl ;
		return -1 ;
	}

	if ( !prog_control->run_mass_analysis &&
			!prog_control->run_deviation_simulation &&
			prog_control->calc_average_wander )
	{
/*
		cerr << "Not possible to calculate average wander " <<
			"when not running mass analysis or deviation simulation" << endl ;
		return -1 ;
*/
		prog_control->calc_average_wander = false ;
	}

	if ( prog_control->constant_step_size &&
			!prog_control->restrict_data_output )
	{
	double steps = prog_control->t_final * (1.0/prog_control->step_size) ;

		if ( steps > (double) prog_control->max_data_points )
		{
			cerr << "The program will produce too many data points " <<
				"for a constant step size and unrestricted ouput" << endl ;
			cerr << "Increase the step size or lower the t-final value" << endl ;
		return -1 ;
		}
	}

	if ( access (prog_control->output_dir.c_str (), W_OK) < 0 )
	{
		cerr << "It is not possible to write to the output directory: " <<
				prog_control->output_dir.c_str () << "  " <<
				strerror (errno) << endl ;
		return -1 ;
	}



	if ( bArgs )
	{
		cout << "Radial deviation/R (AU) : " << prog_params->xr_deviation << endl ;
		cout << "Azimuthal deviation/R (AU) : " << prog_params->a_deviation << endl ;
		cout << "Radial initial speed (AU/yrs) : " << prog_params->vr_deviation << endl ;
		cout << "Tangential initial speed (AU/yrs) : " << prog_params->vt_deviation << endl ;
		cout << "t-final : " << prog_control->t_final << endl ;
		cout << "step-size : " << prog_control->step_size << endl ;
		cout << "Constant step size : " <<
			(prog_control->constant_step_size ? "y" : "n") << endl ;
		cout << "Max data points : " <<
			prog_control->max_data_points << endl ;
		cout << "Max. simulation mass : " << prog_control->max_simulation_mass << endl ;
		cout << "Mvt. simulation mass : " << prog_control->mvt_simulation_mass << endl ;
		cout << "Mass increment : " << prog_control->mass_increment << endl ;
		cout << "Lagrange point : " <<
			((LAGRANGE_4 == prog_params->lagrange_point) ? "L_4" : "L_5") << endl ;
		cout << "Run trajectory : " <<
			(prog_control->run_trajectory ? "y" : "n") << endl ;
		cout << "Run mass analysis : " <<
			(prog_control->run_mass_analysis ? "y" : "n") << endl ;
		cout << "Run deviation simulation : " <<
			(prog_control->run_deviation_simulation ? "y" : "n") << endl ;
		cout << "Run error simulation : " <<
			(prog_control->run_error_simulation ? "y" : "n") << endl ;
		cout << "Calc average wander : " <<
			(prog_control->calc_average_wander ? "y" : "n") << endl ;
		cout << "Debug : " <<
			(prog_control->debug ? "y" : "n") << endl ;
		cout << "Restrict data output : " <<
			(prog_control->restrict_data_output ? "y" : "n") << endl ;
		cout << "Output directory : " <<
			prog_control->output_dir << endl ;
		cout << endl ;
	}


#undef NORUN_TRAJECTORY
#undef NORUN_MASS_ANALYSIS
#undef NORUN_DEVIATION_SIMULATION
#undef NORUN_ERROR_SIMULATION
#undef NOCALC_AVERAGE_WANDER
#undef CONSTANT_STEP_SIZE
#undef MAX_DATA_POINTS

	return 0 ;
}


// utility function for debugging purposes
static
void
dump_main_variables (const body & first_body, const body & second_body,
		const initial_conditions &conditions,
		const program_params &prog_params,
		const program_control &prog_control)
{

	if ( !prog_control.debug )   return ;

	cout << "Main variables:" << endl ;

	cout << "\t" << "First body: " << first_body.m <<
			"    " << "x: " << first_body.x[0] <<
			"    " << "y: " << first_body.x[1] << endl ;

	cout << "\t" << "Second body: " << second_body.m <<
			"    " << "x: " << second_body.x[0] <<
			"    " << "y: " << second_body.x[1] << endl ;

	cout << "\t" << "R_DISTANCE = " << R_DISTANCE << endl ;

	cout << "\t" << "Initial position: " <<
			conditions.initial_position [0] << "    " <<
			conditions.initial_position [1] << endl ;
	cout << "\t" << "Initial speed: " <<
			conditions.initial_speed [0] << "    " <<
			conditions.initial_speed [0] << endl ;

	cout << "\t" << "L_4: " <<
			conditions.L_4 [0] << "    " <<
			conditions.L_4 [1] << endl ;
	cout << "\t" << "L_5: " <<
			conditions.L_5 [0] << "    " <<
			conditions.L_5 [1] << endl ;

	cout << "\t" << "Radial deviation / R (AU): " <<
			std::setprecision (15) << prog_params.xr_deviation << endl ;
	cout << "\t" << "Azimuthal deviation / R (AU): " <<
			std::setprecision (15) << prog_params.a_deviation << endl ;
	cout << "\t" << "Radial inital speed (AU/yrs): " <<
			std::setprecision (15) << prog_params.vr_deviation << endl ;
	cout << "\t" << "Tangential inital speed (AU/yrs): " <<
			std::setprecision (15) << prog_params.vt_deviation << endl ;

	cout << "\t" << "T-final: " <<
			std::setprecision (15) << prog_control.t_final << endl ;
	cout << "\t" << "Current step size: " <<
			std::setprecision (15) << prog_control.step_size << endl ;

}


// 'fuel' for the 'engine'
// initialise position and velocity of asteroid
static
void
set_initial_conditions (initial_conditions &conditions,
		const program_params &prog_params,
		const body &first_body, const body &second_body)
{

	// xr_deviation is the radial initial displacement from L4/L5 in units of
	// a_deviation is the azimuthal initial displacement
	// in the anticlockwise direction from L4/L5

	// vr_deviation and vt_deviations are the initial radial and tangential speeds

	conditions.L_4 [0] = 0.5 * R_DISTANCE *
		(first_body.m - second_body.m) / (first_body.m + second_body.m) ;
	conditions.L_4 [1] = 0.5 * R_DISTANCE * sqrt (3.0) ;

	conditions.L_5 [0] = 0.5 * R_DISTANCE *
		(first_body.m - second_body.m) / (first_body.m + second_body.m) ;
	conditions.L_5 [1] = -0.5 * R_DISTANCE * sqrt (3.0) ;

	conditions.L_point = (LAGRANGE_4 == prog_params.lagrange_point ?
			&conditions.L_4 [0] : &conditions.L_5 [0]) ;

	double L_mag = sqrt (pow (conditions.L_point [0], 2.0) +
				pow (conditions.L_point [1], 2.0)) ;

	// radial vector is (x, y) and tangential vector is (y, -x) at L4/5


	int mul_A = (LAGRANGE_4 == prog_params.lagrange_point ? +1 : +1) ;
	int mul_B = (LAGRANGE_4 == prog_params.lagrange_point ? -1 : +1) ;
	int mul_C = (LAGRANGE_4 == prog_params.lagrange_point ? +1 : +1) ;
	int mul_D = (LAGRANGE_4 == prog_params.lagrange_point ? +1 : -1) ;

	conditions.initial_position [0] =
		(prog_params.xr_deviation * R_DISTANCE * conditions.L_point [0] / L_mag)
		+ (mul_A * conditions.L_point [0] *
			cos (prog_params.a_deviation* R_DISTANCE / L_mag))
		+ (mul_B * conditions.L_point [1] *
			sin (prog_params.a_deviation * R_DISTANCE / L_mag)) ;
	conditions.initial_position [1] =
		(prog_params.xr_deviation * R_DISTANCE * conditions.L_point [1] / L_mag)
		+ (mul_C * conditions.L_point [1] *
			cos (prog_params.a_deviation* R_DISTANCE / L_mag))
		+ (mul_D * conditions.L_point [0] *
			sin (prog_params.a_deviation * R_DISTANCE / L_mag)) ;

	int mul_E = (LAGRANGE_4 == prog_params.lagrange_point ? -1 : +1) ;
	int mul_F = (LAGRANGE_4 == prog_params.lagrange_point ? +1 : -1) ;

	conditions.initial_speed [0] =
		 (prog_params.vr_deviation * conditions.L_point [0] / L_mag)
		+ (mul_E * prog_params.vt_deviation * conditions.L_point [1] / L_mag) ;
	conditions.initial_speed [1] =
		 (prog_params.vr_deviation * conditions.L_point [1] / L_mag)
		+ (mul_F * prog_params.vt_deviation * conditions.L_point [0] / L_mag) ;
}

static
void
initialise_bodies (body &first_body, body &second_body)
{

	first_body.x[0] = - R_DISTANCE * second_body.m / ( first_body.m + second_body.m ) ;
	first_body.x[1] = 0.0 ;

	second_body.x[0] = first_body.x[0] + R_DISTANCE ;
	second_body.x[1] = 0.0 ;
}

/*
The main functionality-implementing functions:
	mass analysis and trajectory mapping
	simulation of deviation changes
	simulation of errors
In the case of mass analysis and deviation simulation, average are calculated for
wander values by performing the functionality for both the user_specified Lagrange
point, and the other non-specified point. This is done by creating lists of the 
values produced for plotting in each case, using c++ vectors, and then calculating
the average value for each paired element from the two lists.

Implements a loop to perform analysis of variations in the mass of the second body.
*/
static
int
run_mass_analysis_and_trajectory_calcs (program_control &prog_control,
		program_params &prog_params,
		body &first_body, body &second_body,
		std::vector <double> * mass_values,
		std::vector <double> & wander_values,
		std::vector <double> & angular_radial_ratio_values,
		bool onlyCalculateWanderValues,
		bool makeListOfWanderValues)
{
	ofstream wander_mass ;
	ofstream small_mass_libration ;

	const double mvt_mass = prog_control.mvt_simulation_mass ;

	int nMass = 0 ;


	if ( !onlyCalculateWanderValues && prog_control.run_mass_analysis )
	{
		wander_mass.open (make_output_file (prog_control, "wander_mass"), ios::trunc) ;
		small_mass_libration.open (make_output_file (prog_control,
				"small_mass_libration"), ios::trunc) ;
	}


	for ( double d = (prog_control.run_trajectory && !prog_control.run_mass_analysis) ?
				prog_control.mvt_simulation_mass :
				0.0001;
				//prog_control.mass_increment;
			d <= prog_control.max_simulation_mass;
			d += prog_control.mass_increment, nMass++ )
	{
		double angular_wander, radial_wander ;
		initial_conditions conditions ;
		bool analyseTrajectory =
			(!onlyCalculateWanderValues &&
				((prog_control.run_trajectory &&
					!prog_control.run_mass_analysis &&
					!nMass) ||
			//fabs(mvt_mass - d) < 0.0005) ;
			(fabs(mvt_mass - d) < (prog_control.mass_increment / 2)))) ;

		if ( !prog_control.run_mass_analysis && !analyseTrajectory )
		{
			continue ;
		}

		second_body.m = d ;

		initialise_bodies (first_body, second_body) ;

		set_initial_conditions (conditions, prog_params,
				first_body, second_body) ;

		dump_main_variables (first_body, second_body,
				conditions, prog_params, prog_control) ;

		if ( prog_control.debug )
		{
			cout << "Doing mass analysis for mass = " <<
					second_body.m << endl ;
		}

		calc_mvt (analyseTrajectory, angular_wander, radial_wander,
				prog_control, conditions, prog_params,
				first_body, second_body, NULL) ;

		double angular_radial_ratio ;

		{
			double L_mag = sqrt (pow (conditions.L_point [0], 2.0) +
				pow (conditions.L_point[1], 2.0)) ;
			// assume that angular_wander will not be zero
			angular_radial_ratio =
				radial_wander / (angular_wander * L_mag) ;
		}

		if ( !onlyCalculateWanderValues && prog_control.run_mass_analysis )
		{
			wander_mass << calc_reduced_mass (first_body.m, second_body.m) <<
				"\t" <<	
				(angular_wander * 180.0 / M_PI) << 
				"\t" << second_body.m << endl ;
			small_mass_libration <<
				log (calc_reduced_mass (first_body, second_body)) <<
				"\t" <<
				log (angular_radial_ratio) << endl ;
		}

		if ( makeListOfWanderValues )
		{
			if ( mass_values != NULL )   mass_values->push_back (d) ;
			wander_values.push_back (angular_wander * 180.0 / M_PI) ;
			angular_radial_ratio_values.push_back (angular_radial_ratio) ;
		}

		if ( prog_control.debug )
		{
			cout << "Finished calc_mvt for mass = " << second_body.m << endl ;
		}
	}

	if ( !onlyCalculateWanderValues && prog_control.run_mass_analysis )
	{
		wander_mass.close() ;
		small_mass_libration.close() ;
	}

	return 0 ;
}


static
int
run_mass_analysis_and_trajectory (program_control &prog_control,
		program_params prog_params,   // deliberate pass-by-value
		body &first_body, body &second_body)
{
	const bool runOnlyToCalculateAverage = true ;
	std::vector <double> cmdline_lagrange ;
	std::vector <double> other_lagrange ;
	std::vector <double> cmdline_angular_radial_ratios ;
	std::vector <double> other_angular_radial_ratios ;
	std::vector <double> mass_values ;

	run_mass_analysis_and_trajectory_calcs (prog_control, prog_params,
				first_body, second_body,
				&mass_values,
				cmdline_lagrange, cmdline_angular_radial_ratios,
				!runOnlyToCalculateAverage,
				prog_control.calc_average_wander) ;

	if ( prog_control.run_mass_analysis && prog_control.calc_average_wander )
	{
		ofstream average_wander_mass ;
		ofstream average_small_mass_libration ;

		prog_params.lagrange_point =
			((LAGRANGE_4 == prog_params.lagrange_point) ?
				LAGRANGE_5 : LAGRANGE_4) ;

		run_mass_analysis_and_trajectory_calcs (prog_control, prog_params,
				first_body, second_body,
				NULL,
				other_lagrange, other_angular_radial_ratios,
				runOnlyToCalculateAverage,
				prog_control.calc_average_wander) ;

		if ( cmdline_lagrange.size () != other_lagrange.size () )
		{
			cerr << "The two mass analysis runs for " <<
				"different Lagrange points have returned " <<
				"inconsistent data sets" << endl ;
			return -1 ;
		}

		average_wander_mass.open (make_output_file (prog_control,
				"wander_mass_average"), ios::trunc) ;

		for ( unsigned int i = 0; i < cmdline_lagrange.size (); i++ )
		{
			double avg = (cmdline_lagrange [i] + other_lagrange [i]) / 2.0 ;

			average_wander_mass << calc_reduced_mass (first_body.m, mass_values [i]) <<
				 "\t" << avg << "\t" << mass_values [i] << endl ;
		}
		average_wander_mass.close () ;


		if ( cmdline_angular_radial_ratios.size () != other_angular_radial_ratios.size () )
		{
			cerr << "The two mass analysis runs for " <<
				"different Lagrange points have returned " <<
				"inconsistent angular radial ratio data sets" << endl ;
			return -1 ;
		}

		// libration data
		average_small_mass_libration.open (make_output_file (prog_control,
				"average_small_mass_libration"), ios::trunc) ;
		for ( unsigned int i = 0; i < cmdline_lagrange.size (); i++ )
		{
			double avg = (cmdline_angular_radial_ratios [i] +
				other_angular_radial_ratios [i]) / 2.0 ;

			average_small_mass_libration <<
				log (calc_reduced_mass (first_body.m, mass_values [i])) <<
				"\t" << log (avg) << endl ;
		}
		average_small_mass_libration.close () ;
	}

	return 0 ;
}

/*
Iterate through a number of specified deviations in order to calculate wander values
for different deviation settings
*/
static
int
run_deviation_simulation_calcs (program_control &prog_control,
		body &first_body, body &second_body,
		deviation_iteration & dev_iter,
		std::vector <double> * deviation_values,
		std::vector <double> & wander_values,
		bool onlyCalculateWanderValues,
		bool makeListOfWanderValues)
{
	ofstream outfile ;
	double angular_wander, radial_wander ;
	double saved_deviation = *dev_iter.deviation ;


	if ( !onlyCalculateWanderValues )
	{
		outfile.open (make_output_file (prog_control, dev_iter.filename), ios::trunc) ;
	}

	for ( double d = dev_iter.from;
				d <= dev_iter.to + 0.0000000001;
				d += dev_iter.step )
	{
		initial_conditions dev_conditions ;

		*dev_iter.deviation = d ;

		set_initial_conditions (dev_conditions, dev_iter.params,
			first_body, second_body) ;

		dump_main_variables (first_body, second_body,
				dev_conditions, dev_iter.params, prog_control) ;

		calc_mvt (false, angular_wander, radial_wander,
				prog_control, dev_conditions, dev_iter.params,
				first_body, second_body, NULL) ;

		if ( !onlyCalculateWanderValues )
		{
			outfile << d << "\t" << (angular_wander * 180.0 / M_PI) 
				<< "\t" << (0.5 * radial_wander / R_DISTANCE) << endl ;
		}
		if ( makeListOfWanderValues )
		{
			wander_values.push_back (angular_wander * 180.0 / M_PI) ;
			if ( deviation_values != NULL )
			{
				deviation_values->push_back (d) ;
			}
		}
	}

	if ( !onlyCalculateWanderValues )
	{
		outfile.close () ;
	}

	*dev_iter.deviation = saved_deviation ;

	return 0;
}

/*
Particular deviations are iterated over by changing their value while the other
deviations are kept constant at the program-specified value
*/
static
int
run_deviation_simulation (program_control &prog_control,
		const program_params &prog_params,
		body &first_body, body &second_body)
{
	const bool runOnlyToCalculateAverage = true ;

	deviation_iteration dev_iters [4] =
	{
		{ "xr_wander",
				{ 0.0, 0.0, 0.0, 0.0, prog_params.lagrange_point },
				-0.015, 0.015, 0.0001,
				&dev_iters [0].params.xr_deviation },
		{ "a_wander",
				{ 0.0, 0.0, 0.0, 0.0, prog_params.lagrange_point },
				-0.9, 5.5, 0.01,
				&dev_iters [1].params.a_deviation },
		{ "vr_wander",
				{ 0.0, 0.0, 0.0, 0.0, prog_params.lagrange_point },
				-0.8, 0.8, 0.002,
				&dev_iters [2].params.vr_deviation },
		{ "vt_wander",
				{ 0.0, 0.0, 0.0, 0.0, prog_params.lagrange_point },
				-0.1, 0.1, 0.002,
				&dev_iters [3].params.vt_deviation }
	} ;


	second_body.m = prog_control.mvt_simulation_mass ;

	initialise_bodies (first_body, second_body) ;

	for ( int i = 0; i < sizeof (dev_iters) / sizeof (deviation_iteration); i++ )
	{
		std::vector <double> cmdline_lagrange ;
		std::vector <double> other_lagrange ;
		std::vector <double> deviation_values ;

		run_deviation_simulation_calcs (prog_control,
				first_body, second_body,
				dev_iters [i],
				&deviation_values, cmdline_lagrange,
				!runOnlyToCalculateAverage,
				prog_control.calc_average_wander) ;

		if ( prog_control.calc_average_wander )
		{
			ofstream average_wander_mass ;
			char filename [PATH_MAX] ;

			dev_iters [i].params.lagrange_point =
				((LAGRANGE_4 == dev_iters [i].params.lagrange_point) ?
					LAGRANGE_5 : LAGRANGE_4) ;

			run_deviation_simulation_calcs (prog_control,
				first_body, second_body,
				dev_iters [i],
				NULL, other_lagrange,
				runOnlyToCalculateAverage,
				prog_control.calc_average_wander) ;


			sprintf (filename, "%s_average", dev_iters [i].filename) ;
			average_wander_mass.open (make_output_file (prog_control,
					filename), ios::trunc) ;
			for ( unsigned int i = 0; i < cmdline_lagrange.size (); i++ )
			{
				double avg = (cmdline_lagrange [i] + other_lagrange [i]) / 2.0 ;

				average_wander_mass << deviation_values [i] <<
						"\t" << avg << endl ;
			}
			average_wander_mass.close () ;
		}
	}

	return 0 ;
}

/*
Iterate through a series of program parameters in order to test that errors remian
acceptable in each case
*/
static
int
run_error_simulation (program_control &prog_control,
		program_params &prog_params,
		body &first_body, body &second_body)
{
	struct program_params error_simul_params [4] =
		{
			{ 0.006, 0.0, 0.0, 0.0, LAGRANGE_4 },
			{ 0.005, 0.0, 0.0, 0.0, LAGRANGE_4 },
			{ 0.004, 0.0, 0.0, 0.0, LAGRANGE_4 },
			{ 0.003, 0.0, 0.0, 0.0, LAGRANGE_4 }
		} ;

	second_body.m = prog_control.mvt_simulation_mass ;

	initialise_bodies (first_body, second_body) ;


	for ( int i = 0; i < sizeof (error_simul_params) / sizeof (program_params); i++ )
	{
	ofstream errorfile ;
	char filename [PATH_MAX] ;
	double angular_wander, radial_wander ;
	initial_conditions err_conditions ;


		sprintf (filename, "error_%d", i) ;

		errorfile.open (make_output_file (prog_control, filename), ios::trunc) ;


		set_initial_conditions (err_conditions, error_simul_params [i],
				first_body, second_body) ;

		dump_main_variables (first_body, second_body,
			err_conditions, error_simul_params [i], prog_control) ;

		calc_mvt (false, angular_wander, radial_wander,
				prog_control, err_conditions, error_simul_params [i],
				first_body, second_body, &errorfile) ;

		errorfile.close() ;
	}

	{
		ofstream paramfile ;

		paramfile.open (make_output_file (prog_control, "error",
					"params"), ios::trunc) ;
		for ( int i = 0; i < sizeof (error_simul_params) / sizeof (program_params); i++ )
		{
			if ( i != 0 )   paramfile << "|" ;
			paramfile <<
				std::fixed <<
				std::setprecision (3) <<
				error_simul_params [i].xr_deviation <<
				"|" <<
				std::fixed <<
				std::setprecision (1) <<
				error_simul_params [i].a_deviation <<
				"|" <<
				std::fixed <<
				std::setprecision (1) <<
				error_simul_params [i].vr_deviation <<
				"|" <<
				std::fixed <<
				std::setprecision (1) <<
				error_simul_params [i].vt_deviation ;
		}
		paramfile << endl ;
		paramfile.close () ;
	}


	return 0 ;
}

// program entry point and set up all default values
// run each function corresponding to the program functionalities where specified
// by the user
int
main (int argc, char **argv)
{
	program_control prog_control ;
	program_params prog_params ;


	body first_body ;
	body second_body ;

	prog_control.debug = false ;
	prog_control.restrict_data_output = true ;

	prog_control.t_final = 1000.0 ;
	prog_control.step_size = 1e-3 ;
	prog_control.constant_step_size = false ;
	prog_control.max_data_points = 1e6 ;

	prog_control.max_simulation_mass = 0.045 ;
	prog_control.mass_increment = 0.001 ;
	prog_control.mvt_simulation_mass = 0.001 ;  // mass of Jupiter

	prog_control.run_trajectory = true ;
	prog_control.run_mass_analysis = true ;
	prog_control.run_deviation_simulation = true ;
	prog_control.run_error_simulation = true ;

	prog_control.calc_average_wander = true ;

	prog_control.output_dir = "./" ;

	// give default values to initial conditions
	prog_params.xr_deviation = 0.005 ;
	prog_params.a_deviation = 0.0 ;
	prog_params.vr_deviation = 0.0 ;
	prog_params.vt_deviation = 0.0 ; 
	prog_params.lagrange_point = LAGRANGE_4 ; 


	if ( get_args (argc, argv, &prog_control, &prog_params) < 0 )
	{
		return -1 ;
	}


	first_body.m = 1.0 ;


	if ( prog_control.run_trajectory || prog_control.run_mass_analysis )
	{
	const char *title = (prog_control.run_mass_analysis ? (prog_control.run_mass_analysis ?
				"Trajectory and Mass Analysis" :
				"Trajectory") :
			"Mass analysis") ;

		cout << "Performing " << title << " .............................." << endl ;

		prog_control.timer.start () ;

		if ( run_mass_analysis_and_trajectory (prog_control, prog_params,
				first_body, second_body) < 0 )
		{
			return -1 ;
		}

		prog_control.timer.end (title) ;
	}




	if ( prog_control.run_deviation_simulation )
	{
		cout << "Performing deviation simulation" <<
			" .............................." << endl ;

		prog_control.timer.start () ;

		if ( run_deviation_simulation (prog_control, prog_params,
				first_body, second_body) < 0 )
		{
			return -1 ;
		}

		prog_control.timer.end ("Deviation simulation") ;
	}

	if ( prog_control.run_error_simulation )
	{
		cout << "Performing error simulation" << " .............................." << endl ;

		prog_control.timer.start () ;

		if ( run_error_simulation (prog_control, prog_params,
				first_body, second_body) < 0 )
		{
			return -1 ;
		}

		prog_control.timer.end ("Error simulation") ;
	}



	return 0 ;
}
