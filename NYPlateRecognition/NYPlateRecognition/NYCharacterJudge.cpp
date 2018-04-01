//
//  NYCharacterJudge.cpp
//  NYPlateRecognition
//
//  Created by NathanYu on 29/01/2018.
//  Copyright © 2018 NathanYu. All rights reserved.
//

#include "NYCharacterJudge.hpp"

#define ANN_DATA_PATH "/Users/NathanYu/Desktop/ann/"
#define ANN_ZH_DATA_PATH "/Users/NathanYu/Desktop/annZH/"

#define ANN_MODEL_PATH "/Users/NathanYu/Documents/Development/NYPlateRecognition/NYPlateRecognition/ann.xml"
#define ANN_ZH_MODEL_PATH "/Users/NathanYu/Documents/Development/NYPlateRecognition/NYPlateRecognition/ann_zh.xml"


// ------------------- ANN ----------------------------
void NYCharacterJudge::test(cv::Mat charMat)
{
    // 加载ann模型
    ann_ = ml::ANN_MLP::load(ANN_MODEL_PATH);
    
    
        // 提取字符特征
        Mat testMat;
        Mat feature = charFeatures(charMat, 5);
        testMat.push_back(feature);
        Mat output(1, kCharactersNumber, CV_32FC1);
    
        // ann字符预测
        string char_res;
          // 非汉字
            ann_ -> predict(testMat, output);
            float maxVal = -2.f;
            
            int result = 0;
            for (int i = 0; i < kCharactersNumber; i++) {
                float val = output.at<float>(i);
                if (val > maxVal) {
                    maxVal = val;
                    result = i;
                }
            }
            char_res = kChars[result];
    
    cout << "char is : " << char_res << endl;
}


// 识别车牌字符
string NYCharacterJudge::classifyChars(NYPlate &plate)
{
    string license;
    // 加载ann模型
    ann_ = ml::ANN_MLP::load(ANN_MODEL_PATH);
//    ann_zh = ml::ANN_MLP::load(ANN_ZH_MODEL_PATH);
    
    vector<NYCharacter> charsVec = plate.getPlateChars();
    
    for (int i = 0; i < charsVec.size(); i++) {
        // 提取字符特征
        Mat testMat;
        Mat charMat = charsVec[i].getCharacterMat();
        Mat feature = charFeatures(charMat,5);
        testMat.push_back(feature);
        Mat output(1, kCharactersNumber, CV_32FC1);
        Mat output_zh(1, KChineseNumber, CV_32FC1);
        
        // ann字符预测
        string char_res;
        if (charsVec[i].getIsChinese()) {  // 汉字
            ann_zh -> predict(testMat, output_zh);
            float maxVal = -2.0;
            
            int result = 0;
            for (int i = 0; i < KChineseNumber; i++) {
                float val = output.at<float>(i);
                if (val > maxVal) {
                    maxVal = val;
                    result = i;
                }
            }
            char_res = ZH[result];
        } else {    // 非汉字
            ann_ -> predict(testMat, output);
            float maxVal = -2.0;
            
            int result = 0;
            for (int i = 0; i < kCharactersNumber; i++) {
                float val = output.at<float>(i);
                if (val > maxVal) {
                    maxVal = val;
                    result = i;
                }
            }
          char_res = kChars[result];
        }
        
        license += char_res;
        charsVec[i].setCharacterStr(char_res);
    }
    
    cout << "license: " << license << endl;
    
    plate.setPlateLicense(license);
    
    return license;
}



// 训练ANN模型
void NYCharacterJudge::trainANN()
{
    
    Mat layers;
    
    int classNumber = kCharactersNumber;    // 字符类数: 0 ~ 9, A ~ Z
    int input_number = kAnnInput;           // 输入层节点数, 特征维数
    int hidden_number = kNeurons;           // 隐藏层节点数
    int output_number = classNumber;        // 输出层节点数
    
    // 单层神经网络
    layers.create(1, 3, CV_32SC1);
    layers.at<int>(0) = input_number;
    layers.at<int>(1) = hidden_number;
    layers.at<int>(2) = output_number;
    
    // ANN参数配置
    ann_ = ml::ANN_MLP::create();
    ann_ -> setLayerSizes(layers);
    ann_ -> setActivationFunction(ml::ANN_MLP::SIGMOID_SYM,1,1);
    ann_ -> setTrainMethod(ml::ANN_MLP::TrainingMethods::BACKPROP);
    ann_ -> setTermCriteria(cvTermCriteria(CV_TERMCRIT_ITER, 30000, 0.001));
    ann_ -> setBackpropWeightScale(0.1);
    ann_ -> setBackpropMomentumScale(0.1);
    
    // 获取训练数据
    Mat trainMat, trainClasses;
    getCharsData(trainMat, trainClasses);
    
    // ANN训练
    Ptr<TrainData> train_data = TrainData::create(trainMat, ROW_SAMPLE, trainClasses);
    ann_ -> train(train_data);
    
    cout << "Training ANN model, please wait..." << endl;
    // 保存ann模型
    ann_ -> save(ANN_MODEL_PATH);
    cout << "Training is done !!!" << endl;
}

