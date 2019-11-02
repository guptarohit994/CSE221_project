#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/resource.h>

int cpuid_caches (size_t * data_caches) {
    int i;
    int num_data_caches = 0;
    for (i = 0; i < 32; i++) {

        // Variables to hold the contents of the 4 i386 legacy registers
        uint32_t eax, ebx, ecx, edx; 

        eax = 4; // get cache info
        ecx = i; // cache id

        asm (
            "cpuid" // call i386 cpuid instruction
            : "+a" (eax) // contains the cpuid command code, 4 for cache query
            , "=b" (ebx)
            , "+c" (ecx) // contains the cache id
            , "=d" (edx)
        ); // generates output in 4 registers eax, ebx, ecx and edx 

        // taken from https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-instruction-set-reference-manual-325383.pdf
        int cache_type = eax & 0x1F; 

        if (cache_type == 0) // end of valid cache identifiers
            break;

        char * cache_type_string;
        switch (cache_type) {
            case 1: cache_type_string = "Data Cache"; break;
            case 2: cache_type_string = "Instruction Cache"; break;
            case 3: cache_type_string = "Unified Cache"; break;
            default: cache_type_string = "Unknown Type Cache"; break;
        }

        int cache_level = (eax >>= 5) & 0x7;

        int cache_is_self_initializing = (eax >>= 3) & 0x1; // does not need SW initialization
        int cache_is_fully_associative = (eax >>= 1) & 0x1;
        //Maximum number of addressable IDs for logical processors sharing this cache
        int count_multithreading = ((eax >>= 5) & 0xFFF) + 1;
        //Maximum number of addressable IDs for processor cores in the physical package
        int count_core_phy_pkg = ((eax >>= 12) & 0x3F) + 1;


        // https://www.intel.com/content/www/us/en/architecture-and-technology/64-ia-32-architectures-software-developer-vol-2a-manual.html
        // ebx contains 3 integers of 10, 10 and 12 bits respectively
        unsigned int cache_sets = ecx + 1;
        unsigned int cache_coherency_line_size = (ebx & 0xFFF) + 1;
        unsigned int cache_physical_line_partitions = ((ebx >>= 12) & 0x3FF) + 1;
        unsigned int cache_ways_of_associativity = ((ebx >>= 10) & 0x3FF) + 1;

        //Write-Back Invalidate/Invalidate.
        //0 = WBINVD/INVD from threads sharing this cache acts upon lower level caches for threads sharing this cache.
        //1 = WBINVD/INVD is not guaranteed to act upon lower level caches of non-originating threads sharing this cache.
        unsigned int wbinvd_invd = (edx & 0x1);
        //Cache Inclusiveness.
        //0 = Cache is not inclusive of lower cache levels.
        //1 = Cache is inclusive of lower cache levels.
        unsigned int cache_inclusiveness = (edx >>= 1) & 0x1;
        //Complex Cache Indexing.
        //0 = Direct mapped cache
        //1 = A complex function is used to index the cache, potentially using all address bits
        unsigned int complex_cache_indexing = (edx >>= 1) & 0x1;

        // Total cache size is the product
        size_t cache_total_size = cache_ways_of_associativity * cache_physical_line_partitions * cache_coherency_line_size * cache_sets;

        if (cache_type == 1 || cache_type == 3) {
            data_caches[num_data_caches++] = cache_total_size;
        }

        printf(
            "Cache ID %d:\n"
            "- Level: %d\n"
            "- Type: %s\n"
            "- Sets: %d\n"
            "- System Coherency Line Size: %d bytes\n"
            "- Physical Line partitions: %d\n"
            "- Ways of associativity: %d\n"
            "- Total Size: %zu bytes (%zu kb)\n"
            "- Is fully associative: %s\n"
            "- Is Self Initializing: %s\n"
            "- Maximum number of addressable IDs for logical processors sharing this cache: %d\n"
            "- Maximum number of addressable IDs for processor cores in the physical package: %d\n"
            "- Write-Back Invalidate/Invalidate acts upon lower level caches: %s\n"
            "- cache inclusiveness: %s\n"
            "- Complex function for indexing used: %s\n"
            "\n"
            , i
            , cache_level
            , cache_type_string
            , cache_sets
            , cache_coherency_line_size
            , cache_physical_line_partitions
            , cache_ways_of_associativity
            , cache_total_size, cache_total_size >> 10
            , cache_is_fully_associative ? "true" : "false"
            , cache_is_self_initializing ? "true" : "false"
            , count_multithreading
            , count_core_phy_pkg
            , wbinvd_invd ? "false" : "true"
            , cache_inclusiveness ? "Cache is inclusive of lower cache levels." : "Cache is not inclusive of lower cache levels"
            , complex_cache_indexing ? "true" : "false"
        );
    }

    return num_data_caches;
}

int main () {
	size_t cache_sizes[32];
    int num_data_caches = cpuid_caches(cache_sizes);
}