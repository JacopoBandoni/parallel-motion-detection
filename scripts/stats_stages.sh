#!/bin/bash

cd ../build

# Computation info
kernel_sizes=(3 7)
percentage_treshold=95
video_name=owl.mp4
frame_number=251

iterations=10

output_file="../analyses/data/stages_time_results_${video_name}.csv"
# if not present create output file
if [ ! -f $output_file ]; then
    echo "video_name,frames_number,kernel_size,percentage_treshold,stage1,stage2,stage3,stage4,completion_time" >> $output_file
fi

for iteration in $(seq 1 $iterations); do
    echo "Iteration: $iteration/$iterations "

    for kernel_size in "${kernel_sizes[@]}"; do
        
        # launching the command
        command_output=$(./main 0 - $kernel_size $percentage_treshold $video_name)

        # taking the info
        frames_numbers=$(echo "$command_output" | grep Frame | cut -d' ' -f3)
        stage1=$(echo "$command_output" | grep timer1 | cut -d' ' -f3)
        stage2=$(echo "$command_output" | grep timer2 | cut -d' ' -f3)
        stage3=$(echo "$command_output" | grep timer3 | cut -d' ' -f3)
        stage4=$(echo "$command_output" | grep timer4 | cut -d' ' -f3)
        completion_time=$(echo "$command_output" | grep usecs | cut -d' ' -f1)

        # writing to file
        echo "$video_name,$frames_numbers,$kernel_size,$percentage_treshold,$stage1,$stage2,$stage3,$stage4,$completion_time" >> $output_file
    
    done
done