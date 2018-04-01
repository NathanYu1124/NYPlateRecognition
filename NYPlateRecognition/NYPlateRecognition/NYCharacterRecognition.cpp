//
//  NYCharacterRecognition.cpp
//  NYPlateRecognition
//
//  Created by NathanYu on 07/02/2018.
//  Copyright © 2018 NathanYu. All rights reserved.
//

#include "NYCharacterRecognition.hpp"


// 字符识别
vector<string> NYCharacterRecognition::recognizeChars(vector<NYCharacter> charsVec)
{
    vector<string> results;
    
    return results;
}

// 车牌字符识别
vector<string> NYCharacterRecognition::recognizeChars(vector<NYPlate> &plates)
{
    vector<string> licenses;
    string plate_lic;
    
    NYCharacterPartition charsDivider;
    NYCharacterJudge charsJudge;
    
    vector<Mat> grayChars;
        
    for (int i = 0; i < plates.size(); i++) {
        grayChars = charsDivider.divideCharacters(plates[i]);
        
        for (int j = 1; j < grayChars.size(); j++) {
            charsJudge.test(grayChars[j]);
        }
        
//        plate_lic = charsJudge.classifyChars(plates[i]);
//        licenses.push_back(plate_lic);
    }
    
    return licenses;
}
