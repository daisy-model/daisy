// vcheck.C -- Check validity of arbitrary alist members.
// 
// Copyright 2001, 2003 Per Abrahamsen and KVL.
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

#include "object_model/vcheck.h"
#include "object_model/units.h"
#include "object_model/metalib.h"
#include "object_model/library.h"
#include "daisy/daisy_time.h"
#include "object_model/plf.h"
#include "util/assertion.h"
#include "util/mathlib.h"
#include "object_model/treelog.h"
#include "object_model/frame_model.h"
#include "object_model/treelog.h"
#include <sstream>
#include <algorithm>
#include <numeric>
#include <map>

// GCC 2.95 -O2 dislike declaring these classes local.
struct ValidYear : public VCheck
{
  static bool valid (int year, Treelog& msg)
  {
    if (!Time::valid (year, 1, 1, 1))
      {
	std::ostringstream tmp;
	tmp << year << " is not a valid year";
        msg.error (tmp.str ());
        return false;
      }
    return true;
  }

  bool verify (const Metalib&, const Frame& frame, const symbol key, 
               Treelog& msg) const
  { 
    daisy_assert (frame.check (key));
    daisy_assert (frame.lookup (key) == Attribute::Integer);
    daisy_assert (!frame.is_log (key));
    
    if (frame.type_size (key) == Attribute::Singleton)
      return valid (frame.integer (key), msg);

    bool ok = true;
    const std::vector<int> years = frame.integer_sequence (key);
    for (size_t i = 0; i < years.size (); i++)
      if (!valid (years[i], msg))
        ok = false;
    return ok;
  }
};

bool
VCheck::Integer::verify (const Metalib&, const Frame& frame, const symbol key, 
                        Treelog& msg) const
{
  daisy_assert (frame.check (key));
  daisy_assert (!frame.is_log (key));
  daisy_assert (frame.lookup (key) == Attribute::Integer);

  if (frame.type_size (key) == Attribute::Singleton)
    return valid (frame.integer (key), msg);

  bool ok = true;
  const std::vector<int> integers = frame.integer_sequence (key);
  for (unsigned int i = 0; i < integers.size (); i++)
    if (!valid (integers[i], msg))
      ok = false;
        
  return ok;
}

VCheck::Integer::Integer ()
{ }

bool
VCheck::IRange::valid (const int value, Treelog& msg) const
{
  if (value < min)
    {
      std::ostringstream tmp;
      tmp << "Value is " << value << " but should be >= " << min;
      msg.error (tmp.str ());
      return false;
    }
  if (value > max)
    {
      std::ostringstream tmp;
      tmp << "Value is " << value << " but should be <= " << max;
      msg.error (tmp.str ());
      return false;
    }
  return true;
}

VCheck::IRange::IRange (const int min_, const int max_)
  : min (min_),
    max (max_)
{ }

bool
VCheck::LargerThan::valid (const int value, Treelog& msg) const
{
  if (value <= below)
    {
      std::ostringstream tmp;
      tmp << "Value is " << value << " but should be > " << below;
      msg.error (tmp.str ());
      return false;
    }
  return true;
}

VCheck::LargerThan::LargerThan (const int below_)
  : below (below_)
{ }

bool
VCheck::SmallerThan::valid (const int value, Treelog& msg) const
{
  if (value >= above)
    {
      std::ostringstream tmp;
      tmp << "Value is " << value << " but should be < " << above;
      msg.error (tmp.str ());
      return false;
    }
  return true;
}

VCheck::SmallerThan::SmallerThan (const int above_)
  : above (above_)
{ }

struct LocalOrder : public VCheck
{
  virtual bool valid (double last, double next, Treelog&) const = 0;

  bool validate_plf (const PLF& plf, Treelog& msg) const
  {
    const int end = plf.size () - 1;
    daisy_assert (end >= 0);
    double last = plf.y (0);

    bool ok = true;
    for (int i = 1; i < end; i++)
      {
	const double next = plf.y (i);
	if (!valid (last, next, msg))
          ok = false;
	last = next;
      }
    return ok;
  }

