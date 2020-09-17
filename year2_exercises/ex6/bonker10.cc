// one-dimensional box with N particles

#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#define TESTING_PARTICLES

using namespace std ;

#define BALLS 50 
#define RANF() \
 (( ( double ) rand () ) / ( ( 1.0 + RAND_MAX ) ))
#define IGNORE_VALUE 1000000
struct particle
{
	double x ;	// position
	double im ;	// inverse mass
	double v ;	// velocity
	double R ;	// radius of particle
	double p ;	// momentum
	double T ;	// kinetic energy
} ;

void init_particle (particle *particella, int n_parts) ;
void dump_particle (particle *particella, int n_parts) ;
void position ( particle *a , double &dt , int N , int &which ) ;
void momentum ( particle *a , int which ) ;
void velocity ( particle *a , int which ) ;
void energy ( particle *a , int which ) ;
double kinetic ( particle *a , int N ) ;
void collide ( particle *a, int which ) ;
void time_before_collision ( particle *a , double *time_to_collide, int N ) ;
double smallest_time ( double *time_to_collide , int N , int &which ) ;
void simulation ( particle *a , int N , double &t , double T , double &E , ostream &filename ) ;
						
int main ( int argc, char* argv[] )
{
	
	int walls_number = 2 ;
	int balls_number = BALLS ;
	int N = walls_number + balls_number ;
	particle *a ;
	a = new particle[N] ;

//	double dt ;		// times between collisions	
	double T = 100 ;	// total simulation time
	double t = 0 ;		// time 

	double E ;

	int seed = 123 ;

	if ( argc > 1 )
	{
		sscanf ( argv [ 1 ] , "%d", &seed ) ;
	}	

	cout << "# " << argv [ 0 ] << " seed = " << seed << endl ;
	srand ( seed ) ;

	// output data
	ofstream filename ;
	filename.open( "bonker_data.txt" , ios::trunc ) ;
	if ( filename.good() == false )
	{
		cout << "Cannot write to file." ;
		exit(0) ;
	}

	init_particle (a, N) ;

	dump_particle (a, N) ;

	// initialise particle momentum and energy
	for ( int i = 0 ; i < N ; i ++ )
	{
		momentum ( a , i ) ;
		energy ( a , i ) ;
	} 

      

	simulation ( a , N , t , T , E , filename ) ;

	return 0 ;
}

void
dump_particle (particle *particella, int n_parts)
{
#ifdef TESTING_PARTICLES
	for ( int i = 0 ; i < n_parts ; i ++ )
	{
		cerr << "Particle n.:" << i << endl ;
		cerr << "\tv:" << particella [i].v << endl ;
		cerr << "\tim:" << particella [i].im << endl ;
		cerr << "\tR:" << particella [i].R << endl ;
		cerr << "\tx:" << particella [i].x << endl ;
	}
       
#endif // TESTING_PARTICLES
}

void
init_particle (particle *particella, int n_parts)
{

	// initial conditions
	for ( int i = 0 ; i < n_parts ; i ++ )
	{
		particella[i].x = ( - n_parts / 2 ) + i ;
	
		if ( i == 0 || i == n_parts - 1 )	// initialise walls
		{
			particella[i].v = 0 ;
			particella[i].im = 0 ;
			particella[i].R = 0 ;
		}
		else
		{
			particella[i].v = RANF() ;
			particella[i].im = RANF() ;
			particella[i].R = RANF() ;
		}
	}
}

//calculate positions of the particles after time dt
void
position ( particle *a , double &dt , int N , int &which )
{
double * time_to_collide = new double [N] ;
cerr << "here\n" ;
	time_before_collision (a, time_to_collide, N) ;
cerr << "here 2\n" ;

	dt = smallest_time ( time_to_collide , N , which ) ;
	cerr << "value of dt is \n" << dt << endl ;

	for ( int i = 0 ; i < N ; i ++ )
	{
		a[i].x += dt * a[i].v ;
	}

	delete time_to_collide ;
cerr << "here 4\n" ;
 dump_particle ( a , N ) ;
}

