#include "edge_detector.hpp"

void EdgeDetector::getEdges()
{
    for (const auto & entry : fs::directory_iterator(this->sPath))
    {
        std::string fileName = entry.path().filename().string();

        if(fileName.substr(fileName.size()-3).compare("png") == 0)
        {
            std::string outputNamePrewitt = this->outputFileName + "_" + fileName.substr(0, fileName.size()-4) + "_Prewitt.jpg";
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
            cv::imshow("(1) Original : (2) Prewitt Edge Detection : (3) Prewitt Edge Detection with NMS", combinedImage);
            cv::waitKey(0);
        }        
    }
}

void EdgeDetector::prewittEdgeDetectorWithNMS(cv::Mat& image, cv::Mat& outputPrewitt, cv::Mat& outputPrewittNMS)
{
    uint8_t magnitudes[image.rows][image.cols];
    uint8_t magnitudesAfterNMS[image.rows][image.cols];
    double gradientDirections[image.rows][image.cols];

    int kernelSize = 3;
    int halfKernelSize = kernelSize / 2;

    for(int i = halfKernelSize; i < image.rows - halfKernelSize; ++i) // vertical axis (rows)
    {
        for(int j = halfKernelSize; j < image.cols - halfKernelSize; ++j) // horizontal axis (columns)
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
            magnitudes[i][j] = (int)sqrt(pow(magnitudeX, 2) + pow(magnitudeY, 2));
            gradientDirections[i][j] = atan(magnitudeX/magnitudeY);
        }
    }

    cv::Mat(image.rows, image.cols, CV_8U, &magnitudes).copyTo(outputPrewitt); // deep copy

    // Algoritmus: Non-maxima suppression
    // 1 From each position (x,y), step in the two directions perpendicular to edge orientation Θ(x,y)
    // 2 Denote inital pixel (x,y) by C, the two neighbouring pixels in perpendicular directions by A and B
    // 3 If M(A) > M(C) or M(B) > M(C), discard pixel (x,y) by setting M(x, y) = 0

    for(int i = halfKernelSize; i < image.rows - halfKernelSize; ++i) // vertical axis (rows)
    {
        for(int j = halfKernelSize; j < image.cols - halfKernelSize; ++j) // horizontal axis (columns)
        {
            double perpendicularDirection = gradientDirections[i][j] + (M_PI/2);

            if(perpendicularDirection < M_PI/6)
            {
                magnitudesAfterNMS[i][j] = (magnitudes[i][j+1] > magnitudes[i][j]) || (magnitudes[i][j-1] > magnitudes[i][j]) ? 0 : magnitudes[i][j];
            }
            else if(perpendicularDirection > M_PI/6)
            {
                magnitudesAfterNMS[i][j] = (magnitudes[i+1][j] > magnitudes[i][j]) || (magnitudes[i-1][j] > magnitudes[i][j]) ? 0 : magnitudes[i][j];
            }
            else
            {
                magnitudesAfterNMS[i][j] = (magnitudes[i+1][j+1] > magnitudes[i][j]) || (magnitudes[i-1][j-1] > magnitudes[i][j]) ? 0 : magnitudes[i][j];
            }
        }
    }

    cv::Mat(image.rows, image.cols, CV_8U, &magnitudesAfterNMS).copyTo(outputPrewittNMS); // deep copy
}