#include "edge_detector.hpp"

void EdgeDetector::getEdges()
{
    for (const auto & entry : fs::directory_iterator(this->sPath))
    {
        std::string fileName = entry.path().filename().string();

        if(fileName.substr(fileName.size()-3).compare("png") == 0)
        {
            std::string outputName = this->outputFileName + "_" + fileName.substr(0, fileName.size()-4) + ".jpg";
        
            std::cout << entry.path().string() << std::endl;
            std::cout << "out = " << outputName << std::endl;

            cv::Mat image = cv::imread(entry.path().string(), cv::IMREAD_GRAYSCALE);
            // cv::Mat output = cv::Mat::zeros(image.rows, image.cols, CV_8UC1);
            // cv::Mat output = image.clone();

            cv::Mat output;

            prewittEdgeDetector(image, output);            

            cv::imwrite(outputName, output);

            cv::namedWindow("Naive", cv::WINDOW_AUTOSIZE);
            cv::imshow("Naive", output);

            cv::waitKey(0);
        }        
    }
}

void EdgeDetector::prewittEdgeDetector(cv::Mat& image, cv::Mat& output)
{
    uint8_t out[image.rows][image.cols];

    int kernelSize = 3;
    int halfKernelSize = kernelSize / 2;

    for (int i = halfKernelSize; i < image.rows - halfKernelSize; ++i) // vertical axis
    {
        for (int j = halfKernelSize; j < image.cols - halfKernelSize; ++j) // horizontal axis
        {
            int magnitudeX = 0;
            int magnitudeY = 0;

            for(int u = -halfKernelSize; u <= halfKernelSize; ++u)
            {
                for(int v = -halfKernelSize; v <= halfKernelSize; ++v)
                {
                    magnitudeX += v*(int)image.at<uchar>(i+u,j+v);
                    magnitudeY += u*(int)image.at<uchar>(i+u,j+v);
                }
            }
            // output.at<uchar>(i, j) = (int)sqrt(pow(magnitudeX, 2) + pow(magnitudeY, 2));
            out[i][j] = (int)sqrt(pow(magnitudeX, 2) + pow(magnitudeY, 2));            
        }
    }

    // output = cv::Mat(image.rows, image.cols, CV_8U, &out); // shallow copy
    cv::Mat(image.rows, image.cols, CV_8U, &out).copyTo(output); // deep copy
}