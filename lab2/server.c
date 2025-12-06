#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUFFER_SIZE 1024

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        exit(EXIT_FAILURE);
    }

    FILE *fp = fopen("received_file", "wb");
    if (fp == NULL)
    {
        close(new_socket);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    int valread;
    while ((valread = recv(new_socket, buffer, BUFFER_SIZE, 0)) > 0)
    {
        fwrite(buffer, 1, valread, fp);
    }

    fclose(fp);
    close(new_socket);
    close(server_fd);

    return 0;
}