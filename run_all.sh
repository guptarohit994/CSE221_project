DIRECTORY="./build/"
if [ ! -d "$DIRECTORY" ]; then
  echo "Error! try doing a make first"
  exit 1
fi


echo "Measurement overhead"
./utils/repeat_cmd.sh ./build/loop_overhead 100
./utils/repeat_cmd.sh ./build/measurement_overhead 100
echo "------------------------------------------------------------------"

echo "Procedure call overhead"
./build/procedure_call
echo "------------------------------------------------------------------"

echo "System call overhead"
./utils/repeat_cmd.sh ./build/system_call 100
echo "------------------------------------------------------------------"

echo "Task creation time"
./utils/repeat_cmd.sh build/creation_processes 20
./utils/repeat_cmd.sh build/creation_kthreads 20
echo "------------------------------------------------------------------"

echo "Context switch time"
./utils/repeat_cmd.sh ./build/context_switch_processes 100
./utils/repeat_cmd.sh ./build/context_switch_kthreads 100
echo "------------------------------------------------------------------"

echo "RAM access time"
./build/ram_access_time
echo "------------------------------------------------------------------"

echo "RAM bandwidth"
./build/ram_bandwidth
echo "------------------------------------------------------------------"

echo "Page fault service time"
./utils/repeat_cmd.sh ./build/page_fault_time 100
echo "------------------------------------------------------------------"

echo "Size of file cache"
./build/file_cache_size
echo "------------------------------------------------------------------"

echo "File read time"
./build/file_read_time_sizes.sh ./build/file_read_time
./build/file_read_time_sizes.sh ./build/file_read_time_seq
echo "------------------------------------------------------------------"

echo "Contention"
./build/file_contention.sh ./build/contention
echo "------------------------------------------------------------------"