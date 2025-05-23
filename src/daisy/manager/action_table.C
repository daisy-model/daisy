// action_table.C
// 
// Copyright 2005 Per Abrahamsen and KVL.
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

#include "daisy/manager/action.h"
#include "object_model/metalib.h"
#include "object_model/library.h"
#include "daisy/daisy.h"
#include "daisy/field.h"
#include "daisy/organic_matter/am.h"
#include "daisy/chemicals/im.h"
#include "object_model/units.h"
#include "util/lexer_table.h"
#include "util/mathlib.h"
#include "object_model/librarian.h"
#include "object_model/treelog.h"
#include "object_model/frame_model.h"
#include "daisy/soil/transport/volume.h"
#include "object_model/check.h"
#include "daisy/daisy_time.h"
#include <set>
#include <map>
#include <memory>
#include <sstream>

struct ActionTable : public Action
{
  const Metalib& metalib;
  const std::unique_ptr<Action> sow;
  const std::unique_ptr<Action> emerge;
  const std::unique_ptr<Action> harvest;
  const std::unique_ptr<FrameModel> am;

  std::set<Time> sow_dates;
  std::set<Time> emerge_dates;
  std::set<Time> harvest_dates;
  std::map<Time, double> fertilize_events;
  std::map<Time, double> irrigate_events;
  std::map<Time, const FrameModel*> fertilizers;
  const double flux; // [mm/h]
  const boost::shared_ptr<Volume> volume;

  static void read_model (const LexerTable&,
                          const std::vector<std::string>& entries, 
                          int tag_c, const Library& library,
                          std::map<Time, const FrameModel*>& alists);
  static void read_event (const LexerTable&,
                          const std::vector<std::string>& entries, 
                          int tag_c, std::map<Time, double>& events);
  static void read_date (const LexerTable&,
                         const std::vector<std::string>& entries, 
                         int rag_c, std::set<Time>& dates);

  void doIt (Daisy& daisy, const Scope&, Treelog& msg);
  void tick (const Daisy&, const Scope&, Treelog&);
  void initialize (const Daisy&, const Scope&, Treelog&);
  bool check (const Daisy&, const Scope&, Treelog& err) const;
  ActionTable (const BlockModel& al);
};

void 
ActionTable::read_event (const LexerTable& lex,
                         const std::vector<std::string>& entries, 
                         int tag_c, std::map<Time, double>& events)
{
  if (tag_c < 0)
    return;

  const std::string val = entries[tag_c];

  if (lex.is_missing (val))
    return;

  Time time (9999, 1, 1, 0);
  if (!lex.get_time_dh (entries, time, 8))
    return;

  const double value = lex.convert_to_double (val);
  events[time] = value;
}

void 
ActionTable::read_model (const LexerTable& lex,
                         const std::vector<std::string>& entries, 
                         int tag_c, const Library& library,
                         std::map<Time, const FrameModel*>& alists)
{
  if (tag_c < 0)
    return;

  const std::string val = entries[tag_c];

  if (lex.is_missing (val))
    return;

  if (!library.check (symbol (val)))
    {
      lex.error ("'" + val + "' undefined");
      return;
    }
  Time time (9999, 1, 1, 0);
  if (!lex.get_time_dh (entries, time, 8))
    return;

  alists[time] = &library.model (symbol (val));
}

void 
ActionTable::read_date (const LexerTable& lex,
                        const std::vector<std::string>& entries, 
                        int tag_c, std::set<Time>& dates)
{
  if (tag_c < 0)
    return;

  const std::string val = entries[tag_c];

  if (lex.is_missing (val))
    return;

  Time time (9999, 1, 1, 0);
  if (!LexerTable::get_time_dh (val, time, 8))
    return;
  
  dates.insert (time);
}

