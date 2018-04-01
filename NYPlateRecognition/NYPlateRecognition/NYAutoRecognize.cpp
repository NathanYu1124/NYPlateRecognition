//
//  NYAutoRecognize.cpp
//  NYPlateRecognition
//
//  Created by NathanYu on 07/02/2018.
//  Copyright © 2018 NathanYu. All rights reserved.
//

#include "NYAutoRecognize.hpp"


vector<string> NYAutoRecognize::recognizePlateNumber(Mat src)
{
    NYPlateDetect detecter;
    NYCharacterRecognition charsJudge;
    vector<string> licenses;
    
    vector<NYPlate> plates = detecter.detectPlates(src);
    licenses = charsJudge.recognizeChars(plates);
    
    return licenses;
}
