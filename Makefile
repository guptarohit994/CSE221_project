CC=gcc
OPTS=-Werror -O0 
#-g for default debug information
#-Werror for warning as error

all: build \
	 reading_time_overhead \
	 loop_overhead \
	 creation_kthreads \
	 creation_processes \
	 context_switch_kthreads \
	 context_switch_processes \
	 procedure_call \
	 system_call \
	 ram_access_time \
	 ram_access_time_seq \
	 round_trip_time

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

## ram_access_time
ram_access_time: build
	$(CC) $(OPTS) -o build/ram_access_time operations/2_memory/ram_access_time/ram_access_time.c

ram_access_time_seq: build
	$(CC) $(OPTS) -D SEQUENTIAL_ACCESS -o build/ram_access_time_seq operations/2_memory/ram_access_time/ram_access_time.c

################### 3_network ###################

## round_trip_time
round_trip_time: build
	$(info ************  Run following command to create a dummy remote server ************)
	$(info ************  ncat -l 2000 --keep-open --exec "/bin/cat" ************)
	# 56B since ping also sends 56 data bytes
	$(CC) $(OPTS) -D SERVERADDR=\"127.0.0.1\" -D SERVERPORT=2000 -D DATABYTES=56 -o build/round_trip_time operations/3_network/round_trip_time/round_trip_time.c

################### 4_file_system ###################

clean:
	rm -f build/*;