void 
ActionTable::doIt (Daisy& daisy, const Scope& scope, Treelog& msg)
{ 
  // Units.
  const Units& units = daisy.units ();
  static const symbol mg_per_square_m ("mg/m^2");
  const Unit& u_mg_per_square_m = units.get_unit (mg_per_square_m);
  const Unit& u_ppm = units.get_unit (Units::ppm ());
  const Unit& u_per_mm = units.get_unit (Units::per_mm ());
  const Unit& u_solute = units.get_unit (IM::solute_unit ());

  if (sow.get () && sow_dates.find (daisy.time ()) != sow_dates.end ())
    sow->doIt (daisy, scope, msg);
  if (emerge.get () && emerge_dates.find (daisy.time ()) != emerge_dates.end ())
    emerge->doIt (daisy, scope, msg);
  if (harvest.get () 
      && harvest_dates.find (daisy.time ()) != harvest_dates.end ())
    harvest->doIt (daisy, scope, msg);
  if ((am.get () 
       || fertilizers.find (daisy.time ()) != fertilizers.end ())
      && fertilize_events.find (daisy.time ()) != fertilize_events.end ())
    {
      FrameModel fert (((fertilizers.find (daisy.time ()) != fertilizers.end ())
                        ? *fertilizers[daisy.time ()] : *am), Frame::parent_link);

      AM::set_utilized_weight (metalib, fert, fertilize_events[daisy.time ()]);
      if (irrigate_events.find (daisy.time ()) != irrigate_events.end ())
        {
          double value = irrigate_events[daisy.time ()];
          std::ostringstream tmp;
          if (iszero (value))
            {
              tmp << "Applying minimum of 0.1 mm\n";
              value = 0.1;
            }
          IM im = AM::get_IM (metalib, u_mg_per_square_m, fert);
	  daisy_assert (std::isnormal (value));
	  im.multiply_assign (Scalar (1.0 / value, u_per_mm), u_ppm);
          daisy.field ().irrigate (value/flux, flux, 
                                 Irrigation::at_air_temperature,
                                 Irrigation::subsoil, 
                                 im, volume, false, msg);
          tmp << "Fertigating " << value << " mm, with";
	  for (IM::const_iterator i = im.begin (); i != im.end (); i++)
	    {
	      const symbol chem = *i;
	      const double value = im.get_value (chem, u_ppm);
	      if (std::isnormal (value))
		tmp << " " << value << " ppm " << chem;
	    }
          msg.message (tmp.str ());
        }
      else
        {
          double water = 0.0;
          std::ostringstream tmp;
          if (AM::is_mineral (metalib, fert))
            tmp << "Fertilizing " << fert.number ("weight") 
                << " kg "<< fert.type_name () << "-N/ha";
          else if (AM::is_organic (metalib, fert))
            {
              tmp  << "Fertilizing " << fert.number ("weight") 
                   << " ton "<< fert.type_name () << " ww/ha";
              const double utilized_weight 
                = AM::utilized_weight (metalib, fert);
              if (utilized_weight > 0.0)
                tmp << "; utilized " << utilized_weight << " kg N/ha";
              water = AM::get_water (metalib, fert);
              if (water > 0.0)
                tmp << "; water " << water << " mm";
            }
          else
            tmp << "Fertilizing " << fert.type_name ();
          msg.message (tmp.str ());
          daisy.field ().fertilize (metalib, fert, 
                                  daisy.time (), msg);
          if (water > 0.0)
            daisy.field ().irrigate (water/flux, flux, 
                                   Irrigation::at_air_temperature,
                                   Irrigation::surface, IM (u_solute),
                                   boost::shared_ptr<Volume> (), false, msg);  
        }
    }
  else if (irrigate_events.find (daisy.time ()) != irrigate_events.end ())
    {
      const double value = irrigate_events[daisy.time ()];
      std::ostringstream tmp;
      IM im;
      daisy.field ().irrigate (value/flux, flux, 
                             Irrigation::at_air_temperature,
                             Irrigation::overhead, im,
                             boost::shared_ptr<Volume> (), false, msg);  
      tmp << "Irrigating " << value << " mm";
      msg.message (tmp.str ());
    }
}

