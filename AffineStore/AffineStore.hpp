//
//  AffineStore.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/06/08.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef AffineStore_hpp
#define AffineStore_hpp

#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class AffineStore
{
public:
    AffineStore();
    ~AffineStore();
    
    void extractRotMatFromAffineMat(const cv::Mat& affMat, cv::Mat& rotMat);
    void accRotMat(const cv::Mat& rotMat);
    
private:
    cv::Mat accMat;
};

#endif /* AffineStore_hpp */
