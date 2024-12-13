#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 8000
#define PORT 8080
#define MAX_USERS 10


typedef struct User {
    char username[51];
    char message[101];
} User;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void handle_client(int cli_sock, char *buffer) {
    // char buffer[BUFFER_SIZE];
    int read_size;
    FILE *f = fopen("client.html", "r");

    // Kirim file HTML ke klien
    if (f == NULL) {
        error("ERROR opening file");
    }
    printf("Request:\n%s", buffer);
    if (strncmp(buffer, "GET /", 5) == 0) {
        while ((read_size = fread(buffer, 1, sizeof(buffer), f)) > 0) {
            if (write(cli_sock, buffer, read_size) < 0) {
                error("ERROR writing to socket");
            }
        }
        fclose(f);
    } else if (strncmp(buffer, "POST /", 6) == 0) {
        char *username = strstr(buffer, "username:");
        if (username != NULL) {
            username += strlen("username:"); // Pindah pointer ke setelah "username:"
            char *message_ptr = strstr(buffer, ";message:");

            if (message_ptr == NULL) {
                printf("\n%s joined\n", username);

                // Kirim balasan ke klien
                sprintf(buffer, "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\nConnection: keep-alive\r\n\r\nSelamat datang, %s!\n", username);
                write(cli_sock, buffer, strlen(buffer));
            } else {
                char *message = (char *) malloc(sizeof(char) * 100);
                char inputted_username[50];
                int n = sscanf(username, "%[^;];message:%[^;];", inputted_username, message);
                printf("\n%s: %s\n", inputted_username, message);
                sprintf(buffer, "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\nConnection: keep-alive\r\n\r\n%s: %s\n", inputted_username, message);
                write(cli_sock, buffer, strlen(buffer));
                // TODO: BROADCAST
            }
        } else {
            sprintf(buffer, "HTTP/1.1 400 Bad Request\r\nContent-type: text/plain\r\nConnection: close\r\n\r\nUsername tidak ditemukan.\n");
            write(cli_sock, buffer, strlen(buffer));
        }
    } else {
        sprintf(buffer, "HTTP/1.1 400 Bad Request\r\nContent-type: text/plain\r\nConnection: close\r\n\r\nHanya mendukung permintaan POST.\n");
        write(cli_sock, buffer, strlen(buffer));
    }
    return;
}

int main(int argc, char *argv[]) {
    int server_sock, client_sock;
    int bytes_read, client_count;
    socklen_t cli_len;
    char buffer[BUFFER_SIZE];
    User users[MAX_USERS];
    struct sockaddr_in serv_addr, cli_addr;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        error("setsockopt(SO_REUSEADDR) failed");
    if (server_sock < 0) error("ERROR opening socket");

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    listen(server_sock, 5);
    cli_len = sizeof(cli_addr);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&cli_addr, &cli_len);
        if (client_sock < 0) error("ERROR on accept");

        if (fork() == 0) {
            close(server_sock); // Proses anak tidak perlu socket server
            bytes_read = read(client_sock, buffer, sizeof(buffer));
            if (bytes_read < 0) error("ERROR on reading.");
            handle_client(client_sock, buffer);
            return 0; // Keluar dari proses anak setelah selesai
        }
        close(client_sock); // Proses induk menutup socket klien
    }

    close(server_sock); // Tutup socket server
    return 0;
}
