#ifndef _UTIL_H_
#define _UTIL_H_

#include <string>
#include <sstream>

static inline std::string itos(int i) {
	std::ostringstream os;
	os << i;
	return os.str();
}

#endif
