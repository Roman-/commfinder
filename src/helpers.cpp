#include <helpers.h>

#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <easylogging++.h>
#include <chrono>
using namespace std;

std::string getFileContents(std::string_view filename, bool quiet) {
    std::ifstream file(filename.data());
    if (!file.is_open()) {
        LOG_IF(!quiet, ERROR) << "getFileContents: can\'t open file " << filename;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();

    file.close();
    return buffer.str();
}

bool saveToFile(std::string_view path, const std::string& content, bool append) {
    std::ofstream outfile;
    outfile.open(path.data(), append ? std::ios_base::app : std::ios_base::out);
    if (!outfile.is_open())
        return false;

    outfile << content;
    outfile.close();
    return true;
}

std::vector<std::string> splitString(const std::string inputString, char c) {
    std::vector<std::string> result;
    std::istringstream f(inputString);
    string part;
    while (getline(f, part, c)) {
        result.push_back(part);
    }
    return result;
}

string joinStrings(const std::vector<string> &strings, char c) {
    std::stringstream ss;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (0 != i)
            ss << c;
        ss << strings[i];
    }
    return ss.str();
}
