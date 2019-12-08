#!/bin/sh

# for KB
for i in {128,256,512}
do
	FILE_SIZE=`expr $i \* 1024`

    FILE_NAME="temp_${i}_KB_file"
    FILE_NAME_WITH_PATH="./${FILE_NAME}"

	TEMP=`truncate -s "${i}K" $FILE_NAME_WITH_PATH`
    if [ "$TEMP" != "" ]
    then
        echo "Error! Could not create a temp file of size ${i}KB"
        exit 1
    fi
done

# for MB
for i in {1,2,4,8,16,32,64,128,256,512}
do
	FILE_SIZE=`expr $i \* 1024 \* 1024`

    FILE_NAME="temp_${i}_MB_file"
    FILE_NAME_WITH_PATH="./${FILE_NAME}"

	TEMP=`truncate -s "${i}M" $FILE_NAME_WITH_PATH`
    if [ "$TEMP" != "" ]
    then
        echo "Error! Could not create a temp file of size ${i}MB."
        exit 1
    fi
done

# for GB
for i in {1..7}
do
	FILE_SIZE=`expr $i \* 1024 \* 1024 \* 1024`

    FILE_NAME="temp_${i}_GB_file"
    FILE_NAME_WITH_PATH="./${FILE_NAME}"

	TEMP=`truncate -s "${i}G" $FILE_NAME_WITH_PATH`
    if [ "$TEMP" != "" ]
    then
        echo "Error! Could not create a temp file of size ${i}GB"
        exit 1
    fi
done
