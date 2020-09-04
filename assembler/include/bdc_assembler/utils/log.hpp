#ifndef BDCA_LOG_H
#define BDCA_LOG_H

#include <iostream>

#define LOG_M(s) std::cout << s << '\n'
#define WARN_M(s) std::cout << "WARNING: " << s << '\n';
#define ERR_M(s) std::cerr << "Error: " << s << '\n';

#ifdef DEBUG
#define DEBUG_M(s) std::cout << "DEBUG: " << s << '\n'
#else
#define DEBUG_M(s) do{}while(0)
#endif

#endif /* BDCA_LOG_H */