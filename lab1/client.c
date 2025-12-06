#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8888
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

void send_file(const char *filename)
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("Error: File '%s' not found.\n", filename);
        return;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        fclose(fp);
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        close(sock);
        fclose(fp);
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Error: Could not connect to the server. Make sure it is running.\n");
        close(sock);
        fclose(fp);
        return;
    }

    printf("Connected to server at %s:%d\n", SERVER_IP, PORT);

    int bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0)
    {
        send(sock, buffer, bytes_read, 0);
    }

    printf("File '%s' sent successfully.\n", filename);

    fclose(fp);
    close(sock);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./client <filename>\n");
        return 1;
    }

    send_file(argv[1]);
    return 0;
}