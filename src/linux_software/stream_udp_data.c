#include <stdio.h>
#include <sys/mman.h> 
#include <fcntl.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
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


int main(int argc, char *argv[])
{
    // check argument provided
    if (argc < 2)
    {
        printf("Unexpected number of arguments! Usage: stream_udp_data <IP address>\n");
        return 1;
    }

    // create socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        printf("Unable to create socket!\n");
        return 1;
    }

    // create server information
    struct sockaddr_in s_info;
    memset(&s_info, 0, sizeof(s_info));

    s_info.sin_family = AF_INET;
    s_info.sin_addr.s_addr = inet_addr(argv[1]);
    s_info.sin_port = htons(25344);

    // first, get a pointer to the peripheral base address using /dev/mem and the function mmap
    volatile unsigned int *my_periph = get_a_pointer(FIFO_PERIPH_ADDRESS);	

    unsigned int total = 0;

    while (1)
    {
        // read FIFO count
        unsigned int curr_count = *(my_periph + FIFO_OCCUPANCY_OFFSET);

        // check if 256 words to be read
        if (curr_count > 256)
        {
            unsigned int message[257];

            // set first 4 bytes as counter
            message[0] = total;

            for (int i = 1; i < 257; i++)
            {
                // read FIFO data
                unsigned int data = *(my_periph + FIFO_DATA_OFFSET);
                
                // pack data
                message[i] = data;
            }

            // send data
            sendto(sock, message, 1028, 0, (struct sockaddr*)&s_info, sizeof(s_info));

            // increment counter
            total += 1;
        }
    }
    
    // close socket when done
    close(sock);
    return 0;
}
