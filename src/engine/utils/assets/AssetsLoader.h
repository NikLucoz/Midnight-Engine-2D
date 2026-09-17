#pragma once
#include <string>
#include <vector>

class Assets;

class AssetsLoader
{
    static std::vector<std::string> splitBySpace(const std::string& str);
public:
    static void loadAssetsFromFile(const std::string &filePath);
};