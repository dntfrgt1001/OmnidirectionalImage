//
//  Match.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/01/11.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Match.hpp"
#include <iostream>
#include <numeric>

Match::Match(const cv::Size& frameSize, const cv::Mat& stdImg, Rotate& rot,
             double matchAngHeight):
frameSize(frameSize), stdImg(stdImg), rot(rot)
{
    int matchOrthHeight = (int) round(matchAngHeight * frameSize.height / M_PI);
    roi = cv::Rect(0, frameSize.height/2 - matchOrthHeight/2,
                   frameSize.width, matchOrthHeight);
    
    cv::Mat stdRoiImg = (stdImg)(roi);
    
    cv::Ptr<cv::FeatureDetector> detector = cv::FeatureDetector::create("SIFT");
    detector->detect(stdRoiImg, stdKeyPoints);
    
    cv::Ptr<cv::DescriptorExtractor> extractor = cv::DescriptorExtractor::create("SIFT");
    extractor->compute(stdRoiImg, stdKeyPoints, stdDescriptor);
}

void Match::getKeyMatch(const cv::Mat &img, std::vector<cv::KeyPoint>& keyPoints, std::vector<cv::DMatch> &dMatches)
{
    const cv::Mat roiImg = img(roi);
    
    cv::Ptr<cv::FeatureDetector> detector = cv::FeatureDetector::create("SIFT");
    detector->detect(roiImg, keyPoints);
    
    cv::Ptr<cv::DescriptorExtractor> extractor = cv::DescriptorExtractor::create("SIFT");
    cv::Mat descriptor;
    extractor->compute(roiImg, keyPoints, descriptor);
    
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("BruteForce");
    std::vector<cv::DMatch> dMatches1, dMatches2;
    matcher->match(stdDescriptor, descriptor, dMatches1);
    matcher->match(descriptor, stdDescriptor, dMatches2);
    
    crossMatch(dMatches1, dMatches2, dMatches);
}

void Match::rotateYMatch(const cv::Mat &img, cv::Mat &rotImg)
{
    /*
    const cv::Mat roiImg = img(roi);

    cv::Ptr<cv::FeatureDetector> detector = cv::FeatureDetector::create("SIFT");
    std::vector<cv::KeyPoint> keyPoints;
    detector->detect(roiImg, keyPoints);
    
    cv::Ptr<cv::DescriptorExtractor> extractor = cv::DescriptorExtractor::create("SIFT");
    cv::Mat descriptor;
    extractor->compute(roiImg, keyPoints, descriptor);
    
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("BruteForce");
    std::vector<cv::DMatch> dmatch1, dmatch2, dmatch;
    matcher->match(stdDescriptor, descriptor, dmatch1);
    matcher->match(descriptor, stdDescriptor, dmatch2);
    
    crossMatch(dmatch1, dmatch2, dmatch);
    */
    std::vector<cv::KeyPoint> keyPoints;
    std::vector<cv::DMatch> dMatches;
    
    getKeyMatch(img, keyPoints, dMatches);
    
    cv::Point2f movePoint;
    getMoveTrimMean(keyPoints, dMatches, movePoint, 1.0/3.0);
    
    rot.rotateYOrth((-1) * round(movePoint.x), img, rotImg);
}

void Match::getMoveMean(std::vector<cv::KeyPoint> &keyPoints,
                        std::vector<cv::DMatch> &dMatches,
                          cv::Point2f& movePoint)
{
    cv::Point2f avePoint(0.0, 0.0);
    for (int i=0; i<dMatches.size(); i++) {
        cv::Point2f pt1 = stdKeyPoints[dMatches[i].queryIdx].pt;
        cv::Point2f pt2 = keyPoints[dMatches[i].trainIdx].pt;
        
        if (pt1.x <= pt2.x) avePoint += pt2 - pt1;
        else avePoint += cv::Point2f(frameSize.width, 0.0) + pt2 - pt1;
    }
    avePoint *= 1.0/dMatches.size();

    int count = 0;
    double aveLength = cv::norm(avePoint);
    movePoint = cv::Point2f(0.0, 0.0);
    for (int i=0; i<dMatches.size(); i++) {
        cv::Point2f pt1 = stdKeyPoints[dMatches[i].queryIdx].pt;
        cv::Point2f pt2 = keyPoints[dMatches[i].trainIdx].pt;
        
        cv::Point2f tmpPoint;
        if (pt1.x <= pt2.x) tmpPoint = pt2 - pt1;
        else tmpPoint = cv::Point2f((float)frameSize.width, 0.0) + pt2 - pt1;
        
        /*
        double err = cv::norm(avePoint - tmpPoint);
        if (err <= aveLength / 4.0) {
            movePoint += tmpPoint;
            count++;
        }
         */
        double err = cv::norm(avePoint - tmpPoint);
        if (std::fabs(tmpPoint.y) <= 20 && err <= aveLength / 2.0) {
            movePoint += tmpPoint;
            count++;
        }
        
    }
    movePoint *= 1.0/count;
    
    std::cout << "delta = " << movePoint << ", number = " << count << std::endl;
}