void 
ActionTable::tick (const Daisy& daisy, const Scope& scope, Treelog& msg)
{ 
  if (sow.get ())
    sow->tick (daisy, scope, msg);
  if (emerge.get ())
    emerge->tick (daisy, scope, msg);
  if (harvest.get ())
    harvest->tick (daisy, scope, msg);
}

void 
ActionTable::initialize (const Daisy& daisy, const Scope& scope, Treelog& msg)
{ 
  if (sow.get ())
    sow->initialize (daisy, scope, msg);
  if (emerge.get ())
    emerge->initialize (daisy, scope, msg);
  if (harvest.get ())
    harvest->initialize (daisy, scope, msg);
}

bool 
ActionTable::check (const Daisy& daisy, const Scope& scope, Treelog& msg) const
{
  bool ok = true;
  if (sow.get () && ! sow->check (daisy, scope, msg))
    ok = false;
  if (emerge.get () && ! emerge->check (daisy, scope, msg))
    ok = false;
  if (harvest.get () && ! harvest->check (daisy, scope, msg))
    ok = false;
  return ok;
}

ActionTable::ActionTable (const BlockModel& al)
  : Action (al),
    metalib (al.metalib ()),
    sow (al.check ("sow") 
         ? Librarian::build_item<Action> (al, "sow")
         : NULL),
    emerge (al.check ("emerge") 
         ? Librarian::build_item<Action> (al, "emerge")
         : NULL),
    harvest (al.check ("harvest") 
             ? Librarian::build_item<Action> (al, "harvest")
             : NULL),
    am (al.check ("fertilizer") 
        ? &al.model ("fertilizer").clone ()
        : NULL),
    flux (al.number ("flux")),
    volume (Volume::build_obsolete (al).release ())
{ 
  LexerTable lex (al);
  if (!lex.read_header (al.msg ()))
    {
      al.error ("Read failed");
      return;
    }
  const int harvest_c = harvest.get () ? lex.find_tag ("Harvest") : -1;
  const int sow_c = sow.get () ? lex.find_tag ("Planting") : -1;
  const int emerge_c = emerge.get () ? lex.find_tag ("Emerging") : -1;
  const int irrigate_c = al.flag ("enable_irrigation") 
    ? lex.find_tag ("Irrigate") : -1;
  const int fertilizer_c = lex.find_tag ("Fertilizer");
  const int fertilize_c = (al.flag ("enable_fertilization") 
                           && (am.get () || fertilizer_c >= 0))
    ? lex.find_tag ("Fertilize") : -1;
  
  if (sow_c < 0 && emerge_c < 0 && harvest_c < 0 
      && irrigate_c < 0 && fertilize_c < 0)
    al.msg ().warning ("No applicable column found");

  if (sow_c < 0 && sow.get ())
    al.msg ().warning ("Specified planting operation not used");
  if (emerge_c < 0 && emerge.get ())
    al.msg ().warning ("Specified emerge operation not used");
  if (harvest_c < 0 && harvest.get ())
    al.msg ().warning ("Specified harvest operation not used");
  if (fertilize_c < 0 && am.get ())
    al.msg ().warning ("Specified fertilizer not used");
  if (fertilizer_c >= 0 && am.get ())
    al.msg ().warning ("Fertilizer specified twice");

  while (lex.good ())
    {
      // Read entries.
      std::vector<std::string> entries;
      if (!lex.get_entries (entries))
        continue;

      read_event (lex, entries, irrigate_c, irrigate_events);
      read_event (lex, entries, fertilize_c, fertilize_events);
      read_model (lex, entries, fertilizer_c, 
                  al.metalib ().library (AM::component),
                  fertilizers);
      read_date (lex, entries, sow_c, sow_dates);
      read_date (lex, entries, emerge_c, emerge_dates);
      read_date (lex, entries, harvest_c, harvest_dates);
    }

  if (sow_dates.size () == 0 
      && emerge_dates.size () == 0 
      && harvest_dates.size () == 0
      && fertilize_events.size () == 0
      && irrigate_events.size () == 0)
    al.msg ().warning ("Nothing to do");
}

