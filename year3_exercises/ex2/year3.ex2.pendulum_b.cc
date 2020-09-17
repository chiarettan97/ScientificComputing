#include <iostream>
#include <fstream>
#include <getopt.h>
#include "math.h"
#include "gsl/gsl_errno.h"
#include "gsl/gsl_odeiv2.h"
#include <vector>

using namespace std ;

//#define STEP_SIZE  1e-3
#define DIMENSIONS  2	// dimensions of the system (number of ODEs)
#define G  9.81
#define MASS  1		// mass of pendulum bob  
#define OSCILLATIONS  10	// number of oscillations over which program runs


// y[0] = theta, y[1] = d(theta)/dt

// gsl_odeiv_system function stores the first order derivatives of the system in an array
// stepping functions advance solution from time t to t+h for a fixed step size, h (gsl_odeiv2_step). So this function actually carries out the algorithm and gives a proposed next value for (y, t).
// gsl_odeiv2_step_apply function applies the stepping function. It takes a pointer to the stepping function as an argument. It also takes a pointer to the function that defines the system of equations as an argument.
// control functions examines the proposed change to the solution produced by a stepping function and determines the optimal step size
// evolution function combines the results of a stepping function and control function to advance the solution forward one step. evolve_apply stores new values in t, y

static
int
calc_derivatives ( double t, const double y[], double dydt[], void *params )
{
double * d_params = (double *) params ;

	double q = d_params [0] ;
	double F = d_params [1] ;
		
	dydt[0] = y[1] ;
	dydt[1] = -sin( y[0] ) - q * y[1] + F * sin( 2/3 * t ) ;

	return GSL_SUCCESS ;
}

static
double
calc_energy ( double *y ) 
{
double energy = ( 0.5 * MASS * G * G * y[1] * y[1] ) + ( MASS * G * G * ( 1 - cos (y[0]) ) ) ;

	return energy ;
}

/*
static
void
display_values ( void (*calc_derivatives)() )
{
	cout << "t" << t << "\t" << "theta" << y[0] << "\t" << "omega" << y[1] << endl ;
}
*/

static
double
calc_period ( vector<double> &zero_crossing_times )
{
double period = 0.0 ;
vector<double> differences ;
double summa = 0.0 ;

	for ( int i = 0 ; i < ( zero_crossing_times.size() - 1 ) ; i ++ )
	{
	double diff = zero_crossing_times [i+1] - zero_crossing_times [i] ;
		differences.push_back (diff) ;
		cout << "Difference = " << diff << endl ;
	}

	for ( int i = 0 ; i < differences.size() ; i ++ )
	{
	 	summa += differences [i] ;
	}

	if ( differences.size () > 0 )    period = summa / differences.size() ;

	return period ;
}

int
calc_pendulum_mvt (bool bPlot, ofstream & compfile, double initial_position, double initial_speed,
				double q, double F)
{
ofstream outfile ;

double t = 0.0 ;
double y [DIMENSIONS] = {initial_position, initial_speed} ;
double d_params [2] = { q, F } ;

double t_final = 2 * M_PI * OSCILLATIONS ;
double step_size = 1e-3 ;

double period ; 

// initialise function pointers (no need for &)
const gsl_odeiv2_step_type *type = gsl_odeiv2_step_rk4 ;
gsl_odeiv2_step *step = gsl_odeiv2_step_alloc ( type , DIMENSIONS) ;		
gsl_odeiv2_control *control = NULL ;		// step size kept constant
gsl_odeiv2_evolve *evolve = gsl_odeiv2_evolve_alloc ( DIMENSIONS ) ;

gsl_odeiv2_system sys = { calc_derivatives, NULL, DIMENSIONS, (void *) d_params } ;

vector<double> zero_crossing_times ;

	cout << "initial_position : " << initial_position << "  bPlot : " << bPlot << endl ;

	if ( bPlot )
	{
		outfile.open ("pendulum_b.dat", ios::trunc) ;
	}

	// advance solution until reach t_final
	while ( t < t_final )
	{
	double energy  = calc_energy ( y ) ;	
	double old_y1 = y [1] ;

//		cout << "t = " << t << "\t" << "theta = " << y[0] << "\t" << "omega = " << y[1] << "energy = " << energy << endl ;

		if ( bPlot )
		{
			outfile << t << "\t" << y[0] << "\t" << y[1] << "\t" << energy << endl ;
		}

		int status = gsl_odeiv2_evolve_apply ( evolve, control, step, &sys, &t, t_final, &step_size, y ) ;
		
		if ( status != GSL_SUCCESS )
		{
			break ;
		}

		if ( (y [1] < 0.0 && old_y1 >= 0.0) || (y [1] > 0.0 && old_y1 <= 0.0) || y [1] == 0.0  )
		{
//			cout << "t = " << t << "  (" << y [1] << "," << old_y1 << ")" << endl ;
			zero_crossing_times.push_back ( t ) ;
//			cout << "t = " << zero_crossing_times.back() ;
		}
	}

	period = calc_period ( zero_crossing_times ) ;

	compfile << initial_position << '\t' << period << endl ;

	// free workspaces
	gsl_odeiv2_evolve_free ( evolve ) ;
	gsl_odeiv2_step_free ( step ) ;

	if ( bPlot )
	{
	ofstream paramfile ;

		outfile.close () ;

		paramfile.open ("pendulum.params", ios::trunc) ;
		paramfile << q << '|' << F << '|' << initial_speed << '|' << initial_position << endl ;
		paramfile.close () ;
	}

	return 0 ;
}


