#include <map>
typedef std::map<const char *, int> Hash;

void assert(bool __expr, const char *__str__ = "", Hash params = Hash()) {
	if(!__expr)	{
		fprintf(stderr, "\x1b[1;31mRuntime error: \x1b[0m");
		fprintf(stderr, "\x1b[1;37m%s\x1b[0m\n", __str__);
		for(const auto &param:params)
			fprintf(stderr, "\t%s: \t%d\n", param.first, param.second);
		exit(1);
	}
}

void debug(bool __expr, const char *__str__ = "", Hash params = Hash()) {
	if(__expr && _DEBUG_)	{
		fprintf(stderr, "\x1b[1;32mDebug: \x1b[0m");
		fprintf(stderr, "\x1b[1;37m%s\x1b[0m\n", __str__);
		for(const auto &param:params)
			fprintf(stderr, "\t%s: \t%d\n", param.first, param.second);
	}
}

void refute(bool __expr, const char *__str__ = "", Hash params = Hash()) {
	if(__expr)	{
		fprintf(stderr, "\x1b[1;31mRuntime error: \x1b[0m");
		fprintf(stderr, "\x1b[1;37m%s\x1b[0m\n", __str__);
		for(const auto &param:params)
			fprintf(stderr, "\t%s: \t%d\n", param.first, param.second);
		exit(1);
	}
}

void note(bool __expr, const char *__str__ = "",  Hash params = Hash()) {
	if(__expr)	
	{
		fprintf(stderr, "\x1b[1;33mNote: \x1b[0m");
		fprintf(stderr, "\x1b[1;37m%s\x1b[0m\n", __str__);
		for(const auto &param:params)
			fprintf(stderr, "\t%s: \t%d\n", param.first, param.second);
	}
}