  bool verify (const Metalib&, const Frame& frame, const symbol key, 
               Treelog& msg) const
  { 
    daisy_assert (frame.check (key));
    daisy_assert (!frame.is_log (key));
  
    switch (frame.lookup (key))
      {
      case Attribute::Number:
	{
	  daisy_assert (frame.type_size (key) != Attribute::Singleton);
	  const std::vector<double>& numbers = frame.number_sequence (key);
	  if (numbers.size () < 2)
	    return true;
	  double last = numbers[0];
          bool ok = true;
	  for (int i = 1; i < numbers.size (); i++)
	    {
	      const double next = numbers[i];
	      if (!valid (last, next, msg))
                ok = false;
	      last = next;
	    }
          return ok;
	}
      case Attribute::PLF:
        {
          if (frame.type_size (key) == Attribute::Singleton)
            return validate_plf (frame.plf (key), msg);
          bool ok = true;
          const std::vector<boost::shared_ptr<const PLF>/**/>& plfs 
            = frame.plf_sequence (key);
          for (unsigned int i = 0; i < plfs.size (); i++)
          if (!validate_plf (*plfs[i], msg))
            ok = false;
          return ok;
        }
      default:
        daisy_notreached ();
      }
  }
};

struct Increasing : public LocalOrder
{
  bool valid (double last, double next, Treelog& msg) const
  {
    if (last < next)
      return true;

    std::ostringstream tmp;
    tmp << last << " >= " << next << ", must be increasing";
    msg.error (tmp.str ());
    return false;
  }
};

struct NonDecreasing : public LocalOrder
{
  bool valid (double last, double next, Treelog& msg) const
  {
    if (last <= next)
      return true;

    std::ostringstream tmp;
    tmp << last << " > " << next << ", must be non-decreasing";
    msg.error (tmp.str ());
    return false;
  }
};

struct NonIncreasing : public LocalOrder
{
  bool valid (double last, double next, Treelog& msg) const
  {
    if (last >= next)
      return true;

    std::ostringstream tmp;
    tmp << last << " < " << next << ", must be non-increasing";
    msg.error (tmp.str ());
    return false;
  }
};

struct Decreasing : public LocalOrder
{
  bool valid (double last, double next, Treelog& msg) const
  {
    if (last > next)
      return true;

    std::ostringstream tmp;
    tmp << last << " <= " << next << ", must be decreasing";
    msg.error (tmp.str ());
    return false;
  }
};

struct Season : public VCheck
{
  bool valid (const PLF& plf, Treelog& msg) const
  {
    if (plf.size () < 1)
      {
        msg.error ("PLF must be non-empty");
        return false;
      }
    const size_t first = 0;
    const size_t last = plf.size () - 1;
    const double first_x = plf.x (first);
    const double first_y = plf.y (first); 
    const double last_x = plf.x (last);
    const double last_y = plf.y (last);

    if (first_x < 1 || last_x > 366)
      {
        msg.error ("Julian day must be between 1 and 366");
        return false;
      }
    if (!approximate (first_y, last_y))
      {
        std::ostringstream tmp;
        tmp << "First (" << first_y << ") and last (" << last_y
            << ") value must be identical";
        msg.error (tmp.str ());
        return false;
      }
    return true;
  }

  bool verify (const Metalib&, const Frame& frame, 
               const symbol key, Treelog& msg) const
  {
    daisy_assert (frame.check (key));
    daisy_assert (!frame.is_log (key));

    switch (frame.lookup (key))
      {
      case Attribute::PLF:
        if (frame.type_size (key) == Attribute::Singleton)
          return valid (frame.plf (key), msg);
        {
          const std::vector<boost::shared_ptr<const PLF>/**/> plfs 
            = frame.plf_sequence (key);
          bool ok = true;
          for (unsigned int i = 0; i < plfs.size (); i++)
            if (!valid (*plfs[i], msg))
              ok = false;
          return ok;
        }
        break;
      default:
        daisy_notreached ();
      }
  }

