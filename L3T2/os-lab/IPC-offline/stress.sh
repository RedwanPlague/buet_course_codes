your_roll=1605004
cmd='g++
-D PARAMED
-D CYCLIST_COUNT=10
-D SERVICE_ROOM_COUNT=3
-D PAYMENT_ROOM_CAPACITY=2'

$cmd -D _REENTRANT $your_roll.cpp -o $your_roll -lpthread
$cmd checker.cpp -o checker

for((i=1;;i++))
do
    echo $i
    ./$your_roll > $your_roll.txt
    ./checker < $your_roll.txt || exit 1
done