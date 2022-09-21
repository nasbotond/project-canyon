#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <string>

#include "edge_detector.hpp"

int main(int argc, char* argv[]) 
{
    if (argc < 3) 
    {
        std::cerr << "Usage: " << argv[0] << " sPath outputFileName" << std::endl;
        return 1;
    }

    std::string sPath = argv[1];
    std::string outputFileName = argv[2];

    // check if path is good

    std::cout << "------------------ Parameters -------------------" << std::endl;
    std::cout << "Path = " << sPath << std::endl;
    std::cout << "Output filename = " << outputFileName << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;


    // Input
    EdgeDetector detector = EdgeDetector(sPath, outputFileName);
    

    try 
    {
        detector.getEdges();
    } 
    catch(const std::exception &e) 
    {
        std::cerr << e.what() << '\n';
        return -1;
    }

    return 0;
}