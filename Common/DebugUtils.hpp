#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

namespace DebugUtils {

// 颜色代码
const std::string RESET = "\033[0m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[36m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";

// 获取当前文件位置
std::string getFileLocation(const std::string &file, int line) {
    std::ostringstream oss;
    oss << file << ":" << line;
    return oss.str();
}

// 打印调试信息
template <typename... Args> void printDebug(const std::string &file, int line, Args... args) {
    std::ostringstream oss;
    ((oss << args << " "), ...);
    std::cout << BLUE << "[DEBUG] " << getFileLocation(file, line) << ": " << oss.str() << RESET << std::endl;
}

// 打印成功信息
template <typename... Args> void printPass(const std::string &file, int line, Args... args) {
    std::ostringstream oss;
    ((oss << args << " "), ...);
    std::cout << GREEN << "[PASS] " << getFileLocation(file, line) << ": " << oss.str() << RESET << std::endl;
}

// 打印错误信息
template <typename... Args> void printError(const std::string &file, int line, Args... args) {
    std::ostringstream oss;
    ((oss << args << " "), ...);
    std::cerr << RED << "[ERROR] " << getFileLocation(file, line) << ": " << oss.str() << RESET << std::endl;
    exit(0);
}

// TODO() 函数
void TODO(const std::string &file, int line) {
    std::cerr << YELLOW << "[TODO] " << getFileLocation(file, line) << ": This feature is not yet implemented." << RESET
              << std::endl;
    exit(0);
}

} // namespace DebugUtils

// 宏定义，方便使用
#define DEBUG(...) DebugUtils::printDebug(__FILE__, __LINE__, __VA_ARGS__)
#define PASS(...) DebugUtils::printPass(__FILE__, __LINE__, __VA_ARGS__)
#define ERROR(...) DebugUtils::printError(__FILE__, __LINE__, __VA_ARGS__)
#define TODO() DebugUtils::TODO(__FILE__, __LINE__)

#endif // DEBUG_UTILS_H
