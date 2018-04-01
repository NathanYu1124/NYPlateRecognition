//
//  NYPlateDetect.cpp
//  NYPlateRecognition
//
//  Created by NathanYu on 24/01/2018.
//  Copyright © 2018 NathanYu. All rights reserved.
//

#include "NYPlateDetect.hpp"


#define MAX_PLATES  3   // 一副图中最大车牌数

// 检测车牌
vector<NYPlate> NYPlateDetect::detectPlates(Mat src)
{
    vector<NYPlate> platesVec;
    vector<NYPlate> sobel_plates;
    vector<NYPlate> color_plates;
    
    NYPlateLocate locater;
    NYPlateJudge judge;
    
    // 颜色定位
    color_plates = locater.plateLocateWithColor(src);
    color_plates = judge.judgePlates(color_plates);
    
    cout << "color plates size :  " << color_plates.size() << endl;

    if (platesVec.size() < MAX_PLATES) { // 少于最大车牌数时需sobel定位
        // sobel定位
        sobel_plates = locater.plateLocateWithSobel(src);
        sobel_plates = judge.judgePlates(sobel_plates);
        cout << "sobel plates size : " << sobel_plates.size() << endl;
    }
    
    
    // 去除重复的车牌，去除sobel与color的交集
    platesVec = deleteRepeatPlates(color_plates, sobel_plates);
    
    cout << "total plates size : " << platesVec.size() << endl;
    
    
    // 输出车牌 (HasPlate)
//    outputPlates(platesVec);
    
    return platesVec;
}

// 去除重复的车牌 (车牌位置，误差率)
vector<NYPlate> NYPlateDetect::deleteRepeatPlates(vector<NYPlate> colorVec, vector<NYPlate> sobelVec)
{
    vector<NYPlate> resultVec;
    
    if ((colorVec.size() == 0) && (sobelVec.size() != 0)) {
        return sobelVec;
    } else if ((colorVec.size() != 0) && (sobelVec.size() == 0)){
        return colorVec;
    } else if ((colorVec.size() == 0) && (sobelVec.size() == 0)){
        return resultVec;
    } else {
        
        vector<NYPlate>::iterator color_itr = colorVec.begin();
        vector<NYPlate>::iterator sobel_itr = sobelVec.begin();
        
        while (color_itr != colorVec.end()) {
            NYPlate color_plate = NYPlate(*color_itr);
            RotatedRect color_rect = color_plate.getPlatePos();
            
            while (sobel_itr != sobelVec.end()) {
                NYPlate sobel_plate = NYPlate(*sobel_itr);
                RotatedRect sobel_rect = sobel_plate.getPlatePos();
                
                float offset_x = sobel_rect.center.x - color_rect.center.x;
                float offset_y = sobel_rect.center.y - sobel_rect.center.y;
                
                if (abs(offset_x) < 15 && abs(offset_y) < 15) {
                    sobelVec.erase(sobel_itr);
                } else{
                    sobel_itr++;
                }
            }
            
            color_itr++;
        }
        
        resultVec.insert(resultVec.end(), colorVec.begin(),colorVec.end());
        resultVec.insert(resultVec.end(), sobelVec.begin(),sobelVec.end());
        
        return resultVec;
    }
}

// 将车牌输出到指定目录
void NYPlateDetect::outputPlates(vector<NYPlate> platesVec)
{
    string output_path = "/Users/NathanYu/Desktop/HasPlate/";
    
    for (int i = 0; i < platesVec.size(); i++) {
        // 取出车牌
        Mat plate = platesVec[i].getPlateMat();
        
        char buff[50];
        sprintf(buff, "%d.jpg",i);
        string imgPath = output_path + string(buff);
        imwrite(imgPath, plate);
    }
}