//calculate momentum from velocity
void
momentum ( particle *a , int which )
{
	if ( a[which].im > 0 )
	{
		a[which].p = a[which].v / a[which].im ;
	}
	else
	{
		a[which].p = 0.0 ;
	}
}

//calculate velocity from momentum
void
velocity ( particle *a , int which )
{
	a[which].v = a[which].p * a[which].im ;
}

// calculate kinetic energy of a particle
void
energy ( particle *a , int which )
{
	a[which].T = 0.5 * a[which].v * a[which].p ;
}	

// calculate total kinetic energy in system
double
kinetic ( particle *a , int N )
{
  double E = 0.0 ;
	for ( int i = 0 ; i < N ; i ++ )
	{
		if ( a[i].im > 0.0 )
		{
			E += 0.5 * a[i].v / a[i].im ;
		}
	}
	return E ;
	cerr<< "energy" << E << endl ;
}

// elastic collision between 2 particles
void
collide ( particle *a , int which )
{
	double com_v = ( a[which].im * a[which+1].v + a[which].v * a[which+1].im ) / ( a[which].im + a[which+1].im ) ;
	a[which].v = 2 * com_v - a[which].v ;
	a[which+1].v = 2 * com_v - a[which+1].v ;
	momentum ( a , which ) ;
	momentum ( a , which+1 ) ;
}

// find the time for the next collision 
//double
//time_before_collision ( particle *a , int N , int &which , double &dt )
//{
//	for ( int i = 0 ; i < N - 1 ; i ++ )
//	{
//		double relative_velocity = a[i].v - a[i+1].v ;

//		if ( relative_velocity > 0.0 )
//		{
//			dt = ( ( a[i+1].x - a[i+1].R ) - ( a[i].x + a[i].R ) ) / relative_velocity ;	
//			which = i ;
//		}

//	}
//	return dt ;
//}

// find the time before each particle collides with its neighbour and create an array with these times
void 
time_before_collision ( particle *a , double *time_to_collide , int N )
{
	for ( int i = 0 ; i < N - 1 ; i ++ )
	{
		double relative_velocity = a[i].v - a[i+1].v ;

		if ( relative_velocity > 0.0 )
		{
			time_to_collide[i] = ( ( a[i+1].x - a[i+1].R ) - ( a[i].x + a[i].R ) ) / relative_velocity ;	
		}
		else 
	        {
		    time_to_collide[i] = IGNORE_VALUE ;
	        }
		cerr << "time to collide" << time_to_collide[i] << endl ;      
	}
}

// find the smallest time in the array time_to_collide - this is equivalent to finding the time for the next 2 particles to collide
double
smallest_time ( double *time_to_collide , int N , int &which )
{
	double dt ;
	int i ;	
         
	for ( i = 0 , which = 0 , dt = time_to_collide[0] ; i < N - 1 ; i ++ )
	{
	  if ( time_to_collide[i] > IGNORE_VALUE - 1 )
	    {
	      continue ;
	    }
	  
		if ( time_to_collide[i] < dt )
		{
			dt = time_to_collide[i] ;
			which = i ;
		}
		cerr << "Time to collide after smallest time" << dt << endl ;
	}
	return dt ;
}

void
simulation ( particle *a , int N , double &t , double T , double &E , ostream &filename )
{
	int which ;
	
	while ( t <= T )
	{
	  cerr << T << endl ;
	  cerr << t << endl ;
		double dt ;

		position ( a , dt , N , which ) ;	
		t += dt ;
		collide ( a , which ) ;
		velocity ( a , which ) ;
		
		for ( int i = 0 ; i < N ; i ++ )
		{
			filename << a[i].x << endl ;
		}
		E = kinetic ( a , N ) ;
		filename << "Total Kinetic energy of system: " << "\t" << E << endl ;
	}
}

