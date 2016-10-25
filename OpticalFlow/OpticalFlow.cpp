//
//  OpticalFlow.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/20.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "OpticalFlow.hpp"

void OpticalFlow::estRotMatFromOpticalFlow
(const cv::Mat &forImg, const cv::Mat &latImg,
 const cv::Mat &froMat, cv::Mat &estRotMat) const
{
    // 特徴点のペアを取り出す
    std::vector<cv::Point2f> forNormals, latNormals;
    getNormalPair(forImg, latImg, froMat, forNormals, latNormals);
    
    cv::Mat mask;
    cv::Mat rotMatFront;
    est.estRotMatEssMatCore(forNormals, latNormals, rotMatFront, mask);
    
    cv::Vec3f rotVecFront, rotVec;
    Rotation::RotMat2RotVec(rotMatFront, rotVecFront);
    rotVec = cv::Vec3f(cv::Mat1f(froMat.inv() *
                                 cv::Mat1f(rotVecFront)));
    Rotation::RotVec2RotMat(rotVec, estRotMat);
}

void OpticalFlow::getNormalPair
(const cv::Mat &forImg, const cv::Mat &latImg, const cv::Mat& rotMat,
 std::vector<cv::Point2f> &forNormals,
 std::vector<cv::Point2f> &latNormals) const
{
    // 特徴点を一時的に格納
    std::vector<cv::Point2f> forNormalsTmp, latNormalsTmp;
    
    // 正面の特徴点
    bool isFront = true;
    getNormalPairOneDir
    (forImg, latImg, rotMat, forNormalsTmp, latNormalsTmp, isFront);
    
    // コピー
    std::copy
    (forNormalsTmp.begin(), forNormalsTmp.end(),
     std::back_inserter(forNormals));
    std::copy
    (latNormalsTmp.begin(), latNormalsTmp.end(),
     std::back_inserter(latNormals));
    
    /*
    // 背面の特徴点
    isFront = false;
    getNormalPairOneDir
    (forImg, latImg, rotMat, forNormalsTmp, latNormalsTmp, isFront);
    
    // コピー
    std::copy
    (forNormalsTmp.begin(), forNormalsTmp.end(),
     std::back_inserter(forNormals));
    std::copy
    (latNormalsTmp.begin(), latNormalsTmp.end(),
     std::back_inserter(latNormals));
     */
    
}

void OpticalFlow::getNormalPairOneDir
(const cv::Mat &forImg, const cv::Mat &latImg, const cv::Mat &rotMat,
 std::vector<cv::Point2f> &forNormals,
 std::vector<cv::Point2f> &latNormals, const bool isFront) const
{
    // 正面の透視投影画像
    cv::Mat forPersImg, latPersImg;
    ps.getPersImg(forImg, forPersImg, rotMat, isFront);
    ps.getPersImg(latImg, latPersImg, rotMat, isFront);
    
    const float margin = 0.1;
    cv::Mat mask;
    ps.getMask(margin, mask);
    
    // 特徴点抽出
    std::vector<cv::Point2f> forPerss, latPerss;
    // 前透視投影画像での特徴点を抽出
    getFeatures(forPersImg, forPerss, mask);
    // オプティカルフローから後透視投影画像での特徴点を抽出
    getOpticalFlow(forPersImg, latPersImg, forPerss, latPerss);
    
    // 外れ値除去
//    removeOutlier(forPerss, latPerss);
    cv::Mat outImg;
    drawOpticalFlow(forPersImg, forPerss, latPerss, outImg);
    cv::namedWindow("opt");
    cv::imshow("opt", outImg);
    
    std::cout << "opt size =" << forPerss.size() << std::endl;
    
    // 正規化画像座標へマッピング
    tf.pers2normal(forPerss, forNormals, ps.getInParaMat());
    tf.pers2normal(latPerss, latNormals, ps.getInParaMat());
}

void OpticalFlow::getFeatures
(const cv::Mat &img, std::vector<cv::Point2f> &pers,
 const cv::Mat &mask) const
{
    cv::Mat grayImg;
    cv::cvtColor(img, grayImg, CV_BGR2GRAY);
    
    cv::goodFeaturesToTrack(grayImg, pers, 300, 0.01, 2, mask);
}

void OpticalFlow::getOpticalFlow
(const cv::Mat &forPersImg, const cv::Mat &latPersImg,
 std::vector<cv::Point2f> &forPerss,
 std::vector<cv::Point2f> &latPerss) const
{
    cv::Mat forGrayImg, latGrayImg;
    cv::cvtColor(forPersImg, forGrayImg, CV_BGR2GRAY);
    cv::cvtColor(latPersImg, latGrayImg, CV_BGR2GRAY);
    
    std::vector<uchar> statuss;
    std::vector<float> errors;
    
    cv::calcOpticalFlowPyrLK
    (forGrayImg, latGrayImg, forPerss, latPerss,
     statuss, errors);
    
    std::cout << "size before = " << forPerss.size() << std::endl;
    removeErrorFlow(forPerss, latPerss, statuss);
    std::cout << "size after = " << forPerss.size() << std::endl;
    
}

void OpticalFlow::removeErrorFlow
(std::vector<cv::Point2f> &forPerss,
 std::vector<cv::Point2f> &latPerss, std::vector<uchar> statuss) const
{
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

void OpticalFlow::removeOutlier
(std::vector<cv::Point2f> &forPerss,
 std::vector<cv::Point2f> &latPerss) const
{
    for (int i = 0; i < forPerss.size(); ) {
        if (isOutlier(forPerss[i], latPerss[i])) {
            forPerss.erase(forPerss.begin() + i);
            latPerss.erase(latPerss.begin() + i);
        } else {
            i++;
        }
    }
}

void OpticalFlow::drawPoint
(const cv::Mat &persImg, const std::vector<cv::Point2f> &perss,
 cv::Mat &outImg) const
{
    outImg = persImg.clone();
    
    for (int i = 0; i < perss.size(); i++) {
        cv::circle(outImg, perss[i], 5,
                   cv::Scalar(rand()%256, rand()%256, rand()%256));
    }
}

void OpticalFlow::drawOpticalFlow
(const cv::Mat &persImg, const std::vector<cv::Point2f> &forPerss,
 const std::vector<cv::Point2f> &latPerss, cv::Mat &outImg) const
{
    outImg = persImg.clone();
    
    for (int i = 0; i < forPerss.size(); i++) {
        cv::line(outImg, forPerss[i], latPerss[i],
                 cv::Scalar(rand()%256, rand()%256, rand()%256), 2, CV_AA);
        cv::circle(outImg, latPerss[i], 3, cv::Scalar(0, 0, 0), -1, CV_AA);
    }
}
