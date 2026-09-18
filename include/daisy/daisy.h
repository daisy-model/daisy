// daisy.h
// 
// Copyright 1996-2001 Per Abrahamsen and Søren Hansen
// Copyright 2000-2001 KVL.
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


#ifndef DAISY_H
#define DAISY_H

#include "programs/program.h"
#include <vector>
#include <memory>

class Harvest;
class Log;
class Field;
class Treelog;
class Time;
class Units;
class Scope;
class Frame;
class FrameModel;

#ifdef __unix
#define DAISY_EXPORT /* nothing */
#elif defined (BUILD_DLL)
#define DAISY_EXPORT __declspec(dllexport)
#else
#define DAISY_EXPORT __declspec(dllimport)
#endif

class DAISY_EXPORT Daisy : public Program
{
public:
  static const char *const default_description;

  // Content.
private:
  class Implementation;
  const std::unique_ptr<Implementation> impl;
public:
  const FrameModel& frame () const;
  const std::vector<const Scope*>& scopes () const;
  const Time& time () const;
  const Time& previous () const;
  const Units& units () const;
  Field& field () const;
  std::vector<const Harvest*>& harvest () const;
  void start ();
  bool is_running () const;
  void stop ();

  // Python/BMI coupling: forwarding to first (or pos-th) column.
  double get_groundwater_table (unsigned int pos = 0u) const; // [cm]
  void   set_groundwater_table (double cm, unsigned int pos = 0u);
  /** Hours from simulation start to the configured stop time, or -1 if open-ended. */
  double stop_duration_hours() const;

  double              get_bottom_flux      (unsigned int pos = 0u) const; // [cm/h]
  std::vector<double> get_flux_array       (unsigned int pos = 0u) const; // [cm/h]
  std::vector<double> get_h_array          (unsigned int pos = 0u) const; // [cm]
  std::vector<double> get_theta_array      (unsigned int pos = 0u) const; // [-]
  std::vector<double> get_theta_sat_array  (unsigned int pos = 0u) const; // [-]
  double              get_runoff_rate      (unsigned int pos = 0u) const; // [mm/day]
  double              get_column_area      (unsigned int pos = 0u) const; // [cm²]
  std::vector<double> get_layer_tops       (unsigned int pos = 0u) const; // [cm]
  std::vector<double> get_layer_bottoms    (unsigned int pos = 0u) const; // [cm]

  // UI.
public:
  void attach_ui (Run* run, const std::vector<Log*>& logs);

  // Simulation.
public:
  bool run (Treelog&);
  void tick (Treelog&);
  void summarize (Treelog&) const;
  void close_output ();
  void output (Log&) const;

  // Create and Destroy.
public:
  void initialize (Block&);
  bool check (Treelog&);
  static void load_syntax (Frame&);
  explicit Daisy (const BlockModel&);
  ~Daisy ();
};

#endif // DAISY_H