  Season ()
  { }
};


const VCheck& 
VCheck::valid_year ()
{
  static ValidYear valid_year;
  return valid_year;
}

const VCheck& 
VCheck::valid_month ()
{
  static IRange valid_month (1, 12);
  return valid_month;
}

const VCheck& 
VCheck::valid_mday ()
{
  static IRange valid_mday (1, 31);
  return valid_mday;
}

const VCheck& 
VCheck::valid_hour ()
{
  static IRange valid_hour (0, 23);
  return valid_hour;
}

const VCheck& 
VCheck::valid_minute ()
{
  static IRange valid_minute (0, 59);
  return valid_minute;
}

const VCheck& 
VCheck::valid_second ()
{
  static IRange valid_second (0, 59);
  return valid_second;
}

const VCheck& 
VCheck::negative ()
{
  static SmallerThan check (0);
  return check;
}

const VCheck& 
VCheck::non_negative ()
{
  static LargerThan check (-1);
  return check;
}

const VCheck& 
VCheck::non_positive ()
{
  static SmallerThan check (1);
  return check;
}


const VCheck& 
VCheck::positive ()
{
  static LargerThan check (0);
  return check;
}

const VCheck& 
VCheck::increasing ()
{
  static Increasing increasing;
  return increasing;
}

const VCheck& 
VCheck::non_decreasing ()
{
  static NonDecreasing non_decreasing;
  return non_decreasing;
}

const VCheck& 
VCheck::non_increasing ()
{
  static NonIncreasing non_increasing;
  return non_increasing;
}

const VCheck& 
VCheck::decreasing ()
{
  static Decreasing decreasing;
  return decreasing;
}

const VCheck& 
VCheck::sum_equal_0 ()
{
  static SumEqual sum_equal (0.0);
  return sum_equal;
}

const VCheck& 
VCheck::sum_equal_1 ()
{
  static SumEqual sum_equal (1.0);
  return sum_equal;
}

const VCheck& 
VCheck::season ()
{
  static Season season;
  return season;
}

const VCheck& 
VCheck::min_size_1 ()
{
  static MinSize min_size (1);
  return min_size;
}

VCheck::VCheck ()
{ }

VCheck::~VCheck ()
{ }

bool
VCheck::SumEqual::valid (double value, Treelog& msg) const
{
  if (approximate (value, sum))
    return true;

  std::ostringstream tmp;
  tmp << "Sum is " << value << " but should be " << sum;
  msg.error (tmp.str ());
  return false;
}

bool
VCheck::SumEqual::valid (const PLF& plf, Treelog& msg) const
{
  const int end = plf.size () - 1;
  daisy_assert (end >= 0);
  bool ok = true;
  if (std::isnormal (plf.y (0)))
    {
      msg.error ("Value at start of PLF should be 0.0");
      ok = false;
    }
  if (std::isnormal (plf.y (end)))
    {
      msg.error ("Value at end of PLF should be 0.0");
      ok = false;
    }
  if (!valid (plf.integrate (plf.x (0), plf.x (end)), msg))
    ok = false;

  return ok;
}

bool
VCheck::SumEqual::verify (const Metalib&, const Frame& frame, 
                          const symbol key, Treelog& msg) const
{
  daisy_assert (frame.check (key));
  daisy_assert (!frame.is_log (key));
  
  switch (frame.lookup (key))
    {
    case Attribute::Number:
      {
	daisy_assert (frame.type_size (key) != Attribute::Singleton);
	const std::vector<double>& numbers = frame.number_sequence (key);
	return valid (accumulate (numbers.begin (), numbers.end (), 0.0), msg);
      }
      break;
    case Attribute::PLF:
      if (frame.type_size (key) == Attribute::Singleton)
	return valid (frame.plf (key), msg);
      else
	{
          bool ok = true;
	  const std::vector<boost::shared_ptr<const PLF>/**/> plfs 
            = frame.plf_sequence (key);
	  for (unsigned int i = 0; i < plfs.size (); i++)
	    if (!valid (*plfs[i], msg))
              ok = false;
          return ok;
	}
      break;
    default:
      daisy_notreached ();
    }
}

