#!/bin/sh

# Sample Usage - 
# ./file_cache_size.sh file_cache_size
# The executable named 'file_cache_size' will be run a temp 8GB file
#
# or 
# ./file_cache_size.sh file_cache_size /tmp/temp_file
# The executable named 'file_cache_size' will be run with the file provided instead
# of creating one

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

echo "==================================================================="

FILE_SIZE_GB=8
FILE_SIZE=`expr $FILE_SIZE_GB \* 1024 \* 1024 \* 1024`

FILE_NAME="temp_${FILE_SIZE_GB}_GB_file"
FILE_NAME_WITH_PATH="/tmp/${FILE_NAME}"

# create a temp file
if [ $SKIP_CREATE -eq 0 ]
then
    # make a fake temp file
    echo "Making ${FILE_NAME_WITH_PATH}..."
    TEMP=`mkfile -n "${FILE_SIZE_GB}g" $FILE_NAME_WITH_PATH`
    echo "Done."
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

./$EXECUTABLE $FILE_NAME_WITH_PATH

if [ $SKIP_CREATE -eq 0 ]
then
    echo "Removing ${FILE_NAME_WITH_PATH}..."
    rm -f $FILE_NAME_WITH_PATH
fi
