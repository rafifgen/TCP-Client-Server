#include <stdio.h> // fclose, FILE, fopen, fread, perror, printf, sprintf, sscanf
#include <stdlib.h> // exit, free, malloc
#include <string.h> // bzero, strlen, strcat, strncmp, strstr
#include <unistd.h> // close, read, write
#include <netinet/in.h> // sockaddr_in
#include <sys/socket.h> // accept, bind, listen, setsockopt, socket, socklen_t,

#define BUFFER_SIZE 8000
#define PORT 8080
#define MAX_USERS 10
#define PEMBAYARAN 0
#define PESANAN 1
#define DANA 2

/* Prompt Pertanyaan tiap kategori */
/* Dideklarasikan sebagai varaibel global karnena tidak ada operasi write sehingga tidak ada potensi reace condition */
char pembayaran_questions[350] = {
    "1. Apa itu Metode Pembayaran COD (Bayar di Tempat)?\n"
    "2. Apa yang terjadi jika saya tidak membayar pesanan\n"
    "    setelah batas waktu pembayaran berakhir?\n"
    "3. Apakah saya dapat mengubah metode pembayaran saya?"
};

char pesanan_questions[300] = {
    "1. Apakah saya dapat membatalkan pesanan saya?\n"
    "2. Bagaimana proses pengembalian dana untuk pesanan yang dibatalkan?\n"
    "3. Bagaimana cara membuat pesanan dengan jasa kirim Ambil di Tempat?"
};

char dana_questions[300] = {
    "1. Siapa yang bertanggung jawab atas kerusakan barang yang terjadi saat pengiriman?\n"
    "2. Bagaimana proses pengembalian dana untuk pesanan yang dibatalkan?\n"
    "3. Bagaimana cara melacak lokasi barang yang dikembalikan ke Penjual?"
};

char pembayaran_answers[3][1500] = { \
    /* PESAN PERTAMA PEMBAYARAN */
    "COD (Bayar di Tempat) adalah metode pembayaran tunai yang dilakukan secara\n"
    "langsung di tempat, setelah pesanan dari Kurir diterima oleh Pembeli.\n\n"
    \
    "Metode pembayaran COD (Bayar di Tempat) berlaku tanpa min. pembelian\n"
    "dengan ketentuan sebagai berikut:\n\n"
    \
    "Maks. pembelian Rp5.000.000/pesanan untuk layanan pengiriman Regular dan Hemat.\n"
    "Maks. pembelian Rp500.000/pesanan untuk layanan pengiriman Instant.\n\n"
    \
    "Untuk metode pembayaran COD (Bayar di Tempat), tidak ada biaya yang dikenakan\n"
    "ke Penjual. Biaya penanganan yang berlaku untuk seluruh Pengguna Shopee\n"
    "dengan ketentuan berikut:\n\n"
    \
    "1. Untuk pesanan COD (Bayar di Tempat) pertama s/d ke-3 kali akan dikenakan\n"
    "    biaya penanganan sebesar 0%.\n"
    "2. Untuk pesanan COD (Bayar di Tempat) ke-4 dan seterusnya akan dikenakan\n"
    "    biaya penanganan sebesar 4%.\n"
    "3. Pengguna yang teridentifikasi sebagai Dropshipper, baik dengan mengaktifkan\n"
    "    tombol fitur dropship ataupun yang teridentifikasi pernah melakukan transaksi\n"
    "    COD (Bayar di Tempat) ke lebih dari 3 alamat yang berbeda, akan dikenakan\n"
    "    biaya penanganan sebesar 10%.\n"
    "4. Penjual yang teridentifikasi secara sistem memiliki pesanan dropship melebihi\n"
    "    pesanan normal, maka seluruh pesanannya akan dikenakan biaya penanganan sebesar 10%.\n"
    "5. Pengguna yang teridentifikasi menggunakan browser Komputer/Handphone saat melakukan\n"
    "    checkout dikenakan biaya penanganan sebesar 10%.",
    \
    /* PESAN KEDUA PEMBAYARAN */
    "Setelah checkout pesanan berhasil, Anda harus melakukan pembayaran dalam batas\n"
    "waktu yang telah ditentukan untuk menyelesaikan pesanan Anda. Batas waktu ini\n"
    "tergantung pada metode pembayaran yang Anda pilih:\n\n"
    \
    "- ShopeePay dan SPayLater: 1 (satu) jam setelah checkout.\n"
    "- Kartu kredit, debit online/kartu kredit, dan OneKlik: 3 (tiga) jam setelah checkout.\n"
    "- Virtual Account, Indomaret, dan Alfamart: 1x24 jam setelah checkout.",
    \
    /* PESAN KETIGA PEMBAYARAN */
    "Anda dapat mengubah metode pembayaran kapan saja sebelum pembayaran berhasil\n"
    "diproses oleh Shopee.\n\n"
    \
    "Pembayaran yang belum diselesaikan dapat ditemukan di halaman Pembelian Saya.\n"
    "Untuk mengubah metode pembayaran, buka halaman Saya > Belum Bayar > Ubah Metode\n"
    "Pembayaran > pilih metode pembayaran yang tersedia > lalu pilih Konfirmasi.\n\n"
    \
    "Anda harus melakukan pembayaran pada tanggal yang ditentukan di halaman Pembelian Saya.\n\n"
    \
    "Jika Anda tidak menemukan pesanan pada halaman Belum Bayar, maka pembayaran Anda\n"
    "telah berhasil diproses dengan metode yang telah dipilih sebelumnya."
};

