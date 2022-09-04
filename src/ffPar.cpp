#include <ff/ff.hpp>
#include <ff/farm.hpp>

using namespace ff;

atomic<int> ffCounter = 0;

struct Stage1 : public ff_node_t<void*, Mat>{

    private:

    VideoCapture* videoCapture;

    public:

    Stage1(VideoCapture* videoCapture): videoCapture(videoCapture){};

    Mat* svc(void**){
        Mat frame;
        while(videoCapture->read(frame)){
            Mat *copyFrame = new Mat(frame.rows, frame.cols, CV_8UC3);
            frame.copyTo(*copyFrame);
            // send the frame
            ff_send_out(copyFrame);
        }
        return EOS;
    }
};

struct Stage234: public ff_node_t<Mat, void>{

    private:
    Mat* background;
    int kernelSize;
    float percentageTreshold;
    Mat *gray;
    Mat *smoothed;

    public:

    Stage234(Mat* background, int kernelSize, float percentageTreshold): background(background), kernelSize(kernelSize), percentageTreshold(percentageTreshold){
        gray = new Mat(background->rows, background->cols, CV_8UC1);
        smoothed = new Mat(background->rows, background->cols, CV_8UC1);
    };

    void* svc(Mat* frame){
        toGray(*frame, gray);
        toSmothed(*gray, smoothed, kernelSize);
        if(areDifferent(*background, *smoothed, percentageTreshold))
            ffCounter++;

        delete frame;
        return GO_ON;
    }

    void svc_end(){
        delete gray;
        delete smoothed;
    }
};

int ffPar(String videoName, int nWorkers, int kernelSize, float percentageTreshold){

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

    vector<ff_node*> workers(nWorkers);
    Stage1* emitter = new Stage1(videoCapture);
    
    for(int i = 0; i < nWorkers; i++)
        workers[i] = new Stage234(background, kernelSize, percentageTreshold);

    ff_farm farm;
    farm.add_emitter(emitter);
    farm.add_workers(move(workers));
    //farm.set_scheduling_ondemand();
    
    {   
        utimer u("", &timer2);
        farm.run_and_wait_end();
    }

    // Release the video capture object
    videoCapture->release();
    
    cout << (timer1 + timer2) << " usecs" << endl;
    cout << "Number of frame with motion: " << ffCounter << endl;

    // DELETING RESOURCES
    delete gray;
    delete smoothed;
    delete background;
    delete videoCapture;
    delete emitter;
    for(int i = 0; i < nWorkers; i++)
        delete workers[i];

    return 0;
}