#include "edge_detector.hpp"

void EdgeDetector::getEdges()
{
    for (const auto & entry : fs::directory_iterator(this->sPath))
    {
        std::string fileName = entry.path().filename().string();

        if(fileName.substr(fileName.size()-3).compare("png") == 0)
        {
            std::string outputNamePrewitt = this->outputFileName + "_" + fileName.substr(0, fileName.size()-4) + "gradientMagnitude.jpg";
            std::string outputNamePrewittNMS = this->outputFileName + "_" + fileName.substr(0, fileName.size()-4) + "_PrewittNMS.jpg";
            std::string outputNameCombined = this->outputFileName + "_" + fileName.substr(0, fileName.size()-4) + "_combined.jpg";
        
            std::cout << "------------------ Parameters -------------------" << std::endl;
            std::cout << "Path = " << sPath << std::endl;
            std::cout << "Output 1 = " << outputNamePrewitt << std::endl;
            std::cout << "Output 2 = " << outputNamePrewittNMS << std::endl;
            std::cout << "Output 3 = " << outputNameCombined << std::endl;
            std::cout << "-------------------------------------------------" << std::endl;

            // Read image into Mat
            cv::Mat image = cv::imread(entry.path().string(), cv::IMREAD_GRAYSCALE);

            cv::Mat outputPrewitt;
            cv::Mat outputPrewittNMS;

            prewittEdgeDetectorWithNMS(image, outputPrewitt, outputPrewittNMS);

            // Create combined image to view results side-by-side
            cv::Mat combinedImage(image.rows, 3*image.cols, CV_8U);
            cv::Mat left(combinedImage, cv::Rect(0, 0, image.cols, image.rows));
            image.copyTo(left);
            cv::Mat center(combinedImage, cv::Rect(image.cols, 0, image.cols, image.rows));
            outputPrewitt.copyTo(center);
            cv::Mat right(combinedImage, cv::Rect(2*image.cols, 0, image.cols, image.rows));
            outputPrewittNMS.copyTo(right);

            // Save results (write to file)
            cv::imwrite(outputNamePrewitt, outputPrewitt);
            cv::imwrite(outputNamePrewittNMS, outputPrewittNMS);
            cv::imwrite(outputNameCombined, combinedImage);

            // Show combined image
            cv::imshow("(1) Original : (2) Gradient Magnitude : (3) Prewitt Edge Detection with NMS", combinedImage);
            cv::waitKey(0);
        }        
    }
}

void EdgeDetector::prewittEdgeDetectorWithNMS(cv::Mat& image, cv::Mat& outputPrewitt, cv::Mat& outputPrewittNMS)
{
    std::vector<uint8_t> magnitudes(image.rows*image.cols);
    std::vector<uint8_t> magnitudesAfterNMS(image.rows*image.cols);
    std::vector<double> gradientDirections(image.rows*image.cols);

    int kernelSize = 3;
    int halfKernelSize = kernelSize / 2;

    for(int i = halfKernelSize; i < image.rows - halfKernelSize; ++i) // vertical axis (rows)
    {
        for(int j = halfKernelSize; j < image.cols - halfKernelSize; ++j) // horizontal axis (columns)
        {
            double magnitudeX = 0;
            double magnitudeY = 0;

            for(int u = -halfKernelSize; u <= halfKernelSize; ++u)
            {
                for(int v = -halfKernelSize; v <= halfKernelSize; ++v)
                {
                    magnitudeX += (double)v*(double)image.at<uchar>(i+u,j+v);
                    magnitudeY += (double)u*(double)image.at<uchar>(i+u,j+v);
                }
            }
            int magnitude = (int)sqrt(pow(magnitudeX, 2) + pow(magnitudeY, 2));
            magnitude = magnitude > 255 ? 255 : magnitude;
            magnitude = magnitude < 0 ? 0 : magnitude;
            magnitudes.at(i*image.cols + j) = magnitude;

            gradientDirections.at(i*image.cols+j) = magnitudeY != 0 ? atan(magnitudeX/magnitudeY) : atan(magnitudeX/(magnitudeY+0.0000001));
        }
    }

    cv::Mat(image.rows, image.cols, CV_8U, magnitudes.data()).copyTo(outputPrewitt); // deep copy

    // Algoritmus: Non-maxima suppression
    // 1 From each position (x,y), step in the two directions perpendicular to edge orientation ??(x,y)
    // 2 Denote inital pixel (x,y) by C, the two neighbouring pixels in perpendicular directions by A and B
    // 3 If M(A) > M(C) or M(B) > M(C), discard pixel (x,y) by setting M(x, y) = 0

    double angle = 22.5*(M_PI/180.0);

    for(int i = halfKernelSize; i < image.rows - halfKernelSize; ++i) // vertical axis (rows)
    {
        for(int j = halfKernelSize; j < image.cols - halfKernelSize; ++j) // horizontal axis (columns)
        {
            double perpendicularDirection = gradientDirections.at(i*image.cols+j);

            if(abs(perpendicularDirection) <= angle)
            {
                // horizontal
                magnitudesAfterNMS.at(i*image.cols+j) = (magnitudes.at((i+1)*image.cols+j) > magnitudes.at(i*image.cols+j)) || (magnitudes.at((i-1)*image.cols+j) > magnitudes.at(i*image.cols+j)) ? 0 : magnitudes.at(i*image.cols+j);
                
            }
            else if(abs(perpendicularDirection) >= (M_PI/2 - angle) && abs(perpendicularDirection) <= (M_PI/2 + angle))
            {
                // vertical
                magnitudesAfterNMS.at(i*image.cols+j) = (magnitudes.at(i*image.cols+j+1) > magnitudes.at(i*image.cols+j)) || (magnitudes.at(i*image.cols+j-1) > magnitudes.at(i*image.cols+j)) ? 0 : magnitudes.at(i*image.cols+j);
            }
            else if(perpendicularDirection > -angle && perpendicularDirection < -(M_PI/2 - angle))
            {
                // negative diagonal
                magnitudesAfterNMS.at(i*image.cols+j) = (magnitudes.at((i-1)*image.cols+j+1) > magnitudes.at(i*image.cols+j)) || (magnitudes.at((i+1)*image.cols+j-1) > magnitudes.at(i*image.cols+j)) ? 0 : magnitudes.at(i*image.cols+j);
                
            }
            else
            {
                // positive diagonal
                magnitudesAfterNMS.at(i*image.cols+j) = (magnitudes.at((i+1)*image.cols+j+1) > magnitudes.at(i*image.cols+j)) || (magnitudes.at((i-1)*image.cols+j-1) > magnitudes.at(i*image.cols+j)) ? 0 : magnitudes.at(i*image.cols+j);
            }
        }
    }

    cv::Mat(image.rows, image.cols, CV_8U, magnitudesAfterNMS.data()).copyTo(outputPrewittNMS); // deep copy
}