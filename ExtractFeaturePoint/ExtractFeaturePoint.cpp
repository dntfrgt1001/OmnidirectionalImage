//
//  ExtractFeaturePoint.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/05/25.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "ExtractFeaturePoint.hpp"

ExtractFeaturePoint::ExtractFeaturePoint
(const cv::Size& frameSize, Transform& transform, int divNum):
frameSize(frameSize), transform(transform), divNum(divNum),
validHeight(transform.dphi2v(M_PI)/divNum)
{
    roi = cv::Rect(0, (frameSize.height - validHeight)/2,
                   frameSize.width, validHeight);
    
    detector = cv::FeatureDetector::create("SIFT");
    extractor = cv::DescriptorExtractor::create("SIFT");
}

ExtractFeaturePoint::~ExtractFeaturePoint()
{
}

/*
void ExtractFeaturePoint::setImage(const cv::Mat &newImg)
{
    newImg.copyTo(img);
}
*/

void ExtractFeaturePoint::extractRoiFeaturePoint
(const cv::Mat& img, std::vector<cv::KeyPoint>& roiKeyPoints,
 cv::Mat& roiDescriptors, int number)
{
    assert(number < divNum);
    
    double rotAngle = (-1) * M_PI / 2 +  M_PI * ((float)number / divNum);
    double invRotAngle = (-1) * rotAngle;
    
    cv::Mat rotImg(frameSize, CV_8UC3);
    transform.rotateVerticalImg(rotAngle, img, rotImg);
    
    detector->detect(rotImg(roi), roiKeyPoints);
    
    roiKeyPoints.erase(roiKeyPoints.begin(), roiKeyPoints.begin()+10);
    
    extractor->compute(rotImg(roi), roiKeyPoints, roiDescriptors);
    
//    std::cout << (descriptors.type() == CV_32F) << std::endl;
 
    // ROIの座標から(回転後の)大域の座標に変換
    roiCoord2GlobalCoord(roiKeyPoints);
    
    filterLowLatitue(roiKeyPoints, roiDescriptors);
    
    for (int i=0; i<roiKeyPoints.size(); i++) {
        int u, v, ur, vr;
        
        u = roiKeyPoints[i].pt.x;
        v = roiKeyPoints[i].pt.y;
        
        transform.rotateVerticalOrthDot(invRotAngle, u, v, ur, vr);
        
        roiKeyPoints[i].pt = cv::Point2f(ur, vr);
    }
    
    cv::Mat outImg, outImg2;
    cv::drawKeypoints(img, roiKeyPoints, outImg);
    
    cv::namedWindow("keypoint");
    cv::imshow("keypoint", outImg);
//    cv::waitKey(-1);
}

void ExtractFeaturePoint::extractRectRoiFeaturePoint
(const cv::Mat& img, std::vector<cv::KeyPoint> &keyPoints, cv::Mat &descriptors)
{
    detector->detect(img(roi), keyPoints);
    extractor->compute(img(roi), keyPoints, descriptors);
    
    roiCoord2GlobalCoord(keyPoints);
}

void ExtractFeaturePoint::extractFeaturePoint
(const cv::Mat& img, std::vector<cv::KeyPoint> &keyPoints, cv::Mat& descriptors)
{
    
    for (int i=0; i<divNum; i++) {
        
        std::vector<cv::KeyPoint> roiKeyPoints;
        cv::Mat roiDescriptors;

        if (i == 0) {
            extractRoiFeaturePoint(img, keyPoints, descriptors, i);
        } else {
            extractRoiFeaturePoint(img, roiKeyPoints, roiDescriptors, i);
        
            keyPointCat(keyPoints, roiKeyPoints);
            descriptorCat(descriptors, roiDescriptors);
            
        }
        
    }

}

void ExtractFeaturePoint::roiCoord2GlobalCoord
(std::vector<cv::KeyPoint> &keyPoints)
{
    for (int i=0; i<keyPoints.size(); i++) {
        keyPoints[i].pt.y += (frameSize.height - validHeight)/2;
    }
}

void ExtractFeaturePoint::keyPointCat
(std::vector<cv::KeyPoint> &dest, const std::vector<cv::KeyPoint> &src)
{
    std::copy(src.begin(), src.end(), std::back_inserter(dest));
}

void ExtractFeaturePoint::descriptorCat(cv::Mat &dest, const cv::Mat &src)
{
    cv::Mat conMat;
    cv::vconcat(src, dest, conMat);
    
    dest = conMat;
}

void ExtractFeaturePoint::filterLowLatitue
(std::vector<cv::KeyPoint> &keyPoints, cv::Mat &descriptors)
{
    int incAmount = 100;
    
    cv::Mat destDescriptors(cv::Size(128, incAmount), CV_32F);
    
    // 無効範囲のキーポイントの記述子を削除
    int count = 0;
    int incCount = 1;
    for (int i=0; i<keyPoints.size(); i++) {
        if (isInLowLatitude(keyPoints[i].pt)) {
            descriptors.row(i).copyTo(destDescriptors.row(count));
            
            count++;
            if (count >= incAmount * incCount) {
                incCount++;
                destDescriptors.resize(incAmount * incCount);
            }
        }
    }
    //　空白行をなくす
    destDescriptors.resize(count);
    
    // 無効範囲のキーポイントを削除
    for (int i=0; i<keyPoints.size();    ) {
        if (!isInLowLatitude(keyPoints[i].pt)){
            keyPoints.erase(keyPoints.begin() + i);
        } else {
            i++;
        }
    }
    
    descriptors = destDescriptors;
    
}

bool ExtractFeaturePoint::isInLowLatitude(const cv::Point2f point)
{
    return isInLowLatitude(point.x, point.y);
}

bool ExtractFeaturePoint::isInLowLatitude(int u, int v)
{
    double theta = transform.u2theta(u);
    double phi = transform.v2phi(v);
    double latitude = atan(tan(phi)/cos(theta));
    
    double validHeightPhi = M_PI / divNum;
    
    if (-1.0 * validHeightPhi/2.0 <= latitude &&
        latitude <= validHeightPhi/2.0)
    {
        return true;
    } else {
        return false;
    }
}



