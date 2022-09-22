#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#ifdef M_PI
#define M_PI 3.1415926535897932384626
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
#include <stdint.h>
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