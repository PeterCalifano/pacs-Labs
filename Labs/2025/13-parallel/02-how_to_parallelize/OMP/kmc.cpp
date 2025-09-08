#include <algorithm>
#include <exception>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <vector>
#include <timer.h>

using REAL = double;
using INT  = unsigned int;

constexpr REAL   xmin= 0;                        // [m] domain bounds
constexpr REAL   xmax= 2;                        // [m] domain bounds
constexpr REAL   ymin= 0;                        // [m] domain bounds
constexpr REAL   ymax= 2;                        // [m] domain bounds
constexpr REAL   KT  = .1;                       // [J] thermal energy
constexpr REAL   r   = 1e-2;                     // [m] particle radius
constexpr REAL   mu  = 1.;                       // [kg s^-1 m_1] viscosity
constexpr REAL   mob = 1. / 6. / M_PI / mu / r;  // [s kg^-1] mobility
constexpr INT    Np  = 100000;                  // [-] number of particles
constexpr INT    SVST= 5000;                     // [-] number of particles
constexpr REAL   dt  = .5e-5;                    // [s] time step
constexpr REAL   T   = 3.0;                      // [s] simulation time
constexpr REAL   g   = 0.5;                       // [m s^-2] gravity
constexpr REAL   m   = 1;                      // [kg] particle mass

void fluid_velocity (REAL x, REAL y, REAL& vx, REAL& vy){
  vx = 2. * M_PI * (1.-y);
  vy = 2. * M_PI * (x-1.);
}

void particle_force (REAL x, REAL y, REAL& fx, REAL& fy){
  fx = 0;
  fy = -m*g;
}

void walls (REAL &x, REAL &y){
  x = std::min (xmax, std::max (xmin, x));
  y = std::min (xmax, std::max (xmin, y));
}

template<class VECTOR>
class
stepper {
private :
  VECTOR &x;
  VECTOR &y;

public :

  stepper (VECTOR &x_, VECTOR &y_)
    : x(x_), y(y_) { };

  void operator() (INT n) {
    REAL vx, vy;
    REAL fx, fy;

    // Compute fluid velocity and forces
    fluid_velocity (x[n], y[n], vx, vy);
    particle_force (x[n], y[n], fx, fy);

    fluid_velocity (x[n] + (vx + mob*fx) * dt/2, y[n] + (vy + mob*fy) * dt/2, vx, vy);
    particle_force (x[n] + (vx + mob*fx) * dt/2, y[n] + (vy + mob*fy) * dt/2, fx, fy);

    //update particles positions
    x[n] += (vx + mob*fx) * dt;
    y[n] += (vy + mob*fy) * dt;

    //enforce BCs
    walls (x[n], y[n]);
  };


  void save(const std::string &filename) const {
    std::ofstream outFile (filename);

    // Check if the file is open
    if (!outFile.is_open()) {
      std::cerr << "Error opening output file." << std::endl;
    }

    // Write the header for the CSV file
    outFile << "%% Ball_X, Ball_Y \n";


    // Write particle position and time to the output file
    for (INT n = 0; n < Np; ++n){
      outFile << x[n] << "," << y[n] << "\n";
    }

    //Close the time-step-specific file
    outFile.close();
  }
};


int main() {

  std::vector<REAL> x (Np);
  std::vector<REAL> y (Np);
  // OR :
  // REAL *x = new REAL[Np];
  // REAL *y = new REAL[Np];

  std::random_device rd;  // Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::normal_distribution<> uniform(3*(xmax-xmin)/4, std::pow((xmax-xmin)/8, 2));

  // initialize particle positions
  for (INT n = 0; n < Np; ++n) {
    x[n]=uniform(gen);
    y[n]=uniform(gen);
  }

  stepper state (x, y);
  cdf::timer::timer_t timer;

  //Calculate the number of time steps
  INT Nt = std::ceil(T/dt);
  std::cout << "T = " << T << "; dt = " << dt << "; Nt = " << Nt << std::endl;

  REAL  t = 0;  //initial time
  INT iff = 0;  //File index

  //Main simulation loop
  bool printframe = false;
  std::ofstream outFile;

#pragma omp parallel default(shared)
  {
    for (INT it = 0; it<Nt; ++it){

      // Loop over all particles
#pragma omp barrier
#pragma omp single
      {
	timer.tic("move partcles");
      }
#pragma omp for
      for (INT n=0; n<Np; ++n)
	state(n);

#pragma omp barrier
#pragma omp single
      {
	timer.toc("move partcles");
	printframe = false;
	if ((it % SVST) == 0) printframe = true;

	if (printframe) {
	  timer.tic("save");
	  //Create a new CSV file for each time step
	  auto particle_move_n = std::to_string(iff++);
	  std::string particle_move = "particle_position_00000.csv";
	  particle_move.replace (23-particle_move_n.length (), particle_move_n.length (), particle_move_n);
	  std::cout << "iteration " << it;
	  std::cout << ", saving file " << particle_move << std::endl;
	  state.save (particle_move);
	  timer.toc("save");
	}
      }

      t += dt;
    }
  }

  std::cout << "Simulation complete. Data saved to particle_positions.csv.\n";
  // if using arrays we should free them manually
  // delete[] x;
  // delete[] y;
  timer.print_report();
  return 0;
}

