#!/bin/bash

black=04431
white=04430

row=8
col=8

echo 5 > shared.txt

make
./gui $row $col a a y y "Lines of Action" $black $white 240 | ./main $row $col 1 4 y 0 4 4 3 0 | ./main $row $col 0 4 y 0 4 4 3 1
# for((i=0;i<1;i++))
# do
    # echo 5 > shared.txt
   # ./gui $row $col a a y y "Lines of Action" $black $white 240 | ./main_$black $row $col 0 4 y | ./main_$white $row $col 1 4 y
    # ./main_$black $row $col 0 4 n | ./main_$white $row $col 1 4 n
# done
# ./gui $row $col h a Y y "Lines of Action" Redwan AI 240 | ./main $row $col 1 4 y 0 2 2 1 1
# ./gui $row $col a h y Y "Lines of Action" AI Redwan 240 | ./main $row $col 0 4 y 0 2 2 1 1
# ./gui $row $col h h Y Y "Lines of Action" Bolod-1 Bolod-2 240
# ./main $row $col 0 4 | ./main $row $col 1 4 y
# ./gui $row $col a h y Y "Lines of Action" AI haha 180 | ./main $row $col 0 4 y
# mkfifo pipe
# ./main_$white $row $col 0 4 < pipe | ./main_$black $row $col 1 4 > pipe
# rm pipe
# ./merger
# ./gui $row $col a a n n "Lines of Action" $white $black 180 < log.txt