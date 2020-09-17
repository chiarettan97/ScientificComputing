//simulation of a small planet travelling near the sun
//uses the Euler method, the leapfrog method, the fourth order Runge-Kutta method
//mass is measured in units of solar mass, distance in astronomical units and time in years

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#define _USE_MATH_DEFINES
#include <cmath>

using namespace std ;

#define D 2			// dimensions

#define	G (4 * M_PI * M_PI)	// gravitational constant 

#define DEFAULT_SIMULATION_TIME 2

struct particle
{
	double m ;	// mass
	double x[D] ;	// position
	double v[D] ;	// velocity
	double f[D] ;	// gravitational force exerted on the particle
	double k_e ;	// kinetic energy
	double p_e ;	// potential energy
	double L ;	// orbital angular momentum
	double r ;	// absolute distance between particles
} ;


void force ( particle &planet , particle &sun ) ;
void position_step ( particle &planet , double dt ) ;
void velocity_step ( particle &planet , double dt ) ;
void energy ( particle &planet , particle &sun ) ;
void show_state ( const particle &planet , double &t ) ;
void euler_method ( particle &planet , particle &sun , double dt , double T , double &t , ostream &filename ) ;
void leapfrog_method ( particle &planet , particle &sun , double dt , double T , double &t , ostream &filename ) ;
void runge_kutta_step ( particle &planet , double dt ) ;
void runge_kutta_method ( particle &planet , particle &sun , double dt , double T , double &t , ostream &filename ) ;


int
main( int argc, char* argv[] )
{

	particle planet ;
	particle sun ;
	
	double t = 0.0 ;
	double dt ;

	int T = DEFAULT_SIMULATION_TIME ;

	if ( argc > 1 )
	{
		sscanf ( argv [ 1 ], "%d", &T ) ;
	}

	// try to write output file called example.txt
	ofstream filename ;
	filename.open ( "example.txt", ios::trunc ) ;
	if ( filename.good() == false )
	{
		cerr << "Can't write to file " << filename << endl ;
		exit(0) ;
	}

	sun.m = 1 ;

	int mode ;
	cout <<"Select 1 if you wish to view the orbit of the Earth around the Sun. Select 2 if you wish to enter your own settings." << endl ;
	cin >> mode ;

	if ( 1 == mode )
	{
		planet.m = pow( 3.003 , -6 ) ;
	}

	else if ( 2 == mode )
	{
		double M ; 
		cout << "Enter the mass of the planet in units of solar masses." << endl ;
		cin >> M ;
	
		if ( M > 0.001 )
		{
			cerr << "This simulation assumes that the mass of the planet is much smaller than that of the Sun. Please choose a smaller mass." << endl ;
			return -1 ; 
		}
		else if ( M <= 0 )
		{
			cerr << "Not physically possible." << endl ;
			return -1 ;
		}
		else
		{
			planet.m = M ;
		}
	}
	else
	{
		cerr << "Please select 1 or 2." << endl ;
		return -1 ;
	}


	cout << "The total time of the simulation is: " << T << ". Enter the time step in units of years." << endl ;
	cin >> dt ;

	if ( (T / dt) < 100 )
	{
		cerr << "It is advisable that you specify a longer simulation time or that you choose a shorter time step." << endl ;
		return -1 ;
	}
	if ( (T / dt) > 10000 )
	{
		cerr << "It is advisable that you specify a shorter simulation time or that you choose a longer time step." << endl ;
		return -1 ;
	}

	if ( D < 2 )
	{
		cerr << "D needs to be at least 2" << endl ;
		return -1 ;
	}

	// initial position and velocity
	// if D > 2 the motion in the upper dimensions will not be initialised
	planet.x[0] = 1.0 ;
	planet.x[1] = 0.0 ;
	planet.v[0] = 0.0 ;
	planet.v[1] = 4.0 ;

	// the sun is massive enough that it can be estimated to remain stationary at the origin
	sun.x[0] = 0.0 ;
	sun.x[1] = 0.0 ;
	sun.v[0] = 0.0 ;
	sun.v[1] = 0.0 ;


	int method ;

	cout << "Select iteration method: \n 1. Euler \n 2. Leapfrog \n 3. Runge-Kutta " << endl ;

	cin >> method ;

	if ( 1 == method )
	{
		euler_method ( planet , sun , dt , T , t , filename ) ;
	}

	else if ( 2 == method )
	{
		leapfrog_method ( planet , sun , dt , T , t , filename ) ;
	}

	else if ( 3 == method )
	{
		runge_kutta_method ( planet , sun , dt , T , t , filename ) ;
	}

	else
	{
		cerr << "Error! Please select either 1, 2 or 3." << endl ;
		return -1 ;
	}

	return 0 ;

}

