//
//  CalcOpticalFlow.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef CalcOpticalFlow_hpp
#define CalcOpticalFlow_hpp

#include <stdio.h>

#include <opencv2/core.hpp>
#include <opencv2/tracking.hpp>

class CalcOpticalFlow {
public:
    CalcOpticalFlow();
    
    void getOpticalFlow
    (const cv::Mat& forPersImg, const cv::Mat& latPersImg);
    
    cv::Mat getMask();
    
private:
    
};

#endif /* CalcOpticalFlow_hpp */
