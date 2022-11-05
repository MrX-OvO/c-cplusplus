#include "./utils/logger.h"
using namespace utils;

int main() {
    Logger::instance()->open("../test.log");
    Logger::instance()->max(1024);

    debug("hello world");
    debug("name is %s, age is %d", "xyz", 10);
    info("info message");
    warn("info message");
    error("error message");
    fatal("fatal message");

    return 0;
}
