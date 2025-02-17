// submodeler.h  --- Utilities for handling submodels.
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


#ifndef SUBMODELER_H
#define SUBMODELER_H

#include "object_model/block_submodel.h"
#include <vector>
#include <memory>

// Old style (no block scope).
template <class T> 
std::vector<T*>
map_construct (const std::vector<boost::shared_ptr<const FrameSubmodel>/**/>& f)
{ 
  std::vector<T*> t;
  for (std::vector<boost::shared_ptr<const FrameSubmodel>/**/>::const_iterator i = f.begin ();
       i != f.end ();
       i++)
    t.push_back (new T (**i));
  return t;
}

template <class T> 
std::vector<const T*>
map_construct_const (const std::vector<boost::shared_ptr<const FrameSubmodel>/**/>& f)
{ 
  std::vector<const T*> t;
  for (std::vector<boost::shared_ptr<const FrameSubmodel>/**/>::const_iterator i = f.begin ();
       i != f.end ();
       i++)
    t.push_back (new T (**i));
  return t;
}

// New style (block scope).

template <class T> 
T
submodel_value_block (const Block& nested)
{ 
  try
    { return T (nested); }
  catch (const std::string& err)
    { nested.error ("Submodel value build failed: " + err); }
  catch (const char *const err)
    { nested.error ("Submodel value build failed: " + std::string (err)); }
  return T::null ();
}

template <class T> 
T
submodel_value (const Block& parent, const symbol key)
{ 
  BlockSubmodel nested (parent, key);
  try
    { return T (nested); }
  catch (const std::string& err)
    { nested.error ("Submodel value build failed: " + err); }
  catch (const char *const err)
    { nested.error ("Submodel value build failed: " + std::string (err)); }
  return T::null ();
}

template <class T> 
T*
submodel_block (const BlockSubmodel& nested)
{ 
  try
    { return new T (nested); }
  catch (const std::string& err)
    { nested.error ("Submodel build failed: " + err); }
  catch (const char *const err)
    { nested.error ("Submodel build failed: " + std::string (err)); }
  return NULL;
}

template <class T> 
T*
submodel (const Block& parent, const symbol key)
{ 
  BlockSubmodel nested (parent, key);
  return submodel_block<T> (nested);
}

// Sequences
template <class T> 
std::vector<T*>
map_submodel (const Block& parent, const symbol key)
{ 
  std::vector<T*> t;
  const size_t size = parent.value_size (key);
  for (size_t i = 0; i < size; i++)
    {
      BlockSubmodel nested (parent, key, i);
      t.push_back (submodel_block<T> (nested));
    }
  return t;
}

template <class T> 
std::vector<const T*>
map_submodel_const (const Block& parent, const symbol key)
{ 
  std::vector<const T*> t;
  const size_t size = parent.value_size (key);
  for (size_t i = 0; i < size; i++)
    {
      BlockSubmodel nested (parent, key, i);
      t.push_back (submodel_block<T> (nested));
    }
  return t;
}

#endif // SUBMODELER_H
