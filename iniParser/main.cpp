#include "./utils/IniFile.h"
using namespace utils;

int main() {
    IniFile Inif;
    // 为什么不是 ./config.ini ？
    // 因为可执行文件路径是 iniParser/cmake-build-debug/iniParser ，
    // 而.ini文件路径是 iniParser/config.ini
    Inif.load("../config.ini");

    Inif.show();

    Inif.save("../tmp.ini");

    return 0;
}
