# Video motion detection

Motion detection implemented by subtracting video frames from some background
images, such that:

1. The first frame of the video is taken as “background picture”
2. Each frame is turned to greyscale, smoothed, and subtracted to the greyscale smoothed background picture
3. Motion detect flag will be true in case more the k% of the pixels differ
4. The output of the program is represented by the number of frames with motion detected

Several parallelism approaches are taken into consideration to reach better results.

## Build instructions

    mkdir build; cd build
    cmake .. -DCMAKE_CXX_COMPILER=g++-10
    make

## Run
    
    ./main 1 2 3 30 earth.mp4

    1° argument: program versions (different parallelism approaches / used libraries)
    2° argument: number of workers
    3° argument: kernel size
    4° argument: percentage treshold
    5° argument: video name (of the one inside video directory)

## Note

For more details regarding different implementations choices you can read the report.pdf