// 训练汉字ANN模型
void NYCharacterJudge::trainANN_ZH()
{
    Mat layers;
    
    int classNumber = KChineseNumber;       // 汉字类数
    int input_number = kAnnInput;           // 输入层节点数, 特征维数
    int hidden_number = kNeurons;           // 隐藏层节点数
    int output_number = classNumber;        // 输出层节点数
    
    // 单层神经网络
    layers.create(1, 3, CV_32SC1);
    layers.at<int>(0) = input_number;
    layers.at<int>(1) = hidden_number;
    layers.at<int>(2) = output_number;
    
    // ANN参数配置
    ann_zh = ml::ANN_MLP::create();
    
    ann_zh -> setLayerSizes(layers);
    ann_zh -> setActivationFunction(ml::ANN_MLP::SIGMOID_SYM,1,1);
    ann_zh -> setTrainMethod(ml::ANN_MLP::TrainingMethods::BACKPROP);
    ann_zh -> setTermCriteria(cvTermCriteria(CV_TERMCRIT_ITER, 30000, 0.001));
    ann_zh -> setBackpropWeightScale(0.1);
    ann_zh -> setBackpropMomentumScale(0.1);
    
    // 获取汉字训练数据
    Mat trainMat, trainClasses;
    getCharsZHData(trainMat, trainClasses);
    
    // ANN训练
    Ptr<TrainData> train_data = TrainData::create(trainMat, ROW_SAMPLE, trainClasses);
    ann_zh -> train(train_data);
    
    // 保存ann模型
    ann_zh -> save(ANN_ZH_MODEL_PATH);
}

// 训练所有的ann模型
void NYCharacterJudge::trainALL()
{
    trainANN();
//    trainANN_ZH();
}

// 加载待训练的汉字数据
void NYCharacterJudge::getCharsZHData(Mat &trainMat, Mat &trainLabels)
{
    char dirPath[80];
    vector<vector<string>> all_Files;   // 共31类
    all_Files.reserve(KChineseNumber);
    vector<string> files;
    
    // 获取所有字符路径
    int sample_count = 0;            // 所有字符总数
    vector<int> char_count;          // 每类字符总数
    char_count.reserve(KChineseNumber);
    for (int i = 0; i < KChineseNumber; i++) {
        files.clear();
        sprintf(dirPath, "%s%s/",ANN_ZH_DATA_PATH,zhChars[i].c_str());
        getFilesInPath(dirPath, files);
        
        char_count[i] = (int)files.size();
        sample_count += files.size();
        all_Files.push_back(files);
    }
    
    // 获取所有字符特征
    Mat trainData;
    Mat feature;
    
    int k = 0;
    // 训练标签  行：字符总数 列：字符类数 初始化为0
    Mat trainClasses(sample_count, KChineseNumber, CV_32FC1, Scalar::all(0));
    for (int i = 0; i < KChineseNumber; i++) {
        for (int j = 0; j < all_Files[i].size() ; j++) {
            Mat char_src = imread(all_Files[i][j]);
            feature = charFeatures(char_src,5);   // 行向量
            trainData.push_back(feature);
            
            trainClasses.at<float>(k,i) = 1.f;
            k++;
        }
    }
    
    trainData.copyTo(trainMat);
    trainClasses.copyTo(trainLabels);
}

