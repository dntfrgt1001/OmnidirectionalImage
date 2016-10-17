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
validHeight(transform.dphi2v(M_PI)/divNum), mergin(10),
roi(cv::Rect(0, (frameSize.height - validHeight)/2 - mergin,
             frameSize.width, validHeight + mergin*2)),
feature(cv::xfeatures2d::SIFT::create())
//feature(cv::AKAZE::create())
//feature(cv::xfeatures2d::SURF::create())
{
}

ExtractFeaturePoint::~ExtractFeaturePoint()
{
}

void ExtractFeaturePoint::extractRectRoiFeaturePoint
(const cv::Mat& img, std::vector<cv::KeyPoint> &keyPoints,
 cv::Mat &descriptors) const
{
    feature->detect(img(roi), keyPoints);
    feature->compute(img(roi), keyPoints, descriptors);
    
    roiCoord2GlobalCoord(keyPoints);
}

void ExtractFeaturePoint::extractRoiFeaturePoint
(const cv::Mat& img, std::vector<cv::KeyPoint>& roiKeyPoints,
 cv::Mat& roiDescriptors, int number) const
{
    assert(number < divNum);
    
    float rotAngle = -1 * M_PI /2 +  M_PI*((float)number / divNum);
    
    cv::Mat rotImg;
    transform.rotateVerticalImgRect(rotAngle, img, roi, rotImg);
    
    // 低緯度領域で特徴点を抽出
    feature->detect(rotImg(roi), roiKeyPoints);
    feature->compute(rotImg(roi), roiKeyPoints, roiDescriptors);
 
    // ROIの座標から(回転後の)大域の座標に変換
    roiCoord2GlobalCoord(roiKeyPoints);
    
    // 重なりがでないようにtanの制約条件を適用
    filterLowLatitue(roiKeyPoints, roiDescriptors);
    
    rotateKeyPointCoord(roiKeyPoints, -1 * rotAngle);
}

void ExtractFeaturePoint::rotateKeyPointCoord
(std::vector<cv::KeyPoint> &keyPoints, float angle) const
{
    for (int i=0; i<keyPoints.size(); i++) {
        cv::Point2f latequirect;
        transform.rotateVerticalequirect(angle, keyPoints[i].pt, latequirect);
        
        keyPoints[i].pt = latequirect;
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
            std::cout << keyPoints.size() << std::endl;
        } else {
            extractRoiFeaturePoint(img, roiKeyPoints, roiDescriptors, i);
        
            keyPointConcat(keyPoints, roiKeyPoints);
            descriptorConcat(descriptors, roiDescriptors);
            
            std::cout << keyPoints.size() << std::endl;
        }
    }
}

void ExtractFeaturePoint::filterLowLatitue
(std::vector<cv::KeyPoint> &keyPoints, cv::Mat &descriptors) const
{
    //cv::Mat destDescriptors(cv::Size(128, descriptors.rows), CV_32FC1);
    cv::Mat destDescriptors(cv::Size(descriptors.cols, descriptors.rows),
                            descriptors.type());

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


bool ExtractFeaturePoint::isInLowLatitude(const cv::Point2f& equirect) const
{
    cv::Point2f psphere;
    transform.equirect2psphere(equirect, psphere);
    
    float theta = psphere.x;
    float phi =psphere.y;
    
    if (cosf(theta) == 0.0) return false;
    float latitude = atanf(-tanf(phi)/cosf(theta));
    
    float validPhi = M_PI / divNum;
    
    if (-1.0 * validPhi/2.0 <= latitude && latitude <= validPhi/2.0) {
        return true;
    } else {
        return false;
    }
}



