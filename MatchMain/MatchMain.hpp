//
//  MatchMain.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/06/15.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef MatchMain_hpp
#define MatchMain_hpp

#include <stdio.h>
#include <iostream>

#include <opencv2/core/core.hpp>

#include "Transform.hpp"
#include "ExtractFeaturePoint.hpp"
#include "MatchFeaturePoint.hpp"
#include "Affine.hpp"


class MatchMain
{
public:
    MatchMain
    (Transform& transform, ExtractFeaturePoint& extractFeature,
     MatchFeaturePoint& matchFeature, Affine& affine);
    ~MatchMain();
    
    void ModifylatterImg
    (const cv::Mat& forImg, const cv::Mat& latImg, cv::Mat& modLatImg);
    
private:
    Transform& transform;
    ExtractFeaturePoint& extractFeature;
    MatchFeaturePoint& matchFeature;
    Affine& affine;
};

#endif /* MatchMain_hpp */
