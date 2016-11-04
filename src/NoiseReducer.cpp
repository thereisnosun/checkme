#include "NoiseReducer.h"

#include <queue>
#include <unordered_map>


NoiseReducer::NoiseReducer()
{

}

bool NoiseReducer::CheckIfGraphEnded(const std::unordered_map<int, PixelNeightbours> &pixelsTable) const
{
    bool bIsEnded = true;

    for (auto currIt = pixelsTable.begin(); currIt != pixelsTable.end(); ++currIt)
    {
        PixelNeightbours currPixel = currIt->second;
        if (!currPixel.IsFinished())
        {
            bIsEnded = false;
            break;
        }
    }

    return bIsEnded;
}

void NoiseReducer::RemoveNoisePixels(const std::vector<int> &vPixels, uchar* imgData) const
{
    for (auto currIt = vPixels.begin(); currIt != vPixels.end(); ++currIt)
    {
        int iIndex = *currIt;
        imgData[iIndex] = WHITE_PIXEL;
    }
}

bool NoiseReducer::CheckPixelNeightbour(std::vector<int> &noisePixels, std::queue<int> &checkQueue,
                                        uchar *internalPtr, int iCurIndex, int iNewIndex)
{
    //TODO: handle case, when down left and its considered as last pixel from current line, maybe handle this cases by hand in calling function?
    bool bTrueNeighbour = false;
    const static int iSize = m_sImageSize.iCols*m_sImageSize.iRows;
    if (iNewIndex >= iSize || iNewIndex < 0)
        return true;



    if (std::end(m_exploredNodes) == std::find(m_exploredNodes.begin(), m_exploredNodes.end(), iNewIndex))
    {
        m_exploredNodes.push_back(iNewIndex);
        if (internalPtr[iNewIndex] == BLACK_PIXEL)
        {
            checkQueue.push(iNewIndex);
            noisePixels.push_back(iNewIndex);
        }
        else
        {
            bTrueNeighbour = true;
        }
    }
    else
    {
        bTrueNeighbour = true;
    }

    return bTrueNeighbour;
}


//TODO: implement update if affected pixel is explored, then invoke specific pixel from noise
PixelNeightbours NoiseReducer::DefinePixelNeighbours(std::vector<int> &noisePixels, std::queue<int> &checkQueue, uchar *internalPtr, int iCurrentPixel)
{
    PixelNeightbours currentNeibours;
    const int iRowLength = m_sImageSize.iCols;
    int iRightIndex = iCurrentPixel + 1;
    if (CheckPixelNeightbour(noisePixels, checkQueue, internalPtr, iCurrentPixel, iRightIndex))
    {
        currentNeibours.m_bRight = true;
    }

    int iDownIndex = iCurrentPixel + iRowLength;
    if (CheckPixelNeightbour(noisePixels, checkQueue, internalPtr, iCurrentPixel, iDownIndex))
    {
        currentNeibours.m_bDown = true;
    }

    int iRightDownIndex = iCurrentPixel + iRowLength + 1;
    if (CheckPixelNeightbour(noisePixels, checkQueue, internalPtr, iCurrentPixel, iRightDownIndex))
    {
        currentNeibours.m_bRightDown = true;
    }

    int iLeftDownIndex = iCurrentPixel + iRowLength - 1;
    if (CheckPixelNeightbour(noisePixels, checkQueue, internalPtr, iCurrentPixel, iLeftDownIndex))
    {
        currentNeibours.m_bLeftDown = true;
    }

    int iLeftIndex = iCurrentPixel - 1;
    if (CheckPixelNeightbour(noisePixels, checkQueue, internalPtr, iCurrentPixel, iLeftIndex))
    {
        currentNeibours.m_bLeft = true;
    }

    int iLeftUpIndex = iCurrentPixel - iRowLength - 1;
    if (CheckPixelNeightbour(noisePixels, checkQueue, internalPtr, iCurrentPixel, iLeftUpIndex))
    {
        currentNeibours.m_bLeftUp = true;
    }

    int iUpIndex = iCurrentPixel - iRowLength;
    if (CheckPixelNeightbour(noisePixels, checkQueue, internalPtr, iCurrentPixel, iUpIndex))
    {
        currentNeibours.m_bUp = true;
    }

    int iRightUpIndex = iCurrentPixel - iRowLength + 1;
    if (CheckPixelNeightbour(noisePixels, checkQueue, internalPtr, iCurrentPixel, iRightUpIndex))
    {
        currentNeibours.m_bRightUp = true;
    }

    return std::move(currentNeibours);
}

void NoiseReducer::RemoveNoise(Mat &srcImage, int iMinPixels)
{
    MatSize size = srcImage.size;
    int channels = srcImage.channels();
    m_sImageSize.iRows = srcImage.rows;
    m_sImageSize.iCols = srcImage.cols;

    if (srcImage.isContinuous())
    {
        int iSize = m_sImageSize.iCols*m_sImageSize.iRows;
        uchar *internalPtr = srcImage.ptr(0);

        m_exploredNodes.reserve(iSize); //chage to set or maybe even unordered_set to increase performance
        std::vector<int> noisePixels(iMinPixels, false);
        std::unordered_map<int, PixelNeightbours> neigbourTable;
        std::queue<int> checkQueue;

        int iIndex = 0;
        checkQueue.push(iIndex);
        m_exploredNodes.push_back(iIndex);
        int iPixelsCounter = 0;
        int iColPos = 0;
        int iRowPos = 0;

        //TODO: outer while loop, till iColPos < iSize;
        while (!checkQueue.empty())
        {//NOTE: seems like this algo will go only through one graph
            int iCurIndex = checkQueue.front();
            checkQueue.pop();

            DefinePixelNeighbours(noisePixels, checkQueue, internalPtr, iCurIndex);
            if (CheckIfGraphEnded(neigbourTable))
            {
                if (noisePixels.size() <= iMinPixels)
                {
                    RemoveNoisePixels(noisePixels, internalPtr);
                    noisePixels.clear();

                }
            }

            if (noisePixels.size() > iMinPixels)
            {
                noisePixels.clear();
                //clear other neightbour pixels object
            }

            //NOTE: on each iteration should pixels neighbour table should be checked
        }


    }
    else
    {
        //TODO: handle other case - create new matrix with Mat::create
    }

    //srcImage.
}