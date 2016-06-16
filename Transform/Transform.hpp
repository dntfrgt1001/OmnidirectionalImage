//
//  Transform.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/01/10.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef Transform_hpp
#define Transform_hpp

#include <stdio.h>

#include <opencv2/core/core.hpp>

class Transform{
public:
    Transform(const cv::Size& frameSize);
    
    int theta2u(float theta) const;
    int phi2v(float phi) const;
    float u2theta(int u) const;
    float v2phi(int v) const;
    // 画像座標と極座標の変換
    // 0<= u <width, 0<= v <height
    // -pi<= theta <pi, -pi/2<= phi < pi/2
    void orth2ang(int u, int v, float& theta, float& phi) const;
    void ang2orth(float theta, float phi, int& u, int &v) const;
    // 画像座標と極座標の長さの変換
    int dtheta2u(float theta) const;
    int dphi2v(float phi) const;
    // ２次元画像上の点を３次元空間上の点に変換
    void orth2d2orth3d(const cv::Point2f& point2d, cv::Point3f& point3d) const;
    // ３次元空間上の点を２次元画像上の点に変換
    void orth3d2orth2d(const cv::Point3f& point3d, cv::Point2f& point2d) const;
    // 回転行列を使って画像座標を変換
    void orth2d2orth2dWithRotMat
    (const cv::Point2f& forPoint2d, cv::Point2f& latPoint2d,
     const cv::Mat& rotMat) const;
    void orth2d2orth2dWithRotMat
    (int u, int v, int& ur, int& vr, const cv::Mat& rotMat) const;
    // 回転行列を使って画像を変換
    void rotateImgWithRotMat
    (const cv::Mat& img, cv::Mat& rotImg, const cv::Mat& rotMat) const;
    // 画像をX軸回り(縦方向)に回転
    void rotateVerticalImg
    (float chi, const cv::Mat& img, cv::Mat& rotImg)const;
    void rotateVerticalOrthDot
    (float chi, int u, int v, int& ur, int& vr) const;
    void rotateVerticalAngDot
    (float chi, float theta, float phi, float& thetar, float& phir) const;
    
    //return the theta value where -pi <= theta < pi
    //return the u value where 0 <= u < width
    double normalizeTheta(float rawTheta) const;
    int normalizeU(int rawU) const;
    double normalizePhi(float rawPhi) const;
    int normalizeV(int rawV) const;
    
private:
    const cv::Size& frameSize;
};
#endif /* Transform_hpp */
