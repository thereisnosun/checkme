#include "utils.h"

#include <iostream>

std::string AddSuffix(const std::string &sString, const std::string &sSuffix)
{
    static const auto reg = std::regex("([^\.]+)\.(.*)");

    std::string sFileName = sString;
    std::smatch base_match;
    if (std::regex_match(sString, base_match, reg))
    {
        if (base_match.size() == 3)
        {
            std::ssub_match name = base_match[1];
            std::ssub_match extension = base_match[2];
            sFileName = name.str() + "_" + sSuffix + "." + extension.str();
        }
    }

    return std::move(sFileName);
}


void SaveImage(const Mat &image, const std::string &sImageName)
{
#ifndef DEBUG
    if (!imwrite(sImageName, image))
    {
        std::cout << "Failed to save image\n";
    }
#endif
}
