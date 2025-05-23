// heatrect_mollerup.C --- Mollerup interpolation between top and bottom.
// 
// Copyright 2008 Per Abrahamsen and KVL.
//
// This file is part of Daisy.
// 
// Daisy is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// Daisy is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
// 
// You should have received a copy of the GNU Lesser Public License
// along with Daisy; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#define BUILD_DLL
#include "daisy/soil/transport/heatrect.h"
#include "util/solver.h"
#include "daisy/soil/transport/geometry_rect.h"
#include "object_model/plf.h"
#include "object_model/block_model.h"
#include "object_model/librarian.h"
#include "util/assertion.h"
#include "object_model/treelog.h"
#include "object_model/frame.h"

#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/banded.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>

namespace ublas = boost::numeric::ublas;
static const double water_heat_capacity = 4.2e7; // [erg/cm^3/dg C]
static const double rho_water = 1.0;             // [g/cm^3]

static void 
convection (const GeometryRect& geo,
            const double upstream_weight,
            const ublas::vector<double>& q_edge,
            Solver::Matrix& convec)
{

  const size_t edge_size = geo.edge_size (); // number of edges  

  for (size_t e = 0; e < edge_size; e++)
    {
      if (geo.edge_is_internal (e))
	{
	  const int from = geo.edge_from (e);
	  const int to = geo.edge_to (e);	   
	  const double value = geo.edge_area (e) *
            water_heat_capacity * rho_water * q_edge[e];
          
          const double alpha = (q_edge[e] >= 0) 
	    ? upstream_weight 
	    : 1.0 - upstream_weight;
	  convec (from, from) += alpha*value;
	  convec (from, to)   += (1.0-alpha)*value;
	  convec (to, from)   -= alpha*value;
	  convec (to, to)     -= (1.0-alpha)*value;
	} 
    }
}

static void 
cond_cell2edge (const GeometryRect& geo,
                const std::vector<double>& conductivity,
                ublas::vector<double>& cond_edge)
{
  const size_t edge_size = geo.edge_size ();
  
  for (size_t e = 0; e < edge_size; e++)
    {
      const int from = geo.edge_from (e);
      const int to = geo.edge_to (e);    
      
      if (geo.edge_is_internal (e))
	cond_edge[e] = 0.5 *
	  (conductivity[from] + conductivity[to]);
      else if (geo.cell_is_internal (from))
	cond_edge[e] = conductivity[from];
      else
	cond_edge[e] = conductivity[to];
    }
}

static void 
conduction (const GeometryRect& geo,
            const ublas::vector<double>& cond_edge,
            Solver::Matrix& conduc)
{
  const size_t edge_size = geo.edge_size (); // number of edges  
  
  for (size_t e = 0; e < edge_size; e++)
    {
      if (geo.edge_is_internal (e))
	{
	  const int from = geo.edge_from (e);
	  const int to = geo.edge_to (e);
	  const double magnitude = geo.edge_area_per_length (e) 
            * cond_edge[e]; 
	  
          conduc (from, from) -= magnitude;
	  conduc (from, to) += magnitude;
	  conduc (to, to) -= magnitude;
	  conduc (to, from) += magnitude; 
	} 
    }
}


static void
lowerboundary (const GeometryRect& geo,
               const bool isflux,
               const double T_border,
               Solver::Matrix& A,
               ublas::vector<double>& b)
{
  //The lower boundary is made by forcing the temperatures in the
  //lower cells to the given value This is maybe not the most precise
  //method, but it is stable...  First we tried to implement the lower
  //precise on boundary - but sometimes this is unstable!

  
  if (isflux)   //Nothing to do for (no)flux bc 
    return;
  
  const std::vector<size_t>& edge_below = geo.cell_edges (Geometry::cell_below);
  const size_t edge_below_size = edge_below.size ();
  
  for (size_t i = 0; i < edge_below_size; i++)
    {
      const size_t edge = edge_below[i];
      const int cell = geo.edge_other (edge, Geometry::cell_below);
      daisy_assert (geo.cell_is_internal (cell));
      const std::vector<size_t>& edge_around = geo.cell_edges (cell);
      const size_t edge_around_size = edge_around.size ();
      
      for (size_t j= 0; j < edge_around_size; j++)
        {
          const size_t edge_next = edge_around[j];
          const int cell_next = geo.edge_other (edge_next, cell);
          if (geo.cell_is_internal (cell_next))
            A (cell, cell_next) = 0;
        }
      A (cell, cell) = 1.0;
      b (cell) = T_border;
    }
}


