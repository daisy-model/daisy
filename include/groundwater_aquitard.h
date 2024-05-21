#ifndef GROUNDWATER_AQUITARD_H
#define GROUNDWATER_AQUITARD_H

#include <map>
#include <memory>

#include "assertion.h"
#include "block_model.h"
#include "check.h"
#include "depth.h"
#include "frame.h"
#include "geometry.h"
#include "groundwater.h"
#include "librarian.h"
#include "log.h"
#include "soil_water.h"
#include "treelog.h"

struct GroundwaterAquitard : public Groundwater
{
public:
    GroundwaterAquitard (const BlockModel& al);
    ~GroundwaterAquitard ();

    void tick (const Geometry& geo, const Soil&, SoilWater& soil_water, double,
              const SoilHeat&, const Time& time,
              const Scope& scope, Treelog& msg);
    void output (Log& log) const;
    void initialize (const Geometry& geo, const Time& time,
                    const Scope& scope, Treelog& msg);
    bool check (const Geometry& geo, const Scope& scope,
               Treelog& msg) const;
    double table () const;
    bottom_t bottom_type () const;
    double q_bottom (size_t edge) const;

private:
    const double K_aquitard;   // Conductivity of the aquitard. [cm h^-1]
    const double Z_aquitard;   // Vertical length of the aquitard. [cm]
    std::unique_ptr<Depth> pressure_table; // Virtual groundwater height. [cm]
    double h_aquifer;          // Pressure potential in the aquifer [cm]

    typedef std::map<size_t, double> edge_flux_map;
    edge_flux_map edge_flux;

    void set_h_aquifer (const Geometry& geo);
};

#endif // GROUNDWATERAQUITARD_H
