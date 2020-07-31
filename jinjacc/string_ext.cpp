#include "string_ext.h"


std::string ReplaceAll(std::string str, const std::string& from, const std::string& to)
{
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos)
  {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
  }
  return str;
}

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string& s)
{
  std::string r;
  size_t begin;
  begin = s.find_first_not_of(WHITESPACE);
  if (begin == std::string::npos)
  {
    //string contains only whitespace
    r = "";
  }
  else
  {
    //do trim
    r = s.substr(begin);
  }
  return r;
}

std::string rtrim(const std::string& s)
{
  std::string r;
  size_t end;
  end = s.find_last_not_of(WHITESPACE);
  if (end == std::string::npos)
  {
    r = "";
  }
  else
  {
    r = s.substr(0, end + 1);
  }
  return r;
}

std::string trim(const std::string& s)
{
  return rtrim(ltrim(s));
}

