CC=gcc
OPTS=-Werror -O0 
#-g for default debug information
#-Werror for warning as error

all: build reading_time_overhead loop_overhead creation_kthreads creation_processes context_switch_kthreads context_switch_processes

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

## context_switch_time
context_switch_kthreads: build
	$(CC) $(OPTS) -o build/context_switch_kthreads operations/1_cpu_scheduling_and_os_services/context_switch_time/context_switch_kthreads.c

context_switch_processes: build
	$(CC) $(OPTS) -o build/context_switch_processes operations/1_cpu_scheduling_and_os_services/context_switch_time/context_switch_processes.c

################### 2_memory ###################

################### 3_network ###################

################### 4_file_system ###################

clean:
	rm -f build/*;
