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

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>

#include "Rotate.hpp"
#include "Transform.hpp"

class ExtractFeaturePoint
{
public:
    ExtractFeaturePoint
    (const cv::Size& frameSize, const Transform& transform, int divNum);
    ~ExtractFeaturePoint();
    
    // 低緯度の矩形部分の特徴点を抽出
    void extractRectRoiFeaturePoint
    (const cv::Mat& img, std::vector<cv::KeyPoint>& keyPoints,
     cv::Mat& descriptors) const;

    // numberで指定された部分の特徴点を抽出（基準画像における座標）
    void extractRoiFeaturePoint
    (const cv::Mat& img, std::vector<cv::KeyPoint>& roiKeyPoints,
     cv::Mat& roiDescriptors, int number) const;
    // 基準画像を回転させながらすべての領域の特徴点を抽出
    void extractFeaturePoint
    (const cv::Mat& img, std::vector<cv::KeyPoint>& keyPoints,
     cv::Mat& descriptors) const;
    
    // 回転前の座標の特徴点から回転後の座標の特徴点に変換
    void rotateKeyPointCoord
    (std::vector<cv::KeyPoint>& keyPoints, float angle) const;
    // ROIの座標から大域の座標に変換
    void roiCoord2GlobalCoord(std::vector<cv::KeyPoint>& keyPoints) const {
        for (int i=0; i<keyPoints.size(); i++)
            keyPoints[i].pt.y += (frameSize.height - validHeight)/2;
    };
    // 特徴点を連結させる
    static void keyPointConcat
    (std::vector<cv::KeyPoint>& dest,
     const std::vector<cv::KeyPoint>& src) {
        std::copy(src.begin(), src.end(), std::back_inserter(dest));
    };
    // 特徴点記述子を連結させる
    static void descriptorConcat(cv::Mat& dest, const cv::Mat& src) {
        cv::vconcat(dest, src, dest);
    };
    // 有効範囲にある特徴点のみを取り出す
    void filterLowLatitue
    (std::vector<cv::KeyPoint>& keyPoints, cv::Mat& descriptors) const;
    // 特徴点が有効範囲にあるか
    bool isInLowLatitude(const cv::Point2f& equirect) const;
    
private:
    const cv::Size& frameSize;
    const Transform& transform;
    
    const int validHeight;
    
    const cv::Ptr<cv::Feature2D> feature;
    
    const int divNum;
    const cv::Rect roi;
};

#endif /* ExtractFeaturePoint_hpp */