static
int
get_args (int argc, char **argv, double *initial_position, double *initial_speed, double *q, double *F)
{
bool bArgs = false ;
double dummy ;

	while (1)
	{
	int c;
	int this_option_optind = optind ? optind : 1;
	int option_index = 0;
	static struct option long_options[] = {
		{"help",  no_argument,       0,  'h' },
		{"initial-position",     required_argument, 0,  'p' },
		{"initial-speed",     required_argument, 0,  's' },
		{"q",     required_argument, 0,  'q' },
		{"F",     required_argument, 0,  'f' },
		{0,         0,                 0,  0 }
	};

		//c = getopt_long(argc, argv, "abc:d:012",
		c = getopt_long(argc, argv, "",
			long_options, &option_index);
		if (c == -1)
			break;

		switch (c)
		{
		case 0:
			printf("option %s", long_options[option_index].name);
			if (optarg)
				printf(" with arg %s", optarg);
			printf("\n");
			break;
		case 'h':
			cout << "usage: " << argv [0] << " [--help] [--initial-position num] [--initial-speed num] [--q num] [--F num]" << endl ;
			return -1 ;
			break;
		case 'p':
			dummy = atof (optarg) ;
			if ( dummy < 0.0 || dummy > 4.0 )
			{
				cerr << "Initial position is not valid : " << dummy << endl ;
				return -1 ;
			}
			*initial_position = dummy ;
			bArgs = true ;
			break;
		case 's':
			dummy = atof (optarg) ;
			if ( dummy < 0.0 )
			{
				cerr << "Initial speed is not valid : " << dummy << endl ;
				return -1 ;
			}
			*initial_speed = dummy ;
			bArgs = true ;
			break;
		case 'q':
			dummy = atof (optarg) ;
			if ( dummy < 0.0 )
			{
				cerr << "q is not valid : " << dummy << endl ;
				return -1 ;
			}
			*q = dummy ;
			bArgs = true ;
			break;
		case 'f':
			dummy = atof (optarg) ;
			if ( dummy < 0.0 )
			{
				cerr << "F is not valid : " << dummy << endl ;
				return -1 ;
			}
			*F = dummy ;
			bArgs = true ;
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

/*
	if (optind < argc)
	{
		printf("non-option ARGV-elements: ");
		while (optind < argc)
			printf("%s ", argv[optind++]);
		printf("\n");
	}
*/

	if ( bArgs )
	{
		cout << "Initial position : " << *initial_position << endl ;
		cout << "Initial speed : " << *initial_speed << endl ;
		cout << "q : " << *q << endl ;
		cout << "F : " << *F << endl ;
	}

	return 0 ;
}

int
main (int argc, char **argv)
{
ofstream compfile ;
double first_initial_position = 0.01 ;
double initial_speed = 0.0 ;
double q = 0.0 ;
double F = 0.0 ;
int i = 0 ;
double initial_position = 0.01 ;

	if ( get_args (argc, argv, &initial_position, &initial_speed, &q, &F) < 0 )
	{
		return -1 ;
	}

	compfile.open ("year3.ex2.pendulum_period_analysis.dat", ios::trunc) ;

	for ( double d = first_initial_position; d <= M_PI; d += (0 == i ? 0.09 : 0.10), i++ )
	{
	bool bPlot = ( fabs (initial_position - d) < 0.000001 ) ;

		if ( bPlot )
		{
			// debug so as to see if/when an initial position is used for plotting
			cout << "PLOTTING " << d << endl ;
		}

		calc_pendulum_mvt (bPlot, compfile, d, initial_speed, q, F) ;
	}

	compfile.close () ;

	return 0 ;
}
