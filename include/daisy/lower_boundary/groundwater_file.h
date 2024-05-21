#ifndef GROUNDWATER_FILE_H
#define GROUNDWATER_FILE_H

#include "daisy/lower_boundary/groundwater.h"
#include "object_model/block_model.h"
#include "util/lexer_data.h"
#include "util/assertion.h"
#include "daisy/daisy_time.h"
#include "daisy/timestep.h"
#include "object_model/librarian.h"
#include "util/path.h"
#include "object_model/frame.h"
#include <istream>

class GroundwaterFile : public Groundwater
{
  // Data.
private:
  Path& path;
  const double offset;
  Time previous_time;
  Time next_time;
  double previous_depth;
  double next_depth;
  double depth;

  // File.
  const symbol file_name;
  std::unique_ptr<std::istream> owned_stream;
  std::unique_ptr<LexerData> lex;
  
  // Groundwater.
public:
  bottom_t bottom_type () const;
  double q_bottom (size_t) const
  { daisy_notreached (); }

  // Simulation.
public:
  void tick (const Geometry&,
             const Soil&, SoilWater&, double, const SoilHeat&,
	     const Time& time, const Scope&, Treelog& msg)
  { tick (time, msg); }
  void tick (const Time&, Treelog&);
  double table () const;

  // Create and Destroy.
public:
  void initialize (const Geometry&, const Time& time, const Scope&, Treelog&);
  bool check (const Geometry&, const Scope&, Treelog&) const
  { return true; }
  GroundwaterFile (const BlockModel&);
  ~GroundwaterFile ();
};
#endif // GROUNDWATER_FILE_H
