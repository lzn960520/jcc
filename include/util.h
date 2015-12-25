#ifndef _UTIL_H_
#define _UTIL_H_

#include <string>
#include <sstream>
#include <cmath>

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

static inline std::string getTempName(const std::string &ext = "") {
	std::string ans = "/tmp/";
	for (int i = 0; i < 8; i++) {
		char c = rand();
		while (!(isalpha(c) || isdigit(c)))
			c = rand();
		ans += c;
	}
	ans += ext;
	return ans;
}

inline std::string getFilename(const std::string &path) {
	if (path.rfind('/') == std::string::npos)
		return path;
	else
		return path.substr(path.rfind('/') + 1);
}

inline std::string getDir(const std::string &path) {
	if (path.rfind('/') == std::string::npos)
		return "";
	else
		return path.substr(0, path.rfind('/')) + "/";
}

inline std::string getAbsoluteDir(const std::string &cwd, const std::string &path) {
	if (path[0] == '/')
		return path;
	else
		return cwd + getDir(path);
}

inline std::string resolveRelativePath(const std::string &path, const std::string &relpath) {
	if (relpath[0] == '/')
		return relpath;
	else
		return getDir(path) + relpath;
}

inline std::string replaceExt(const std::string &path, const std::string &newExt) {
	if (path.rfind(".") == std::string::npos)
		return path + "." + newExt;
	else
		return path.substr(0, path.rfind(".") + 1) + newExt;
}

#endif
