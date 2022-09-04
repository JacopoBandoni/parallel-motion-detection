#include <ff/ff.hpp>
#include <ff/farm.hpp>

using namespace ff;

atomic<int> ffCounter2 = 0;

struct Stage12: ff_node_t<void*, Mat>{
    private:

    VideoCapture* videoCapture;

    public:

    Stage12(VideoCapture* videoCapture): videoCapture(videoCapture){};

    Mat* svc(void**){

        Mat frame;
        while(videoCapture->read(frame)){
            Mat *gray = new Mat(frame.rows, frame.cols, CV_8UC1);
            toGray(frame, gray);
            // send the frame
            ff_send_out(gray);
        }
        return EOS;
    }
};

struct Stage2: ff_node_t<Mat, Mat>{

    public:

    Stage2(){};

    Mat* svc(Mat* frame){
        Mat *gray = new Mat(frame->rows, frame->cols, CV_8UC1);
        toGray(*frame, gray);
        delete frame;
        return gray;    
    }
};

struct Stage34: ff_node_t<Mat, void>{

    private:
    Mat* background;
    Mat* smoothed;
    int kernelSize;
    float percentageTreshold;

    public:

    Stage34(Mat* background, int kernelSize, float percentageTreshold): background(background),  kernelSize(kernelSize), percentageTreshold(percentageTreshold) {
        smoothed = new Mat(background->rows, background->cols, CV_8UC1);
    };

    void* svc(Mat* frame){
        toSmothed(*frame, smoothed, kernelSize);
        if (areDifferent(*background, *smoothed, percentageTreshold))
            ffCounter2 ++;

        delete frame;
        return GO_ON;
    }

    void svc_end(){
        delete smoothed;
    }
};

int ffPar2(String videoName, int nWorkers, int kernelSize, float percentageTreshold){

    VideoCapture* videoCapture = new VideoCapture(videoName);

    long timer1 = 0, timer2 = 0;

    Mat *gray;
    Mat *smoothed;
    Mat *background;

    {
        utimer u("", &timer1);
        
        // READING & PROCESSING FIRST FRAME
        Mat frame;

        // Initialize a boolean to check if frames are there or not
        bool isSuccess = videoCapture->read(frame);
        // If frames are not there, close it
        if (isSuccess == false)
        {
            cout << "Video camera is disconnected" << endl;
            return 1;
        }

        gray = new Mat(frame.rows, frame.cols, CV_8UC1);
        smoothed = new Mat(frame.rows, frame.cols, CV_8UC1);
        background = new Mat(frame.rows, frame.cols, CV_8UC1);
        toGray(frame, gray);
        toSmothed(*gray, background, kernelSize);
    }

    ff_farm farm;
    Stage12* stage12 = new Stage12(videoCapture);
    vector<ff_node*> workers(nWorkers);
    for(int i = 0; i < nWorkers; i++)
        workers[i] = new Stage34(background, kernelSize, percentageTreshold);
    farm.add_emitter(stage12);
    farm.add_workers(move(workers));
    //farm.set_scheduling_ondemand();
    {   
        utimer u("", &timer2);
        farm.run_and_wait_end();
    }

    // Release the video capture object
    videoCapture->release();
    
    cout << (timer1 + timer2) << " usecs" << endl;
    cout << "Number of frame with motion: " << ffCounter2 << endl;

    // DELETING RESOURCES
    delete gray;
    delete smoothed;
    delete background;
    delete videoCapture;
    delete stage12;
    for(int i = 0; i < nWorkers; i++)
          delete workers[i];

    return 0;
}