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

        // std::string fileName; // csv file containing the data we want to work with (IMU data)
        // inline static const std::string newVideoName = "../eltekor.avi"; // output video name and path
        // std::vector<std::vector<std::string>> content;

        // std::vector<std::vector<cv::Mat>> frames;

        // std::vector<float> accX;
        // std::vector<float> accY;
        // std::vector<float> accZ;

        std::string sPath;
        std::string outputFileName;

    public:

        EdgeDetector(const std::string &sPath, const std::string &outputFileName) : sPath(sPath), outputFileName(outputFileName) {}
        ~EdgeDetector() {}

        // Main functions
        void getEdges();
        void prewittEdgeDetector(cv::Mat& image, cv::Mat& output);

        // Helper functions
        // std::vector<int> getCameraIDs();
        // void showFrames() const;
};