char pesanan_answers[3][1500] = {
    /* JAWABAN PERTAMA PESANAN */
    "Anda dapat membatalkan pesanan Anda dengan mengirimkan permintaan pembatalan. Untuk melakukannya,\n"
    "pilih Batalkan Pesanan di halaman Rincian Pesanan > pilih Alasan Pembatalan > pilih Konfirmasi.\n\n"

    "⚠ Pembeli hanya dapat mengajukan 1 (satu) kali permintaan pembatalan untuk setiap\n"
    "pesanan. Jika pengajuan pembatalan Anda ditolak oleh Penjual, Anda tidak dapat\n"
    "mengajukan pembatalan lagi untuk pesanan yang sama.\n\n"

    "Pembatalan memiliki ketentuan yang berbeda-beda sesuai dengan jenis pembatalan\n"
    "yang dipilih. Pelajari lebih lanjut tentang jenis-jenis pembatalan pesanan di Shopee.\n\n"

    "Untuk memeriksa status pengajuan pembatalan Anda, pilih Pesanan Saya > pilih\n"
    "Tampilkan Rincian Pembatalan pada pesanan.\n\n"

    "Jika Anda ingin membatalkan pengajuan, pilih Batalkan Pengajuan > pilih Konfirmasi.",

    /* JAWABAN KEDUA PESANAN */
    "Pengembalian dana untuk pesanan yang berhasil dibatalkan akan segera diproses\n"
    "sesuai dengan metode pembayaran yang digunakan. Waktu proses pengembalian\n"
    "dana berbeda-beda untuk setiap metode pembayaran.\n\n"

    "Anda akan menerima notifikasi melalui aplikasi Shopee dan email setelah\n"
    "pengembalian Anda diproses. Untuk melihat status pengembalian, pilih Status\n"
    "Pesanan melalui tab Notifikasi di aplikasi Shopee.",

    /* JAWABAN KETIGA PESANAN */
    "Anda dapat menggunakan jasa kirim Ambil di Tempat dengan cara memilih Ambil\n"
    "di Tempat pada opsi pengiriman saat checkout.\n\n"

    "1. Pilih Ambil di Tempat.\n"
    "2. Pilih jasa kirim Ambil di Tempat yang tersedia seperti J&T Jemari,\n"
    "    Agen SPX Express, SPX Express Point, atau Indopaket (Ambil di Indomaret).\n"
    "3. Pilih Tempat Pengambilan, kemudian Anda dapat mengatur Provinsi, Kota/Kabupaten, dan Kecamatan.\n"
    "4. Pilih Cari dan dilanjutkan dengan pilih Cabang dan pilih Konfirmasi.\n"
    "5. Setelah mengatur alamat pengambilan, pastikan juga Nama dan No. Handphone\n"
    "    yang dapat dihubungi sudah diatur dengan benar, lalu pilih Konfirmasi.\n\n"

    "Untuk melanjutkan pesanan, Anda dapat menggunakan Voucher yang dimiliki dengan\n"
    "pilih Voucher Shopee > pilih Metode Pembayaran > pilih Buat Pesanan."
};

