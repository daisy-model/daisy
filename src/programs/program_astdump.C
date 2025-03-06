// program_astdump.C -- Print abstract syntax tree.
// 
// Copyright 2002, 2005 Per Abrahamsen and KVL.
// Copyright 1996-2001 Per Abrahamsen and Søren Hansen
// Copyright 2000-2001 KVL
// Copyright 2025 KU
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

#include "programs/program.h"
#include "object_model/library.h"
#include "object_model/metalib.h"
#include "object_model/block_model.h"
#include "object_model/printer_file.h"
#include "object_model/xref.h"
#include "object_model/plf.h"
#include "util/format.h"
#include "object_model/treelog.h"
#include "util/assertion.h"
#include "object_model/librarian.h"
#include "object_model/frame_submodel.h"
#include "object_model/frame_model.h"
#include "util/filepos.h"
#include "object_model/function.h"
#include "object_model/version.h"
#include <sstream>
#include <fstream>
#include <memory>
#include <algorithm>

struct ProgramASTdump : public Program
{
  // Content.
  const Metalib& metalib;
  const symbol where;

  struct PrettyPrint
  {
    int indent;
    std::ofstream out;
    void print (const std::string& str)
    {
      for (int i = 0; i < indent * 2; i++)
	out << " ";
      out << str << "\n";
    }
    void print (const symbol name)
    { print (name.name ()); }
    void print (const char *const name)
    { print (std::string (name)); }
    PrettyPrint (const symbol where)
      : indent (0),
	out (where.name ().c_str ())
    { }
  } pp;
  struct Nest
  {
    PrettyPrint & pp;
    Nest (PrettyPrint& pp_, const symbol a, const symbol b)
      : pp (pp_)
    {
      pp.print ("(" + quote_string (a.name ())
		+ " " + quote_string (b.name ()));
      pp.indent += 1;
    }
    Nest (PrettyPrint& pp_, const symbol a)
      : pp (pp_)
    {
      pp.print ("(" + quote_string (a.name ()));
      pp.indent += 1;
    }
    ~Nest ()
    {
      pp.indent -= 1;
      pp.print (")");
    }
  };
    
    
  // remember this for models.
  symbol current_component;
  static const symbol root_name;

  // Private functions.

  static std::string quote_string  (const std::string& raw)
  {
    std::string result;
    for (char c : raw)
      switch (c)
	{
	case '\n':
	  result.append ("\\n");
	  break;
	case '"':
	  result.append ("\\\"");
	  break;
	default:
	  result.append (1, c);
	}
    if (raw.find (' ') != std::string::npos)
      result = "\"" + result + "\"";
    return result;
  }
  void print_string (const symbol);
  void print_description (const symbol description)
  { print_attribute ("description", description); }

  void print_attribute (const symbol name, const symbol value)
  { print_string ("(" + quote_string (name.name ())
		  + " " + quote_string (value.name ()) + ")"); }
  void print_attribute (const symbol name, const bool value)
  { print_attribute (name, symbol (value ? "true" : "false")); }

  // Document functions.
  void print_entry_type (const symbol name, 
			 const Frame& frame);
  void print_entry_submodel (const symbol name, 
			     const Frame& frame);
  void print_entry_category (const symbol name, 
			     const Frame& frame);
  void print_entry_value (const symbol name, 
			  const Frame& frame);

  void print_users (const XRef::Users&);

  // Print parts of it.
  static void own_entries (const Metalib&,
                           const Library& library, const symbol name, 
                           std::set<symbol>& entries, 
                           bool new_only = false);
  static void inherited_entries (const Metalib&, const Library& library,
                                 const symbol name, 
                                 std::set<symbol>& entries);
  void print_submodel (const symbol name, const Frame& frame);
  void print_submodel_entries (const Frame& frame,
                               const std::set<symbol>& entries);
  void print_submodel_entry (const symbol, 
                             const Frame& frame);
  void print_model (symbol name, const Library& library, Treelog&);
  void print_fixed (const symbol name, 
		    const Frame& frame,
                    const symbol description);
  void print_component (const Library& library, Treelog& msg);

  // Print it.
  void print_document (Treelog&);

  // Program.
  bool run (Treelog& msg)
  {
    print_document (msg); 
    return true;
  }

  // Create and Destroy.
  void initialize (Block&)
  { };
  bool check (Treelog&)
  { return true; }
  ProgramASTdump (const BlockModel& al)
    : Program (al),
      metalib (al.metalib ()),
      where (al.name ("where")),
      pp (where)
  { }
  ~ProgramASTdump ()
  { }
};

const symbol 
ProgramASTdump::root_name ("component");

void
ProgramASTdump::print_string (const symbol name)
{ pp.print (name); }

