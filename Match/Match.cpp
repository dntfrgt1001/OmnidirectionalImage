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

Match::Match(const cv::Size& frameSize, const cv::Mat& stdImg, Transform& transform, Rotate& rot, int divNum):
frameSize(frameSize), transform(transform), stdImg(stdImg), rot(rot),
divNum(divNum)
{
    int matchOrthHeight = transform.dphi2v(M_PI / divNum);
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

void Match::rotateYMatch(const cv::Mat &img, cv::Mat &modImg)
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
    
    rot.rotateYOrth((-1) * round(movePoint.x), img, modImg);
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
    
//    std::cout << "delta = " << movePoint << ", number = " << moveVec.size() << std::endl;
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

void Match::rotateXMatch(const cv::Mat &img, cv::Mat &rotImg)
{
    double staChi = -1.0 * M_PI / 2.0;
    double chiWidth = M_PI;
    double maxChiOneScan = searchRotateX(img, staChi, chiWidth, divNum);
    //rot.rotateXAng(maxChiOneScan, img, rotImg);
    
    //std::cout << "modAngle1 = " << maxChiOneScan << std::endl;
    
    double staChi2 = maxChiOneScan - M_PI / (divNum*2);
    double chiWidth2 = M_PI / divNum;
    double maxChiTwoScan = searchRotateX(img, staChi2, chiWidth2, divNum);
    //rot.rotateXAng(maxChiTwoScan, img, rotImg);
    
    modUpDown(img, rotImg, maxChiTwoScan);
    
    std::cout << "modAngle2 = " << maxChiTwoScan << std::endl;
    
//    cv::namedWindow("Modified Image", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
//    cv::namedWindow("Standard Image", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
//    cv::imshow("Modified Image", rotImg);
//    cv::imshow("Standard Image", stdImg);
    
//    cv::waitKey(-1);
}

void Match::modUpDown(const cv::Mat &img, cv::Mat &modImg, double chi)
{
    cv::Mat smpModImg(frameSize, CV_8UC3);
    cv::Mat invModImg(frameSize, CV_8UC3);
    
    rot.rotateXAng(chi, img, smpModImg);
    rot.rotateXAng(chi + M_PI, img, invModImg);
    
    std::vector<cv::KeyPoint> keyPoints1, keyPoints2;
    std::vector<cv::DMatch> dMatches1, dMatches2;
    getKeyMatch(smpModImg, keyPoints1, dMatches1);
    getKeyMatch(invModImg, keyPoints2, dMatches2);
    
    cv::Point2f movePoint1, movePoint2;
    
    double trimRatio = 1.0 / 3.0;
/*    getMoveTrimMean(keyPoints1, dMatches1, movePoint1, trimRatio);
    getMoveTrimMean(keyPoints2, dMatches2, movePoint2, trimRatio);
    
    if (movePoint1.x < movePoint2.x)
        smpModImg.copyTo(modImg);
    else
        invModImg.copyTo(modImg);
*/
    double score1 = getMatchDistance(keyPoints1, dMatches1, trimRatio);
    double score2 = getMatchDistance(keyPoints2, dMatches2, trimRatio);

    if (score1 < score2)
        smpModImg.copyTo(modImg);
    else
        invModImg.copyTo(modImg);
    
    cv::namedWindow("smpImg", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::namedWindow("invImg", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::imshow("smpImg", smpModImg);
    cv::imshow("invImg", invModImg);
    
    std::cout << "(score1, score2) = (" << score1 << ", " << score2 << ")" << std::endl;
    
 //   cv::waitKey(-1);
}

double Match::getMatchDistance(std::vector<cv::KeyPoint> &keyPoints, std::vector<cv::DMatch> &dMatches, double trimRatio)
{
    std::vector<float> accVec;
    
    for (int i=0; i<dMatches.size(); i++) {
        cv::Point2f pt1 = stdKeyPoints[dMatches[i].queryIdx].pt;
        cv::Point2f pt2 = keyPoints[dMatches[i].trainIdx].pt;
        
        //accVec.push_back(fabs(pt1.x - pt2.x));
        accVec.push_back(cv::norm(pt1 - pt2));
    }
    
    std::sort(accVec.begin(), accVec.end());
    int range = (dMatches.size() * trimRatio);
    accVec.erase(accVec.begin(), accVec.begin()+range);
    accVec.erase(accVec.begin()+range, accVec.end());
    
    float average = std::accumulate(accVec.begin(), accVec.end(), 0.0) / accVec.size();
    
    return average;
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
    
    return accum;
}

void Match::lowLatitudeMask(const cv::Mat &img, cv::Mat maskImg)
{
    double larPhi = M_PI / 6;
    
    for (int u=0; u<frameSize.width; u++) {
        for (int v=0; v<frameSize.height; v++) {
            double theta = transform.u2theta(u);
            double phi = transform.v2phi(v);
            double lat = atan(tan(phi)/cos(theta));
            
            if (-1.0 * larPhi/2.0 <= lat && lat < larPhi/2.0) {
                maskImg.at<cv::Vec3b>(v, u) = cv::Vec3b(0, 0, 0);
            }else{
                maskImg.at<cv::Vec3b>(v, u) = img.at<cv::Vec3b>(v, u);
            }
        }
    }
}

double Match::searchRotateX(const cv::Mat& img,
                            double staChi, double chiWidth, int divNum)
{
    double deltaChi = chiWidth / divNum;
    /*
    cv::namedWindow("Standard Image", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::namedWindow("Input Image", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::namedWindow("CurRot Image", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    */
    double maxScore = 0.0;
    double maxChi = 0.0;
    for (int i=0; i<divNum; i++) {
        double curChi = staChi + deltaChi * i;
        
        cv::Mat curRotImg(frameSize, CV_8UC3);
        rot.rotateXAng(curChi, img, curRotImg);
        
        std::vector<cv::KeyPoint> keyPoints;
        std::vector<cv::DMatch> dMatches;
        getKeyMatch(curRotImg, keyPoints, dMatches);
        
        double curScore = getMatchScoreNum(dMatches);
        
        if (maxScore < curScore) {
            maxScore = curScore;
            maxChi = curChi;
        }
        /*
        cv::imshow("Standard Image", stdImg);
        cv::imshow("Input Image", img);
        cv::imshow("CurRot Image", curRotImg);
        */
        //std::cout << "(maxChi, maxScore, curChi, #curScore) = " << maxChi << ", " << maxScore << ", " << curChi << ", " << curScore << ")" << std::endl;
        //std::cout << curChi << ", " << curScore << std::endl;
        
        //cv::waitKey(-1);
    }
    
    return maxChi;
}