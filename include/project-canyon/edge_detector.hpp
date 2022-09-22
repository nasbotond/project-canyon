#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "opencv2/opencv.hpp"
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <numeric>
#include <vector>
#include <cmath>
namespace fs = std::filesystem;

class EdgeDetector
{
    private:
    
        std::string sPath;
        std::string outputFileName;

    public:

        EdgeDetector(const std::string &sPath, const std::string &outputFileName) : sPath(sPath), outputFileName(outputFileName) {}
        ~EdgeDetector() {}

        // Main functions
        void getEdges();
        void prewittEdgeDetectorWithNMS(cv::Mat& image, cv::Mat& outputPrewitt, cv::Mat& outputPrewittNMS);
};