void Match::getMoveTrimMean(std::vector<cv::KeyPoint> &keyPoints,
                            std::vector<cv::DMatch> &dMatches,
                            cv::Point2f &movePoint, double trimRatio)
{
    std::vector<float> moveVec;
    for (int i=0; i<dMatches.size(); i++) {
        cv::Point2f pt1 = stdKeyPoints[dMatches[i].queryIdx].pt;
        cv::Point2f pt2 = keyPoints[dMatches[i].trainIdx].pt;
        
        cv::Point2f tmpVec;
        if (pt1.x <= pt2.x) tmpVec = pt2 - pt1;
        else tmpVec = cv::Point2f(frameSize.width, 0.0) + pt2 - pt1;
        
        moveVec.push_back(cv::norm(tmpVec));
    }
    
    std::sort(moveVec.begin(), moveVec.end());
    
    int range = round(dMatches.size() * trimRatio);
    moveVec.erase(moveVec.begin(), moveVec.begin()+range);
    moveVec.erase(moveVec.begin()+range, moveVec.end());
    
    float average = std::accumulate(moveVec.begin(), moveVec.end(), 0.0)/moveVec.size();
    
    movePoint = cv::Point2f(average, 0);
    
    std::cout << "delta = " << movePoint << ", number = " << moveVec.size() << std::endl;
}

void Match::crossMatch(std::vector<cv::DMatch> &dMatches1, std::vector<cv::DMatch> &dMatches2, std::vector<cv::DMatch> &dMatches)
{
    for (int i=0; i<dMatches1.size(); i++) {
        cv::DMatch forward = dMatches1[i];
        cv::DMatch backward = dMatches2[forward.trainIdx];
        /*
        std::cout << "[" << i << "] = (" << dMatches1[i].queryIdx << ", " << dMatches1[i].trainIdx << "), (" << dMatches2[i].queryIdx << ", " << dMatches2[i].trainIdx << ")" << std::endl;
        */

        if (forward.queryIdx == backward.trainIdx) {
            dMatches.push_back(forward);
        }
    }
}


#include <iostream>
#include <fstream>
void Match::rotateXMatch(const cv::Mat &img, cv::Mat &rotImg)
{
    const int divNum = 12;
    const double deltaChi = M_PI / (double)divNum;
    
    cv::namedWindow("StandardImg", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::namedWindow("DeltaRotImg", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    
//    const std::string workDir = "/Users/masakazu/Documents/Koike lab/product/OmnidirectionalImage/working/pictures/";
    
    
    for (int i=0; i<=divNum; i++) {
        double curChi = -1 * M_PI /2 + deltaChi * i;
        
        cv::Mat deltaRotImg(frameSize, CV_8UC3);
        rot.rotateXAng(curChi, img, deltaRotImg);
        
        const std::string curName = "rot" + std::to_string(i) + ".jpg";
//        cv::imwrite(workDir+curName, deltaRotImg);
        
        std::vector<cv::KeyPoint> keyPoints;
        std::vector<cv::DMatch> dMatches;
        getKeyMatch(deltaRotImg, keyPoints, dMatches);
        
        cv::imshow("StandardImg", stdImg);
        cv::imshow("DeltaRotImg", deltaRotImg);
        
//        cv::waitKey(-1);
    }
}

double Match::getMatchScoreNum(std::vector<cv::DMatch> &dMatches)
{
    return (double) dMatches.size();
}

double Match::getMatchScoreDistance(std::vector<cv::DMatch> &dMatches)
{
    double accum = 0.0;
    
    for (int i=0; i<dMatches.size(); i++) {
        accum += dMatches[i].distance;
    }
    accum /= dMatches.size();
    
    return (double) accum;
}