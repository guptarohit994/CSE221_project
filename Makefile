CC=gcc
OPTS=-Werror -O0 
#-g for default debug information
#-Werror for warning as error

define standard_compile
	$(CC) $(OPTS) -o build/$2 operations/$1/$2/$2.c
endef
# standard_compile Usage:
# e.g. $(eval $(call standard_compile,dir,foo))
# is equivalent to
# foo: build
# 	$(CC) $(OPTS) -o build/foo operations/dir/foo/foo.c

all: build \
	 reading_time_overhead \
	 loop_overhead \
	 creation_kthreads \
	 creation_processes \
	 context_switch_kthreads \
	 context_switch_processes \
	 procedure_call \
	 system_call \
	 cpuid_memory_info \
	 ram_access_time \
	 ram_access_time_seq \
	 ram_bandwidth \
	 page_fault_time \
	 round_trip_time \
	 peak_bandwidth \
	 connection_overhead_setup \
	 connection_overhead_teardown \
	 file_read_time \
	 file_read_time_seq

build: 
	mkdir -p build

################### 1_cpu_scheduling_and_os_services ###################
## measurement_overhead
reading_time_overhead: build
	$(CC) $(OPTS) -o build/reading_time_overhead operations/1_cpu_scheduling_and_os_services/measurement_overhead/reading_time_overhead.c

loop_overhead: build
	$(CC) $(OPTS) -o build/loop_overhead operations/1_cpu_scheduling_and_os_services/measurement_overhead/loop_overhead.c

## task_creation
creation_kthreads: build
	$(CC) $(OPTS) -o build/creation_kthreads operations/1_cpu_scheduling_and_os_services/task_creation_time/creation_kthreads.c

creation_processes: build
	$(CC) $(OPTS) -o build/creation_processes operations/1_cpu_scheduling_and_os_services/task_creation_time/creation_processes.c

## procedure_call

procedure_call: build
	$(CC) $(OPTS) -o build/procedure_call operations/1_cpu_scheduling_and_os_services/procedure_call/procedure_call.c

## system_call

system_call: build
	$(CC) $(OPTS) -o build/system_call operations/1_cpu_scheduling_and_os_services/system_call/system_call.c

## context_switch_time
context_switch_kthreads: build
	$(CC) $(OPTS) -o build/context_switch_kthreads operations/1_cpu_scheduling_and_os_services/context_switch_time/context_switch_kthreads.c

context_switch_processes: build
	$(CC) $(OPTS) -o build/context_switch_processes operations/1_cpu_scheduling_and_os_services/context_switch_time/context_switch_processes.c

################### 2_memory ###################

## cpuid_memory_info
cpuid_memory_info: build
	$(CC) $(OPTS) -o build/cpuid_memory_info operations/2_memory/cpuid_memory_info/cpuid_memory_info.c

## ram_access_time
ram_access_time: build
	$(CC) $(OPTS) -o build/ram_access_time operations/2_memory/ram_access_time/ram_access_time.c

ram_access_time_seq: build
	$(CC) $(OPTS) -D SEQUENTIAL_ACCESS -o build/ram_access_time_seq operations/2_memory/ram_access_time/ram_access_time.c

ram_bandwidth: build
	python3 operations/2_memory/ram_bandwidth/prepare.py -s 8 -l 1024
	$(CC) -O3 -o build/ram_bandwidth operations/2_memory/ram_bandwidth/ram_bandwidth.c

build/temp_1GB_file: build
	$(info ************ A fake large file temp_1GB_file will be created ************)
	mkfile -n 1g build/temp_1GB_file

page_fault_time: build build/temp_1GB_file
	$(call standard_compile,2_memory,page_fault_time)

################### 3_network ###################

## round_trip_time
round_trip_time: build
	$(info ************  Run following command to create a dummy remote echo server ************)
	$(info ************  ncat -l 2000 --keep-open --exec "/bin/cat" ************)
	# 56B since ping also sends 56 data bytes
	$(CC) $(OPTS) -D SERVERADDR=\"127.0.0.1\" -D SERVERPORT=2000 -D DATABYTES=56 -o build/round_trip_time operations/3_network/round_trip_time/round_trip_time.c

## peak_bandwidth
peak_bandwidth: build
	$(info ************  Run two instance of the program as the client and server respectively ************)
	$(info ************  build/peak_bandwidth -s & build/peak_bandwidth  ************)
	$(CC) $(OPTS) -D SERVERADDR=\"127.0.0.1\" -D SERVERPORT=2000 -o build/peak_bandwidth operations/3_network/peak_bandwidth/peak_bandwidth.c

## connection_overhead_setup
connection_overhead_setup: build
	$(info ************  Run following command to create a dummy remote server ************)
	$(info ************  ncat -l 2000 --keep-open ************)
	$(CC) $(OPTS) -D SERVERADDR=\"127.0.0.1\" -D SERVERPORT=2000 -o build/connection_overhead_setup operations/3_network/connection_overhead/connection_overhead_setup.c

## connection_overhead_teardown
connection_overhead_teardown: build
	$(info ************  Run following command to create a dummy remote server ************)
	$(info ************  ncat -l 2000 --keep-open ************)
	$(CC) $(OPTS) -D SERVERADDR=\"127.0.0.1\" -D SERVERPORT=2000 -o build/connection_overhead_teardown operations/3_network/connection_overhead/connection_overhead_teardown.c

################### 4_file_system ###################

## file_read_time / remote_file_read_time
file_read_time: build
	cp operations/4_file_system/file_read_time/file_read_time_sizes.sh build/file_read_time_sizes.sh
	$(CC) $(OPTS) -o build/file_read_time operations/4_file_system/file_read_time/file_read_time.c

## file_read_time_seq / remote_file_read_time_seq
file_read_time_seq: build
	cp operations/4_file_system/file_read_time/file_read_time_sizes.sh build/file_read_time_sizes.sh
	$(CC) $(OPTS) -D SEQUENTIAL_ACCESS -o build/file_read_time_seq operations/4_file_system/file_read_time/file_read_time.c

clean:
	rm -f build/*;
