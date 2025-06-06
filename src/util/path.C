// path.C -- Handle Unix and WinDOS style paths.
//
// Copyright 1996-2007 KVL, Per Abrahamsen and SÃ¸ren Hansen
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

#include "util/path.h"
#include "util/assertion.h"
#include "windows/windows_util.h"
#include "object_model/version.h"

// Get chdir.
#if defined (__unix)
#include <unistd.h>
#elif defined (__MINGW32__) || defined (_MSC_VER)
extern "C" int chdir (const char* dir);
extern "C" char *getcwd (char *buf, size_t size);
#else
#include <dir.h>
#endif
extern "C" int mkdir(const char *pathname, int mode);

#include <fstream>
#include <sstream>
#include <cstdlib>

#if defined (__unix) 
#define DIRECTORY_SEPARATOR "/"
#define PATH_SEPARATOR ":"
#else
#define DIRECTORY_SEPARATOR "\\"
#define PATH_SEPARATOR ";"
#endif

// Find current directory.
static const std::string
get_cwd ()
{
  const size_t BUFFER_SIZE = 10000;
  char buffer[BUFFER_SIZE];
  char *wd = getcwd (buffer, BUFFER_SIZE);
  if (!wd)
    {
      daisy_bug ("Current directory path is too long");
      return ".";
    }
  return wd;
}

std::vector<symbol> Path::daisy_path;


void 
Path::parse_path (const std::string& colon_path, std::vector<symbol>& result)
{ 
  int last = 0;
  for (;;)
    {
      const int next = colon_path.find (PATH_SEPARATOR, last);
      if (next < 0)
        break;
      result.push_back (colon_path.substr (last, next - last));
      last = next + 1;
    }
  result.push_back (colon_path.substr (last));
}

symbol
Path::get_daisy_home ()
{
  static symbol daisy_home;

  if (daisy_home == symbol ())
    {
      // Check DAISYHOME
      const char* daisy_home_env = getenv ("DAISYHOME");
      if (daisy_home_env)
	{
	  Assertion::debug ("Has DAISYHOME environment variable");
	  daisy_home = daisy_home_env;
	}
      else
	{
#if defined (_WIN32) || defined (__CYGWIN32__)
  // Get the path from location of the exe
  auto exe_path = get_exe_path();
  Assertion::debug ("Trying to get DAISYHOME from '" + exe_path + "'");
  auto result = get_daisy_home_from_exe_path();
  if (result.length() > 0) {
    daisy_home = result;
  } else {
    Assertion::debug("Could not get DAISHOME from exe path");
    Assertion::debug("Using standard MS Windows home.");
    daisy_home = "C:/daisy";
  }
#else // !MS WINDOWS
#ifdef __APPLE__
	  Assertion::debug ("OSX conventional home.");
	  daisy_home =  "/Applications/Daisy";
#else
	  Assertion::debug ("Using standard Unix home.");
	  daisy_home =  "/opt/daisy";
#endif
#endif // !MS WINDOWS
	}
    }
  return daisy_home;
}

const std::vector<symbol>&
Path::get_daisy_path ()
{
  if (daisy_path.size () == 0)
    {
      const char *const daisy_path_env = getenv ("DAISYPATH");
      if (daisy_path_env)
	{
	  Assertion::debug ("Has DAISYPATH environment variable.");
          parse_path (daisy_path_env, daisy_path);
	}
      else
	{
	  const symbol daisy_home = get_daisy_home ();
	  Assertion::debug ("Using '" + daisy_home + "' as daisy home.");
	  daisy_path.push_back (".");
	  daisy_path.push_back (daisy_home + "/lib");
	  daisy_path.push_back (daisy_home + "/sample");
	}
    }
  daisy_assert (daisy_path.size () > 0);
  return daisy_path;
}

