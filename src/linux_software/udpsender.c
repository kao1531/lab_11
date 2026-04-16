#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[]) 
{
    // check two arguments were provided
    if (argc < 3)
    {
        printf("Unexpected number of arguments! Usage: udpsender <IP address> <num packets to send>\n");
        return 1;
    }

    // check second argument for number of packets to send
    int num_packets = atoi(argv[2]);

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

    printf("Sending %d packets... ", num_packets);

    // loop through and send packets
    for (int i = 0; i < num_packets; i++)
    {
        // pack data into packet (1028 bytes per packet)
        uint8_t message[1028];
        for (int j = 0; j < 1028; j++)
        {
            message[j] = i+j;
        }

        sendto(sock, message, 1028, 0, (struct sockaddr*)&s_info, sizeof(s_info));
    }

    printf("Done\n");

    // close socket when done
    close(sock);
    return 0;
}