static void
upperboundary (const GeometryRect& geo,
               const bool isflux,
               const double T_border,
               Solver::Matrix& A,
               ublas::vector<double>& b)
{
  //The upper boundary is made by forcing the temperatures in the
  //upper cells to the given value This is maybe not the most precise
  //method, but it is stable...  First we tried to implement the upper
  //boundary similar to the lower boundary but it is unstable -
  //probably because the large temperature fluctuations. It should
  //also be possible to do it similar as in the solute movement
  //calculations, but it divides the timestep into smaller
  //timesteps. In it should in heat calculations probalbly be done
  //very often....
  
  if (isflux)   //Nothing to do for (no)flux bc 
    return;
  
  const std::vector<size_t>& edge_above = geo.cell_edges (Geometry::cell_above);
  const size_t edge_above_size = edge_above.size ();
  
  for (size_t i = 0; i < edge_above_size; i++)
    {
      const size_t edge = edge_above[i];
      const int cell = geo.edge_other (edge, Geometry::cell_above);
      daisy_assert (geo.cell_is_internal (cell));
      const std::vector<size_t>& edge_around = geo.cell_edges (cell);
      const size_t edge_around_size = edge_around.size ();
      
      for (size_t j= 0; j < edge_around_size; j++)
        {
          const size_t edge_next = edge_around[j];
          const int cell_next = geo.edge_other (edge_next, cell);
          if (geo.cell_is_internal (cell_next))
            A (cell, cell_next) = 0;
        }
      A (cell, cell) = 1.0;
      b (cell) = T_border;
    }
}


//-----------------------new mmo 20080708 ------------------------
static void 
fluxes (const GeometryRect& geo,
        const bool isflux_lower,
        const bool isflux_upper,
        const double upstream_weight,
        const ublas::vector<double>& q_edge,
        const ublas::vector<double>& cond_edge,
        const ublas::vector<double>& T,
        const Solver::Matrix& A_before,                      
        const ublas::vector<double>& b_before,
        ublas::vector<double>& dQ) 
{
  const size_t edge_size = geo.edge_size (); // number of edges  
  
  daisy_assert (q_edge.size () == edge_size);
  daisy_assert (cond_edge.size () == edge_size);
  daisy_assert (dQ.size () == edge_size);

  //First all the internal edges...
  for (size_t e = 0; e < edge_size; e++)
    {
      if (geo.edge_is_internal (e))
        {
          const int from = geo.edge_from (e);
          const int to = geo.edge_to (e);  
          
          daisy_assert (from >= 0);
          daisy_assert (to >= 0);
          daisy_assert (static_cast<size_t>(from) < T.size ());
          daisy_assert (static_cast<size_t>(to) < T.size ());
          
          //--- Convective part ---
          const double alpha = (q_edge[e] >= 0) 
            ? upstream_weight 
            : 1.0 - upstream_weight;
          dQ[e] = water_heat_capacity * rho_water * 
            (alpha * T[from] + (1.0-alpha) * T[to]) * q_edge[e];
              
          //--- Conductive part - xx_zz --- 
          const double gradient = geo.edge_area_per_length (e) /
            geo.edge_area (e) * (T[to] - T[from]);    //mmo 20080708
          dQ[e] -= cond_edge[e]*gradient;  //xx_zz convection
        }
    } 
 
  //Then we take the boundaries (only if there are Dirichlet boundaries)...
  if (!isflux_lower | !isflux_upper)
    {
      //Calculate B_dir
      const size_t cell_size = geo.cell_size ();
      ublas::vector<double> B_dir (cell_size);
      B_dir = prod (A_before, T) - b_before;
      
      //Lower boundaries 
      if (!isflux_lower)   
        {           
          const std::vector<size_t>& edge_below
            = geo.cell_edges (Geometry::cell_below);
          const size_t edge_below_size = edge_below.size ();
          
          for (size_t i = 0; i < edge_below_size; i++)
            {
              const size_t edge = edge_below[i];
              const int cell = geo.edge_other (edge, Geometry::cell_below);
              daisy_assert (geo.cell_is_internal (cell));
              daisy_assert (cell >= 0);
              daisy_assert (static_cast<size_t>(cell) < T.size ());
              const double in_sign 
                = geo.cell_is_internal (geo.edge_to (edge)) ? 1.0 : -1.0;
              dQ[edge] = in_sign * B_dir (cell) / geo.edge_area (edge);
              //std::cout << "------  Lower boundary -----\n";
              //std::cout << "in_sign" << in_sign << '\n';
              //std::cout << "edge: " << edge << "dQ[edge]: " << dQ[edge] << '\n';
            }
        }    
      
      //Upper boundaries 
      if (!isflux_upper) 
        {
          const std::vector<size_t>& edge_above
            = geo.cell_edges (Geometry::cell_above);
          const size_t edge_above_size = edge_above.size ();
          
          for (size_t i = 0; i < edge_above_size; i++)
            {
              const size_t edge = edge_above[i];
              const int cell = geo.edge_other (edge, Geometry::cell_above);
              daisy_assert (geo.cell_is_internal (cell));
              daisy_assert (cell >= 0);
              daisy_assert (static_cast<size_t>(cell) < T.size ());
              const double in_sign 
                = geo.cell_is_internal (geo.edge_to (edge)) ? 1.0 : -1.0;
              dQ[edge] = in_sign * B_dir (cell) / geo.edge_area (edge);
              //std::cout << "------  Upper boundary -----\n";
              //std::cout << "in_sign" << in_sign << '\n';
              //std::cout << "edge: " << edge << "dQ[edge]: " << dQ[edge] << '\n';
            }
        }
    }
  //std::cout << "dQ-vector: " << dQ << '\n'; 
}
//-----------------------------------------------------------


