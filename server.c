#include <stdio.h> // fclose, FILE, fopen, fread, perror, printf, sprintf, sscanf
#include <stdlib.h> // exit, free, malloc
#include <string.h> // bzero, strlen, strncmp, strstr
#include <unistd.h> // close, read, write
#include <netinet/in.h> // sockaddr_in
#include <sys/socket.h> // accept, bind, listen, setsockopt, socket, socklen_t,

#define BUFFER_SIZE 8000
#define PORT 8080
#define MAX_USERS 10
#define PEMBAYARAN 0
#define PESANAN 1
#define DANA 2

typedef struct question_and_answer {
    char ques[100];
    char ans[1500];
} QnA;

typedef struct Category {
    // char cat[10];
    QnA ques_and_ans[3];
} Cat;

/* Prompt Pertanyaan tiap kategori */
/* Dideklarasikan sebagai varaibel global karnena tidak ada operasi write sehingga tidak ada potensi reace condition */
char pembayaran_questions[3][100] = {
    "Apa itu Metode Pembayaran COD (Bayar di Tempat)?",
    "Apa yang terjadi jika saya tidak membayar pesanan setelah batas waktu pembayaran berakhir?",
    "Apakah saya dapat mengubah metode pembayaran saya?"
};

char pesanan_questions[3][75] = {
    "Apakah saya dapat membatalkan pesanan saya?",
    "Bagaimana proses pengembalian dana untuk pesanan yang dibatalkan?",
    "Bagaimana cara membuat pesanan dengan jasa kirim Ambil di Tempat?"
};

char dana_questions[3][20];

char pembayaran_answers[3][1500] = { \
    /* PESAN PERTAMA PEMBAYARAN */
    "COD (Bayar di Tempat) adalah metode pembayaran tunai yang dilakukan secara "
    "langsung di tempat, setelah pesanan dari Kurir diterima oleh Pembeli.\n\n"
    \
    "Metode pembayaran COD (Bayar di Tempat) berlaku tanpa min. pembelian "
    "dengan ketentuan sebagai berikut:\n\n"
    \
    "Maks. pembelian Rp5.000.000/pesanan untuk layanan pengiriman Regular dan Hemat.\n"
    "Maks. pembelian Rp500.000/pesanan untuk layanan pengiriman Instant.\n\n"
    \
    "Untuk metode pembayaran COD (Bayar di Tempat), tidak ada biaya yang dikenakan "
    "ke Penjual. Biaya penanganan yang berlaku untuk seluruh Pengguna Shopee dengan ketentuan berikut:\n\n"
    \
    "Untuk pesanan COD (Bayar di Tempat) pertama s/d ke-3 kali akan dikenakan biaya penanganan sebesar 0%.\n"
    "Untuk pesanan COD (Bayar di Tempat) ke-4 dan seterusnya akan dikenakan biaya penanganan sebesar 4%.\n"
    "Pengguna yang teridentifikasi sebagai Dropshipper, baik dengan mengaktifkan tombol fitur dropship "
    "ataupun yang teridentifikasi pernah melakukan transaksi COD (Bayar di Tempat) ke lebih dari 3 alamat "
    "yang berbeda, akan dikenakan biaya penanganan sebesar 10%.\n"
    "Penjual yang teridentifikasi secara sistem memiliki pesanan dropship melebihi pesanan normal, "
    "maka seluruh pesanannya akan dikenakan biaya penanganan sebesar 10%.\n"
    "Pengguna yang teridentifikasi menggunakan browser Komputer/Handphone saat melakukan "
    "checkout dikenakan biaya penanganan sebesar 10%.",
    \
    /* PESAN KEDUA PEMBAYARAN */
    "Setelah checkout pesanan berhasil, Anda harus melakukan pembayaran dalam batas "
    "waktu yang telah ditentukan untuk menyelesaikan pesanan Anda. Batas waktu ini "
    "tergantung pada metode pembayaran yang Anda pilih:\n\n"
    \
    "ShopeePay dan SPayLater: 1 (satu) jam setelah checkout.\n"
    "Kartu kredit, debit online/kartu kredit, dan OneKlik: 3 (tiga) jam setelah checkout.\n"
    "Virtual Account, Indomaret, dan Alfamart: 1x24 jam setelah checkout.\n",
    \
    /* PESAN KETIGA PEMBAYARAN */
    "Anda dapat mengubah metode pembayaran kapan saja sebelum pembayaran berhasil diproses oleh Shopee.\n\n"
    \
    "Pembayaran yang belum diselesaikan dapat ditemukan di halaman Pembelian Saya. Untuk mengubah metode "
    "pembayaran, buka halaman Saya > Belum Bayar > Ubah Metode Pembayaran > pilih metode pembayaran "
    "yang tersedia > lalu pilih Konfirmasi.\n\n"
    \
    "Anda harus melakukan pembayaran pada tanggal yang ditentukan di halaman Pembelian Saya.\n\n"
    \
    "Jika Anda tidak menemukan pesanan pada halaman Belum Bayar, maka pembayaran Anda "
    "telah berhasil diproses dengan metode yang telah dipilih sebelumnya."
};

