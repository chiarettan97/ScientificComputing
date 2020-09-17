#include <iostream>
#include <fstream>
#include "math.h"
#include "gsl/gsl_errno.h"
#include "gsl/gsl_odeiv2.h"

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
int
main (int argc, char **argv)
{
ofstream outfile ;

double t = 0.0 ;
double initial_position = 0.01 ;
double initial_speed = 0.0 ;
double y [DIMENSIONS] = {initial_position, initial_speed} ;
double q = 0.0 ;
double F = 0.0 ;
double d_params [2] = { q, F } ;

double t_final = 2 * M_PI * OSCILLATIONS ;
double step_size = 1e-3 ;

// initialise function pointers (no need for &)
const gsl_odeiv2_step_type *type = gsl_odeiv2_step_rk4 ;
gsl_odeiv2_step *step = gsl_odeiv2_step_alloc ( type , DIMENSIONS) ;		
gsl_odeiv2_control *control = NULL ;		// step size kept constant
gsl_odeiv2_evolve *evolve = gsl_odeiv2_evolve_alloc ( DIMENSIONS ) ;

gsl_odeiv2_system sys = { calc_derivatives, NULL, DIMENSIONS, (void *) d_params } ;

	outfile.open ("pendulum.dat", ios::trunc) ;

	// advance solution until reach t_final
	while ( t < t_final )
	{
	double energy  = calc_energy ( y ) ;	

//		cout << "t = " << t << "\t" << "theta = " << y[0] << "\t" << "omega = " << y[1] << "energy = " << energy << endl ;

		outfile << t << "\t" << y[0] << "\t" << y[1] << "\t" << energy << endl ;

		int status = gsl_odeiv2_evolve_apply ( evolve, control, step, &sys, &t, t_final, &step_size, y ) ;
		
		if ( status != GSL_SUCCESS )
		{
			break ;
		}
	}

	// free workspaces
	gsl_odeiv2_evolve_free ( evolve ) ;
	gsl_odeiv2_step_free ( step ) ;

	outfile.close () ;

	{
	ofstream paramfile ;
		paramfile.open ("pendulum.params", ios::trunc) ;
		paramfile << q << '|' << F << '|' << initial_speed << '|' << initial_position << endl ;
		paramfile.close () ;
	}

	return 0 ;
}