void 
ProgramASTdump::print_entry_type (const symbol name,
				  const Frame& frame)
{
  const Attribute::type type = frame.lookup (name);

  print_attribute ("type", Attribute::type_name (type));
  switch (type)
    {
    case Attribute::Number:
      print_attribute ("unit", frame.dimension (name));
      break;
    case Attribute::Submodel:
      print_attribute ("submodel", frame.submodel_name (name));
      break;
    case Attribute::PLF:
    case Attribute::Function:
      print_attribute ("domain", frame.domain (name));
      print_attribute ("range", frame.range (name));
      break;
    case Attribute::Boolean:
    case Attribute::String:
    case Attribute::Integer:
      break;
    case Attribute::Model:
      print_attribute ("name", frame.component (name));
      break;
    case Attribute::Scalar:
    case Attribute::Reference:
    case Attribute::Error:
    default:
      daisy_panic ("Unknown entry '" + name + "'");
    };
}

void 
ProgramASTdump::print_entry_submodel (const symbol name, 
				      const Frame& frame)
{
  const Attribute::type type = frame.lookup (name);
  if (type == Attribute::Submodel)
    {
      const FrameSubmodel& child = frame.submodel (name);
      if (frame.submodel_name (name) == Attribute::None ())
	{
	  print_submodel (name, child);
	}
    }
}
    
void 
ProgramASTdump::print_entry_category (const symbol name, 
				      const Frame& frame)
{
  print_attribute ("optional", frame.is_optional (name));
  
  // Models and Submodels don't have categories.
  const Attribute::type type = frame.lookup (name);
  if (type == Attribute::Model || type == Attribute::Submodel)
    // Models and Submodels don't have categories.
    return;

  print_attribute ("const", frame.is_const (name));
  print_attribute ("state", frame.is_state (name));
  print_attribute ("log", frame.is_log (name));
}

void 
ProgramASTdump::print_entry_value (const symbol name, const Frame& frame)
{ print_attribute ("has_default", frame.check (name)); }

void
ProgramASTdump::own_entries (const Metalib& metalib,
			     const Library& library, const symbol name, 
			     std::set<symbol>& entries,
			     const bool new_only)
{
  const FrameModel& frame = library.model (name);

  frame.entries (entries);

  // Remove base entries.
  const symbol base_model = frame.base_name ();
  if (base_model != Attribute::None ())
    {
      daisy_assert (base_model != name);

      const FrameModel& base_frame = library.model (base_model);
      std::set<symbol> base_entries;
      base_frame.entries (base_entries);
      for (std::set<symbol>::const_iterator i = base_entries.begin (); 
           i != base_entries.end (); 
           i++)
        {
          const symbol key = *i;

          if (new_only
              || key == "description"
              || key == "cite"
              || frame.subset (metalib, base_frame, key))
            {
              const std::set<symbol>::iterator j 
                = find (entries.begin (), entries.end (), key);
              if (j != entries.end ())
                entries.erase (j);
            }
        }
    }
}

void
ProgramASTdump::inherited_entries (const Metalib& metalib,
				   const Library& library, const symbol name, 
				   std::set<symbol>& entries)
{
  const FrameModel& frame = library.model (name);

  const symbol base_model = frame.base_name ();

  if (base_model != Attribute::None ())
    {
      const FrameModel& base_frame = library.model (base_model);
      base_frame.entries (entries);
      for (std::set<symbol>::const_iterator i = entries.begin ();
           i != entries.end (); )
        {
          const symbol key = *i;
          if (key != "description" 
              && !frame.subset (metalib, base_frame, key))
	    i = entries.erase (find (entries.begin (), entries.end (), key));
	  else
	    ++i;
        }
    }
}


void 
ProgramASTdump::print_submodel (const symbol name, const Frame& frame)
{
  std::set<symbol> entries;
  frame.entries (entries);
  print_submodel_entries (frame, entries);
}

void 
ProgramASTdump::print_submodel_entries (const Frame& frame,
					const std::set<symbol>& entries)
{
  Nest nest (pp, "entries");      

  const std::vector<symbol>& order = frame.order ();

  if (order.size () > 0)
    {
      std::string result;
      for (auto s:  order)
	result += " " + quote_string (s.name ());
      print_string ("(order" + result + ")");
    }
  for (std::set<symbol>::const_iterator i = entries.begin (); 
       i != entries.end (); 
       i++)
    print_submodel_entry (*i, frame);
}

void 
ProgramASTdump::print_submodel_entry (const symbol name, 
				      const Frame& frame)
{
  Nest nest (pp, "entry", name);
  // Print type.
  print_entry_type (name, frame);

  // Print size.
  const int size = frame.type_size (name);
  print_attribute ("size",
		   (size >= 0 ?
		    std::to_string (size)
		    : Attribute::size_name (size)));


  if (!frame.is_log (name))
    {
      // Print category.
      print_entry_category (name, frame);

      // Print value.
      if (name != "description")
        print_entry_value (name, frame);
    }

  // Print description line.
  const symbol description = frame.description (name);
  if (description != Attribute::Unknown ())
      print_description (description);

  // print submodel entries, if applicable
  print_entry_submodel (name, frame);
}


