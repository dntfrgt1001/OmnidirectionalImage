//
//  ExtractFeaturePoint.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/05/25.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "ExtractFeaturePoint.hpp"

ExtractFeaturePoint::ExtractFeaturePoint
(const cv::Size& fs, const int divNum, const int mergin):
divNum(divNum),
roi(cv::Rect(0, (fs.height - fs.height/divNum)/2 - mergin,
             fs.width, fs.height/divNum + mergin*2)),
feature(cv::xfeatures2d::SIFT::create())
//feature(cv::xfeatures2d::SURF::create())
//feature(cv::AKAZE::create())
//feature(cv::ORB::create())
{
    setLowLatMask(fs);
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
        if (i == 0) {
            // 特徴を引数に直接保存
            extFeatRoi(img, i, keyPoints, descs);
            
            std::cout << keyPoints.size() << std::endl;
        } else {
            // 一時保存用
            std::vector<cv::KeyPoint> roiKeyPoints;
            cv::Mat roiDescs;
            
            // 特徴をローカルに一時保存
            extFeatRoi(img, i, roiKeyPoints, roiDescs);
            
            // 特徴を統合
            keyPointConcat(roiKeyPoints, keyPoints);
            descConcat(roiDescs, descs);
            
            std::cout << keyPoints.size() << std::endl;
        }
    }
}

void ExtractFeaturePoint::extFeatRoi
(const cv::Mat& img, const int number,
 std::vector<cv::KeyPoint>& roiKeyPoints, cv::Mat& roiDescs) const
{
    const float angle = - M_PI_2 + M_PI * ((float) number / divNum);
    
    //cv::Mat grayImg;
    //cv::cvtColor(img, grayImg, CV_BGR2GRAY);
    
    cv::Mat rotImg;
    Map::rotateImgVert(img, angle, roi, rotImg);
    
    //feature->detect(rotImg, roiKeyPoints, mask);
    //feature->compute(rotImg, roiKeyPoints, roiDescs);
    //feature->detectAndCompute(rotImg, mask, roiKeyPoints, roiDescs);

    
    // 低緯度領域(矩形)で特徴点を抽出
    feature->detect(rotImg(roi), roiKeyPoints);
    feature->compute(rotImg(roi), roiKeyPoints, roiDescs);
    
    // 特徴点の座標をROIから画像全体の座標に
    getGlobalCrd(roiKeyPoints);

    // 矩形領域内の特徴点から曲線領域内の特徴点をフィルタ
    filterLowLat(roiKeyPoints, roiDescs);
    
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
(std::vector<cv::KeyPoint> &keyPoints, const float angle) const
{
    for (int i = 0; i < keyPoints.size(); i++) {
        keyPoints[i].pt =
        Map::rotateEquirectVert(Equirect(keyPoints[i].pt), angle);
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
 
void ExtractFeaturePoint::setLowLatMask(const cv::Size& fs)
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
    if (img.channels() == 1) cv::cvtColor(img, outImg, CV_GRAY2BGR);
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
