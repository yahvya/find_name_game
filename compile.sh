if [ ! -d "build" ]
then
	mkdir build
fi

cd build

if [ ! -f "Makefile" ]
then
	cmake ..
fi

make
clear
./app
cd ..