VCheck::SumEqual::SumEqual (double value)
  : sum (value)
{ }

bool
VCheck::StartValue::valid (double value, Treelog& msg) const
{
  if (approximate (value, fixed))
    return true;

  std::ostringstream tmp;
  tmp << "Start value is " << value << " but should be " << fixed;
  msg.error (tmp.str ());
  return false;
}

bool
VCheck::StartValue::valid (const PLF& plf, Treelog& msg) const
{ return valid (plf.y (0), msg); }

bool
VCheck::StartValue::verify (const Metalib&, const Frame& frame, 
                            const symbol key, Treelog& msg) const
{
  daisy_assert (frame.check (key));
  daisy_assert (!frame.is_log (key));
  
  switch (frame.lookup (key))
    {
    case Attribute::Number:
      {
	daisy_assert (frame.type_size (key) != Attribute::Singleton);
	const std::vector<double>& numbers = frame.number_sequence (key);
	if (numbers.size () > 0)
	  return valid (numbers[0], msg);
      }
      return true;
    case Attribute::PLF:
      if (frame.type_size (key) == Attribute::Singleton)
	return valid (frame.plf (key), msg);
      {
        const std::vector<boost::shared_ptr<const PLF>/**/> plfs 
          = frame.plf_sequence (key);
        bool ok = true;
        for (unsigned int i = 0; i < plfs.size (); i++)
          if (!valid (*plfs[i], msg))
            ok = false;
        return ok;
      }
      break;
    default:
      daisy_notreached ();
    }
}

VCheck::StartValue::StartValue (double value)
  : fixed (value)
{ }

bool
VCheck::EndValue::valid (double value, Treelog& msg) const
{
  if (approximate (value, fixed))
    return true;

  std::ostringstream tmp;
  tmp << "End value is " << value << " but should be " << fixed;
  msg.error (tmp.str ());
  return false;
}

bool
VCheck::EndValue::valid (const PLF& plf, Treelog& msg) const
{ 
  daisy_assert (plf.size () > 0);
  return valid (plf.y (plf.size () - 1), msg); 
}

bool
VCheck::EndValue::verify (const Metalib&, const Frame& frame, 
                          const symbol key, Treelog& msg) const
{
  daisy_assert (frame.check (key));
  daisy_assert (!frame.is_log (key));
  
  switch (frame.lookup (key))
    {
    case Attribute::Number:
      {
	daisy_assert (frame.type_size (key) != Attribute::Singleton);
	const std::vector<double>& numbers = frame.number_sequence (key);
	if (numbers.size () > 0)
	  return valid (numbers[numbers.size () - 1], msg);
      }
      return true;
    case Attribute::PLF:
      if (frame.type_size (key) == Attribute::Singleton)
	return valid (frame.plf (key), msg);
      {
        const std::vector<boost::shared_ptr<const PLF>/**/> plfs
          = frame.plf_sequence (key);
        bool ok = true;
        for (unsigned int i = 0; i < plfs.size (); i++)
          if (!valid (*plfs[i], msg))
            ok = false;
        return ok;
      }
      break;
    default:
      daisy_notreached ();
    }
}

VCheck::EndValue::EndValue (double value)
  : fixed (value)
{ }

bool
VCheck::FixedPoint::valid (const PLF& plf, Treelog& msg) const
{ 
  if (approximate (plf (fixed_x), fixed_y))
    return true;

  std::ostringstream tmp;
  tmp << "Value at " << fixed_x << " should be " << fixed_y 
      << " but is << " << plf (fixed_x);
  msg.error (tmp.str ());
  return false;
}

