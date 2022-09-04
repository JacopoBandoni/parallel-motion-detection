#include <SharedQueue.cpp>
#include<thread>

atomic<ulong> counter(0) ;

/**
* @brief Perform stage1: read frame from a video source and push them to the shared queue.
* @param videoCapture The video source
* @param queue The queue
*/
void stage1(VideoCapture* videoCapture, SharedQueue* queue){
    
    Mat frame;
    Mat* copyFrame;
    int width  = videoCapture->get(CAP_PROP_FRAME_WIDTH);
    int height = videoCapture->get(CAP_PROP_FRAME_HEIGHT);
    int nbytes = sizeof(unsigned char)*width*height*3;

    while(videoCapture->read(frame)){
        copyFrame = new Mat(height,width,CV_8UC3,Scalar(0,0,0));
        memcpy(copyFrame->data, frame.data, nbytes); 
        queue->push(copyFrame);
    }
    queue->finish();
}

/**
* @brief Perform from stage 2 to 4 reading from a SharedQueue. The stage on the frame are respectively:
*        2° stage - to gray
*        3° stage - to smoothed
*        4° stage - comparison with background
* @param queue The queue
* @param background The frame used as a comparison for motion detection
*/
void stage234(SharedQueue* queue, Mat* background, int kernelSize, float percentageTreshold){
    
    Mat *frame;
    Mat *gray = new Mat(background->rows, background->cols, CV_8UC1);
    Mat *smoothed = new Mat(background->rows, background->cols, CV_8UC1);
    int i = 0;
    
    while(true){
        frame = queue->pop();
        if(frame == nullptr)
            return;
        toGray(*frame, gray);
        delete frame;
        toSmothed(*gray, smoothed, kernelSize);
        if (areDifferent(*background, *smoothed, percentageTreshold))
        { 
            counter ++;
        }
        i++;
    }

    delete gray;
    delete smoothed;
}

int threadPar(string videoName, int nWorkers, int kernelSize, float percentageTreshold){

    SharedQueue* queue = new SharedQueue();
    VideoCapture* videoCapture = new VideoCapture(videoName);

    // START MAIN TIMER
    auto start = chrono::high_resolution_clock::now();

    // READING & PROCESSING FIRST FRAME
    Mat frame;
    // Initialize a boolean to check if frames are there or not
    bool isSuccess = videoCapture->read(frame);
    // If frames are not there, close it
    if (isSuccess == false){
        cout << "Video camera is disconnected" << endl;
        return 1;
    }
    Mat *gray = new Mat(frame.rows, frame.cols, CV_8UC1);
    Mat *smoothed = new Mat(frame.rows, frame.cols, CV_8UC1);
    Mat *background = new Mat(frame.rows, frame.cols, CV_8UC1);
    toGray(frame, gray);
    toSmothed(*gray, background, kernelSize);

    // PARALLEL VERSION
    // Stage1 worker
    thread stage1Worker(stage1, videoCapture, queue);
    // Stage234 workers
    vector<thread*> threads(nWorkers);
    for(int i = 0; i < nWorkers; i++){
        threads[i] = new thread(stage234, queue, background, kernelSize, percentageTreshold);
    }
    // join the threads
    stage1Worker.join();
    for(int i = 0; i < nWorkers; i++){
        threads[i]->join();
    }

    // Release the video capture object
    videoCapture->release();

    // Calculate: elapsed time = end time - start time
    auto total_elapsed = chrono::high_resolution_clock::now() - start;
    cout << chrono::duration_cast<chrono::microseconds>(total_elapsed).count() << " usecs" << endl;
    cout << "Number of frame with motion: " << counter << endl;

    // DELETING RESOURCES
    delete gray;
    delete smoothed;
    delete background;
    delete videoCapture;
    for(int i = 0; i < nWorkers; i++)
        delete threads[i];

    return 0;
}