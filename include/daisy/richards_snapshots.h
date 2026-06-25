// richards_snapshots.h
//
// General pre/post snapshot mechanism for the Richards solver state.
// Owns two named slots (pre, post) for each state variable (h, theta, S_sum)
// and the groundwater table scalar.
//
// Usage in tick_move:
//   snapshots_.snap_pre (*soil_water, *groundwater);   // just before movement->tick()
//   snapshots_.snap_post(*soil_water, *groundwater);   // just after  movement->tick()
//
// Usage in perturbation_tick:
//   snapshots_.restore_pre (*soil_water, *groundwater);   // reset to A
//   snapshots_.restore_post(*soil_water, *groundwater);   // restore to B (RAII guard)

#ifndef RICHARDS_SNAPSHOTS_H
#define RICHARDS_SNAPSHOTS_H

#include "daisy/soil/soil_water.h"
#include "daisy/lower_boundary/groundwater.h"

// ---------------------------------------------------------------------------
// One variable, two slots.
// ---------------------------------------------------------------------------
struct VarSnapshot
{
  std::vector<double> pre;   // snapshot A — captured just before Richards solve
  std::vector<double> post;  // snapshot B — captured just after  Richards solve
};

// ---------------------------------------------------------------------------
// Owns snapshots for all Richards state variables.
// Adding a new variable requires: one new VarSnapshot member + one line each
// in snap_arrays() and restore_arrays(). No other changes needed.
// ---------------------------------------------------------------------------
class RichardsSnapshots
{
  VarSnapshot h_;
  VarSnapshot theta_;
  VarSnapshot S_sum_;
  double      table_pre_  = 0.0;
  double      table_post_ = 0.0;

  // Pointer-to-member selects .pre or .post without duplicating loop bodies.
  using Slot = std::vector<double> VarSnapshot::*;

  void snap_arrays (Slot s, const SoilWater& sw)
  {
    const size_t n = sw.h_all ().size ();
    auto& vh = h_.*s;
    auto& vt = theta_.*s;
    auto& vs = S_sum_.*s;
    vh.resize (n);
    vt.resize (n);
    vs.resize (n);
    for (size_t i = 0; i < n; ++i)
      {
        vh[i] = sw.h     (i);
        vt[i] = sw.Theta (i);
        vs[i] = sw.S_sum (i);
      }
  }

  void restore_arrays (Slot s, SoilWater& sw) const
  {
    const auto& vh = h_.*s;
    const auto& vt = theta_.*s;
    const auto& vs = S_sum_.*s;
    for (size_t i = 0; i < vh.size (); ++i)
      sw.set_content (i, vh[i], vt[i]);
    sw.restore_S_sum (vs);
  }

public:
  // True after snap_pre has been called at least once.
  bool ready () const { return !h_.pre.empty (); }

  void snap_pre (const SoilWater& sw, const Groundwater& gw)
  {
    snap_arrays (&VarSnapshot::pre, sw);
    table_pre_ = gw.table ();
  }

  void snap_post (const SoilWater& sw, const Groundwater& gw)
  {
    snap_arrays (&VarSnapshot::post, sw);
    table_post_ = gw.table ();
  }

  void restore_pre (SoilWater& sw, Groundwater& gw) const
  {
    restore_arrays (&VarSnapshot::pre, sw);
    gw.set_table (table_pre_);
  }

  void restore_post (SoilWater& sw, Groundwater& gw) const
  {
    restore_arrays (&VarSnapshot::post, sw);
    gw.set_table (table_post_);
  }

  double table_pre  () const { return table_pre_;  }
  double table_post () const { return table_post_; }
};

#endif // RICHARDS_SNAPSHOTS_H