// Add the ActionTable syntax to the syntax table.
static struct ActionTableSyntax : DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new ActionTable (al); }

  ActionTableSyntax ()
    : DeclareModel (Action::component, "table", "\
Read management actions from a Daisy data file.\n\
\n\
After the ddf header, the following column tags are recognized (with\n\
the dimension for the dimension linein square brackets).\n\
\n\
Date [date]: The date for fertilization or irrigation.\n\
\n\
Planting [date]: The content should be a date in yyyy-mm-dd format,\n\
where the operation specified by the 'sow' attribute will be perfomed.\n\
\n\
Emerging [date]: The content should be a date in yyyy-mm-dd format,\n\
where the operation specified by the 'emerge' attribute will be perfomed.\n\
\n\
Harvest [date]: The content should be a date in yyyy-mm-dd format,\n\
where the operation specified by the 'harvest' attribute will be\n\
perfomed.\n\
\n\
Irrigate [mm]: The content should be an irrigation amount, that will\n\
be applied as overhead irrigation for the date specified in the 'Date'\n\
field.  You can disable it with the 'enable_irrigation' attribute.\n\
\n\
Fertilize [kg N/ha]: The content should be an amount of nitrogen\n\
fertilizer to be applied on the date specified in the 'Date' field.\n\
The fertilizer type will be either the one specified in the\n\
'Fertilizer' column, or the 'fertilizer' attribute.  You can disable\n\
it with the 'enable_fertilization' attribute.\n\
\n\
Fertilizer [name]: The type of fertilizer to be applied.")
  { }
  static bool check_alist (const Metalib&, const Frame& al, Treelog& err)
  { 
    bool ok = true;
    if (al.check ("from") && al.check ("to"))
      {
        const double from = al.number ("from");
        const double to = al.number ("to");
        if (from <= to)
          {
            err.entry ("'from' must be higher than 'to' in"
                       " the subsoil irrigation zone");
            ok = false;
          }
      }
    return ok;
  }
  void load_frame (Frame& frame) const
  { 
    frame.add_check (check_alist);	
    LexerTable::load_syntax (frame);
    frame.declare_object ("sow", Action::component, 
                       Attribute::OptionalConst, Attribute::Singleton, 
                       "Sow action.");
    frame.declare_object ("emerge", Action::component, 
                       Attribute::OptionalConst, Attribute::Singleton, 
                       "Emerge action.");
    frame.declare_object ("harvest", Action::component, 
                       Attribute::OptionalConst, Attribute::Singleton, 
                       "Harvest action.");
    frame.declare_object ("fertilizer", AM::component,
                       Attribute::OptionalConst, Attribute::Singleton, "\
The fertilizer you want to apply.");
    frame.declare_boolean ("enable_irrigation", Attribute::Const, "\
Set this to false to ignore any irrigation information in the file.");
    frame.set ("enable_irrigation", true);
    frame.declare_boolean ("enable_fertilization", Attribute::Const, "\
Set this to false to ignore any fertilization information in the file.");
    frame.set ("enable_fertilization", true);
    frame.declare ("flux", "mm/h", Check::positive (), Attribute::Const,
                   "Water application speed.");
    frame.set ("flux", 2.0);
    frame.declare_object ("volume", Volume::component, 
                       Attribute::Const, Attribute::Singleton,
                       "Soil volume to add irritaion.");
    frame.set ("volume", "box");
    frame.declare ("from", "cm", Check::non_positive (), Attribute::Const, "\
Height where you want to start the incorporation (a negative number).\n\
OBSOLETE: Use (volume box (top FROM)) instead.");
    frame.set ("from", -5.0);
    frame.declare ("to", "cm", Check::non_positive (), Attribute::Const, "\
Height where you want to end the incorporation (a negative number).\n\
OBSOLETE: Use (volume box (bottom TO)) instead.");
    frame.set ("to", -25.0);
  }
} ActionTable_syntax;

// action_table.C ends here.
