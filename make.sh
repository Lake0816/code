#/bin/bash

if [[ $# > 2 ]];
then
	echo "Usage: ./make.sh all/clean"
	exit 1;
fi

if [[ $1 = "all" ]]
then
	mkdir build
	cd build

	if [ "$2" = "-d" ];
	then
        	echo "----------------------------cmake debug----------------------------"
        	cmake -DDEBUG=ON ../
	else      
        	echo "----------------------------cmake release----------------------------"
        	cmake -DDEBUG=NO ../
	fi

	make 
	cd ..
	rm build -rf
elif [[ $1 = "clean" ]];
then
	rm build -rf
	rm output/ -rf
	rm public/config.h -rf
else
	echo "Usage: ./make.sh all/clean"
fi
