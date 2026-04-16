#include <stdio.h>
#include <sys/mman.h> 
#include <fcntl.h> 
#include <unistd.h>
#define _BSD_SOURCE

#define FIFO_PERIPH_ADDRESS 0x43C10000
#define FIFO_OCCUPANCY_OFFSET 7 //0x1C
#define FIFO_DATA_OFFSET 8 //0x20

// the below code uses a device called /dev/mem to get a pointer to a physical
// address.  We will use this pointer to read/write the custom peripheral
volatile unsigned int * get_a_pointer(unsigned int phys_addr)
{

	int mem_fd = open("/dev/mem", O_RDWR | O_SYNC); 
	void *map_base = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, phys_addr); 
	volatile unsigned int *radio_base = (volatile unsigned int *)map_base; 
	return (radio_base);
}


int main()
{
    // first, get a pointer to the peripheral base address using /dev/mem and the function mmap
    volatile unsigned int *my_periph = get_a_pointer(FIFO_PERIPH_ADDRESS);	

    printf("\r\n\r\n\r\nhello I am going to read 10 seconds worth of data now...\n\r");

    int total = 0;

    while (total < 480000)
    {
        // read FIFO count
        unsigned int curr_count = *(my_periph + FIFO_OCCUPANCY_OFFSET);
        // printf("reading %d samples\n", curr_count);

        // check if words to be read
        if (curr_count > 0)
        {
            for (int i = 0; i < curr_count; i++)
            {
                // read FIFO data
                unsigned int data = *(my_periph + FIFO_DATA_OFFSET);
                // printf("%d\n", data);
            }

            // add count to total read
            total += curr_count;
        }
    }
    
    printf("Finished!\n");
    return 0;
}
