// CheckRecognition.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <iostream>
#include <set>
#include <cmath>
#include <map>
#include <algorithm>
#include "utils.h"
#include "NoiseReducer.h"

#include "../squares.h"
using namespace cv;

const int THRESHOLD = 128;

const int EPS_X = 1;
const int HEIGHT_EPS = 20;

const int MINIMUM_DOTS = 3;
const int DOTS_CHECKED = 5;

struct YLine
{
    int iYcord1;
    int iYcord2;
    Vec4i m_Line;
    YLine(int y1, int y2, Vec4i line): 
        iYcord1(y1), iYcord2(y2), m_Line(line)
    {

    }
    friend bool operator<(const YLine &y1, const YLine &y2);
    friend bool operator==(const YLine &y1, const YLine &y2);
    friend std::ostream& operator<<(std::ostream &stream, const YLine &y);
};

bool operator<(const YLine &y1, const YLine &y2)
{
    if (y2.iYcord1 - y1.iYcord1 > EPS_X && y2.iYcord2 - y1.iYcord2 > EPS_X)
        return true;

    return false;
}

bool operator==(const YLine &y1, const YLine &y2)
{
    if (abs(y2.iYcord1 - y1.iYcord1) < EPS_X && abs(y2.iYcord2 - y1.iYcord2) < EPS_X)
        return true;

    return false;
}
std::ostream& operator<<(std::ostream &stream, const YLine &y)
{
    stream << "(" << y.iYcord1 << ", " << y.iYcord2 << ")";
    return stream;
}

void FindLinesOpenCV(const Mat &srcImage)
{
    Mat dst, cdst;
    Canny(srcImage, dst, 0, 128, 3);
    cvtColor(dst, cdst, COLOR_GRAY2BGR);

    std::multimap<YLine, Vec4i> mYLines;
    std::set<YLine> sUniqueYLines;
    
    std::vector<Vec4i> lines;
    HoughLinesP(dst, lines, 1, CV_PI / 180, 50, 50, 10);

    std::cout << "Number of detected lines is - " << lines.size() << "\n";
    for (size_t i = 0; i < lines.size(); i++)
    {
        Vec4i curLine = lines[i];
        int x1 = curLine[0];
        int y1 = curLine[1];

        int x2 = curLine[2];
        int y2 = curLine[3];

       mYLines.insert(std::make_pair(YLine{x1, x2, curLine}, curLine));
       sUniqueYLines.insert(YLine{ x1, x2, curLine });

    }
    std::cout << "Number of paralel lines is - " << sUniqueYLines.size() << " \n";


    std::vector<YLine> vBillLines;
    int iHeight = srcImage.rows;
    for (auto itCurr = sUniqueYLines.begin(); itCurr != sUniqueYLines.end(); ++itCurr)
    {
        YLine CurAngle = *itCurr;
        auto paralelLines = mYLines.equal_range(CurAngle);

        if (paralelLines.first == mYLines.end() || paralelLines.second == mYLines.end())
        {
            std::cout << "Skipping lines with " << CurAngle << ". Not found in the map. \n";
            continue;
        }
        int iRangeSize = std::distance(paralelLines.first, paralelLines.second);

    //    std::cout << "Current lines with " << CurAngle << ".Range size is - " << iRangeSize << "\n";
        if (iRangeSize < 5)
        {
            std::cout << "Skipping lines with " << CurAngle << ".Range size is - " << iRangeSize << "\n";
            continue;
        }

        int iMax = 0;
        int iMin = 0;
        int xCoord = 0;
        std::vector<std::pair<int, int>> vLineSegments;
        for (auto itRange = paralelLines.first; itRange != paralelLines.second; ++itRange)
        {
            Vec4i curLine = itRange->second;
            int x1 = curLine[0];
            int y1 = curLine[1];

            int x2 = curLine[2];
            int y2 = curLine[3];

            if (y1 < iMin)
                iMin = y1;

            if (y2 < iMin)
                iMin = y2;

            if (y1 > iMax)
                iMax = y1;

            if (y2 > iMax)
                iMax = y2;
            vLineSegments.push_back(std::make_pair(y1, y2));

            xCoord = abs(x1 + x2) / 2;
        }
        

        if (iHeight - (iMax - iMin) < HEIGHT_EPS)
        {
            int iTotalLen = 0;
            for (auto itCurr = vLineSegments.begin(); itCurr != vLineSegments.end(); ++itCurr)
            {
                int iCurLen = abs(itCurr->first - itCurr->second);
                iTotalLen += iCurLen;
            }

            std::cout << "Current length - " << iTotalLen << "\n";
            if (iTotalLen > iHeight /2)
                line(cdst, Point(xCoord, iMax), Point(xCoord, iMin), Scalar(0, 0, 255), 3, LINE_AA);

            //int iCurrentDot = iHeight / DOTS_CHECKED;
            //int iDotsFound = 0;
            //while (iCurrentDot < iHeight)
            //{
            //    bool bIsPresent = false;

            //    for (auto itCurr = vLineSegments.begin(); itCurr != vLineSegments.end(); ++itCurr)
            //    {
            //        int iMaxY = max(itCurr->first, itCurr->second);
            //        int iMinY = min(itCurr->first, itCurr->second);
            //        if (iCurrentDot < iMaxY && iCurrentDot > iMinY)
            //        {
            //            bIsPresent = true;
            //            break;
            //        }
            //    }

            //    if (bIsPresent)
            //        ++iDotsFound;
            //    iCurrentDot += iCurrentDot;
            //}

            //std::cout << "Dots count is - " << iDotsFound << "\n";
            //if (iDotsFound > MINIMUM_DOTS)
            //    line(cdst, Point(xCoord, iMax), Point(xCoord, iMin), Scalar(0, 0, 255), 3, LINE_AA);
            
        }

    }
    SaveImage(cdst, "hough.jpg");

    imshow("source", srcImage);
    imshow("detected lines", cdst);
   
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
        
        FindLinesOpenCV(adaptiveImage);

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

