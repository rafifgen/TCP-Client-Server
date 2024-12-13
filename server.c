#include <stdio.h> // fclose, FILE, fopen, fread, perror, printf, sprintf, sscanf
#include <stdlib.h> // exit, free, malloc
#include <string.h> // bzero, strlen, strncmp, strstr
#include <unistd.h> // close, read, write
#include <netinet/in.h> // sockaddr_in
#include <sys/socket.h> // accept, bind, listen, setsockopt, socket, socklen_t,

#define BUFFER_SIZE 8000
#define PORT 8080
#define MAX_USERS 10


void error(const char *msg) {
    perror(msg);
    exit(1);
}

void handle_client(int sending_cli_sock, char *buffer) {
    int read_size;
    int err_handler;
    FILE *f = fopen("client.html", "r");


    // Kirim file HTML ke klien
    if (f == NULL) {
        error("ERROR opening file");
    }

    if (strncmp(buffer, "GET /", 5) == 0) {
        while ((read_size = fread(buffer, 1, sizeof(buffer), f)) > 0) {
            if (write(sending_cli_sock, buffer, read_size) < 0) {
                error("ERROR writing HTML file to socket");
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
                        "HTTP/1.1 200 OK\r\n"
                        "Content-type: text/plain\r\n"
                        "Connection: keep-alive\r\n"
                        "\r\n"
                        "Selamat datang, %s!\n", username);
                if (write(sending_cli_sock, buffer, strlen(buffer)) < 0)
                    error("ERROR on writing welcoming message");
            } else {
                char *message = (char *) malloc(sizeof(char) * 100);
                char *inputted_username = (char *) malloc(sizeof(char) * 50);
                sscanf(username, "%[^;];message:%[^;];", inputted_username, message);
                printf("\n%s: %s\n", inputted_username, message);
                sprintf(buffer, \
                        "HTTP/1.1 200 OK\r\n"
                        "Content-type: text/plain\r\n"
                        "Connection: keep-alive\r\n"
                        "\r\n"
                        "Server: saya sudah mendapat pesan anda, %s. Berikut adalah pesan anda: %s\n", \
                        inputted_username, message);
                if (write(sending_cli_sock, buffer, strlen(buffer)) < 0)
                    error("ERROR on writing reply message");

                /* Pembebasan Memori */
                free(message);
                free(inputted_username);
            }
        } else {
            sprintf(buffer, \
                    "HTTP/1.1 400 Bad Request\r\n"
                    "Content-type: text/plain\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "Username tidak ditemukan.\n");
            if (write(sending_cli_sock, buffer, strlen(buffer)) < 0) 
                error("ERROR on sending bad request message");
        }
    } else {
        sprintf(buffer, \
                "HTTP/1.1 400 Bad Request\r\n"
                "Content-type: text/plain\r\n"
                "Connection: close\r\n"
                "\r\n"
                "Hanya mendukung permintaan POST dan GET.\n");
        if (write(sending_cli_sock, buffer, strlen(buffer)))
            error("ERROR on sending bad request message"); 
    }
    return;
}

int main() {
    /* INISIALISASI VARIABEL */ 
    int server_sock, client_sock;
    int bytes_read, client_count, err_handler;
    socklen_t cli_len;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;

    /* INISIALISASI SOCKET */
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) error("ERROR opening socket");

    /* PENGATURAN ADDRESS REUSING */
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        error("setsockopt(SO_REUSEADDR) failed");

    bzero((char *)&serv_addr, sizeof(serv_addr));

    /* INISIALISASI ALAMAT SERVER */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    err_handler = bind(server_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (err_handler < 0) 
        error("ERROR on binding");

    err_handler = listen(server_sock, 5);
    if (err_handler < 0)
        error("ERROR on listening");
    cli_len = sizeof(cli_addr);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&cli_addr, &cli_len);
        if (client_sock < 0)
            error("ERROR on accept");

        if (fork() == 0) {
            close(server_sock); // Proses anak tidak perlu socket server
            bytes_read = read(client_sock, buffer, sizeof(buffer));
            if (bytes_read < 0)
                error("ERROR on reading.");
            handle_client(client_sock, buffer);
            exit(0); // Keluar dari proses anak setelah selesai
        }
        close(client_sock); // Proses induk menutup socket klien
    }

    close(server_sock); // Tutup socket server
    return 0;
}