//calculate the gravitational force on the planet
void
force ( particle &planet , particle &sun )
{
	double r_squared = 0.0 ;

	for ( int i = 0 ; i < D ; i++ )
	{
		r_squared += ( planet.x[i] - sun.x[i] ) * ( planet.x[i] - sun.x[i] ) ;
	}

	double r = sqrt (r_squared) ;
	planet.r = r ;

	for ( int i = 0 ; i < D ; i++ )
	{
		planet.f[i] = - G * sun.m * planet.m * ( planet.x[i] - sun.x[i] ) / (r * r_squared) ;
	}
}

//calculate the new position a time dt later
void
position_step ( particle &planet , double dt )
{
	for ( int i = 0 ; i < D ; i++ )
	{
		planet.x[i] += dt * planet.v[i] ;
	}
}

//calculate the new velocity a time dt later
void
velocity_step ( particle &planet , double dt )
{
	for ( int i = 0 ; i < D ; i++ )
	{
		planet.v[i] += dt * planet.f[i] / planet.m ;
	}
}

//calculate energies and orbital angular momentum
void
energy ( particle &planet , particle &sun )
{
	double r_squared = 0.0 ;

	for ( int i = 0 ; i < D ; i++ )
	{
		r_squared += ( planet.x[i] - sun.x[i] ) * ( planet.x[i] - sun.x[i] ) ;
	}
	
	double r = sqrt ( r_squared ) ;

	double speed_squared = 0.0 ;

	for ( int i = 0 ; i < D ; i ++ )
	{
		speed_squared += planet.v[i] * planet.v[i] ;
	}

	double speed = sqrt ( speed_squared ) ;
	planet.k_e = 0.5 * planet.m * speed_squared ;
	planet.p_e = - G * sun.m * planet.m / r ;

	planet.L = r * planet.m * speed ;
}

void
show_state ( const particle &planet , double &t , ostream &filename )
{
	filename << "time: " << t << "\t" ;

	for ( int i = 0 ; i < D ; i ++ )
	{
		filename << "position: " << planet.x[i] << "\t" ;
	}

	for ( int i = 0 ; i < D ; i ++ )
	{
		filename << "velocity: " << planet.v[i] << "\t" ;
	}

	filename << "kinetic energy: " << planet.k_e << "\t" << "potential energy: " << planet.p_e << "\t" << "orbital angular momentum: " << planet.L << "\t" << endl ;	
}

void
euler_method ( particle &planet , particle &sun , double dt , double T , double &t , ostream &filename )
{
	for ( int i = 0 ; i < (T/dt) ; i ++ )
	{
		show_state ( planet , t , filename ) ;
		
		force ( planet , sun ) ;
		position_step ( planet , dt ) ;
		velocity_step ( planet , dt ) ;
		t += dt ; 
		energy ( planet , sun ) ;
	}
}

void
leapfrog_method ( particle &planet , particle &sun , double dt , double T , double &t , ostream &filename )
{
	for ( int i = 0 ; i < (T/dt) ; i ++ )
	{
		show_state ( planet , t , filename ) ;	

		position_step ( planet , dt * 0.5 ) ;
		force ( planet , sun ) ;
		velocity_step ( planet , dt ) ;
		position_step ( planet , dt * 0.5 ) ;
		energy ( planet , sun ) ;
		t += dt ;
	}
}

void
runge_kutta_step ( particle &planet , double dt )
{

	for ( int i = 0 ; i < D ; i ++ )
	{
		double k_x_1 = planet.v[i] ;
		double k_v_1 = planet.f[i] / planet.m ;
	
		planet.x[i] += 0.5 * dt * k_x_1 ;

		double k_v_2 = planet.f[i] / planet.m ;
		double k_x_2 = planet.v[i] + 0.5 * dt * k_v_1 ;	

		planet.x[i] += ( 0.5 * dt * k_x_2 ) - ( 0.5 * dt * k_x_1 ) ;

		double k_v_3 = planet.f[i] / planet.m ;
		double k_x_3 = planet.v[i] + 0.5 * dt * k_v_2 ;

		planet.x[i] += ( dt * k_x_3 ) - ( 0.5 * dt * k_x_2 ) ;

		double k_v_4 = planet.f[i] / planet.m ;
		double k_x_4 = planet.v[i] + dt * k_v_3 ;

		planet.v[i] += ( k_v_1 + 2 * k_v_2 + 2 * k_v_3 + k_v_4 ) * dt / 6 ;
		planet.x[i] += ( k_x_1 + 2 * k_x_2 + 2 * k_x_3 + k_x_4 ) * dt / 6 - dt * k_x_3 ;
	
	}
}

void
runge_kutta_method ( particle &planet , particle &sun , double dt , double T , double &t , ostream &filename )
{
	for ( int i = 0 ; i < (T/dt) ; i ++ )
	{
		show_state ( planet , t , filename ) ;

		runge_kutta_step ( planet , dt ) ;
		energy ( planet , sun ) ;
		t += dt ;
	}
}
