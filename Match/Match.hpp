//
//  Match.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/01/11.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef Match_hpp
#define Match_hpp

#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include "Rotate.hpp"

class Match{
public:
    Match(const cv::Size* frameSize, cv::Mat* stdImg, Rotate* rot,
          int matchRange);
    void rotateYMatch(const cv::Mat& img, cv::Mat& rotImg);
    void crossMatch(cv::vector<cv::DMatch>& dmatch1,
                    cv::vector<cv::DMatch>& dmatch2,
                    cv::vector<cv::DMatch>& crossMatch);
    
private:
    const cv::Size* frameSize;
    cv::Mat* stdImg;
    Rotate* rot;
    
    int matchRange;
    
    cv::Rect roi;
    cv::vector<cv::KeyPoint> stdKeyPoints;
    cv::Mat stdDescriptor;
};

#endif /* Match_hpp */
