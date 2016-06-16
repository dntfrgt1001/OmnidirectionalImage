//
//  MatchImagePair.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/06/15.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef MatchImagePair_hpp
#define MatchImagePair_hpp

#include <stdio.h>
#include <iostream>

#include <opencv2/core/core.hpp>

#include "Transform.hpp"
#include "ExtractFeaturePoint.hpp"
#include "MatchFeaturePoint.hpp"
#include "Affine.hpp"


class MatchImagePair
{
public:
    MatchImagePair
    (Transform& transform, ExtractFeaturePoint& extractFeature,
     MatchFeaturePoint& matchFeature, Affine& affine);
    ~MatchImagePair();
    
    void ModifylatterImg
    (const cv::Mat& forImg, const cv::Mat& latImg, cv::Mat& modLatImg);
    
private:
    Transform& transform;
    ExtractFeaturePoint& extractFeature;
    MatchFeaturePoint& matchFeature;
    Affine& affine;
};

#endif /* MatchImagePair_hpp */
