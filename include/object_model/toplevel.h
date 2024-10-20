// toplevel.h -- The top level syntax for .dai files.
// 
// Copyright 2007 Per Abrahamsen and KVL.
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


#ifndef TOPLEVEL_H
#define TOPLEVEL_H

#include <vector>
#include <string>
#include <memory>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

class Frame;
class Metalib;
class Program;
class Treelog;

#ifdef __GNUC__
#define NORETURN __attribute__ ((noreturn))
#elif defined (_MSC_VER)
#define NORETURN __declspec(noreturn)
#else
#define NORETURN
#endif

#ifdef __unix
#define EXPORT /* Nothing */
#elif defined (BUILD_DLL)
/* DLL export */
#define EXPORT __declspec(dllexport)
#else
/* EXE import */
#define EXPORT __declspec(dllimport)
#endif

class EXPORT Toplevel : private boost::noncopyable
{
  //Content.
private:
  struct Implementation;
  std::unique_ptr<Implementation> impl;
public:
  static const char *const default_description;

  enum state_t { is_unloaded, is_uninitialized,
		 is_ready, is_running, is_done, is_error };

  // Accessors.
public:
  const Frame& program_frame () const;
  Program& program () const;
  Metalib& metalib ();
  Treelog& msg ();
  const std::vector<std::string>& files_found () const;
  void add_treelog (boost::shared_ptr<Treelog>);
  void set_ui_progress ();
  void set_ui_none ();

  // Messages.
public:
  NORETURN void usage ();
private:
  void copyright ();
  void start_message () const;
  void end_message () const;

  // Use.
public:
  void user_interface ();
private:
  void failure_interface ();
public:
  void run ();
  void error (const std::string&);
  state_t state () const;

  // Create and Destroy.
public:
  void initialize ();
  void reset ();
private:
  static std::string get_arg (int& argc, char**& argv);
public:
  void command_line (int& argc, char**& argv);
  void parse_file (const std::string&);
  void parse_system_file (const std::string&);
  static void load_submodel (Frame&);
  static void load_frame (Frame&);
private:
  static void load_syntax (Frame&);
public:
  Toplevel (const std::string& preferred_ui);
  ~Toplevel ();
private:                        // Disable defaults.
  Toplevel ();
};

#endif // TOPLEVEL_H
