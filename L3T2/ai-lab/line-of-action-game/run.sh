mkfifo pipe
g++ one.cpp -o one
python3 two.py < pipe | ./one | tee pipe
rm one
rm pipe