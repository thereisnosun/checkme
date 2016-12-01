// CheckRecognition.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <iostream>
#include <set>
#include <cmath>
#include "utils.h"
#include "NoiseReducer.h"
using namespace cv;

const int THRESHOLD = 128;
const int FLUFF = 30;

void SaveImage(const Mat &image, const std::string &sImageName)
{
#ifndef DEBUG
    if (!imwrite(sImageName, image))
    {
        std::cout << "Failed to save image\n";
    }
#endif
}

void FindLines(const Mat &adaptiveImage)
{
    std::vector<Vec4i> vLines;
    vLines.reserve(adaptiveImage.rows*adaptiveImage.cols);
    HoughLinesP(adaptiveImage, vLines, 1, CV_PI / 180, 128, 30, 10);
    Mat cdst(adaptiveImage.size(), CV_8UC1);
    cdst = Scalar(255);
    //Mat cdst;
    //cvtColor(adaptiveImage, cdst, CV_GRAY2BGR);

    Vec4i longestLine;
    int iHeight = adaptiveImage.rows;
    std::cout << "Line detection is done. Total number of lines is - " << vLines.size() << "\n";
    //first take care of vertical lines
    for (size_t i = 0; i < vLines.size(); i++)
    {
        Vec4i curLine = vLines[i];
        
        int x1 = curLine[0];
        int y1 = curLine[1];

        int x2 = curLine[2];
        int y2 = curLine[3];
      
        if (y1 < FLUFF && iHeight - y2 < FLUFF ||
            iHeight - y1 < FLUFF && y2 < FLUFF)
        {
            longestLine = curLine;
            line(cdst, Point(x1, y1), Point(x2, y2), Scalar(0, 0, 255), 3, CV_AA);
            std::cout << "Drawing the line... Iteration number " << i << ". Point 1 - ("
                << x1 << ", " << y1 << "). Point 2 - (" << x2 << ", " << y2 << ").\n";
        }
       
        //line(cdst, Point(curLine[0], curLine[1]), Point(curLine[2], curLine[3]), Scalar(0, 0, 255), 3, CV_AA);
    }
    //TODO: handle horizontal lines

    SaveImage(adaptiveImage, "lines.jpg");
    namedWindow("Rock", WINDOW_NORMAL);
    resizeWindow("Rock", adaptiveImage.cols/5, adaptiveImage.rows/5);
    imshow("Rock", cdst);
    waitKey();
     
}

//TODO:
//Write test in parallel, practice to use cppunit, gmock

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
        threshold(normalizedImage, binaryImage, 127, 255, CV_THRESH_BINARY); //this one really nessecary for clear picture
        std::string sBinaryImage = AddSuffix(sImagePath, "binary");
        SaveImage(binaryImage, sBinaryImage);

        Mat adaptiveImage;
        adaptiveThreshold(binaryImage, adaptiveImage, 255, ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 11, 1);
        std::string sAdaptImage = AddSuffix(sImagePath, "adaptive");
        SaveImage(adaptiveImage, sAdaptImage);
        
        FindLines(adaptiveImage);

        //NoiseReducer reducer;
        //reducer.RemoveNoise(adaptiveImage, MINIMAL_PIECE_SIZE);
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