bool
VCheck::FixedPoint::verify (const Metalib&, const Frame& frame, 
                            const symbol key, Treelog& msg) const
{
  daisy_assert (frame.check (key));
  daisy_assert (!frame.is_log (key));
  
  switch (frame.lookup (key))
    {
    case Attribute::PLF:
      if (frame.type_size (key) == Attribute::Singleton)
	return valid (frame.plf (key), msg);
      {
        const std::vector<boost::shared_ptr<const PLF>/**/> plfs 
          = frame.plf_sequence (key);
        bool ok = true;
        for (unsigned int i = 0; i < plfs.size (); i++)
          if (!valid (*plfs[i], msg))
            ok = false;
        return ok;
      }
      break;
    default:
      daisy_notreached ();
    }
}

VCheck::FixedPoint::FixedPoint (double x, double y)
  : fixed_x (x),
    fixed_y (y)
{ }

bool
VCheck::MinSize::verify (const Metalib&, const Frame& frame, 
                         const symbol key, Treelog& msg) const
{
  daisy_assert (frame.check (key));
  daisy_assert (!frame.is_log (key));
  daisy_assert (Attribute::flexible_size (frame.type_size (key)));
  if (frame.value_size (key) >= min_size)
    return true;
  std::ostringstream tmp;
  tmp << "Need at least " << min_size << " elements, got " 
      << frame.value_size (key);
  msg.error (tmp.str ());
  return false;
}

VCheck::MinSize::MinSize (unsigned int size)
  : min_size (size)
{ }

bool
VCheck::String::verify (const Metalib&, const Frame& frame, 
                        const symbol key, Treelog& msg) const
{
  daisy_assert (frame.check (key));
  daisy_assert (!frame.is_log (key));
  if (frame.type_size (key) == Attribute::Singleton)
    return valid (frame.name (key), msg);
  {
    const std::vector<symbol> names = frame.name_sequence (key);
    bool ok = true;
    for (size_t i = 0; i < names.size (); i++)
      if (!valid (names[i].name (), msg))
        ok = false;
    return ok;
  }
}

bool
VCheck::Compatible::valid (const Units& units, symbol value, Treelog& msg) const
{
  if (units.can_convert (dimension, value))
    return true;
  
  std::ostringstream tmp;
  tmp << "Cannot convert [" << dimension << "] to [" << value << "]";
  msg.error (tmp.str ());
  return false;
}

bool
VCheck::Compatible::verify (const Metalib& metalib, const Frame& frame, 
                            const symbol key, Treelog& msg) const
{
  daisy_assert (frame.check (key));
  daisy_assert (!frame.is_log (key));
  const Units& units = metalib.units ();
  if (frame.type_size (key) == Attribute::Singleton)
    return valid (units, frame.name (key), msg);

  const std::vector<symbol> names = frame.name_sequence (key);
  bool ok = true;
  for (size_t i = 0; i < names.size (); i++)
    if (!valid (units, names[i], msg))
      ok = false;
  return ok;
}

VCheck::Compatible::Compatible (const symbol dim)
  : dimension (dim)
{ }

const VCheck& 
VCheck::fraction ()
{
  static Compatible fraction (Attribute::Fraction ());
  return fraction;
}

bool
VCheck::Enum::valid (const symbol value, Treelog& msg) const
{
  if (ids.find (value) != ids.end ())
    return true;

  msg.error ("Invalid value '" + value + "'");
  return false;
}

VCheck::Enum::Enum ()
{ }

void 
VCheck::Enum::add (const symbol a)
{ ids.insert (a); }

size_t 
VCheck::Enum::size () const
{ return ids.size (); }

VCheck::Enum::Enum (const symbol a)
{
  ids.insert (a);
}

VCheck::Enum::Enum (const symbol a, const symbol b)
{
  ids.insert (a);
  ids.insert (b);
}

VCheck::Enum::Enum (const symbol a, const symbol b, const symbol c)
{
  ids.insert (a);
  ids.insert (b);
  ids.insert (c);
}

VCheck::Enum::Enum (const symbol a, const symbol b, const symbol c, 
		  const symbol d)
{
  ids.insert (a);
  ids.insert (b);
  ids.insert (c);
  ids.insert (d);
}

