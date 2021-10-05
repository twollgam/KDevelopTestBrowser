#include "utilities.h"

#include <fstream>

void trace(const std::string& text)
{
    std::ofstream("/tmp/TestBrowser.trace", std::ios::app) << text << std::endl; 
}

