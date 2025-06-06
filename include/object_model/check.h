// check.h -- Check validity of numeric alist members.
// 
// Copyright 2001 Per Abrahamsen and KVL.
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


#ifndef CHECK_H
#define CHECK_H

#include <string>
#include <boost/noncopyable.hpp>

class Treelog;

class Check : private boost::noncopyable
{ 
  // Use.
public:
  virtual bool verify (const double value, Treelog&) const = 0;

  // Utilities.
public:
  static const Check& none ();
  static const Check& unknown ();
  static const Check& non_zero ();
  static const Check& non_negative ();
  static const Check& non_positive ();
  static const Check& negative ();
  static const Check& positive ();
  static const Check& fraction ();
  static const Check& DS ();

  // Create and Destroy.
protected:
  Check ();
public:
  virtual ~Check ();
};

#endif // CHECK_H
