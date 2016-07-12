//
//  ExtractFeaturePoint.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/05/25.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "ExtractFeaturePoint.hpp"

ExtractFeaturePoint::ExtractFeaturePoint
(const cv::Size& frameSize, const Transform& transform, int divNum):
frameSize(frameSize), transform(transform), divNum(divNum),
validHeight(transform.dphi2v(M_PI)/divNum),
roi(cv::Rect(0, (frameSize.height - validHeight)/2,
             frameSize.width, validHeight)),
detector(cv::FeatureDetector::create("SIFT")),
extractor(cv::DescriptorExtractor::create("SIFT"))
{
}

ExtractFeaturePoint::~ExtractFeaturePoint()
{
}

void ExtractFeaturePoint::extractRectRoiFeaturePoint
(const cv::Mat& img, std::vector<cv::KeyPoint> &keyPoints,
 cv::Mat &descriptors) const
{
    detector->detect(img(roi), keyPoints);
    extractor->compute(img(roi), keyPoints, descriptors);
    
    roiCoord2GlobalCoord(keyPoints);
}

void ExtractFeaturePoint::extractRoiFeaturePoint
(const cv::Mat& img, std::vector<cv::KeyPoint>& roiKeyPoints,
 cv::Mat& roiDescriptors, int number) const
{
    assert(number < divNum);
    
    float rotAngle = -1 * M_PI /2 +  M_PI*((float)number / divNum);
    
    cv::Mat rotImg(frameSize, CV_8UC3);
    transform.rotateVerticalImg(rotAngle, img, rotImg);
    
    detector->detect(rotImg(roi), roiKeyPoints);
    extractor->compute(rotImg(roi), roiKeyPoints, roiDescriptors);
 
    // ROIの座標から(回転後の)大域の座標に変換
    roiCoord2GlobalCoord(roiKeyPoints);
    
    // 重なりがでないようにtanの条件を適用
    filterLowLatitue(roiKeyPoints, roiDescriptors);
    
    rotateKeyPointCoord(roiKeyPoints, -1 * rotAngle);
}

void ExtractFeaturePoint::rotateKeyPointCoord
(std::vector<cv::KeyPoint> &keyPoints, float angle) const
{
    for (int i=0; i<keyPoints.size(); i++) {
        float u, v, ur, vr;
        
        u = keyPoints[i].pt.x;
        v = keyPoints[i].pt.y;
        
        transform.rotateVerticalOrthDot(angle, u, v, ur, vr);
        
        keyPoints[i].pt = cv::Point2f(ur, vr);
    }
}

void ExtractFeaturePoint::extractFeaturePoint
(const cv::Mat& img, std::vector<cv::KeyPoint> &keyPoints,
 cv::Mat& descriptors) const
{
    for (int i=0; i<divNum; i++) {
        std::vector<cv::KeyPoint> roiKeyPoints;
        cv::Mat roiDescriptors;

        if (i == 0) {
            extractRoiFeaturePoint(img, keyPoints, descriptors, i);
        } else {
            extractRoiFeaturePoint(img, roiKeyPoints, roiDescriptors, i);
        
            keyPointConcat(keyPoints, roiKeyPoints);
            descriptorConcat(descriptors, roiDescriptors);
            
            std::cout << keyPoints.size() << std::endl;
        }
    }
}

/*
void ExtractFeaturePoint::roiCoord2GlobalCoord
(std::vector<cv::KeyPoint> &keyPoints) const
{
    for (int i=0; i<keyPoints.size(); i++) {
        keyPoints[i].pt.y += (frameSize.height - validHeight)/2;
    }
}

void ExtractFeaturePoint::keyPointCat
(std::vector<cv::KeyPoint> &dest, const std::vector<cv::KeyPoint> &src) const
{
    std::copy(src.begin(), src.end(), std::back_inserter(dest));
}

void ExtractFeaturePoint::descriptorCat(cv::Mat &dest, const cv::Mat &src)
{
    cv::Mat conMat;
    cv::vconcat(dest, src, conMat);
    
    dest = conMat.clone();
}
*/

void ExtractFeaturePoint::filterLowLatitue
(std::vector<cv::KeyPoint> &keyPoints, cv::Mat &descriptors) const
{
    cv::Mat destDescriptors(cv::Size(128, descriptors.rows), CV_32FC1);
    
    for (int i=0, j=0; i<keyPoints.size(); j++) {
        // 有効範囲内の記述子をコピーする
        if (isInLowLatitude(keyPoints[i].pt)) {
            descriptors.row(j).copyTo(destDescriptors.row(i));
            i++;
        // 有効範囲外のキーポイントを削除する
        } else {
            keyPoints.erase(keyPoints.begin() + i);
        }
    }
    // 空白行をなくす
    destDescriptors.resize(keyPoints.size());
    descriptors = destDescriptors.clone();
}

/*
bool ExtractFeaturePoint::isInLowLatitude(const cv::Point2f point) const
{
    return isInLowLatitude(point.x, point.y);
}
*/
 
bool ExtractFeaturePoint::isInLowLatitude(float u, float v) const
{
    float theta = transform.u2theta(u);
    float phi = transform.v2phi(v);
    
    if (cosf(theta) == 0.0) return false;
    float latitude = atanf(-tanf(phi)/cosf(theta));
    
    float validPhi = M_PI / divNum;
    
    if (-1.0 * validPhi/2.0 <= latitude && latitude <= validPhi/2.0) {
        return true;
    } else {
        return false;
    }
}



