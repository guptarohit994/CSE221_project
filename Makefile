CC=gcc
OPTS=-Werror
#-g for default debug information
#-Werror for warning as error

all: build rdtsc

build: 
	mkdir -p build

################### 1_cpu_scheduling_and_os_services ###################
## measurement_overhead
rdtsc: build
	$(CC) $(OPTS) -o build/rdtsc operations/1_cpu_scheduling_and_os_services/measurement_overhead/rdtsc.c

################### 2_memory ###################

################### 3_network ###################

################### 4_file_system ###################

clean:
	rm -f build/*;