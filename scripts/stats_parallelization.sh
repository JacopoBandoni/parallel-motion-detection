#!/bin/bash

cd ../build

kernel_sizes=(3 7)

percentage_treshold=95
video_name=owl.mp4
frames_number=251

iterations=10
max_workers=32
program_versions=3

for kernel_size in "${kernel_sizes[@]}"; do

    output_file="../analyses/data/programs_time_results_${video_name}_${kernel_size}_nodemand.csv"
    # if not present create output file
    if [ ! -f $output_file ]; then
        echo "iteration,n_workers,p_version,completion_time,frames_num,frames_motion_num" >> $output_file
    fi

    for iteration in $(seq 1 $iterations); do
        echo "Iteration: $iteration/$iterations "

        # run the sequential program
        echo "Sequential Program"
        command_output=$(./main 0 - $kernel_size $percentage_treshold $video_name)
        sequential_time=$(echo "$command_output" | grep usecs | cut -d' ' -f1)
        frames_motion_num=$(echo "$command_output" | grep frame | cut -d' ' -f6)
        echo "$iteration,1,0,$sequential_time,$frames_number,$frames_motion_num" >> $output_file

        for n_workers in $(seq 1 $max_workers); do
            echo "Number of workers: $n_workers"

            for p_version in $(seq 1 $program_versions); do

                # run the parallel program
                echo "Program version number: $p_version"
                
                command_output=$(./main $p_version $n_workers $kernel_size $percentage_treshold $video_name)

                completion_time=$(echo "$command_output" | grep usecs | cut -d' ' -f1)
                frames_motion_num=$(echo "$command_output" | grep frame | cut -d' ' -f6)
                # write results to the file
                echo "$iteration,$n_workers,$p_version,$completion_time,$frames_number,$frames_motion_num" >> $output_file
            done
        done
    done
done