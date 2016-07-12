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
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class Transform{
public:
    Transform(const cv::Size& frameSize);
    
    // 画像の大きさを変換
    void resizeImg(const cv::Mat& img, cv::Mat& resImg) const {
        cv::resize(img, resImg, frameSize);
    }
    // 画像座標と球面極座標の変換
    float theta2u(float theta) const {
        return theta*frameSize.width/(2.0*M_PI) + frameSize.width/2.0;
    }
    float phi2v(float phi) const {
        return -1.0*phi*frameSize.height/M_PI + frameSize.height/2.0;
    }
    float u2theta(float u) const {
        return (u - frameSize.width/2.0) * 2.0*M_PI/frameSize.width;
    }
    float v2phi(float v) const {
        return -1.0 * (v - frameSize.height/2.0) * M_PI/frameSize.height;
    }
    // 画像座標と極座標の変換 0<=u<width,0<=v<height,-pi<=theta<pi,-pi/2<=phi<pi/2
    void ang2orth(float theta, float phi, float& u, float &v) const {
        u = theta2u(theta); v = phi2v(phi);
    }
    void orth2ang(float u, float v, float& theta, float& phi) const {
        theta = u2theta(u); phi = v2phi(v);
    }
    // 画像座標と極座標の長さの変換
    float dtheta2u(float theta) const {
        return theta * frameSize.width / (2.0 * M_PI);
    }
    float dphi2v(float phi) const {
        return phi * frameSize.height / M_PI;
    }
    
    // ２次元画像上の点群を３次元空間上の点群に変換
    void orth2D2orth3D
    (const std::vector<cv::Point2f>& points2d,
     std::vector<cv::Point3f>& points3d) const;
    // ２次元画像上の点を３次元空間上の点に変換
    void orth2D2orth3D(const cv::Point2f& point2d, cv::Point3f& point3d) const;
    // ３次元空間上の点群を２次元画像上の点群に変換
    void orth3D2orth2D
    (const std::vector<cv::Point3f>& points3d,
     std::vector<cv::Point2f>& points2d) const;
    // ３次元空間上の点を２次元画像上の点に変換
    void orth3D2orth2D(const cv::Point3f& point3d, cv::Point2f& point2d) const;
    
    // 回転行列を使って３次元空間で変換
    void orth3D2orth3DWithRotMat
    (const cv::Point3f& forPoint3d, cv::Point3f& latPoint3d,
     const cv::Mat& rotMat) const;
    // 回転行列を使って画像座標を変換
    void orth2D2orth2DWithRotMat
    (const cv::Point2f& forPoint2d, cv::Point2f& latPoint2d,
     const cv::Mat& rotMat) const;
    void orth2D2orth2DWithRotMat
    (float u, float v, float& ur, float& vr, const cv::Mat& rotMat) const;
    // 回転行列を使って画像を変換　逆向きの回転行列を使うことに注意
    void rotateImgWithRotMat
    (const cv::Mat& img, cv::Mat& rotImg, const cv::Mat& rotMat) const;
    // バイリニア補間による画素値の決定
    void getDotBilinear
    (const cv::Mat& img, float u, float v, cv::Vec3b& dot) const;
    
    // 画像をX軸回り(縦方向)に回転
    void rotateVerticalImg
    (float chi, const cv::Mat& img, cv::Mat& rotImg) const;
    void rotateVerticalOrthDot
    (float chi, float u, float v, float& ur, float& vr) const;
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