// 加载待训练的字符数据
void NYCharacterJudge::getCharsData(Mat &trainMat, Mat &trainLabels)
{
    // 每个字符为一个行向量，列数为字符类数
    char  dirPath[80];
    vector<vector<string>> all_Files;   // 0~9,A~Z, 共34类,
    all_Files.reserve(kCharactersNumber);
    vector<string> files;
    // 获取所有字符路径
    int off = 0;
    int sample_count = 0;            // 所有字符总数
    for (int i = 0; i < kCharactersNumber; i++) {
        files.clear();
        
        if (i <= 9) {   // 0 ~ 9
            sprintf(dirPath, "%s%d/",ANN_DATA_PATH,i);
            getFilesInPath(dirPath, files);
            
        } else if (i == 18) {   // 处理I缺失
            off = 1;
            sprintf(dirPath, "%s%c/",ANN_DATA_PATH,i+off+55);
            getFilesInPath(dirPath, files);
        }  else if (i == 23) {  // 处理O缺失
            off = 2;
            sprintf(dirPath, "%s%c/",ANN_DATA_PATH,i+off+55);
            getFilesInPath(dirPath, files);
        } else {
            sprintf(dirPath, "%s%c/",ANN_DATA_PATH,i+off+55);
            getFilesInPath(dirPath, files);
        }
        sample_count += files.size();
        all_Files.push_back(files);
    }
    
    // 获取所有字符特征
    Mat trainData;
    Mat feature;
    
    int k = 0;
    // 训练标签  行：字符总数 列：字符类数 初始化为0
    Mat trainClasses(sample_count, kCharactersNumber, CV_32FC1, Scalar::all(0));
    for (int i = 0; i < kCharactersNumber; i++) {
        for (int j = 0; j < all_Files[i].size() ; j++) {
            Mat char_src = imread(all_Files[i][j]);
            
            // 灰度化
            cvtColor(char_src, char_src, CV_BGR2GRAY);
            // 二值化
            threshold(char_src, char_src, 0, 255, THRESH_OTSU+THRESH_BINARY);
            
            feature = charFeatures(char_src,5);   // 行向量
            trainData.push_back(feature);
            
            trainClasses.at<float>(k,i) = 1.f;
            k++;
        }
    }
    
    trainData.copyTo(trainMat);
    trainClasses.copyTo(trainLabels);
}


// 获取字符特征 （用作字符特征输入）
Mat NYCharacterJudge::charFeatures(Mat in, int sizeData)
{
    const int VERTICAL = 0;
    const int HORIZONTAL = 1;
    
    Mat lowData;
    resize(in, lowData, Size(sizeData,sizeData));
    
    // 获取累计直方图特征
    Mat vhist = projectHistogram(lowData, VERTICAL);
    Mat hhist = projectHistogram(lowData, HORIZONTAL);
    
    int numCols = vhist.cols + hhist.cols + lowData.cols * lowData.cols;
        
    Mat out = Mat::zeros(1, numCols, CV_32F);   // 存储特征行向量
    
    int j = 0;
    for (int i = 0; i < vhist.cols; i++) {  // 存储垂直直方图特征
        out.at<float>(j) = vhist.at<float>(i);
        j++;
    }
    for (int i = 0; i < hhist.cols; i++) {  // 存储水平直方图特征
        out.at<float>(j) = hhist.at<float>(i);
        j++;
    }
    for (int x = 0; x < lowData.cols; x++) {
        for (int y = 0; y < lowData.rows; y++) {
            out.at<float>(j) += (float)lowData.at<unsigned char>(x,y);
            j++;
        }
    }
  
    return out; // 行向量
}

// 获取累计直方图特征（行/列 非零元素个数）
Mat NYCharacterJudge::projectHistogram(Mat img, int model, int threshold)
{
    int sz = (model) ? img.rows : img.cols;     // 水平或垂直方向
    
    Mat mhist = Mat::zeros(1, sz, CV_32F);      // 行向量存储特征
    
    for (int j = 0; j < sz; j++) {
        Mat data = (model) ? img.row(j) : img.col(j);   // 获取输入Mat的行或列向量
        mhist.at<float>(j) = countNonZero(data);
    }
    
    double min, max;
    minMaxLoc(mhist, &min, &max);
    
    // 归一化
    if (max > 0)
        mhist.convertTo(mhist, -1, 1.0f / max, 0);

    return mhist;
}

void NYCharacterJudge::calPerFeature(cv::Mat src, vector<float> &feats)
{
    // 5*5小方格非零像素比例
    int totalVal = countNonZero(src);
    for (int i = 0; i < src.rows; i+=5) {
        for (int j = 0; j < src.cols; j+=5) {
            Mat subImg = src(Rect(j,i,5,5));
            int subVal = countNonZero(subImg);
            feats.push_back((float)subVal / totalVal);
        }
    }
}


// 计算Mat中行/列向量中大于阀值的个数
float NYCharacterJudge::countBigValue(Mat &img, int iValue)
{
    float iCount = 0.0;
    if (img.rows > 1) { // 列向量处理
        for (int i = 0; i < img.rows; i++) {    // 遍历每行
            if (img.data[i * img.step[0]] > iValue) {   // 二维Mat step[0] 一行元素的字节数
                iCount += 1.0;
            }
        }
        return iCount;
    } else {    // 行向量处理
        for (int i = 0; i < img.cols; i++) {
            if (img.data[i] > iValue) {
                iCount += 1.0;
            }
        }
        return iCount;
    }
}

#pragma mark - 梯度特征测试













































