CC=gcc
OPTS=-Werror -O0 
#-g for default debug information
#-Werror for warning as error

all: build rdtsc context_switch_kthreads

build: 
	mkdir -p build

################### 1_cpu_scheduling_and_os_services ###################
## measurement_overhead
rdtsc: build
	$(CC) $(OPTS) -o build/rdtsc operations/1_cpu_scheduling_and_os_services/measurement_overhead/rdtsc.c


## context_switch_time
context_switch_kthreads: build
	$(CC) $(OPTS) -o build/context_switch_kthreads operations/1_cpu_scheduling_and_os_services/context_switch_time/context_switch_kthreads.c

################### 2_memory ###################

################### 3_network ###################

################### 4_file_system ###################

clean:
	rm -f build/*;
