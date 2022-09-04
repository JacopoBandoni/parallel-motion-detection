#include <iostream>
#include <thread>
#include <utility>
#include <chrono>
#include <vector>
#include <stdio.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

#include <Timer.cpp>
#include <FrameComputation.cpp>
#include <Seq.cpp>
#include <ThreadPar.cpp>
#include <ffPar.cpp>
#include <ffPar2.cpp>

using namespace cv;
using namespace std;

int main(int argc, char * argv[]){

    if(argc <= 1){
        cout << "Choose an index of the program you would like to run" << endl;
        cout << "0 sequential program" << '\n';
        cout << "1 thread implementation program" << '\n';
        cout << "2 fast flow implementation program" << endl;
        return -1;
    }
    if(argc <= 2)
    {
        cout << "Pass as input the kernel size"<< endl;
        return -1;
    } 
    if(argc <= 3)
    {
        cout << "Pass as input the percentage treshold" << endl;
        return -1;
    }

    int programVersion = atoi(argv[1]);
    int kernelSize = atoi(argv[3]);
    float percentageTreshold = atoi(argv[4]);
    string videoName = argv[5];
    int nWorkers;

    if(programVersion > 0){
        if(argc <= 3)
            cout << "Pass another parameter to indicate the number of worker" << endl;
        nWorkers = atoi(argv[2]);
    }

    if(atoi(argv[1]) == 0)
        sequential("../video/" + videoName, kernelSize, percentageTreshold);
    
    if(atoi(argv[1]) == 1)
        threadPar("../video/" + videoName, nWorkers, kernelSize, percentageTreshold);

    if(atoi(argv[1]) == 2)
        ffPar("../video/" + videoName, nWorkers, kernelSize, percentageTreshold);

    if(atoi(argv[1]) == 3)
        ffPar2("../video/" + videoName, nWorkers, kernelSize, percentageTreshold);
}