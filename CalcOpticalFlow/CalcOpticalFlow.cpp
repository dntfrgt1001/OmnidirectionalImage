//
//  CalcOpticalFlow.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "CalcOpticalFlow.hpp"

void CalcOpticalFlow::getOpticalFlow
(const cv::Mat &forPersImg, const cv::Mat &latPersImg,
 std::vector<Pers> &forPerss, std::vector<Pers> &latPerss) const
{
    // グレースケールに変換
    cv::Mat forGrayPersImg;
    cv::cvtColor(forPersImg, forGrayPersImg, CV_BGR2GRAY);
    
    // 前透視投影画像からトラッキング用の特徴点を抽出
    std::vector<cv::Point2f> forPoints;
    cv::goodFeaturesToTrack
    (forGrayPersImg, forPoints, 400, 0.01, 1, optflowMask);
    
    // グレースケールに変換
    cv::Mat latGrayPersImg;
    cv::cvtColor(latPersImg, latGrayPersImg, CV_BGR2GRAY);
    
    // オプティカルフローを計算
    std::vector<cv::Point2f> latPoints;
    std::vector<uchar> statuss;
    std::vector<float> errors;
    cv::calcOpticalFlowPyrLK
    (forGrayPersImg, latGrayPersImg, forPoints, latPoints, statuss, errors);
    
    // エラーを取り除く
    for (int i = 0; i < forPoints.size(); ) {
        if (statuss[i] == 0) {
            forPoints.erase(forPoints.begin() + i);
            latPoints.erase(latPoints.begin() + i);
            statuss.erase(statuss.begin() + i);
        } else {
            i++;
        }
    }
    
    // Pers型に変換
    forPerss = std::vector<Pers>(forPoints.size());
    latPerss = std::vector<Pers>(latPoints.size());
    for (int i = 0; i < forPoints.size(); i++) {
        forPerss[i] = Pers(forPoints[i]);
        latPerss[i] = Pers(latPoints[i]);
    }
}

void CalcOpticalFlow::remOrthOutlier
(std::vector<Normal> &forNormals, std::vector<Normal> &latNormals) const
{
    for (int i = 0; i < forNormals.size(); ) {
        if (!isOrthCond(forNormals[i], latNormals[i])) {
            forNormals.erase(forNormals.begin() + i);
            latNormals.erase(latNormals.begin() + i);
        } else {
            i++;
        }
    }
}

void CalcOpticalFlow::remRotOutlier
(std::vector<Normal> &forNormals, std::vector<Normal> &latNormals) const
{
    for (int i = 0; i < forNormals.size(); ) {
        if (!isRotDirCond(forNormals[i], latNormals[i])) {
            forNormals.erase(forNormals.begin() + i);
            latNormals.erase(latNormals.begin() + i);
        } else {
            i++;
        }
    }
}

void CalcOpticalFlow::remNormOutlier
(std::vector<Normal> &forNormals, std::vector<Normal> &latNormals,
 const float rotAng) const
{
    for (int i = 0; i < forNormals.size(); ) {
        if (!isNormCond(forNormals[i], latNormals[i], rotAng)) {
            forNormals.erase(forNormals.begin() + i);
            latNormals.erase(latNormals.begin() + i);
        } else {
            i++;
        }
    }
}

void CalcOpticalFlow::drawOpticalFlow
(const cv::Mat &persImg, const std::vector<Pers> &forPerss,
 const std::vector<Pers> &latPerss, cv::Mat &drawImg) const
{
    drawImg = persImg.clone();
    
    for (int i = 0; i < forPerss.size(); i++) {
        cv::line
        (drawImg, forPerss[i], latPerss[i],
         cv::Scalar(rand()%256, rand()%256, rand()%256), 2, CV_AA);
        cv::circle
        (drawImg, latPerss[i], 3, cv::Scalar(0, 0, 0), -1, CV_AA);
    }
}
