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
    
    int theta2u(double theta) const;
    int phi2v(double phi) const;
    double u2theta(int u) const;
    double v2phi(int v) const;
    // 画像座標と極座標の変換
    // 0<= u <width, 0<= v <height
    // -pi<= theta <pi, -pi/2<= phi < pi/2
    void orth2ang(int u, int v, double& theta, double& phi) const;
    void ang2orth(double theta, double phi, int& u, int &v) const;
    // 画像座標と極座標の長さの変換
    int dtheta2u(double theta) const;
    int dphi2v(double phi) const;
    // ２次元画像上の点を３次元空間上の点に変換
    void orth2d2orth3d(const cv::Point2f& point2d, cv::Point3f& point3d);
    // ３次元空間上の点を２次元画像上の点に変換
    void orth3d2orth2d(const cv::Point3f& point3d, cv::Point2f& point2d);
    // 回転行列を使って画像座標を変換
    void orth2d2orth2dWithRotMat
    (const cv::Point2f& forPoint2d, cv::Point2f& latPoint2d,
     const cv::Mat& rotMat);
    void orth2d2orth2dWithRotMat
    (int u, int v, int& ur, int& vr, const cv::Mat& rotMat);
    // 回転行列を使って画像を変換
    void rotateImgWithRotMat
    (const cv::Mat& img, cv::Mat& rotImg, const cv::Mat& rotMat);
    
    
    //return the theta value where -pi <= theta < pi
    //return the u value where 0 <= u < width
    double normalizeTheta(double rawTheta) const;
    int normalizeU(int rawU) const;
    double normalizePhi(double rawPhi) const;
    int normalizeV(int rawV) const;
    
private:
    const cv::Size& frameSize;
};
#endif /* Transform_hpp */
