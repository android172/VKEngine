#include "logger.hpp"

Logger::Logger(/* args */) {}

Logger::~Logger() {}

void Logger::fatal(std::string message) {
    Platform::Console::write("FATAL ERROR :: " + message, 1);
}

void Logger::error(std::string message) {
    Platform::Console::write("ERR :: " + message, 2);
}

void Logger::warning(std::string message) {
#if LOG_WARNING_ENABLED
    Platform::Console::write("WAR :: " + message, 3);
#endif
}

void Logger::log(std::string message) {
#if LOG_INFO_ENABLED
    Platform::Console::write("INF :: " + message, 4);
#endif
}

void Logger::debug(std::string message) {
#if LOG_DEBUG_ENABLED
    Platform::Console::write("DEB :: " + message, 5);
#endif
}

void Logger::verbose(std::string message) {
#if LOG_VERBOSE_ENABLED
    Platform::Console::write("VER :: " + message, 0);
#endif
}
