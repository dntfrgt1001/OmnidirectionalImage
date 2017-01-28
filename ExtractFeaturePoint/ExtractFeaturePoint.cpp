//
//  ExtractFeaturePoint.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/05/25.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "ExtractFeaturePoint.hpp"

ExtractFeaturePoint::ExtractFeaturePoint
(const cv::Size& fs, const Transform& tf, const int divNum):
fs(fs), tf(tf), divNum(divNum), validHeight(tf.dphi2v(M_PI)/divNum),
mergin(10), roi(cv::Rect(0, (fs.height - validHeight)/2 - mergin,
                         fs.width, validHeight + mergin*2)),
feature(cv::xfeatures2d::SIFT::create())
//feature(cv::AKAZE::create())
//feature(cv::xfeatures2d::SURF::create())
//feature(cv::ORB::create())
{
    setLowLatMask();
}

/*
void ExtractFeaturePoint::extractRectRoiFeaturePoint
(const cv::Mat& img, std::vector<cv::KeyPoint> &keyPoints,
 cv::Mat &descriptors) const
{
    feature->detect(img(roi), keyPoints);
    feature->compute(img(roi), keyPoints, descriptors);
    
    roiCoord2GlobalCoord(keyPoints);
}
*/

void ExtractFeaturePoint::extFeatSimp
(const cv::Mat &img, std::vector<cv::KeyPoint> &keyPoints,
 cv::Mat &descs) const
{
    feature->detect(img, keyPoints);
    feature->compute(img, keyPoints, descs);
}

void ExtractFeaturePoint::extFeat
(const cv::Mat& img, std::vector<cv::KeyPoint> &keyPoints,
 cv::Mat& descs) const
{
    for (int i = 0; i < divNum; i++) {
        std::vector<cv::KeyPoint> roiKeyPoints;
        cv::Mat roiDescs;
        
        if (i == 0) {
            extFeatRoi(img, i, keyPoints, descs);
            
            std::cout << keyPoints.size() << std::endl;
        } else {
            extFeatRoi(img, i, roiKeyPoints, roiDescs);
            
            keyPointConcat(keyPoints, roiKeyPoints);
            descConcat(descs, roiDescs);
            
            std::cout << keyPoints.size() << std::endl;
        }
    }
}

void ExtractFeaturePoint::extFeatRoi
(const cv::Mat& img, const int number,
 std::vector<cv::KeyPoint>& roiKeyPoints, cv::Mat& roiDescriptors) const
{
    const float angle = - M_PI_2 + M_PI * ((float) number / divNum);
    
    //cv::Mat grayImg;
    //cv::cvtColor(img, grayImg, CV_BGR2GRAY);
    
    cv::Mat rotImg;
    tf.rotateImgVertRect(img, angle, roi, rotImg);
    //tf.rotateImgVertRect(rotAngle, grayImg, roi, rotImg);
    
    //feature->detect(rotImg, roiKeyPoints, mask);
    //feature->compute(rotImg, roiKeyPoints, roiDescriptors);
    //feature->detectAndCompute(rotImg, mask, roiKeyPoints, roiDescriptors);
    
    // 低緯度領域で特徴点を抽出
    feature->detect(rotImg(roi), roiKeyPoints);
    feature->compute(rotImg(roi), roiKeyPoints, roiDescriptors);
    
    getGlobalCrd(roiKeyPoints);
    filterLowLat(roiKeyPoints, roiDescriptors);
    
    /*
    cv::namedWindow("img");
    cv::imshow("img", img);
    cv::namedWindow("rotImg");
    cv::imshow("rotImg", rotImg);
    
    cv::Mat keyImgRot;
    drawKeyPoint(rotImg, roiKeyPoints, keyImgRot);
    cv::namedWindow("keyrot");
    cv::imshow("keyrot", keyImgRot);
    */
    
    // 特徴点座標を逆回転
    rotKeyPointCrd(roiKeyPoints, -1 * angle);
    
    /*
    cv::namedWindow("mask");
    cv::imshow("mask", mask);
    
    cv::Mat keyImg;
    drawKeyPoint(img, roiKeyPoints, keyImg);
    cv::namedWindow("key");
    cv::imshow("key", keyImg);
//    cv::waitKey();
     */
}

void ExtractFeaturePoint::rotKeyPointCrd
(std::vector<cv::KeyPoint> &keyPoints, float angle) const
{
    for (int i = 0; i < keyPoints.size(); i++) {
        keyPoints[i].pt =
            tf.rotateEquirectVert(Equirect(keyPoints[i].pt), angle);
    }
}


void ExtractFeaturePoint::filterLowLat
(std::vector<cv::KeyPoint> &keyPoints, cv::Mat &descriptors) const
{
    cv::Mat destDescriptors(descriptors.size(), descriptors.type());
    
    for (int i = 0, j = 0; i < keyPoints.size(); j++) {
        // 有効範囲内の記述子をコピーする
        if (isInLowLat(keyPoints[i].pt)) {
            descriptors.row(j).copyTo(destDescriptors.row(i));
            i++;
        }
        // 有効範囲外のキーポイントを削除する
        else {
            keyPoints.erase(keyPoints.begin() + i);
        }
    }

    // 空白行をなくす
    destDescriptors.resize(keyPoints.size());
    destDescriptors.copyTo(descriptors);
}
 
void ExtractFeaturePoint::setLowLatMask()
{
    // マスクは8bitの整数型
    mask = cv::Mat(fs, CV_8UC1);
    
    for (int v = 0; v < mask.rows; v++) {
        uchar* row = mask.ptr<uchar>(v);
        
        for (int u = 0; u < mask.cols; u++) {
            // 画像座標が低緯度ならセット
            row[u] = isInLowLat(Equirect(u,v))? 255: 0;
        }
    }
}

void ExtractFeaturePoint::drawKeyPoint
(const cv::Mat& img, const std::vector<cv::KeyPoint> &keyPoints,
 cv::Mat& outImg) const
{
    cv::drawKeypoints(img, keyPoints, outImg);
}

void ExtractFeaturePoint::drawKeyPointClear
(const cv::Mat &img, const std::vector<cv::KeyPoint> &keyPoints,
 cv::Mat &outImg)
{
    // 画像をクローン
    if (img.channels() == 1) tf.changeChannel(img, outImg);
    else outImg = img.clone();
    
    // 円を描写
    const int radius = 5.0;
    const int thickness = -1;
    const int lineType = CV_AA;
    for (int i = 0; i < keyPoints.size(); i++) {
        cv::circle(outImg, keyPoints[i].pt, radius,
                   cv::Scalar(rand()%256, rand()%256, rand()%256),
                   thickness, lineType);
    }
}
