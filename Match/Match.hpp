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
#include "Transform.hpp"

class Match{
public:
    Match(const cv::Size& frameSize, const cv::Mat& stdImg,
          Transform& transform, Rotate& rot, int divNum);
    
    void rotateYMatch(const cv::Mat& img, cv::Mat& modImg);
    void crossMatch(std::vector<cv::DMatch>& dMatches1,
                    std::vector<cv::DMatch>& dMatches2,
                    std::vector<cv::DMatch>& dMatches);
    void getMoveMean(std::vector<cv::KeyPoint>& keyPoints,
                     std::vector<cv::DMatch>& dMathces,
                       cv::Point2f& movePoint);
    void getMoveTrimMean(std::vector<cv::KeyPoint>& keyPoints,
                         std::vector<cv::DMatch>& dMatches,
                         cv::Point2f& movePoint, double trimRatio);
    
    void rotateXMatch(const cv::Mat& img, cv::Mat& modImg);
    
    void getKeyMatch(const cv::Mat& img,
                     std::vector<cv::KeyPoint>& keyPoints,
                     std::vector<cv::DMatch>& dMatches);
    
    double getMatchScoreNum(std::vector<cv::DMatch>& dMatches);
    double getMatchScoreDistance(std::vector<cv::DMatch>& dMatches);
    
    void lowLatitudeMask(const cv::Mat& img, cv::Mat maskImg);
    
    double searchRotateX(const cv::Mat& img,
                         double staChi, double chiWidth, int divNum);
    void modUpDown(const cv::Mat& img, cv::Mat& modImg, double chi);
    
    double getMatchDistance(std::vector<cv::KeyPoint>& keyPoints,
                            std::vector<cv::DMatch>& dMatches,
                            double trimRatio);
    
private:
    const cv::Size& frameSize;
    const cv::Mat& stdImg;
    Transform& transform;
    Rotate& rot;
    
    int divNum;
    cv::Rect roi;
    std::vector<cv::KeyPoint> stdKeyPoints;
    cv::Mat stdDescriptor;
};

#endif /* Match_hpp */
