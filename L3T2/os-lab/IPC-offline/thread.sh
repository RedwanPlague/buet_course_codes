your_roll=1605004
g++ -Wall -Wextra -O2 -g -fsanitize=thread -D _REENTRANT $your_roll.cpp -o $your_roll -lpthread
time ./$your_roll > $your_roll.txt
rm $your_roll