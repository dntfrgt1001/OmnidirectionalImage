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
#include "Estimate.hpp"
#include "Rotation.hpp"

class OpticalFlow {
public:
    OpticalFlow
    (const Transform& tf, const Perspective& ps,
     const Estimate& est): tf(tf), ps(ps), est(est){};
    
    void estRotMatFromOpticalFlow
    (const cv::Mat& forImg, const cv::Mat& latImg,
     const cv::Mat& froMat, cv::Mat& estRotMat) const;
    
    void getNormalPair
    (const cv::Mat& forImg, const cv::Mat& latImg, const cv::Mat& rotMat,
     std::vector<cv::Point2f>& forNormals,
     std::vector<cv::Point2f>& latNormlas) const;
    
    void getNormalPairOneDir
    (const cv::Mat& forImg, const cv::Mat& latImg, const cv::Mat& rotMat,
     std::vector<cv::Point2f>& forNormals,
     std::vector<cv::Point2f>& latNormals, const bool isFront) const;
    
    void getFeatures
    (const cv::Mat& persImg, std::vector<cv::Point2f>& pers,
     const cv::Mat& mask) const;

    void getOpticalFlow
    (const cv::Mat& forPersImg, const cv::Mat& latPersImg,
     std::vector<cv::Point2f>& forPerss,
     std::vector<cv::Point2f>& latPerss) const;
    
    void removeErrorFlow
    (std::vector<cv::Point2f>& forPerss,
     std::vector<cv::Point2f>& latPerss,
     std::vector<uchar> status) const;
    
    void removeOutlier
    (std::vector<cv::Point2f>& forPerss,
     std::vector<cv::Point2f>& latPerss) const;
    
    bool isOutlier
    (const cv::Point2f& forPers, const cv::Point2f& latPers) const {
        const cv::Mat& inParaMat = ps.getInParaMat();
        cv::Point2f forNormal, latNormal;
        tf.pers2normal(forPers, forNormal, inParaMat);
        tf.pers2normal(latPers, latNormal, inParaMat);
        return !isCosCond(forNormal, latNormal);
    };

    bool isCosCond
    (const cv::Point2f& forNormal, const cv::Point2f& latNormal) const
    {
        cv::Point2f radVec = (forNormal + latNormal) / 2.0;
        cv::Point2f cirVec = latNormal - forNormal;
        
        float cosAng = radVec.dot(cirVec) /
                       (cv::norm(radVec) * cv::norm(cirVec));
        
        const float cosRange = 1.0 / 4.0;
        
        if (-cosRange < cosAng && cosAng < cosRange) {
            return true;
        } else {
            return false;
        }
    }
    
    void drawPoint
    (const cv::Mat& persImg, const std::vector<cv::Point2f>& perss,
     cv::Mat& outImg) const;
    
    void drawOpticalFlow
    (const cv::Mat& persImg, const std::vector<cv::Point2f>& forPerss,
     const std::vector<cv::Point2f>& latPerss, cv::Mat& outImg) const;
    
private:
    const Transform& tf;
    const Perspective& ps;
    const Estimate& est;
};

#endif /* OpticalFlow_hpp */
