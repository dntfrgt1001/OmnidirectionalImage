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
    
    void orth2ang(int u, int v, double& theta, double& phi) const;
    void ang2orth(double theta, double phi, int& u, int &v) const;
    
    int dtheta2u(double theta) const;
    int dphi2v(double phi) const;
    
    //return the theta value where -pi <= theta < pi
    //return the u value where 0 <= u < width
    double normalizeTheta(double rawTheta) const;
    int normalizeU(int rawU) const;
    
private:
    const cv::Size& frameSize;
};
#endif /* Transform_hpp */
