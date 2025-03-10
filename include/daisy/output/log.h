// log.h -- Saving information about the simulation.
// 
// Copyright 1996-2001, 2004 Per Abrahamsen and Søren Hansen
// Copyright 2000-2001, 2004 KVL.
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


#ifndef LOG_H
#define LOG_H

#include "daisy/daisy_time.h"
#include "daisy/soil/transport/border.h"
#include "object_model/model_framed.h"
#include "object_model/symbol.h"
#include <iosfwd>
#include <memory>
#include <vector>

#ifdef __unix
#define EXPORT /* Nothing */
#elif defined (BUILD_DLL)
/* DLL export */
#define EXPORT __declspec(dllexport)
#else
/* EXE import */
#define EXPORT __declspec(dllimport)
#endif

class Daisy;
class PLF;
class Metalib;
class Treelog;
class BlockModel;
class Frame;
class Column;
class Field;
class Scope;

class EXPORT Log : public ModelFramed
{
  // Content.
private:
  struct Implementation;
  std::unique_ptr<Implementation> impl;
public:
  static const char *const component;
  symbol library_id () const;
  const Metalib& metalib () const;
  virtual void find_scopes (std::vector<const Scope*>&) const;

  // Filter
public:
  virtual bool check_leaf (symbol) const = 0;
  virtual bool check_interior (symbol) const = 0;
  virtual bool check_entry (symbol, symbol library) const;
  virtual bool check_derived (symbol field, symbol name, 
			      symbol library) const = 0;

  // Use.  
public:
  // Called at the start of each time step.
  virtual bool match (const Daisy&, Treelog&) = 0;
  // Called at the end of each time step.
  virtual void done (const std::vector<Time::component_t>& time_columns,
		     const Time& time, const double dt, Treelog&) = 0;

  // Initial line.
public:
  virtual bool initial_match (const Daisy&, const Time& previous, Treelog&) = 0;
  virtual void initial_done (const std::vector<Time::component_t>& time_columns,
			     const Time& previous, Treelog&) = 0;

  // Normal items.
public:
  struct Open
  {
  private:
    Log& ll;
  public:
    Open (Log& l, symbol name)
      : ll (l)
    { ll.open (name); }
    ~Open ()
    { ll.close (); }
  };
private:
  virtual void open (symbol) = 0;
  virtual void close () = 0;
  friend struct Log::Open;

public:

  // Backward compatible lists.
public:
  struct Unnamed
  {
  private:
    Log& ll;
  public:
    Unnamed (Log& l)
      : ll (l)
    { ll.open_unnamed (); }
    ~Unnamed ()
    { ll.close_unnamed (); }
  };
private:
  virtual void open_unnamed () = 0;
  virtual void close_unnamed () = 0;
  friend struct Log::Unnamed;
  
  // Named lists.
public:
  struct Named
  {
  private:
    Log& ll;
  public:
    Named (Log& l, const symbol name)
      : ll (l)
    { ll.open_named (name); }
    ~Named ()
    { ll.close_named (); }
  };
private:
  virtual void open_named (symbol name);
  virtual void close_named ();
  friend struct Log::Named;

  // Ordered lists.
public:
  struct Ordered
  {
  private:
    Log& ll;
  public:
    Ordered (Log& l, int index)
      : ll (l)
    { ll.open_ordered (index); }
    ~Ordered ()
    { ll.close_ordered (); }
  };
private:
  virtual void open_ordered (int index);
  virtual void close_ordered ();
  friend struct Log::Ordered;

  // Submodel singletons variant.
public:
  struct Submodel
  {
  private:
    Log& ll;
  public:
    Submodel (Log& l, symbol name, const Frame& alist)
      : ll (l)
    { ll.open_alist (name, alist); }
    ~Submodel ()
    { ll.close_alist (); }
  };
private:
  virtual void open_alist (symbol name, const Frame& alist);
  virtual void close_alist ();
  friend struct Log::Submodel;

  // Derived objects.
public:
  struct Derived
  {
  private:
    Log& ll;
  public:
    Derived (Log& l, const symbol field, const symbol type, 
	     const symbol library)
      : ll (l)
    { ll.open_derived (field, type, library); }
    ~Derived ()
    { ll.close_derived (); }
  };
private:
  virtual void open_derived (symbol field, symbol type, 
			     const symbol library) = 0;
  virtual void close_derived () = 0;
  friend struct Log::Derived;

  // Derived objects with their own alist.
public:
  struct Model
  {
  private:
    Log& ll;
  public:
    Model (Log& l, const symbol field, const symbol type, 
            const Frame& alist, const symbol library)
      : ll (l)
    { ll.open_object (field, type, alist, library); }
    ~Model ()
    { ll.close_object (); }
  };
private:
  virtual void open_object (symbol field, symbol type, 
			    const Frame& alist, 
			    const symbol library) = 0;
  virtual void close_object () = 0;
  friend struct Log::Model;

