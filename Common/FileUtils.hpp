#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include "DebugUtils.hpp"
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace FileUtils {

// 检查文件是否存在
bool fileExists(const std::string &filename) {
    std::ifstream file(filename);
    return file.good();
}

// 读取文件内容到字符串
std::string readFileToString(const std::string &filename) {
    if (!fileExists(filename)) {
        ERROR("file not exists : ", filename);
    }
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// 读取文件内容到字符串向量，每行一个元素
std::vector<std::string> readFileToVector(const std::string &filename) {
    if (!fileExists(filename)) {
        ERROR("file not exists : ", filename);
    }
    std::ifstream file(filename);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

// 写入字符串到文件
bool writeStringToFile(const std::string &filename, const std::string &content) {
    std::ofstream file(filename);
    if (!file) {
        return false;
    }
    file << content;
    return true;
}

// 追加字符串到文件
bool appendStringToFile(const std::string &filename, const std::string &content) {
    std::ofstream file(filename, std::ios::app);
    if (!file) {
        return false;
    }
    file << content;
    return true;
}

// 获取文件大小
size_t getFileSize(const std::string &filename) {
    std::ifstream file(filename, std::ifstream::ate | std::ifstream::binary);
    return file.tellg();
}

// 读取文件内容到二进制向量
std::vector<uint8_t> readFileToBinaryVector(const std::string &filename) {
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(file), {});
    return buffer;
}

} // namespace FileUtils

#endif // FILE_UTILS_H
