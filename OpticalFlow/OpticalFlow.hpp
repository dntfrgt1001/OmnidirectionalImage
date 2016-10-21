//
//  OpticalFlow.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/20.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef OpticalFlow_hpp
#define OpticalFlow_hpp

#include <stdio.h>

#include <opencv2/tracking.hpp>
#include <opencv2/video.hpp>

#include "Transform.hpp"
#include "Perspective.hpp"

class OpticalFlow {
public:
    OpticalFlow(const Transform& tf, const Perspective& ps): tf(tf), ps(ps){};

    void getFeatures
    (const cv::Mat& persImg, std::vector<cv::Point2f>& pers,
     const cv::Mat& mask) const;

    void getOpticalFlow
    (const cv::Mat& forPersImg, const cv::Mat& latPersImg,
     const std::vector<cv::Point2f>& forPerss,
     std::vector<cv::Point2f>& latPerss) const;
    
    void removeOutlier
    (std::vector<cv::Point2f>& forPerss,
     std::vector<cv::Point2f>& latPerss) const;
    
    bool isOutlier
    (const cv::Point2f& forPers, const cv::Point2f& latPers) const {
        const cv::Mat& inParaMat = ps.getInParaMat();
        cv::Point2f forNormal, latNormal;
        tf.pers2normal(forPers, forNormal, inParaMat);
        tf.pers2normal(latPers, latNormal, inParaMat);
        return true;
    };

    
    void drawPoint
    (const cv::Mat& persImg, const std::vector<cv::Point2f>& perss,
     cv::Mat& outImg);
    
    void drawOpticalFlow
    (const cv::Mat& persImg, const std::vector<cv::Point2f>& forPerss,
     const std::vector<cv::Point2f>& latPerss, cv::Mat& outImg);
    
private:
    const Transform& tf;
    const Perspective& ps;
};

#endif /* OpticalFlow_hpp */