char dana_answers[3][1500] = {
    /* JAWABAN PERTAMA DANA */
    "Apabila produk rusak dalam pengiriman, Pembeli dapat mengajukan pengembalian\n"
    "barang/dana melalui aplikasi Shopee dengan menyertakan bukti yang dibutuhkan\n"
    "untuk keperluan peninjauan ke pihak jasa kirim. Penyerahan bukti dilakukan melalui\n"
    "aplikasi Shopee dan Pembeli harus melampirkannya sesuai dengan kriteria wajib atau opsional.\n\n"

    "Keputusan yang diberikan didasarkan pada analisis dan bukti peninjauan yang\n"
    "dilakukan oleh pihak jasa kirim. Shopee memiliki kewenangan yang tidak terbatas\n"
    "dalam menentukan langkah selanjutnya serta pihak yang harus menanggung ongkos kirim\n"
    "pengembalian barang jika diperlukan.\n\n"

    "Keputusan yang dibuat oleh Shopee tidak dapat diganggu gugat.",

    /* JAWABAN KEDUA DANA */
    "Pengembalian dana untuk pesanan yang berhasil dibatalkan akan segera diproses sesuai dengan\n"
    "metode pembayaran yang digunakan. Waktu proses pengembalian dana berbeda-beda untuk setiap metode pembayaran.\n\n"

    "Anda akan menerima notifikasi melalui aplikasi Shopee dan email setelah pengembalian Anda diproses.\n"
    "Untuk melihat status pengembalian, pilih Status Pesanan melalui tab Notifikasi di aplikasi Shopee.",

    /* JAWABAN KETIGA DANA */
    "Anda dapat melacak pengembalian paket dengan no. resi yang diberikan setelah Anda mengajukan\n"
    "pengembalian barang/dana, atau mengatur sendiri pengiriman dengan jasa kirim pilihan Anda.\n\n"

    "Silakan lacak barang melalui:\n"
    "- Aplikasi Shopee untuk pembelian produk dari toko Shopee Mall.\n"
    "- Situs masing-masing jasa kirim untuk pembelian produk dari toko non-Shopee Mall\n"
    "  dengan memasukkan no. resi."
};

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void handle_not_post_get(int client_fd)
{
    char buffer[BUFFER_SIZE];

    sprintf(buffer, \
            "HTTP/1.1 200 OK\r\n"
            "Content-type: text/plain\r\n"
            "Connection: keep-alive\r\n"
            "\r\n");
    sprintf(buffer, \
        "HTTP/1.1 400 Bad Request\r\n"
        "Content-type: text/plain\r\n"
        "Connection: close\r\n"
        "\r\n"
        "Server hanya mendukung permintaan POST dan GET.\n");
    if (write(client_fd, buffer, strlen(buffer)))
        error("ERROR on sending bad request message"); 

}

void handle_cat_dana(int client_fd, int ques)
{
    char buffer[BUFFER_SIZE];

    sprintf(buffer, \
            "HTTP/1.1 200 OK\r\n"
            "Content-type: text/plain\r\n"
            "Connection: keep-alive\r\n"
            "\r\n");
    if (ques == -1) {
        strcat(buffer, "Hal apa yang ingin Anda tanyakan?\n");
        strcat(buffer, \
                dana_questions);
        strcat(buffer, "\n");
        if (write(client_fd, buffer, strlen(buffer)) < 0)
            error("ERROR on writing welcoming message");
    } else if (ques == 0) {
        strcat(buffer, dana_answers[0]);
        if (write(client_fd, buffer, strlen(buffer)) < 0)
            error("ERROR on writing welcoming message");
    } else if (ques == 1) {
        strcat(buffer, dana_answers[1]);
        if (write(client_fd, buffer, strlen(buffer)) < 0)
            error("ERROR on writing welcoming message");
    } else {
        strcat(buffer, dana_answers[2]);
        if (write(client_fd, buffer, strlen(buffer)) < 0)
            error("ERROR on writing welcoming message");
    }
    return;
}

