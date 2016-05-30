//
//  ExtractFeaturePoint.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/05/25.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef ExtractFeaturePoint_hpp
#define ExtractFeaturePoint_hpp

#include <stdio.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>

#include "Rotate.hpp"
#include "Transform.hpp"

class ExtractFeaturePoint
{
public:
    ExtractFeaturePoint
    (const cv::Size& frameSize, int divNum, Transform& transform, Rotate& rot);
    ~ExtractFeaturePoint();
    
    // 画像を変更する
    void setImage(const cv::Mat& newImg);
    // numberで指定された部分の特徴点を抽出（基準画像における座標）
    void extractRoiFeaturePoint
    (int number, std::vector<cv::KeyPoint>& roiKeyPoints,
     cv::Mat& roiDescriptors);
    // 基準画像を回転させながらすべての領域の特徴点を抽出
    void extractFeaturePoint
    (std::vector<cv::KeyPoint>& keyPoints, cv::Mat& descriptors);
    // 特徴点を連結させる
    void keyPointCat
    (std::vector<cv::KeyPoint>& dest, const std::vector<cv::KeyPoint>& src);
    // 特徴点記述子を連結させる
    void descriptorCat(cv::Mat& dest, const cv::Mat& src);
    // 有効範囲にある特徴点のみを取り出す
    void filterLowLatitue
    (std::vector<cv::KeyPoint>& keyPoints, cv::Mat& descriptors);
    bool isInLowLatitude(int u, int v);
    bool isInLowLatitude(const cv::Point2f point);
    
private:
    const cv::Size& frameSize;
    const int validHeight;
    cv::Mat img;
    
    Transform& transform;
    Rotate& rotate;
    
    cv::Ptr<cv::FeatureDetector> detector;
    cv::Ptr<cv::DescriptorExtractor> extractor;
    
    const int divNum;
    cv::Rect roi;
    std::vector<cv::KeyPoint> keyPoints;
};

#endif /* ExtractFeaturePoint_hpp */
