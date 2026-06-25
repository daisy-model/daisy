// groundwater_api.C --- Groundwater table driven from Python/BMI coupling.
//
// Copyright 2024 Daisy contributors.
//
// This file is part of Daisy.
//
// Daisy is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your
// option) any later version.
//
// Daisy is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser Public License for
// more details.

#define BUILD_DLL

#include "daisy/lower_boundary/groundwater.h"
#include "object_model/block_model.h"
#include "object_model/librarian.h"
#include "object_model/frame.h"
#include "object_model/check.h"
#include "util/assertion.h"

// GroundwaterApi: a pressure-type lower boundary whose table depth is
// written each coupling step by DaisyController::set_groundwater_depth().
class GroundwaterApi : public Groundwater
{
  double table_;   // [cm], negative = below surface (Daisy convention)

public:
  // --- Groundwater interface ---

  bottom_t bottom_type () const
  { return pressure; }

  // q_bottom is computed by the Richards solver; we never prescribe it.
  double q_bottom (size_t) const
  { daisy_notreached (); }

  double table () const
  { return table_; }

  void set_table (double cm)
  { table_ = cm; }

  // --- Simulation ---

  void tick (const Geometry&, const Soil&, SoilWater&, double /*h_surface*/,
             const SoilHeat&, const Time&, const Scope&, Treelog&)
  { /* table_ is set externally before each tick; nothing to do here */ }

  void output (Log&) const
  { /* nothing extra to log */ }

  // --- Create and Destroy ---

  void initialize (const Geometry&, const Time&, const Scope&, Treelog&)
  { /* nothing to initialise */ }

  bool check (const Geometry&, const Scope&, Treelog&) const
  { return true; }

  explicit GroundwaterApi (const BlockModel& al)
    : Groundwater (al),
      table_ (al.number ("initial_table", -100.0))
  { }

  ~GroundwaterApi ()
  { }
};

static struct GroundwaterApiSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new GroundwaterApi (al); }

  GroundwaterApiSyntax ()
    : DeclareModel (Groundwater::component, "api", "common",
                    "Groundwater table set externally from Python/BMI coupling.\n"
                    "Use DaisyController::set_groundwater_depth() each\n"
                    "timestep before calling tick().")
  { }

  void load_frame (Frame& frame) const
  {
    frame.declare ("initial_table", "cm", Check::none (), Attribute::OptionalConst,
                   "Initial groundwater depth [cm, negative = below surface].\n"
                   "Default: -100 cm (1 m below surface).");
    frame.set ("initial_table", -100.0);
  }
} GroundwaterApi_syntax;
