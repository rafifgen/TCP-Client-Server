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


void error(const char *msg) {
    perror(msg);
    exit(1);
}

void send_broadcast(char *broadcast_msg, int *client_fds)
{
    for (int i=0; i < MAX_USERS; i++) {
        if (client_fds[i] != 0) {
            write(client_fds[i], broadcast_msg, strlen(broadcast_msg));
        }
    }
}

void handle_client(int sending_cli_sock, char *buffer, int *client_fds) {
    int read_size;
    FILE *f = fopen("client.html", "r");


    // Kirim file HTML ke klien
    if (f == NULL) {
        error("ERROR opening file");
    }
    // printf("Request:\n%s", buffer);
    if (strncmp(buffer, "GET /", 5) == 0) {
        while ((read_size = fread(buffer, 1, sizeof(buffer), f)) > 0) {
            if (write(sending_cli_sock, buffer, read_size) < 0) {
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
                sprintf(buffer, \
                        "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\nConnection: keep-alive\r\n\r\nSelamat datang, %s!\n", username);
                write(sending_cli_sock, buffer, strlen(buffer));
            } else {
                char *message = (char *) malloc(sizeof(char) * 100);
                char inputted_username[50];
                int n = sscanf(username, "%[^;];message:%[^;];", inputted_username, message);
                printf("\n%s: %s\n", inputted_username, message);
                sprintf(buffer, \
                        "HTTP/1.1 200 OK\r\n"
                        "Content-type: text/plain\r\n"
                        "Connection: keep-alive\r\n"
                        "\r\n"
                        "Server: I got your message, %s. Here it is: %s\n", \
                        inputted_username, message);
                printf("buffer:\n%s", buffer);
                write(sending_cli_sock, buffer, strlen(buffer));
                // send_broadcast(buffer, client_fds);
            }
        } else {
            sprintf(buffer, \
                    "HTTP/1.1 400 Bad Request\r\n"
                    "Content-type: text/plain\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "Username tidak ditemukan.\n");
            write(sending_cli_sock, buffer, strlen(buffer));
        }
    } else {
        sprintf(buffer, "HTTP/1.1 400 Bad Request\r\nContent-type: text/plain\r\nConnection: close\r\n\r\nHanya mendukung permintaan POST.\n");
        write(sending_cli_sock, buffer, strlen(buffer));
    }
    return;
}

int main(int argc, char *argv[]) {
    int server_sock, client_sock;
    int bytes_read, client_count;
    socklen_t cli_len;
    char buffer[BUFFER_SIZE];
    int client_fds[MAX_USERS];
    struct sockaddr_in serv_addr, cli_addr;

    // inisialisasi file descriptor client
    for (int i; i < MAX_USERS; i++) {
        client_fds[i] = 0;
    }

    // inisialisasi socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);

    // pengaturan address reusing
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

        // Mengecek apakah file descriptor sudah ada
        for (int i = 0; i < MAX_USERS; i++) {
            printf("Client fds ke-%d: %d\n", i+1, client_fds[i]);
            if (client_fds[i] == 0) {
                client_fds[i] = client_sock;
                break;
            } else if (client_fds[i] == client_sock) {
                break;
            }
        }
        if (fork() == 0) {
            close(server_sock); // Proses anak tidak perlu socket server
            bytes_read = read(client_sock, buffer, sizeof(buffer));
            if (bytes_read < 0) error("ERROR on reading.");
            printf("Request:\n%s", buffer);
            handle_client(client_sock, buffer, client_fds);
            return 0; // Keluar dari proses anak setelah selesai
        }
        close(client_sock); // Proses induk menutup socket klien
    }

    close(server_sock); // Tutup socket server
    return 0;
}
