// CheckRecognition.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <iostream>
#include <set>
#include "utils.h"
#include "NoiseReducer.h"
using namespace cv;

const int THRESHOLD = 128;

void FindLines(const Mat &adaptiveImage)
{
    std::vector<Vec4i> vLines;
    HoughLinesP(adaptiveImage, vLines, CV_PI / 180, 50, 50, 10);
    Mat cdst;
    cvtColor(adaptiveImage, cdst, CV_GRAY2BGR);
    for (size_t i = 0; i < vLines.size(); i++)
    {
        Vec4i l = vLines[i];
        line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
    }
    imshow("detected lines", cdst);
    waitKey();
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
        SaveImage(imgGray, sBWImage);

        Mat normalizedImage;
        int iDepth = imgGray.depth();
        normalize(imgGray, normalizedImage, 255, 0, NORM_INF);
        std::string sNormImage = AddSuffix(sImagePath, "norm");
        SaveImage(normalizedImage, sNormImage);

        std::cout << "collumns - " << normalizedImage.cols << " rows - " << normalizedImage.rows << "\n";

        Mat binaryImage;
        threshold(normalizedImage, binaryImage, 128, 255, CV_THRESH_BINARY); //this one really nessecary for clear picture
        std::string sBinaryImage = AddSuffix(sImagePath, "binary");
        SaveImage(binaryImage, sBinaryImage);

        Mat adaptiveImage;
        adaptiveThreshold(binaryImage, adaptiveImage, 255, ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 11, 1);
        std::string sAdaptImage = AddSuffix(sImagePath, "adaptive");
        SaveImage(adaptiveImage, sAdaptImage);
        //NoiseReducer reducer;
        //reducer.RemoveNoise(adaptiveImage, MINIMAL_PIECE_SIZE);
        ////FindLines(adaptiveImage);
        //std::string sClearImage = AddSuffix(sImagePath, "clear");
        //SaveImage(adaptiveImage, sClearImage);
    }
    catch (cv::Exception &exception)
    {
        std::cout << "Exception caught: " << exception.what() << "\n";
    }

    //TODO: some algo for finding similiar pieces of pixels on image(for locating the exact letters and numbers)
    //maybe two parallel lines can be used to localize check from the whole picture
    return 0;
}