struct HeatrectMollerup : public Heatrect
{
  // Content.
  const std::unique_ptr<Solver> solver;
  const int debug;
  
  // Use.
  void solve (const GeometryRect& geo,
              const std::vector<double>& q_water,
              const std::vector<double>& S_water,
              const std::vector<double>& S_heat,
              const std::vector<double>& capacity_new,
              const std::vector<double>& conductivity,
              const double T_top,
              const double T_top_new,
              const double T_bottom,
              std::vector<double>& T,
              const double dt, Treelog&) const;
  // Create.
  HeatrectMollerup (const BlockModel& al)
    : Heatrect (al),
      solver (Librarian::build_item<Solver> (al, "solver")),
      debug (al.integer ("debug"))
  { }
  ~HeatrectMollerup ()
  { }
};

void
HeatrectMollerup::solve (const GeometryRect& geo,
                         const std::vector<double>& q_water,
                         const std::vector<double>& S_water,
                         const std::vector<double>& S_heat,
                         const std::vector<double>& capacity_new,
                         const std::vector<double>& conductivity,
                         double T_top_old,      //mmo should not be constant
                         double T_top_new,      //mmo should not be constant
                         double T_bottom,       //mmo should not be constant 
                         std::vector<double>& T,
                         const double dt, Treelog& msg) const
{
  // mmo
  // Note S_h should be calculated correctly the value imported 
  // here is most probably only from heatsource like wires and 
  // not added/removed water
  //
  // S_h_water = S_water * water_heat_capacity * T 

  const size_t cell_size = geo.cell_size ();
  const size_t edge_size = geo.edge_size ();  
  
  // Solution old
  ublas::vector<double> T_old (cell_size);
  for (size_t c = 0; c < cell_size; c++)
    T_old (c) = T[c];
  ublas::vector<double> T_n (cell_size);  // Maybe not neccessary with both T_old and T_n
  T_n = T_old;

  // Mean temp at upper boundary
  double T_top_mean = 0.5 * (T_top_old + T_top_new);

  // Area (volume) Multiplied with heat capacity 
  ublas::banded_matrix<double> Q_Ch_mat_n (cell_size, cell_size, 0 ,0);
  for (size_t c = 0; c < cell_size; c++)
    Q_Ch_mat_n (c, c) = geo.cell_volume (c) * capacity_new[c];
  ublas::banded_matrix<double> Q_Ch_mat_np1 (cell_size, cell_size, 0 ,0);
  for (size_t c = 0; c < cell_size; c++)
    Q_Ch_mat_np1 (c, c) = geo.cell_volume (c) * capacity_new[c];
  
  // Flux in timestep
  ublas::vector<double> q_edge (edge_size);	
  for (size_t e = 0; e < edge_size; e++)
    q_edge (e) = q_water[e];
  
  //Convection
  //Equal weight: upstream_weight = 0.5
  //Upstr weight: upstream_weight = 1.0
  const double upstream_weight = 0.5;
  //const double upstream_weight = 1.0;

  Solver::Matrix convec (cell_size);
  convection (geo, upstream_weight, q_edge, convec);  

  //Conduction
  ublas::vector<double> cond_edge (edge_size); 
  cond_cell2edge (geo, conductivity, cond_edge);
  Solver::Matrix conduc (cell_size);
  conduction (geo, cond_edge, conduc);

  //Sink term
  ublas::vector<double> S_vol (cell_size); // sink term 
  for (size_t cell = 0; cell != cell_size ; ++cell) 
    S_vol (cell) = - geo.cell_volume (cell) *             //water 
      water_heat_capacity * rho_water *  S_water[cell]    //water
      - geo.cell_volume (cell) * S_heat[cell];            //electricity
  
  //Boundary vectors  
  ublas::vector<double> B_dir_vec = ublas::zero_vector<double> (cell_size);

  const bool isflux_lower = false;      //true;    //lower BC 
  const bool isflux_upper = false; //true;   //upper BC
  
  // Solver parameter , gamma
  // gamma = 0      : Backward Euler 
  // gamma = 0.5    : Crank - Nicholson
  const double gamma = 0.5;

  //Initialize A-matrix (left hand side)
  Solver::Matrix A (cell_size);  
  
  //Initialize b-vector (right hand side)
  ublas::vector<double> b (cell_size);   
  ublas::vector<double> b_before (cell_size);   
  Solver::Matrix b_mat (cell_size);  

  A = (1.0 / dt) * Q_Ch_mat_np1             // dT/dt
    - gamma * conduc                        // conduction
    + gamma * convec;                       // convection
  
  b_mat = (1.0 / dt) * Q_Ch_mat_n 
    + (1 - gamma) * conduc                  // conduction  
    - (1 - gamma) * convec;                 // convection
   
  b = prod (b_mat, T_n)
    - S_vol;                                // Sink term        
  
  
  if (debug > 0)
    {
      std::ostringstream tmp;
      //tmp << "A: \n" << A << '\n'
      //	  << "b_mat \n" << b_mat << '\n'
      //  << "b \n" << b << '\n';
      
      tmp << "T \n" << T_n << '\n';
      
      tmp << "T_top_new: \n" << T_top_new << '\n'
          << "T_top_old: \n" << T_top_old << '\n'
          << "T_top_mean: \n" << T_top_mean << '\n'
          << "T_bottom: \n" << T_bottom << '\n';
      
      msg.message (tmp.str ());
    }
  
  
  //Forced temperature in upper cell
  
  Solver::Matrix A_before = A;
  b_before = b;  //for computing 
  upperboundary (geo, isflux_upper, T_top_mean, A, b);
  lowerboundary (geo, isflux_lower, T_bottom, A, b);

  if (debug > 0)
    {
      std::ostringstream tmp;
      tmp << "T \n" << T_n << '\n';
      tmp << "T_top_new: \n" << T_top_new << '\n'
          << "T_top_old: \n" << T_top_old << '\n'
          << "T_top_mean: \n" << T_top_mean << '\n'
          << "T_bottom: \n" << T_bottom << '\n'
          << "A: \n" << A << '\n'
          << "b: \n" << b << '\n';
      msg.message (tmp.str ());
    }
  
  solver->solve (A, b, T_n); // Solve A T_n = b with regard to T_n.
  
  
  //New solution into T
  for (size_t c = 0; c < cell_size; c++)
    T[c] = T_n (c);
  
  //Calculate fluxes 
  ublas::vector<double> dQ = ublas::zero_vector<double> (edge_size);
  fluxes (geo, isflux_lower, isflux_upper, upstream_weight, q_edge, 
          cond_edge, T_n, A_before, b_before, dQ); 
  
  
}

static struct HeatrectMollerupSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new HeatrectMollerup (al); }

  HeatrectMollerupSyntax ()
    : DeclareModel (Heatrect::component, "Mollerup", "\
Finite volume solution to heat transfer by Mikkel Mollerup.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare_object ("solver", Solver::component, 
                       Attribute::Const, Attribute::Singleton, "\
Model used for solving matrix equation system.");
    frame.set ("solver", "cxsparse");
    frame.declare_integer ("debug", Attribute::Const, "\
Enable additional debug message.\n\
A value of 0 means no message, higher numbers means more messages.");
    frame.set ("debug", 0);
  }
} HeatrectMollerup_syntax;

// heatrect_mollerup.C ends here.
