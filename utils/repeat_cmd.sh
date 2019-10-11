#!/bin/sh

# Sample Usage - 
# ./repeat_cmd.sh rdtsc 20
# The executable named 'rdtsc' will be run 20 times with the expectation that 
# it will print precisely 'cycles_taken:<some uint64>'

EXECUTABLE=$1
REPEAT_COUNT=$2

PWD=`pwd`
echo "Currently in $PWD"

if [ -z "$EXECUTABLE" ]
then
    echo "Error! Need an EXECUTABLE: depthfirst_parallel | depthfirst_serial"
    exit 1
else
    if [ ! -f "$EXECUTABLE" ]
    then 
        echo "Error! EXECUTABLE does not found!"
        exit 1
    fi
fi

if [ -z "$REPEAT_COUNT" ]
then
    echo "INFO! REPEAT_COUNT not provided. Using 10 by default"
    REPEAT_COUNT=10
fi

declare -a AVERAGE

echo "Running ${EXECUTABLE} ${REPEAT_COUNT} times to get the average."

echo "\nS.No.\t       \tAv time (in cycles)"
echo "==================================================================="
SUM=0
LOOP_COUNT=0
for i in {1..100000}
do
    LOOP_COUNT=`expr $LOOP_COUNT + 1`
    TIME_TAKEN=`./$EXECUTABLE | grep -i cycles_taken | cut -d : -f2`
    printf "%d\t    \t%d\n" "$i" "${TIME_TAKEN}"
    SUM=$(echo "$TIME_TAKEN + $SUM" | bc)

    if [ $LOOP_COUNT -eq $REPEAT_COUNT ]
    then
        break
    fi
done

AVERAGE=$(echo "scale = 6; $SUM / $REPEAT_COUNT" | bc)
printf "Average:%.2f\n" "${AVERAGE}"
echo "==================================================================="

