#include "NoiseReducer.h"

#include <queue>
#include <unordered_map>
#include <iostream>

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
    const static int iSize = m_sImageSize.iCols*m_sImageSize.iRows;
    if (iNewIndex >= iSize || iNewIndex < 0)
        return true;

    //checking the very left pixels
    if (iNewIndex - iCurIndex == m_sImageSize.iCols - 1 && iCurIndex % m_sImageSize.iCols == 0) //left down
        return true;

    if (iCurIndex - iNewIndex == 1 && iCurIndex % m_sImageSize.iCols == 0) // left 
        return true;

    if (iCurIndex - iNewIndex == m_sImageSize.iCols - 1 && iCurIndex % m_sImageSize.iCols == 0) //left up
        return true;

    //checking the very right pixels
    if (iNewIndex - iCurIndex == 1 && iNewIndex % m_sImageSize.iCols == 0) //right
        return true;

    if (iNewIndex - iCurIndex == m_sImageSize.iCols + 1 && iNewIndex % m_sImageSize.iCols == 0) //right down
        return true;

    if (iCurIndex - iNewIndex == m_sImageSize.iCols + 1 && iNewIndex % m_sImageSize.iCols == 0) //right up
        return true;

    bool bTrueNeighbour = false;
    if (!IsExplored(iNewIndex))
    {
        m_exploredNodes.insert(iNewIndex);
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

bool NoiseReducer::IsExplored(int iIndex) const
{
    return m_exploredNodes.find(iIndex) == m_exploredNodes.end() ? false : true;
    //for vector
    //std::end(m_exploredNodes) == std::find(m_exploredNodes.begin(), m_exploredNodes.end(), iNewIndex)
}

//TODO: implement update if affected pixel is explored, then invoke specific pixel from noise
PixelNeightbours NoiseReducer::DefinePixelNeighbours(std::vector<int> &noisePixels, std::queue<int> &checkQueue, uchar *internalPtr, int iCurrentPixel)
{
    PixelNeightbours currentNeibours;
    static const int iRowLength = m_sImageSize.iCols;
    int iRightIndex = iCurrentPixel + 1;
    if (CheckPixelNeightbour(noisePixels, checkQueue, internalPtr, iCurrentPixel, iRightIndex))
    {
        currentNeibours.m_bRight = true;
    }
    else
    {
        currentNeibours.m_vNoiseNeightbour.push_back(iRightIndex);
    }

    int iDownIndex = iCurrentPixel + iRowLength;
    if (CheckPixelNeightbour(noisePixels, checkQueue, internalPtr, iCurrentPixel, iDownIndex))
    {
        currentNeibours.m_bDown = true;
    }
    else
    {
        currentNeibours.m_vNoiseNeightbour.push_back(iDownIndex); 
    }

    int iRightDownIndex = iCurrentPixel + iRowLength + 1;
    if (CheckPixelNeightbour(noisePixels, checkQueue, internalPtr, iCurrentPixel, iRightDownIndex))
    {
        currentNeibours.m_bRightDown = true;
    }
    else
    {
        currentNeibours.m_vNoiseNeightbour.push_back(iRightDownIndex);
    }

    int iLeftDownIndex = iCurrentPixel + iRowLength - 1;
    if (CheckPixelNeightbour(noisePixels, checkQueue, internalPtr, iCurrentPixel, iLeftDownIndex))
    {
        currentNeibours.m_bLeftDown = true;
    }
    else
    {
        currentNeibours.m_vNoiseNeightbour.push_back(iLeftDownIndex);
    }

    int iLeftIndex = iCurrentPixel - 1;
    if (CheckPixelNeightbour(noisePixels, checkQueue, internalPtr, iCurrentPixel, iLeftIndex))
    {
        currentNeibours.m_bLeft = true;
    }
    else
    {
        currentNeibours.m_vNoiseNeightbour.push_back(iLeftIndex);
    }

    int iLeftUpIndex = iCurrentPixel - iRowLength - 1;
    if (CheckPixelNeightbour(noisePixels, checkQueue, internalPtr, iCurrentPixel, iLeftUpIndex))
    {
        currentNeibours.m_bLeftUp = true;
    }
    else
    {
        currentNeibours.m_vNoiseNeightbour.push_back(iLeftUpIndex);
    }


    int iUpIndex = iCurrentPixel - iRowLength;
    if (CheckPixelNeightbour(noisePixels, checkQueue, internalPtr, iCurrentPixel, iUpIndex))
    {
        currentNeibours.m_bUp = true;
    }
    else
    {
        currentNeibours.m_vNoiseNeightbour.push_back(iUpIndex);
    }


    int iRightUpIndex = iCurrentPixel - iRowLength + 1;
    if (CheckPixelNeightbour(noisePixels, checkQueue, internalPtr, iCurrentPixel, iRightUpIndex))
    {
        currentNeibours.m_bRightUp = true;
    }
    else
    {
        currentNeibours.m_vNoiseNeightbour.push_back(iRightUpIndex);
    }

    return std::move(currentNeibours);
}

void NoiseReducer::RemoveNoise(Mat &srcImage, int iMinPixels)
{
    auto size = srcImage.size;
    int channels = srcImage.channels();
    m_sImageSize.iRows = srcImage.rows;
    m_sImageSize.iCols = srcImage.cols;

    if (srcImage.isContinuous())
    {
        int iSize = m_sImageSize.iCols*m_sImageSize.iRows;
        uchar *internalPtr = srcImage.ptr(0);

        std::vector<int> noisePixels;
        noisePixels.reserve(iMinPixels);
        m_exploredNodes.reserve(iSize); //chage to set or maybe even unordered_set to increase performance
        std::unordered_map<int, PixelNeightbours> neigbourTable;
        std::queue<int> checkQueue;

        int iIndex = 0;
        while (iIndex < iSize)
        {//TODO: why this works super slowly ? 
            checkQueue.push(iIndex);
            m_exploredNodes.insert(iIndex);
            while (!checkQueue.empty())
            {
                int iCurIndex = checkQueue.front();
                checkQueue.pop();
                PixelNeightbours currNeightbour = DefinePixelNeighbours(noisePixels, checkQueue, internalPtr, iCurIndex);
                neigbourTable.insert(std::make_pair(iCurIndex, currNeightbour));
                //TODO: update other pixel neightbours
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
                    neigbourTable.clear();
                }
            }

            if (noisePixels.size() <= iMinPixels)
            {
                RemoveNoisePixels(noisePixels, internalPtr);
            }
            while (IsExplored(iIndex))
                ++iIndex;
            noisePixels.clear();
            neigbourTable.clear();
          //  std::cout << "Curr index = " << iIndex << ". Size = " << iSize << "\n";
     
        }
    }
    else
    {
        //TODO: handle other case - create new matrix with Mat::create
    }
}