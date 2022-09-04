int sequential(string videoName, int kernelSize, float percentageTreshold)
{

    VideoCapture videoCapture(videoName);

    if (!videoCapture.isOpened())
    {
        cout << "Error opening video stream or file" << endl;
        return 1;
    }

    // take frame count
    int frame_count = videoCapture.get(CAP_PROP_FRAME_COUNT);
    cout << "Frame count: " << frame_count << endl;

    // timer variables
    uint timer1 = 0, timer2 = 0, timer3 = 0, timer4 = 0;

    // START MAIN TIMER
    auto start = chrono::high_resolution_clock::now();

    // READING & PROCESSING FIRST FRAME
    Mat frame;
    // Initialize a boolean to check if frames are there or not
    bool isSuccess = videoCapture.read(frame);
    // If frames are not there, close it
    if (isSuccess == false)
    {
        cout << "Video camera is disconnected" << endl;
        return 1;
    }
    Mat *gray = new Mat(frame.rows, frame.cols, CV_8UC1);
    Mat *smoothed = new Mat(frame.rows, frame.cols, CV_8UC1);
    Mat *background = new Mat(frame.rows, frame.cols, CV_8UC1);
    toGray(frame, gray);
    toSmothed(*gray, background, kernelSize);

    // START PROCESSING NEXT FRAMES
    int i = 0;
    int num = 0;
    long elapsed;
    while (videoCapture.isOpened())
    {   

        // STAGE 1
        {   utimer u("", &elapsed);
            // Initialize a boolean to check if frames are there or not
            isSuccess = videoCapture.read(frame);
            // If frames are not there, close it
            if (isSuccess == false){
                break;
            }
        } 
        timer1 += elapsed;

        // STAGE 2
        {   utimer u("", &elapsed);
            toGray(frame, gray);
        }timer2 += elapsed;
        
        // STAGE 3
        {   utimer u("", &elapsed);
            toSmothed(*gray, smoothed, kernelSize);
        }timer3 += elapsed;
        
        // STAGE 4
        {   utimer u("", &elapsed);
            num += areDifferent(*background, *smoothed, percentageTreshold);
        }timer4 += elapsed;
    }

    // Calculate: elapsed time = end time - start time
    auto total_elapsed = chrono::high_resolution_clock::now() - start;

    // print timer for each stage
    cout << "timer1 = " << timer1/(frame_count-1) << endl;
    cout << "timer2 = " << timer2/(frame_count-1) << endl;
    cout << "timer3 = " << timer3/(frame_count-1) << endl;
    cout << "timer4 = " << timer4/(frame_count-1) << endl;

    // Release the video capture object
    videoCapture.release();

    cout << "Number of frame with motion: " << num << endl;
    cout << chrono::duration_cast<chrono::microseconds>(total_elapsed).count() << " usecs" << endl;

    // DELETING RESOURCES
    delete gray;
    delete smoothed;
    delete background;

    return 0;
}