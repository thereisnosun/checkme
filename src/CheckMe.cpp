// CheckRecognition.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <iostream>
#include <set>
#include "utils.h"
#include "NoiseReducer.h"
using namespace cv;

const int THRESHOLD = 128;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Please provide the path to the image!\n";
        return 1;
    }
   
    try
    {
        std::string sImagePath(argv[1]);
        Mat imgRGB = imread(sImagePath);
        Mat imgGray;
        cvtColor(imgRGB, imgGray, CV_RGB2GRAY);
        std::string sBWImage = AddSuffix(sImagePath, "bw");
        //if (!imwrite(sBWImage, imgGray))
        //{
        //    std::cout << "Failed to save image\n";
        //}

        Mat normalizedImage;
        imgGray.channels();
        int iDepth = imgGray.depth();
        std::cout << "Depth is - " << iDepth << "\n";
        normalize(imgGray, normalizedImage, 0, 255, NORM_MINMAX, iDepth);
        //equalizeHist(adaptiveImage, normalizedImage);
        std::string sNormImage = AddSuffix(sImagePath, "norm");
        //if (!imwrite(sNormImage, normalizedImage))
        //{
        //    std::cout << "Failed to save image\n";
        //}

        Mat adaptiveImage;
        std::cout << "collumns - " << normalizedImage.cols << " rows - " << normalizedImage.rows << "\n";
        
        adaptiveThreshold(normalizedImage, adaptiveImage, 255, ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 11, 1);
        NoiseReducer reducer;
        reducer.RemoveNoise(adaptiveImage, MINIMAL_PIECE_SIZE);
        std::string sAdaptImage = AddSuffix(sImagePath, "adaptive");
        if (!imwrite(sAdaptImage, adaptiveImage))
        {
            std::cout << "Failed to save image\n";
        }

    }
    catch (cv::Exception &exception)
    {
        std::cout << "Exception caught: " << exception.what() << "\n";
    }

    //TODO: some algo for finding similiar pieces of pixels on image(for locating the exact letters and numbers)
    //maybe two parallel lines can be used to localize check from the whole picture
    return 0;
}

