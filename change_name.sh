#!/bin/bash
if [ ! $1 ]; then
	echo need dist directory parameter
	exit 1
fi
 
original='Soft'
newvalue='Soft'
 
function changename() {
	name=$1
	ss=`echo $name |grep $original`
	if [ $ss ]; then
		echo changename: $name
		echo $name > name.txt
		sed -i "s/$original/$newvalue/g" name.txt
		newname=`cat name.txt`
		mv $name $newname
		fullpath=$newname
	fi
}
function getfile(){
    for e in `ls $1`
    do
        fullpath=$1/$e
 
        changename $fullpath
 
        echo $fullpath
 
        if [ -f $fullpath ]; then
            sed -i "s/$original/$newvalue/g" $fullpath
        elif [ -d $fullpath ]; then
            getfile $fullpath
        fi  
    done
}
 
read -p "Are you want to change filecontent?[Y]:" flag
if [[ ! $flag || "$flag" == "Y" || "$flag" == "y" ]]; then
    cd $1
    getfile .
    rm -rf name.txt
fi

