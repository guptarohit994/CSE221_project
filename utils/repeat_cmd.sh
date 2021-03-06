#!/bin/sh

# Sample Usage - 
# ./repeat_cmd.sh rdtsc 20
# The executable named 'rdtsc' will be run 20 times with the expectation that 
# it will print precisely 'cycles_taken:<some uint64>'

EXECUTABLE=$1
REPEAT_COUNT=$2

PWD=`pwd`
echo "Running command - ${0} ${1} ${2}"
echo "Currently in $PWD"

if [ -z "$EXECUTABLE" ]
then
    echo "Error! Need an EXECUTABLE: depthfirst_parallel | depthfirst_serial"
    exit 1
else
    if [ ! -f "$EXECUTABLE" ]
    then 
        echo "Error! ${EXECUTABLE} not found!"
        exit 1
    fi
fi

if [ -z "$REPEAT_COUNT" ]
then
    echo "INFO! REPEAT_COUNT not provided. Using 10 by default"
    REPEAT_COUNT=10
fi

declare -a TIMES

echo "Running ${EXECUTABLE} ${REPEAT_COUNT} times to get the median & average."

echo "\nS.No.\t       \tAv time (in cycles)"
echo "==================================================================="
SUM=0
LOOP_COUNT=0
for i in {1..100000}
do
    LOOP_COUNT=`expr $LOOP_COUNT + 1`
    TIME_TAKEN=`./$EXECUTABLE | grep -i "cycles_taken =" | rev | cut -d = -f1 | rev | cut -d ' ' -f2`
    printf "%d\t    \t%d\n" "$i" "${TIME_TAKEN}"
    SUM=$(echo "$TIME_TAKEN + $SUM" | bc)
    TIMES[$i]=$TIME_TAKEN

    if [ $LOOP_COUNT -eq $REPEAT_COUNT ]
    then
        break
    fi
done

median() {
  #printf '%s ' "${@}"
  arr=($(printf '%d\n' "${@}" | sort -n))
  nel=${#arr[@]}
  if (( $nel % 2 == 1 )); then     # Odd number of elements
    val="${arr[ $(($nel/2)) ]}"
  else                            # Even number of elements
    (( j=nel/2 ))
    (( k=j-1 ))
    (( val=(${arr[j]} + ${arr[k]})/2 ))
  fi
  printf "Median:%d\n" "${val}"
}

stddev() {
  arr=$(printf '%d\n' "${@}")
  stddev=$(
    echo "$arr" |
    awk '{sum+=$1; sumsq+=$1*$1}END{print sqrt(sumsq/NR - (sum/NR)**2)}'
  )
  printf "Stddev:$stddev\n"
}

median ${TIMES[@]}
AVERAGE=$(echo "scale = 6; $SUM / $REPEAT_COUNT" | bc)
printf "Average:%.2f\n" "${AVERAGE}"
stddev ${TIMES[@]}
echo "==================================================================="