void handle_cat_pembayaran(int client_fd, int ques)
{
    char buffer[BUFFER_SIZE];

    sprintf(buffer, \
            "HTTP/1.1 200 OK\r\n"
            "Content-type: text/plain\r\n"
            "Connection: keep-alive\r\n"
            "\r\n");
    if (ques == -1) {
        strcat(buffer, "Hal apa yang ingin Anda tanyakan?\n");
        strcat(buffer, \
                pembayaran_questions);
        strcat(buffer, "\n");
        if (write(client_fd, buffer, strlen(buffer)) < 0)
            error("ERROR on writing welcoming message");
    } else if (ques == 0) {
        strcat(buffer, pembayaran_answers[0]);
        if (write(client_fd, buffer, strlen(buffer)) < 0)
            error("ERROR on writing welcoming message");
    } else if (ques == 1) {
        strcat(buffer, pembayaran_answers[1]);
        if (write(client_fd, buffer, strlen(buffer)) < 0)
            error("ERROR on writing welcoming message");
    } else {
        strcat(buffer, pembayaran_answers[2]);
        if (write(client_fd, buffer, strlen(buffer)) < 0)
            error("ERROR on writing welcoming message");
    }
}

void handle_cat_pesanan(int client_fd, int ques)
{
    char buffer[BUFFER_SIZE];

    sprintf(buffer, \
            "HTTP/1.1 200 OK\r\n"
            "Content-type: text/plain\r\n"
            "Connection: keep-alive\r\n"
            "\r\n");
    if (ques == -1) {
        strcat(buffer, "Hal apa yang ingin Anda tanyakan?\n");
        strcat(buffer, \
                pesanan_questions);
        strcat(buffer, "\n");
        if (write(client_fd, buffer, strlen(buffer)) < 0)
            error("ERROR on writing welcoming message");
    } else if (ques == 0) {
        strcat(buffer, pesanan_answers[0]);
        if (write(client_fd, buffer, strlen(buffer)) < 0)
            error("ERROR on writing welcoming message");
    } else if (ques == 1) {
        strcat(buffer, pesanan_answers[1]);
        if (write(client_fd, buffer, strlen(buffer)) < 0)
            error("ERROR on writing welcoming message");
    } else {
        strcat(buffer, pesanan_answers[2]);
        if (write(client_fd, buffer, strlen(buffer)) < 0)
            error("ERROR on writing welcoming message");
    }
    return;
}

void handle_bad_request(int client_fd)
{
    char buffer[BUFFER_SIZE];

    sprintf(buffer, \
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Username tidak ditemukan.\n");
    if (write(client_fd, buffer, strlen(buffer)) < 0) 
        error("ERROR on sending bad request message");
    return;
}

void handle_menu_awal(int client_fd, char *username)
{
    char buffer[BUFFER_SIZE];

    sprintf(buffer, \
        "HTTP/1.1 200 OK\r\n"
        "Content-type: text/plain\r\n"
        "Connection: keep-alive\r\n"
        "\r\n"
        "Selamat datang, %s!\n", username);
    strcat(buffer, \
            "Pilih kategori bantuan yang ingin ditampilkan.\n"
            "1. Pembayaran\n"
            "2. Pesanan dan Pengiriman\n"
            "3. Pengembalian Dana"
            );
    if (write(client_fd, buffer, strlen(buffer)) < 0)
        error("ERROR on writing welcoming message");
    return;
}

void handle_client(int sending_cli_sock, char *buffer) {
    int read_size;
    int err_handler;
    FILE *f = fopen("client.html", "r");
    char help_categories[3][50];
    char username[50];
    int cat, ques; // inisialisasi
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
        char *body = strstr(buffer, "username:");
        if (body != NULL) {
            sscanf(body, "username:%[^;];cat:%d;ques:%d;", username, &cat, &ques);
            cat--;
            ques--;

            if (cat == PEMBAYARAN) {
                handle_cat_pembayaran(sending_cli_sock, ques);
            } else if (cat == PESANAN) {
                handle_cat_pesanan(sending_cli_sock, ques);
            } else if (cat == DANA) {
                handle_cat_dana(sending_cli_sock, ques);
            } else {
                // Kirim balasan ke klien
                handle_menu_awal(sending_cli_sock, username);
            }
        } else {
            handle_bad_request(sending_cli_sock);
        }
    } else {
        handle_not_post_get(sending_cli_sock);
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
            // printf("Request:\n%s", buffer); // line ini dapat di-uncomment untuk melakukan pengecekan request yang diterima
            handle_client(client_sock, buffer);
            exit(0); // Keluar dari proses anak setelah selesai
        }
        close(client_sock); // Proses induk menutup socket klien
    }

    close(server_sock); // Tutup socket server
    return 0;
}
