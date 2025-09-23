#ifndef DAISY_UTIL_RUN_CMD_H
#define DAISY_UTIL_RUN_CMD_H
#include <string>
#include <utility>

std::string quote_if_unquoted(const std::string &);
std::pair<int, std::string> run_cmd(const std::string & exe, const std::string & args, const std::string & name);

#endif
