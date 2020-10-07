your_roll=1605004
g++ -D _REENTRANT $your_roll.cpp -o $your_roll -lpthread
time ./$your_roll > $your_roll.txt
# ./$your_roll
rm $your_roll

# g++ checker.cpp -o checker
# ./checker < $your_roll.txt
# rm ./checker