char pesanan_answers[3][1500] = {
    /* JAWABAN PERTAMA PESANAN */
    "Anda dapat membatalkan pesanan Anda dengan mengirimkan permintaan pembatalan."
    "Untuk melakukannya, pilih Batalkan Pesanan di halaman Rincian Pesanan > pilih Alasan Pembatalan > pilih Konfirmasi.\n\n"

    "⚠ Pembeli hanya dapat mengajukan 1 (satu) kali permintaan pembatalan untuk setiap "
    "pesanan. Jika pengajuan pembatalan Anda ditolak oleh Penjual, Anda tidak dapat mengajukan pembatalan lagi untuk pesanan yang sama.\n\n"

    "Pembatalan memiliki ketentuan yang berbeda-beda sesuai dengan jenis pembatalan "
    "yang dipilih. Pelajari lebih lanjut tentang jenis-jenis pembatalan pesanan di Shopee.\n\n"
    // "Memeriksa status pengajuan pembatalan:"

    "Untuk memeriksa status pengajuan pembatalan Anda, pilih Pesanan Saya > pilih "
    "Tampilkan Rincian Pembatalan pada pesanan.\n\n"

    "Jika Anda ingin membatalkan pengajuan, pilih Batalkan Pengajuan > pilih Konfirmasi.",

    /* JAWABAN KEDUA PESANAN */
    "Pengembalian dana untuk pesanan yang berhasil dibatalkan akan segera diproses "
    "sesuai dengan metode pembayaran yang digunakan. Waktu proses pengembalian"
    "dana berbeda-beda untuk setiap metode pembayaran.\n\n"

    "Anda akan menerima notifikasi melalui aplikasi Shopee dan email setelah "
    "pengembalian Anda diproses. Untuk melihat status pengembalian, pilih Status"
    "Pesanan melalui tab Notifikasi di aplikasi Shopee.",

    /* JAWABAN KETIGA PESANAN */
    "Anda dapat menggunakan jasa kirim Ambil di Tempat dengan cara memilih Ambil "
    "di Tempat pada opsi pengiriman saat checkout.\n\n"

    "Pilih Ambil di Tempat.\n"
    "Pilih jasa kirim Ambil di Tempat yang tersedia seperti J&T Jemari, "
    "Agen SPX Express, SPX Express Point, atau Indopaket (Ambil di Indomaret).\n"
    "Pilih Tempat Pengambilan, kemudian Anda dapat mengatur Provinsi, Kota/Kabupaten, dan Kecamatan.\n"
    "Pilih Cari dan dilanjutkan dengan pilih Cabang dan pilih Konfirmasi.\n"
    "Setelah mengatur alamat pengambilan, pastikan juga Nama dan No. Handphone "
    "yang dapat dihubungi sudah diatur dengan benar, lalu pilih Konfirmasi.\n\n"

    "Untuk melanjutkan pesanan, Anda dapat menggunakan Voucher yang dimiliki dengan "
    "pilih Voucher Shopee > pilih Metode Pembayaran > pilih Buat Pesanan."
};

char dana_answers[3][1500]
void error(const char *msg) {
    perror(msg);
    exit(1);
}

void handle_client(int sending_cli_sock, char *buffer) {
    int read_size;
    int err_handler;
    FILE *f = fopen("client.html", "r");
    char help_categories[3][50];
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

            // User baru login
            if (message_ptr == NULL) {
                printf("%s joined\n", username);

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
                printf("%s: %s\n", inputted_username, message);
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
                "Server hanya mendukung permintaan POST dan GET.\n");
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
            // prinf("Request:\n%s", buffer); // line ini dapat di-uncomment untuk melakukan pengecekan request yang diterima
            handle_client(client_sock, buffer);
            exit(0); // Keluar dari proses anak setelah selesai
        }
        close(client_sock); // Proses induk menutup socket klien
    }

    close(server_sock); // Tutup socket server
    return 0;
}
