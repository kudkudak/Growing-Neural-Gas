#ifndef LOG_HPP
#define LOG_HPP

#include <iostream>
#include <string>
#include <sstream>

#ifdef DEBUG
#define LOG(logger, level, text) logger.log(level, text);
#define DBG(logger, level, text) logger.log(level, text);
#else
#define LOG(logger, level, text) logger.log(level, text);
#define DBG(logger, level, text)
#endif

class LogLevel {
public:

    static const int NO_LOGGING  = 0;
    static const int FATAL       = 1;
    static const int ERR         = 2;
    static const int WARNING     = 3;
    static const int INFO        = 4;
    static const int DEBUG       = 5;
    static const int TRACE       = 6;

};

class Logger {
public:

    int verbosity;

    Logger() {
        verbosity = LogLevel::INFO;
    }

    void log(int level, std::string text) {
        if (level <= verbosity) {
            std::cout << text << std::endl << std::flush;
        }
    }

    void log(int level, int number) {
        if (level <= verbosity) {
            std::stringstream ss;
            ss << number;
            std::cout << ss.str() << std::endl << std::flush;
        }
    }

    void log(int level, double number) {
        if (level <= verbosity) {
            std::stringstream ss;
            ss << number;
            std::cout << ss.str() << std::endl << std::flush;
        }
    }

};

#endif /* LOG_HPP */
