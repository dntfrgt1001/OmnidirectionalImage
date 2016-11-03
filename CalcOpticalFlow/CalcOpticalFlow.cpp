//
//  CalcOpticalFlow.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "CalcOpticalFlow.hpp"

void CalcOpticalFlow::getNormalPair
(const cv::Mat &forPersImg, const cv::Mat &latPersImg,
 const cv::Mat &curRotMat, std::vector<cv::Point2f> &forNormals,
 std::vector<cv::Point2f> &latNormals) const
{
    getNormalPairOneDir
    (forPersImg, latPersImg, curRotMat, isFront,
     forNormalsTmp, latNormalsTmp);
}

void CalcOpticalFlow::getNormalPairOneDir
(const cv::Mat &forPersImg, const cv::Mat &latPersImg,
 const cv::Mat &curRotMat, const bool isFront,
 std::vector<cv::Point2f> &forNormals,
 std::vector<cv::Point2f> &latNormals) const
{
    // オプティカルフローを抽出
    std::vector<cv::Point2f> forPerss, latPerss;
    getOpticalFlow(forPersImg, latPersImg, forPerss, latPerss);
}

void CalcOpticalFlow::getOpticalFlow
(const cv::Mat &forPersImg, const cv::Mat &latPersImg,
 std::vector<cv::Point2f> &forPerss,
 std::vector<cv::Point2f> &latPerss) const
{
    // グレースケールに変換
    cv::Mat forGrayPersImg;
    cv::cvtColor(forPersImg, forGrayPersImg, CV_BGR2GRAY);
    
    // 前透視投影画像からトラッキング用の特徴点を抽出
    cv::goodFeaturesToTrack
    (forGrayPersImg, forPerss, 300, 0.01, 2, optflowMask);
    
    // グレースケールに変換
    cv::Mat latGrayPersImg;
    cv::cvtColor(latPersImg, latGrayPersImg, CV_BGR2GRAY);
    
    // オプティカルフローを計算
    std::vector<uchar> statuss;
    std::vector<float> errors;
    cv::calcOpticalFlowPyrLK
    (forGrayPersImg, latGrayPersImg, forPerss, latPerss, statuss, errors);
    
    // エラーを取り除く
    for (int i = 0; i < forPerss.size(); ) {
        if (statuss[i] == 0) {
            forPerss.erase(forPerss.begin() + i);
            latPerss.erase(latPerss.begin() + i);
            statuss.erase(statuss.begin() + i);
        } else {
            i++;
        }
    }
}

void CalcOpticalFlow::remOrthOutlier
(std::vector<cv::Point2f> &forNormals,
 std::vector<cv::Point2f> &latNormals) const
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

void CalcOpticalFlow::drawOpticalFlow
(const cv::Mat &persImg, const std::vector<cv::Point2f> &forPerss,
 const std::vector<cv::Point2f> &latPerss, cv::Mat &drawImg) const
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