  // Derived objects in a variable length list.
public:
  struct Entry
  {
  private:
    Log& ll;
  public:
    Entry (Log& l, const symbol type,
	   const Frame& alist, const symbol library)
      : ll (l)
    { ll.open_entry (type, alist, library); }
    ~Entry ()
    { ll.close_entry (); }
  };
private:
  virtual void open_entry (symbol type, const Frame&, 
			   symbol library) = 0;
  virtual void close_entry () = 0;
  friend struct Log::Entry;

  // Named derived objects in a variable length list.
public:
  struct NamedEntry
  {
  private:
    Log& ll;
  public:
    NamedEntry (Log& l, symbol name, symbol type, 
		const Frame& alist)
      : ll (l)
    { ll.open_named_entry (name, type, alist); }
    ~NamedEntry ()
    { ll.close_named_entry (); }
  };
private:
  virtual void open_named_entry (symbol name, symbol type,
				 const Frame&) = 0;
  virtual void close_named_entry () = 0;
  friend struct Log::NamedEntry;

  // Object names in a variable length list.
public:
  struct Shallow
  {
  private:
    Log& ll;
  public:
    Shallow (Log& l, const symbol type, const symbol library)
      : ll (l)
    { ll.open_shallow (type, library); }
    ~Shallow ()
    { ll.close_shallow (); }
  };
private:
  virtual void open_shallow (symbol type, const symbol library) = 0;
  virtual void close_shallow () = 0;
  friend struct Log::Shallow;

  // The data.
public:
  virtual void output_entry (symbol, bool) = 0;
  virtual void output_entry (symbol, double) = 0;
  virtual void output_entry (symbol, int) = 0;
  virtual void output_entry (symbol, symbol) = 0;
  virtual void output_entry (symbol, const std::vector<double>&) = 0;
  virtual void output_entry (symbol, const PLF&) = 0;

  // Keep track of geometry for logging arrays.
public:
  virtual void column_clear ();
  virtual void column_add_to_total (const Column& column);
  virtual void column_select (const Column& column);

  // Self use.
public:
  virtual void output (Log&) const;

  // Create and Destroy.
public:
  virtual bool check (const Border&, Treelog& err) const = 0;
protected:
  virtual void initialize (const symbol log_dir, const symbol suffix,
			   Treelog& out) = 0;
public:
  void initialize_common (const symbol log_dir, const symbol suffix,
			  const Metalib&, Treelog& out);
protected:
  Log (const BlockModel& al);
  Log (const char* id);
public:
  virtual void summarize (Treelog& out);
  ~Log ();
};

// Output atom.
#define output_value(value, key, log)\
do { \
  static const symbol MACRO_name (key); \
  (log).output_entry (MACRO_name, (value)); \
} while (false)

// Shorthand for when the C++ and log variable are named the same.
#define output_variable(var, log) output_value (var, #var, log)

#define output_lazy(value, key, log)\
do { \
  static const symbol MACRO_name (key); \
  Log& MACRO_log = (log); \
  if (MACRO_log.check_leaf (MACRO_name)) \
    MACRO_log.output_entry (MACRO_name, (value)); \
} while (false)

// Output an alist.
#define output_submodule(submodule, key, log)\
do { \
  static const symbol MACRO_name (key); \
  if (log.check_interior (MACRO_name)) \
    { \
      Log::Open open (log, MACRO_name); \
      (submodule).output (log); \
    } \
} while (false)

// Output an object.
#define output_derived(submodule, key, log) \
do { \
  static const symbol MACRO_name (key); \
  submodule->output_as_derived (MACRO_name, (log));   \
} while (false)

// Like output_derived, except that SUBMODULE contain its own alist member.
// Useful for dynamically created object singletons, where the alist can't
// be found in the parent.
#define output_object(submodule, key, log) \
do { \
  static const symbol MACRO_name (key); \
  submodule->output_as_object (MACRO_name, (log));      \
} while (false)

// Output a list of objects.
#define output_list(items, key, log, lib) \
do { \
  static const symbol MACRO_name (key); \
  output_list_ ((items), MACRO_name, (log), (lib)); \
} while (false)

template <class T> void
output_list_ (T const& items, const symbol name, Log& log,
              const symbol library) // FIXME: Why is library not used?
{
  if (log.check_interior (name))
    {
      Log::Open open (log, name);
      for (typename T::const_iterator item = items.begin(); 
	   item != items.end();
	   item++)
        (*item)->output_as_entry (log);
    }
}

// Output an ordered list of alists.
#define output_ordered(items, key, log) \
do { \
  static const symbol MACRO_name (key); \
  output_ordered_ ((items), MACRO_name, (log)); \
} while (false)

template <class T> void
output_ordered_ (T const& items, const symbol name, Log& log)
{
  if (log.check_interior (name))
    {
      Log::Open open (log, name);
      int i = 0;
      for (typename T::const_iterator item = items.begin ();
	   item != items.end ();
	   item++)
	{
	  Log::Ordered ordered (log, i);
	  (*item)->output (log);
	  i++;
	}
    }
}

#endif // LOG_H
