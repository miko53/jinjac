#ifndef STRING_EXT_H
#define STRING_EXT_H

#include <string>

extern std::string ReplaceAll(std::string str, const std::string& from, const std::string& to);
extern std::string ltrim(const std::string& s);
extern std::string rtrim(const std::string& s);
extern std::string trim(const std::string& s);

#endif /* STRING_EXT_H */
