#ifndef _UTIL_H_
#define _UTIL_H_

#include <string>
#include <sstream>

static inline std::string itos(int i) {
	std::ostringstream os;
	os << i;
	return os.str();
}

static inline const std::string changeExtName(const std::string &ori, const std::string &new_ext) {
	if (ori.rfind('.') == std::string::npos)
		return ori + "." + new_ext;
	else
		return ori.substr(0, ori.rfind('.')) + "." + new_ext;
}

static inline void replace_all_inplace(std::string &source, const std::string &pattern, const std::string &to) {
	size_t pattern_size = pattern.size();
	for (;;) {
		std::string::size_type pos(0);
		if ((pos = source.find(pattern)) != std::string::npos)
			source.replace(pos, pattern_size, to);
		else
			break;
	}
}

static inline std::string replace_all(std::string source, const std::string &pattern, const std::string &to) {
	replace_all_inplace(source, pattern, to);
	return source;
}

#endif
