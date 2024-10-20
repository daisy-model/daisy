// assertion.h -- Managed assertions.
// 
// Copyright 2002, 2003 Per Abrahamsen and KVL.
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
//
// We want a copy of assertion failures in daisy.log, yet we don't
// want to drag the correspondig treelog into every function.

#ifndef ASSERTION_H
#define ASSERTION_H

#include <string>
#include <vector>

class Treelog;

#ifdef __unix
#define EXPORT /* Nothing */
#elif defined (BUILD_DLL)
/* DLL export */
#define EXPORT __declspec(dllexport)
#else
/* EXE import */
#define EXPORT __declspec(dllimport)
#endif

#ifdef __GNUC__
#define NORETURN __attribute__ ((noreturn))
#elif defined (_MSC_VER)
#define NORETURN __declspec(noreturn)
#else
#define NORETURN
#endif

namespace Assertion
{
  EXPORT bool full (const char* file, const int line, bool is_debug = false);
  EXPORT void message (const std::string&);
  EXPORT void warning (const std::string&);
  EXPORT void error (const std::string&);
  EXPORT void debug (const std::string&);
  EXPORT NORETURN void failure (const char* file, int line, const char* fun,
				const char* test);
  EXPORT void bug (const char* file, int line, const char* fun, 
		   const std::string& msg);
  EXPORT void warning (const char* file, int line, const char* fun, 
		const std::string& msg);
  EXPORT NORETURN void panic (const char* file, int line, const char* fun,
			      const std::string& msg);
  EXPORT NORETURN void notreached (const char* file, int line,
				   const char* fun);
  EXPORT void non_negative (const char* file, int line, const char* fun,
			    const std::vector<double>& v);
  EXPORT void approximate (const char* file, int line, const char* fun,
			   double a, double b);
  EXPORT void balance (const char* file, int line, const char* fun,
		       double oldval, double newbal, double growth);

  class Register
  {
    Treelog& treelog;
  public:
    Register (Treelog&);
    ~Register ();
  };
}

#define daisy_full_debug() Assertion::full (__FILE__, __LINE__, true)
#define daisy_assert(condition) \
  while (!(condition)) \
    Assertion::failure (__FILE__, __LINE__, __FUNCTION__, #condition)
#define daisy_soft_assert(condition) \
  while (!(condition)) \
    Assertion::warning (__FILE__, __LINE__, __FUNCTION__, #condition)
#define daisy_safe_assert(condition) \
  while (!(condition)) \
    Assertion::bug (__FILE__, __LINE__, __FUNCTION__, #condition)
#define daisy_bug(msg) Assertion::bug (__FILE__, __LINE__, __FUNCTION__, msg)
#define daisy_warning(msg) \
  Assertion::warning (__FILE__, __LINE__, __FUNCTION__, msg)
#define daisy_panic(msg) \
  Assertion::panic (__FILE__, __LINE__, __FUNCTION__, msg)
#define daisy_notreached() \
  Assertion::notreached (__FILE__, __LINE__, __FUNCTION__)
#define daisy_non_negative(v) \
  Assertion::non_negative (__FILE__, __LINE__, __FUNCTION__, v)
#define daisy_approximate(a, b) \
  Assertion::approximate (__FILE__, __LINE__, __FUNCTION__, a, b)
#define daisy_balance(oldval, newval, growth) \
  Assertion::balance (__FILE__, __LINE__, __FUNCTION__, oldval, newval, growth)

#endif // ASSERTION_H
