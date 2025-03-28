// time.h -- tick tick
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


#ifndef TIME_H
#define TIME_H

#include "object_model/attribute.h"
#include "object_model/symbol.h"
#include <string>
#include <vector>
#include <memory>

#ifdef __unix
#define EXPORT /* Nothing */
#elif defined (BUILD_DLL)
/* DLL export */
#define EXPORT __declspec(dllexport)
#else
/* EXE import */
#define EXPORT __declspec(dllimport)
#endif

class Frame;
class FrameSubmodel;
class Log;
class Block;

class EXPORT Time
{
  // Content.
private:
  struct Implementation;
  const std::unique_ptr<Implementation> impl;
    
  // Extract.
public:
  int year () const;
  int month () const;
  int week () const;
  int yday () const;
  int mday () const;
  int wday () const;		// 1=monday, 7=sunday.
  int hour () const;
  int minute () const;
  int second () const;
  int microsecond () const;
  std::string print () const;
  void set_time (Frame&, symbol key) const;
  double year_fraction () const; // Fraction of year since Jan 1.
  double day_fraction () const;  // Fraction of day since midnight.

  // Time components.
  enum component_t {
    Year, Month, Week, Yday, Mday, Wday, Hour, Minute, Second, Microsecond,
    First = Year, Last = Microsecond
  };
  int component_value (component_t) const;
  static symbol component_name (component_t);
  static symbol component_documentation (component_t);
  static std::vector<component_t> 
    find_time_components (const std::vector<symbol>&);
  static void declare_time_components (Frame&, const symbol name, 
                                       const Attribute::category cat,
                                       const std::string& doc);

  // Simulate. 
  int tick_generic (const int amount, const int limit, 
                    void (Time::*next) (int), const int old);
public:
  void tick_microsecond (int microseconds = 1);
  void tick_second (int seconds = 1);
  void tick_minute (int minutes = 1);
  void tick_hour (int hours = 1);
  void tick_day (int days = 1);
  void tick_year (int years = 1);
  void output (Log&) const;
  
  // Convert.
public:
  static symbol month_name (int month);
  static symbol wday_name (int wday);
  static int month_number (symbol name);
  static int wday_number (symbol day);
  static int mday2yday (int year, int month, int mday);
  static int yday2mday (int year, int yday);
  static int yday2wday (int year, int yday); // 0=monday, 6=sunday.
  static int yday2month (int year, int yday);
  static int yday2week (int year, int yday);

  // Test.
  static bool leap (int year);
  static int month_length (int year, int month);
  static bool valid (int year, int month, int mday, 
                     int hour = 0, int minute = 0, int second = 0,
                     int microsecond = 0);
  static int whole_days_between (const Time& first, const Time& last);
  static int whole_hours_between (const Time& first, const Time& last);
  static double fraction_hours_between (const Time& first, const Time& last);

  bool operator== (const Time&) const;
  bool operator!= (const Time&) const;
  bool operator<  (const Time&) const;
  bool operator<= (const Time&) const;
  bool operator>= (const Time&) const;
  bool operator>  (const Time&) const;
  bool between (const Time&, const Time&) const;

  // Create.
public:
  static void load_syntax (Frame&);
  explicit Time (const FrameSubmodel&);
  explicit Time (const Block&);

  // Construct.
public:
  static const Time& null ();
  static Time now ();
  const Time& operator= (const Time&);
  Time (const Time&);
  Time (Time&&);
  Time (int year, int month, int mday, int hour,
        int minute = 0, int second = 0, int microsecond = 0);
  ~Time ();
  explicit Time ();
};

void
operator++ (Time::component_t& val, int);

#endif // TIME_H
