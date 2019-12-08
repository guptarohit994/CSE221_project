#!/bin/sh

# Sample Usage - 
# ./file_read_time_sizes.sh file_read_time
# The executable named 'file_read_time' will be run with different size fake files
# created to be checked for access times
# Expects 'clock cycles' to be given as output by the binary!
#
# or 
# ./file_read_time_sizes.sh file_read_time /tmp/tmp_file
# The executable named 'file_read_time' will be run with the file provided instead
# of creating one and various access times will be checked


EXECUTABLE=$1
# looks for files in this path
PATH_TO_SEARCH=$2

# check if run as root
ID=`id -u`
if [[ $ID -ne 0 ]]; then
   echo "It is recommended to run this script as root (need nice and purge).." 
   exit 1
fi

if [ -z "$EXECUTABLE" ]
then
    echo "Error! Need an EXECUTABLE: file_read_time | file_read_time_seq ??"
    exit 1
else
    if [ ! -f "$EXECUTABLE" ]
    then 
        echo "Error! ${EXECUTABLE} not found!"
        exit 1
    fi
fi

if [ -z "$PATH_TO_SEARCH" ]
then
    SKIP_CREATE=0
else
    SKIP_CREATE=1
    echo "Using files (like temp_4_KB_file) in path:${PATH_TO_SEARCH} instead of creating one"
fi

#echo "\nS.No.\t       \tAv time (in cycles)"
echo "==================================================================="

# for KB
for i in {128,256,512}
do
    FILE_SIZE=`expr $i \* 1024`

    FILE_NAME="temp_${i}_KB_file"
    FILE_NAME_WITH_PATH="/tmp/${FILE_NAME}"
    # printf "%d\t%s\n" "${FILE_SIZE}" "${FILE_NAME_WITH_PATH}"

    if [ $SKIP_CREATE -eq 0 ]
    then
        # make a fake temp file in MBs
        TEMP=`mkfile -n "${i}k" $FILE_NAME_WITH_PATH`
        if [ "$TEMP" != "" ]
        then
            echo "Error! Could not create a temp file."
            exit 1
        fi
    else
        FILE_NAME_WITH_PATH="${PATH_TO_SEARCH}/${FILE_NAME}"
    fi
    # clean mac file cache
    sudo purge

    OUTPUT=`./$EXECUTABLE $FILE_NAME_WITH_PATH $FILE_SIZE | grep -i "clock cycles"`
    if [ "$OUTPUT" = "" ]
    then
        echo "Error! Something went wrong for FILE_SIZE:${FILE_SIZE}B"
    else
        printf "%s\n" "${OUTPUT}"
    fi

    if [ $SKIP_CREATE -eq 0 ]
    then
        rm -f $FILE_NAME_WITH_PATH
    fi
done


# for MB
for i in {1,2,4,8,16,32,64,128,256,512}
do
    FILE_SIZE=`expr $i \* 1024 \* 1024`

    FILE_NAME="temp_${i}_MB_file"
    FILE_NAME_WITH_PATH="/tmp/${FILE_NAME}"
    # printf "%d\t%s\n" "${FILE_SIZE}" "${FILE_NAME_WITH_PATH}"

    if [ $SKIP_CREATE -eq 0 ]
    then
        # make a fake temp file in MBs
        TEMP=`mkfile -n "${i}m" $FILE_NAME_WITH_PATH`
        if [ "$TEMP" != "" ]
        then
            echo "Error! Could not create a temp file."
            exit 1
        fi
    else
        FILE_NAME_WITH_PATH="${PATH_TO_SEARCH}/${FILE_NAME}"
    fi
    # clean mac file cache
    sudo purge

    OUTPUT=`./$EXECUTABLE $FILE_NAME_WITH_PATH $FILE_SIZE | grep -i "clock cycles"`
    if [ "$OUTPUT" = "" ]
    then
        echo "Error! Something went wrong for FILE_SIZE:${FILE_SIZE}B"
    else
        printf "%s\n" "${OUTPUT}"
    fi

    if [ $SKIP_CREATE -eq 0 ]
    then
        rm -f $FILE_NAME_WITH_PATH
    fi
done

# for GB
for i in {1..7}
do
    FILE_SIZE=`expr $i \* 1024 \* 1024 \* 1024`

    FILE_NAME="temp_${i}_GB_file"
    FILE_NAME_WITH_PATH="/tmp/${FILE_NAME}"
    #printf "%d\t%s\n" "${FILE_SIZE}" "${FILE_NAME}"

    if [ $SKIP_CREATE -eq 0 ]
    then
        # make a fake temp file in MBs
        TEMP=`mkfile -n "${i}g" $FILE_NAME_WITH_PATH`
        if [ "$TEMP" != "" ]
        then
            echo "Error! Could not create a temp file."
            exit 1
        fi
    else
        FILE_NAME_WITH_PATH="${PATH_TO_SEARCH}/${FILE_NAME}"
    fi
    # clean mac file cache
    sudo purge

    OUTPUT=`./$EXECUTABLE $FILE_NAME_WITH_PATH $FILE_SIZE | grep -i "clock cycles"`
    if [ "$OUTPUT" = "" ]
    then
        echo "Error! Something went wrong for FILE_SIZE:${FILE_SIZE}B"
    else
        printf "%s\n" "${OUTPUT}"
    fi

    if [ $SKIP_CREATE -eq 0 ]
    then
        rm -f $FILE_NAME_WITH_PATH
    fi
done
