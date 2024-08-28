#define BUILD_DLL

#include "daisy/lower_boundary/groundwater_aquitard.h"

GroundwaterAquitard::GroundwaterAquitard (const BlockModel& al)
    : Groundwater (al),
      K_aquitard (al.number ("K_aquitard")),
      Z_aquitard (al.number ("Z_aquitard")),
      pressure_table (al.check ("pressure_table")
                         ? Librarian::build_item<Depth> (al, "pressure_table")
                         : NULL),
      h_aquifer (al.number ("h_aquifer", Z_aquitard))
{
}

GroundwaterAquitard::~GroundwaterAquitard ()
{
}

void GroundwaterAquitard::tick (const Geometry& geo, const Soil&, SoilWater& soil_water, double,
                                const SoilHeat&, const Time& time,
                                const Scope& scope, Treelog& msg)
{
    // Virtual pressure table.
    pressure_table->tick (time, scope, msg);
    set_h_aquifer (geo);

    // Deep percolation.
    const std::vector<size_t>& bottom_edges
      = geo.cell_edges (Geometry::cell_below);
    const size_t bottom_edges_size = bottom_edges.size ();

    for (size_t i = 0; i < bottom_edges_size; i++)
    {
        const size_t edge = bottom_edges[i];
        const int cell = geo.edge_other (edge, Geometry::cell_below);
        daisy_assert (geo.cell_is_internal (cell));
        const double in_sign = (geo.edge_to (edge) == cell) ? 1.0 : -1.0;

        // Multiplied with 2 because it is a boundary cell...
        const double Dz_i = 2 * geo.edge_length (edge);

        const double K_i = soil_water.K_cell (cell);   //Conductivity in cell
        const double h_i = soil_water.h (cell);   //Pressure in cell

        const double numerator = K_i * (2.0 * h_i / Dz_i + 1.0)
          + K_aquitard * (h_aquifer / Z_aquitard - 1.0);
        const double denominator = K_aquitard + 2.0 * K_i * Z_aquitard / Dz_i;

        // Flux into domain.
        const double q_up = -K_aquitard * (numerator / denominator
                                           - h_aquifer / Z_aquitard + 1.0);
        edge_flux[edge] = in_sign * q_up;
    }
}

void GroundwaterAquitard::output (Log& log) const
{
    Groundwater::output (log);
    output_variable (h_aquifer, log);
}

void GroundwaterAquitard::initialize (const Geometry& geo, const Time& time,
                                      const Scope& scope, Treelog& msg)
{
    if (!pressure_table.get ())
        pressure_table.reset (Depth::create ((geo.bottom () - Z_aquitard)
                                             + h_aquifer));
    pressure_table->initialize (time, scope, msg);

    // Pressure below aquitard.
    if (pressure_table->check (scope, msg))
        set_h_aquifer (geo);
}

bool GroundwaterAquitard::check (const Geometry& geo, const Scope& scope,
                                 Treelog& msg) const
{
    bool ok = true;
    if (!pressure_table->check (scope, msg))
        ok = false;
    return ok;
}

double GroundwaterAquitard::table () const
{ return pressure_table->operator()(); }

GroundwaterAquitard::bottom_t GroundwaterAquitard::bottom_type() const
{ return forced_flux; }

double GroundwaterAquitard::q_bottom (size_t edge) const
{
    edge_flux_map::const_iterator i = edge_flux.find (edge);
    daisy_assert (i != edge_flux.end ());
    return (*i).second;
}

void GroundwaterAquitard::set_h_aquifer (const Geometry& geo)
{
    const double aquitard_bottom = geo.bottom () - Z_aquitard;
    h_aquifer = pressure_table->operator()() - aquitard_bottom;
}

static struct GroundwaterAquitardSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new GroundwaterAquitard (al); }

  GroundwaterAquitardSyntax ()
    : DeclareModel (Groundwater::component, "aquitard", "common", "\
Aquitard groundwater, free drainage.")
  { }
  void load_frame (Frame& frame) const
    { 
      frame.declare ("K_aquitard", "cm/h", Check::non_negative (), Attribute::Const,
		  "Conductivity of the aquitard.");
      frame.set ("K_aquitard", 1e-3);
      frame.declare ("Z_aquitard", "cm", Check::positive (), Attribute::Const,
		  "Thickness of the aquitard.\n\
The aquitard begins below the bottommost soil horizon.");
      frame.set ("Z_aquitard", 200.0);
      frame.declare ("h_aquifer", "cm", Check::positive (), Attribute::OptionalState,
		  "Pressure potential in the aquifer below the aquitard.\n\
By default. this is Z_aquitard.\n\
You can alternatively specify the pressure as a virtual groundwater level.\n\
See 'pressure_table'.");
      frame.declare_object ("pressure_table", Depth::component,
                         Attribute::OptionalConst, Attribute::Singleton, "\
Height of groundwater the corresponds to the pressure in the aquifer.  \n\
\n\
If you drilled a well down to the aquifer, this is number what the\n\
water level in the well would be as height above ground (a negative\n\
number).  This is different from the actual groundwater table, because\n\
the aquitart block the water, and the pipes lead the water away.\n\
You can alternatively specify the pressure directly, with 'h_aquifer'.");
    }
} GroundwaterAquitard_syntax;

// groundwater_aquitard.C ends here.
