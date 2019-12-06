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
FILE_TO_USE=$2

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

if [ -z "$FILE_TO_USE" ]
then
    SKIP_CREATE=0
else
    SKIP_CREATE=1
    echo "Using file:$FILE_TO_USE instead of creating one"
fi

#echo "\nS.No.\t       \tAv time (in cycles)"
echo "==================================================================="

# each reads 2GB file
for i in {1..9}
do
    FILE_SIZE=`expr 16 \* 1024 \* 1024`
    COARSE_FILE_NAME_WITH_PATH="/tmp/temp_16MB_file"

    # create i number of files
    j=1
    while [ $j -le $i ]
    do
        #FILE_NAME="temp_4MB_file${j}"
        FILE_NAME_WITH_PATH="${COARSE_FILE_NAME_WITH_PATH}${j}"
        # echo "creating ${FILE_NAME_WITH_PATH}"
        # printf "%d\t%s\n" "${FILE_SIZE}" "${FILE_NAME_WITH_PATH}"

        if [ $SKIP_CREATE -eq 0 ]
        then
            # make a fake temp file in MBs
            TEMP=`mkfile -n 16m $FILE_NAME_WITH_PATH`
            if [ "$TEMP" != "" ]
            then
                echo "Error! Could not create a temp file."
                exit 1
            fi
        else
            FILE_NAME_WITH_PATH=$FILE_TO_USE
        fi
        j=$(( j+1 ))
    done

    # clean mac file cache
    sudo purge

    # COMMAND="./${EXECUTABLE} ${COARSE_FILE_NAME_WITH_PATH} ${FILE_SIZE} ${i}"
    # echo "running ${COMMAND}"
    # OUTPUT=`$COMMAND`
    OUTPUT=`./$EXECUTABLE $COARSE_FILE_NAME_WITH_PATH $FILE_SIZE $i | grep -i "clock cycles" | awk -F ':' '{print $4}' | awk -F ' clock cycles' '{sum += $1; average = sum / NR;} END {print average}'`
    if [ "$OUTPUT" = "" ]
    then
        echo "Error! Something went wrong for ${i} childs"
    else
        printf "%s readers, average time / 4KB block = %s clock cycles\n" "${i}" "${OUTPUT}"
    fi

    # delete i number of files
    j=1
    while [ $j -le $i ]
    do
        FILE_NAME_WITH_PATH="${COARSE_FILE_NAME_WITH_PATH}${j}"
        # echo "removing ${FILE_NAME_WITH_PATH}"
        rm -f $FILE_NAME_WITH_PATH
        j=$(( j+1 ))
    done
    
    echo "------------------------------------------------------------"
        
done