VCheck::Enum::Enum (const symbol a, const symbol b, const symbol c,
		  const symbol d, const symbol e)
{
  ids.insert (a);
  ids.insert (b);
  ids.insert (c);
  ids.insert (d);
  ids.insert (e);
}

VCheck::Enum::Enum (const symbol a, const symbol b, const symbol c,
		  const symbol d, const symbol e, const symbol f)
{
  ids.insert (a);
  ids.insert (b);
  ids.insert (c);
  ids.insert (d);
  ids.insert (e);
  ids.insert (f);
}

bool
VCheck::InLibrary::verify (const Metalib& metalib, const Frame& frame, 
                           const symbol key, Treelog& msg) const
{
  daisy_assert (frame.check (key));
  daisy_assert (!frame.is_log (key));
  if (frame.type_size (key) == Attribute::Singleton)
    return valid (metalib, frame.name (key), msg);

  const std::vector<symbol> names = frame.name_sequence (key);
  bool ok = true;
  for (size_t i = 0; i < names.size (); i++)
    if (!valid (metalib, names[i], msg))
      ok = false;
  return ok;
}

bool
VCheck::InLibrary::valid (const Metalib& metalib, const symbol type, 
                          Treelog& msg) const
{
  if (type.name ().substr (0, 1) == "$")
    return true;

  daisy_assert (metalib.exist (lib_name));
  const Library& library = metalib.library (lib_name);
  
  if (!library.check (type))
    {
      msg.error ("Unknown '" + lib_name + "' type '" + type + "'");
      return false;
    }
  return true;
}

VCheck::InLibrary::InLibrary (const symbol lib)
  : lib_name (lib)
{ }

bool
VCheck::Buildable::valid (const Metalib& metalib, const symbol type, 
                          Treelog& msg) const
{
  if (type.name ().substr (0, 1) == "$")
    return true;

  if (!InLibrary::valid (metalib, type, msg))
    return false;

  const Library& library = metalib.library (lib_name);
  const FrameModel& frame = library.model (type);
  if (!frame.check (metalib, Treelog::null ()))
    {
      msg.error ("Incomplete type '" + type + "'");
      return false;
    }
  return true;
}

VCheck::Buildable::Buildable (const symbol lib)
  : InLibrary (lib)
{ }

template<class T> 
bool unique_validate (const std::vector<T>& list, Treelog& msg)
{
  bool ok = true;
  std::map<T, size_t> found;
  for (size_t i = 0; i < list.size (); i++)
    {
      T v = list[i];
      typename std::map<T, size_t>::const_iterator f = found.find (v);
      if (f != found.end ())
	{
	  std::ostringstream tmp;
	  tmp << "Entry " << (*f).second << " and " << i 
	      << " are both '" << v << "'";
	  msg.error (tmp.str ());
          ok = false;
	}
      found[v] = i;
    }
  return ok;
}

const VCheck& 
VCheck::unique ()
{
  static struct Unique : public VCheck
  {
    bool verify (const Metalib&, const Frame& frame, const symbol key, 
                 Treelog& msg) const
    { 
      daisy_assert (frame.check (key));
      daisy_assert (frame.type_size (key) != Attribute::Singleton);
      daisy_assert (!frame.is_log (key));
      
      switch (frame.lookup (key))
        {
        case Attribute::Number:
          return unique_validate (frame.number_sequence (key), msg);
        case Attribute::PLF:
          return unique_validate (frame.plf_sequence (key), msg);
        case Attribute::Boolean:
          return unique_validate (frame.flag_sequence (key), msg);
        case Attribute::String:
          return unique_validate (frame.name_sequence (key), msg);
        case Attribute::Integer:
          return unique_validate (frame.integer_sequence (key), msg);
	case Attribute::Model:
	  {
            bool ok = true;
	    const std::vector<boost::shared_ptr<const FrameModel>/**/>& list 
              = frame.model_sequence (key);
	    std::map<symbol, size_t> found;
	    for (size_t i = 0; i < list.size (); i++)
	      {
		const symbol type = list[i]->type_name ();
		std::map<symbol, size_t>::const_iterator f = found.find (type);
		if (f != found.end ())
		  {
		    std::ostringstream tmp;
		    tmp << "Entry " << (*f).second << " and " << i 
			<< " are both '" << type << "'";
		    msg.error (tmp.str ());
                    ok = false;
		  }
		found[type] = i;
	      }
            return ok;
	  }
        default:
          daisy_panic ("Unhandled list type "
                       + Attribute::type_name (frame.lookup (key)));
        }
    }      
  } unique;

  return unique;
}

