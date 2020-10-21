g++ main.cpp -o main

for file in datasets/*.stu
do
    name=${file%.*}
    echo 'for file:' $file
    time ./main $name.sol < $file
    echo
    echo
done

rm main