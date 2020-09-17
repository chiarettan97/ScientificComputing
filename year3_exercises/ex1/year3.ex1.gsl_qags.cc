// Quadrature technique to calculate Fresnel integrals ( Core Task 2 )

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include "gsl/gsl_integration.h"
/*
#include "gsl/gsl_rng.h"
#include "gsl/gsl_monte.h"
#include "gsl/gsl_math.h"
#include "gsl/gsl_monte_plain.h"
#include "gsl/gsl_monte_miser.h"
#include "gsl/gsl_monte_vegas.h"
*/

#define N  10000	// number of intervals
#define N_PLOTS	 200	// number of values of u, the maximum value of u is given by N-PLOTS/100

using namespace std ;

// function to be integrated
static
double
sine_function( double x, void * params )
{
double sine_u ;
	sine_u = sin( 0.5 * M_PI * x * x ) ;
	return sine_u ;
}

static
double
cosine_function ( double x, void * params )
{
double cosine_u ;
	cosine_u = cos( 0.5 * M_PI * x * x ) ;
	return cosine_u ;
}

static
void
display ( const char *integral, double result, double error)
{
	cout << "The integral of " << integral << "=" << result << endl ;
	cout << "The error = " << error << endl ;
}


static
int
do_integration (ofstream & outfile, bool bNegative)
{
double sine_result, sine_error, cosine_result, cosine_error ;
gsl_function sine_u ;
gsl_function cosine_u ;

	sine_u.function = &sine_function ;
	cosine_u.function = &cosine_function ;


	for ( double d = 0.1; d < (double) (N_PLOTS/10); d += 0.1 )
	{
	double xl = 0.0 ;	// set upper and lower limits of integration
	double xu = (bNegative ? -d : d) ;
		{
			gsl_integration_workspace *w = gsl_integration_workspace_alloc (N) ;
			gsl_integration_qags (&sine_u, xl, xu, 0, 1e-7, N, w, &sine_result, &sine_error) ;
			gsl_integration_workspace_free (w) ;
			display ( "S(u)", sine_result, sine_error ) ;
		}

		{
			gsl_integration_workspace *w = gsl_integration_workspace_alloc (N) ;
			gsl_integration_qags (&cosine_u, xl, xu, 0, 1e-7, N, w, &cosine_result, &cosine_error) ;
			gsl_integration_workspace_free (w) ;
			display ( "C(u)", cosine_result, cosine_error ) ;
		}

		outfile << d << "\t" << sine_result << "\t" << sine_error << "\t" << cosine_result << "\t" << cosine_error << endl ;
	}


	return 0 ;
}

int
main()
{
ofstream outfile ;
// random number generator initialisation
//gsl_rng *r = gsl_rng_alloc (gsl_rng_default) ;


//	gsl_rng_env_setup () ;

	outfile.open ("qags_fresnel.dat", ios::trunc) ;
	do_integration (outfile, false) ;
	do_integration (outfile, true) ;

//	gsl_rng_free (r) ;

	outfile.close () ;

	return 0 ;
}
