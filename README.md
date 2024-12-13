# **Proyek Server-Client Sederhana**

## **Deskripsi Proyek**
Proyek ini merupakan tugas akhir mata kuliah Sistem Operasi pada semester 3 Teknik Informatika di Politeknik Negeri Bandung. Pembuatan Web Server bertujuan untuk mendemonstrasikan hal-hal yang telah dipelajari selama perkuliahan. Di antaranya adalah _system call_ terkait proses seperti fork.

Proyek ini merupakan implementasi komunikasi antara server dan _client_ menggunakan socket programming. Server berfungsi untuk menerima permintaan HTTP dari klien (metode GET dan POST) dan meresponsnya sesuai dengan data yang dikirimkan. _Client_ menggunakan file HTML dan JavaScript untuk berkomunikasi dengan server melalui HTTP. Selain itu, proyek ini juga bertujuan untuk mendemonstrasikan penggunaan _system call_ pada Linux. Dengan _system call_ ini, proses baru akan dibuat untuk request yang datang. Kemudian, proses tersebut akan keluar setelah selesai menangani request.

## Tim
- Muhammad Rafif Genadratama
- Nadia Rachma Yuninda

## **Fitur**
- **Metode GET:**
  _Client_ dapat meminta fail `client.html` dari server. Fail ini akan dikirimkan secara langsung oleh server ke klien.
- **Metode POST:**
  - Klien dapat mengirimkan username untuk melakukan registrasi ke server.
  - Klien dapat mengirimkan pesan setelah username terdaftar.
  - Server akan merespons dengan pesan yang sesuai.

## **Cara Kerja**
1. **Server:**
   - Server mendengarkan koneksi pada port `8080`.
   - Permintaan GET akan memberikan file `client.html`.
   - Permintaan POST akan diproses untuk mendapatkan data seperti username dan pesan.

2. **_Client_ (client.html):**
   - _Client_ memiliki _interface_ HTML sederhana untuk mengirimkan username dan pesan ke server.
   - Data dikirimkan ke server menggunakan metode POST.

## **Cara Menjalankan**
### **Persyaratan**
- Sistem operasi Linux atau Windows (dengan dukungan socket programming).
- Compiler C (misalnya `gcc` untuk Linux).
- Browser modern untuk membuka file `client.html`.

### **Langkah-Langkah**
1. **Menjalankan Server:**
   - Kompilasi server menggunakan perintah berikut:
     ```bash
     gcc server.c -o server
     ```
   - Jalankan server:  
     ```bash
     ./server
     ```

2. **Menjalankan Klien:**
   - Buka file `client.html` di browser Anda.
   - Masukkan username, klik "Hubungkan", lalu kirim pesan menggunakan tombol "Kirim".

## **Struktur File**
- `server.c`: File kode server.
- `client.html`: File HTML dan JavaScript untuk klien.

## **Contoh Interaksi**
1. **Koneksi Awal:**  
   - Klien mengirimkan username melalui POST.
   - Server merespons dengan pesan selamat datang:
     ```
     Selamat datang, [username]!
     ```

2. **Mengirim Pesan:**
   - Klien mengirimkan pesan melalui POST.
   - Server merespons dengan pesan balasan:
     ```
     Server: saya sudah mendapat pesan anda, [username]. Berikut adalah pesan anda: [pesan].
     ```

## **Catatan Tambahan**
- Server hanya mendukung permintaan dengan metode GET dan POST.
- File `client.html` harus berada di direktori yang sama dengan file eksekusi server agar dapat ditemukan.

## Referensi
- Jawaban [user3820950](https://stackoverflow.com/a/27148626) pada StackOverflow
- Format dari [HTTP message](https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages)
