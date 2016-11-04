#pragma once

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>

#include <queue>
#include <unordered_map>

using namespace cv;
const int BLACK_PIXEL = 0;
const int WHITE_PIXEL = 255;
const int MINIMAL_PIECE_SIZE = 10;

// if all blck pixels neighbours are included in to noise vector, then there is no path from this particular pixel
struct PixelNeightbours
{ //TODO: store each neighbour index
    PixelNeightbours() :
        m_bLeft(false), m_bRight(false), m_bDown(false), m_bUp(false),
        m_bLeftUp(false), m_bLeftDown(false), m_bRightUp(false), m_bRightDown(false)
    {

    }

    bool IsFinished() const
    {
        return m_bLeft && m_bRight && m_bDown && m_bUp &&
            m_bLeftUp && m_bLeftDown && m_bRightUp && m_bRightDown;
    }

    bool m_bLeft;
    bool m_bRight;
    bool m_bDown;
    bool m_bUp;
    bool m_bLeftUp;
    bool m_bLeftDown;
    bool m_bRightUp;
    bool m_bRightDown;

    std::vector<int> m_vNoiseNeightbour;
};

struct ImageSize
{
    int iRows;
    int iCols;
};

class NoiseReducer
{
public:
    NoiseReducer();
    void RemoveNoise(Mat &srcImage, int iMinPixels);
private:
    bool CheckIfGraphEnded(const std::unordered_map<int, PixelNeightbours> &pixelsTable) const;
    void RemoveNoisePixels(const std::vector<int> &vPixels, uchar* imgData) const;
    bool CheckPixelNeightbour(std::vector<int> &noisePixels, std::queue<int> &checkQueue, 
                              uchar *internalPtr, int iCurIndex, int iNewIndex);
    PixelNeightbours DefinePixelNeighbours(std::vector<int> &noisePixels, std::queue<int> &checkQueue, uchar *internalPtr,
                                           int iCurrentPixel);
private:
    ImageSize m_sImageSize;
    std::vector<int> m_exploredNodes; //which data structures fits for this purposes best ?
};

