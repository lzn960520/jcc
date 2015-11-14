#ifndef _LOCATION_H_
#define _LOCATION_H_

#include <string>

struct Position {
	std::string filename;
	unsigned int line;
	unsigned int column;
};

struct Location {
	Position begin, end;
};

#endif
