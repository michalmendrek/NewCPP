all:
	g++ --std=c++11 -o main ./main.cpp ./loger.cpp ./unix_notify.cpp -I ./

clean:
	rm -rf ./main
