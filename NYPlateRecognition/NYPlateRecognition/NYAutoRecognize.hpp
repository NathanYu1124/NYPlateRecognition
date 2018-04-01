//
//  NYAutoRecognize.hpp
//  NYPlateRecognition
//
//  Created by NathanYu on 07/02/2018.
//  Copyright © 2018 NathanYu. All rights reserved.
//

#ifndef NYAutoRecognize_hpp
#define NYAutoRecognize_hpp

#include <stdio.h>
#include "NYCharacterRecognition.hpp"
#include "NYPlateDetect.hpp"

class NYAutoRecognize {
    
public:
    
    // 识别图片上的车牌号
    vector<string> recognizePlateNumber(Mat src);
    
};


#endif /* NYAutoRecognize_hpp */