bool
VCheck::MultiSize::verify (const Metalib&, const Frame& frame, 
                           const symbol key, Treelog& msg) const
{
  daisy_assert (frame.check (key));
  daisy_assert (!frame.is_log (key));
  daisy_assert (frame.type_size (key) != Attribute::Singleton);

  if (sizes.find (frame.value_size (key)) != sizes.end ())
    return true;

  std::ostringstream tmp;
  tmp << "'" << key << "' has " << frame.value_size (key)
      << " elements, expected one of { ";
  bool first = true;
  for (std::set<size_t>::const_iterator i = sizes.begin ();
       i != sizes.end ();
       i++)
    {
      if (first)
        first = false;
      else
        tmp << ", ";
      tmp << *i;
    }
  tmp << " } elements";
  msg.error (tmp.str ());
  return false;
}

VCheck::MultiSize::MultiSize (const size_t a, const size_t b)
{
  sizes.insert (a);
  sizes.insert (b);
}

bool
VCheck::All::verify (const Metalib& metalib, const Frame& frame, const symbol key,
                     Treelog& msg) const
{
  bool ok = true;
  for (int i = 0; i < checks.size (); i++)
    if (!checks[i]->verify (metalib, frame, key, msg))
      ok = false;
  return ok;
}

VCheck::All::All (const VCheck& a, const VCheck& b)
{
  checks.push_back (&a);
  checks.push_back (&b);
}

VCheck::All::All (const VCheck& a, const VCheck& b, const VCheck& c)
{
  checks.push_back (&a);
  checks.push_back (&b);
  checks.push_back (&c);
}

VCheck::All::All (const VCheck& a, const VCheck& b, const VCheck& c, 
		  const VCheck& d)
{
  checks.push_back (&a);
  checks.push_back (&b);
  checks.push_back (&c);
  checks.push_back (&d);
}

VCheck::All::All (const VCheck& a, const VCheck& b, const VCheck& c,
		  const VCheck& d, const VCheck& e)
{
  checks.push_back (&a);
  checks.push_back (&b);
  checks.push_back (&c);
  checks.push_back (&d);
  checks.push_back (&e);
}

bool
VCheck::Any::verify (const Metalib& metalib, const Frame& frame, const symbol key,
                     Treelog& msg) const
{
  for (int i = 0; i < checks.size (); i++)
    if (checks[i]->verify (metalib, frame, key, Treelog::null ()))
      return true;
  msg.error ("no criteria fulfilled");
  return false;
}

VCheck::Any::Any (const VCheck& a, const VCheck& b)
{
  checks.push_back (&a);
  checks.push_back (&b);
}

VCheck::Any::Any (const VCheck& a, const VCheck& b, const VCheck& c)
{
  checks.push_back (&a);
  checks.push_back (&b);
  checks.push_back (&c);
}

VCheck::Any::Any (const VCheck& a, const VCheck& b, const VCheck& c, 
		  const VCheck& d)
{
  checks.push_back (&a);
  checks.push_back (&b);
  checks.push_back (&c);
  checks.push_back (&d);
}

VCheck::Any::Any (const VCheck& a, const VCheck& b, const VCheck& c,
		  const VCheck& d, const VCheck& e)
{
  checks.push_back (&a);
  checks.push_back (&b);
  checks.push_back (&c);
  checks.push_back (&d);
  checks.push_back (&e);
}

// vcheck.C ends here.
