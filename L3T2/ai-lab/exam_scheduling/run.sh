g++ main.cpp -o main

g++ eval.cc -o eval

# for file in toronto-usage/*.stu
for file in toronto-usage/car-s-91.stu
do
    name=${file%.*}
    echo 'for file:' $file
    time ./main < $file > order_check.txt
    ./eval $name.crs $file out.sol
    echo
    echo
done

rm main