symbol 
Path::nodir (symbol name_s)
{
  const std::string name = name_s.name ();
  size_t start = name.size ();

  for (;start > 0; start--)
    {
      const char prev = name[start-1];

      if (prev == '/')
	break;
      
#if !defined (__unix) 
      if (prev == '\\' || prev == ':')
	break;
#endif // !unix
    }
  
  std::string result;

  for (;start < name.size (); start++)
    result += name[start];

  return result;
}

std::unique_ptr<std::istream> 
Path::open_file (symbol name_s) const
{
  const std::string& name = name_s.name ();

  struct Message : std::ostringstream 
  {
    ~Message ()
    { Assertion::debug (this->str ()); }
  } tmp;
  tmp << "In directory '" << get_output_directory () << "':";

  std::unique_ptr<std::istream> in;

  // Absolute filename.
  if (name[0] == '.' || name[0] == '/'
#ifndef __unix__
      || name[0] == '\\' || (name.size () > 1 && name[1] == ':')
#endif
      )
    {
      tmp << "\nOpening absolute file name '" << name << "'";
      in.reset (new std::ifstream (name.c_str ()));
      return in;
    }

  tmp << "\nLooking for file '" << name << "'";

  // Look in path.
  for (unsigned int i = 0; i < path.size (); i++)
    {
      const symbol dir = (path[i] == "." ? input_directory : path[i]);
      const symbol file = dir + DIRECTORY_SEPARATOR + name;
      tmp << "\nTrying '" << file << "'";
      if (path[i] == ".")
	tmp << " (cwd)";
      in.reset (new std::ifstream (file.name ().c_str ()));
      if (in->good ())
	{
	  tmp << " success!";
	  return in;
	}
    }
  tmp << "\nGiving up";
  daisy_assert (in.get ());		
  return in;			// Return last bad stream.
}

bool 
Path::set_directory (symbol directory_s)
{ 
  input_directory = directory_s;
  
  const std::string& directory = directory_s.name ();
  const char *const dir = directory.c_str ();
  const bool result 
    = chdir (dir) == 0 || (mkdir (dir, 0777) == 0 && chdir (dir) == 0); 
  
  if (!result)
    /* Do nothing */;
  else if (directory[0] == '/'
#ifndef __unix__
           || directory[0] == '\\' || directory[1] == ':'
#endif
           )
    // Already absolute.
    output_directory = directory;
  else
    // Make it absolute.
    output_directory = get_cwd ();

  std::ostringstream tmp;
  tmp << "Changing directory to '" << directory << "' " 
      << (result ? "success" : "failure");
  Assertion::debug (tmp.str ());

  return result;
}

void
Path::set_input_directory (symbol directory_s)
{ 
  input_directory = directory_s;

  std::ostringstream tmp;
  tmp << "Interpreting '.' in path as " << input_directory;
  Assertion::debug (tmp.str ());
}

void 
Path::set_path (const std::vector<symbol>& value)
{ 
  path = value;

  std::ostringstream tmp;
  tmp << "Path set to:";
  for (size_t i = 0; i < value.size (); i++)
    tmp << "\n" << i << ": '" << value[i] << "'";
  tmp << "\ndone";
  Assertion::debug (tmp.str ());
}

void 
Path::set_path (const std::string& colon_path)
{ 
  std::vector<symbol> result;
  parse_path (colon_path, result);
  set_path (result);
}

Path::InDirectory::InDirectory (Path& p, const symbol to)
  : path (p),
    from (path.get_output_directory ()),
    ok (path.set_directory (to))
{ }

bool 
Path::InDirectory::check () const
{ return ok; }

Path::InDirectory::~InDirectory ()
{ path.set_directory (from); }

void
Path::reset ()
{
  // Find path.
  path = get_daisy_path ();
  input_directory = output_directory = get_cwd ();

  std::ostringstream tmp;
  tmp << "Reseting current directory to '" << output_directory << "'";
  Assertion::debug (tmp.str ());
}

Path::Path ()
{ reset (); }

Path::~Path ()
{ }

// path.C ends here
