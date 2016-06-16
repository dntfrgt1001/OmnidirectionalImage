//
//  Affine.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/06/09.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef Affine_hpp
#define Affine_hpp

#include <stdio.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include "Transform.hpp"

class Affine
{
public:
    Affine(Transform& transform);
    ~Affine();
    
    void estimate3DRotMat
    (std::vector<cv::Point3f>& forPoints, std::vector<cv::Point3f>& latPoints,
     cv::Mat& estRotMat);
    // アフィン行列から回転行列を取り出す
    void extractRotMatFromAffineMat(const cv::Mat& affMat, cv::Mat& rotMat);
    // 現在の回転行列を集約する
    void accRotMat(const cv::Mat& curRotMat);
    // アフィン行列を推定する
    void estimate3DAffineMat
    (std::vector<cv::Point3f>& forPoints, std::vector<cv::Point3f>& latPoints,
     cv::Mat& affMat);
    // アフィン行列の推定のための座標の対を取り出す
    void get3DPointPair
    (const std::vector<cv::KeyPoint>& forKeyPoints,
     const std::vector<cv::KeyPoint>& latKeyPoints,
     const std::vector<cv::DMatch>& dMatches,
     std::vector<cv::Point3f>& for3DPoints,
     std::vector<cv::Point3f>& lat3DPoints);
    
    // 誤差の乗った回転行列を正規化
    void normalizeRotMat(cv::Mat& rotMat);
    
private:
    Transform& transform;
    cv::Mat accMat;
};

#endif /* Affine_hpp */