void
ProgramASTdump::print_model (const symbol name, const Library& library,
			     Treelog& msg)
{
  Nest nest (pp, "model", name);      

  const FrameModel& frame = library.model (name);


  const symbol type = frame.base_name ();
   static const symbol function_name (Function::component);
  print_attribute ("buildable", frame.buildable ());

  if (library.name () == function_name)
    {
      print_attribute ("domain", frame.name ("domain"));
      print_attribute ("range", frame.name ("range"));
      
    }
  print_attribute ("super", type);
  print_attribute ("buildable", frame.buildable ());
  const Filepos& pos = frame.own_position ();
  print_attribute ("defined", pos == Filepos::none ()
		   ? "builtin"
		   : pos.filename ());

  std::set<symbol> entries;
  own_entries (metalib, library, name, entries, true);
  if (entries.size () > 0)
    {
      print_submodel_entries (frame, entries);
    }
}

void
ProgramASTdump::print_fixed (const symbol name, 
			     const Frame& frame,
			     const symbol description)
{
  Nest nest (pp, "fixed", name);

  // Print description, if any.
  print_description (description);

  print_submodel (name, frame);
}

class ModelCompare
{ 
  const Library& library;

  const symbol find_next_in_line (const symbol root, const symbol leaf) const
  {
    // Find the child of root that leaf is descended from.
    daisy_assert (root != leaf);
    const FrameModel& al = library.model (leaf);
    const symbol type = al.base_name ();
    if (type == root)
      return leaf;
    
    return find_next_in_line (root, type);
  }

public:
  bool operator() (const symbol a, const symbol b) const
  {
    // They may be the same.
    if (a == b)
      return false;

    if (!library.check (a))
      return false;
    if (!library.check (b))
      return false;

    // One may be a derivative of the other.  Sort base first.
    if (library.is_derived_from (a, b))
      return false;
    if (library.is_derived_from (b, a))
      return true;

    symbol base_a = library.base_model (a);
    symbol base_b = library.base_model (b);
    
    // They may be otherwise related.
    while (base_a == base_b)
      {
	// Find place where tree branches,
	base_a = find_next_in_line (base_a, a);
	base_b = find_next_in_line (base_b, b);
      }
    // Unrelated, sort according to their base classes.
    return  base_a.name () < base_b.name ();
  }
  ModelCompare (const Library& lib)
    : library (lib)
  { }
};

void
ProgramASTdump::print_component (const Library& library, Treelog& msg)
{
  const symbol name = library.name ();
  Nest nest (pp, "component", name);

  const symbol description = library.description ();
  if (description != symbol ())
    print_description (description);

  // Print own entries.
  daisy_assert (library.check (root_name));
  std::set<symbol> my_entries;
  own_entries (metalib, library, root_name, my_entries);
  if (my_entries.size () > 0)
    {
      const FrameModel& frame = library.model (root_name);
      print_submodel_entries (frame, my_entries);
    }

  // For all members...
  std::vector<symbol> entries;
  library.entries (entries);
  ModelCompare model_compare (library);
  sort (entries.begin (), entries.end (), model_compare);
  for (unsigned int i = 0; i < entries.size (); i++)
    if (entries[i] != root_name)
      print_model (entries[i], library, msg);

  static const symbol Daisy_symbol ("Daisy");
  current_component = Daisy_symbol;
}

void
ProgramASTdump::print_document (Treelog& msg)
{
  // For all components...
  std::vector<symbol> entries;
  metalib.all (entries);
  sort (entries.begin (), entries.end (), symbol::alphabetical);
  for (unsigned int i = 0; i < entries.size (); i++)
    print_component (metalib.library (entries[i]), msg);

  // Fixed components.
  std::vector<symbol> fixed;
  Librarian::submodel_all (fixed);
  for (unsigned int i = 0; i < fixed.size (); i++)
    {
      const symbol name = fixed[i];
      const Frame& frame = *Librarian::submodel_frame (name).get ();
      const symbol description = Librarian::submodel_description (name);
      print_fixed (name, frame, description);
    }
  
  // Daisy version.
  print_attribute ("version", version);
}

static struct ProgramASTdumpSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new ProgramASTdump (al); }
  ProgramASTdumpSyntax ()
    : DeclareModel (Program::component, "astdump", "\
Generate a dump of the abstract syntax tree.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare_string ("where", Attribute::Const, 
			  "Name of file to store results in.");
    frame.set ("where", "astdump.txt");
  }
} ProgramASTdump_syntax;

// program_document.C ends here.
