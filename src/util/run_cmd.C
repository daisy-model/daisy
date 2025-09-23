#include "util/run_cmd.h"
#include <string>
#include <utility>

std::string quote_if_unquoted(const std::string& str) {
    if (str.size() < 2 || str.front() != '"' || str.back() != '"') {
        return '"' + str + '"';
    }
    return str;
}

#if defined(_WIN32) || defined(__CYGWIN__32__)
#include <windows.h>
#include <processthreadsapi.h>
std::pair<int, std::string> run_cmd(const std::string & exe, const std::string & args, const std::string & name) {
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si);
  ZeroMemory( &pi, sizeof(pi) );
  int status = 0;
  if (CreateProcessA(exe.c_str(), const_cast<char *>(args.c_str()), NULL, NULL, false, 0, NULL, NULL, &si, &pi)) {
    // WaitForSingleObject returns WAIT_OBJECT_0 on success
    // https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobject
    if (WaitForSingleObject(pi.hProcess, INFINITE) == 0) {
      // Check the exit code. Is zero if there was an error
      // https://learn.microsoft.com/en-gb/windows/win32/api/processthreadsapi/nf-processthreadsapi-getexitcodeprocess
      DWORD exit_code;
      if (GetExitCodeProcess(pi.hProcess, &exit_code)) {
        status = exit_code;
      }
      else {
        status = -3;
      }
    }
    else {
      status = -2;
    }
  }
  else {
    status = -1;
  }
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  return std::make_pair(status, name);

}
#else
#include <cstdlib>
std::pair<int, std::string> run_cmd(const std::string & exe, const std::string & args, const std::string & name) {
  int status = std::system((quote_if_unquoted(exe) + " " + args).c_str());
  return std::make_pair(status, name);
}
#endif
