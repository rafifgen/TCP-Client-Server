# **Proyek Server-Client Sederhana**

## **Deskripsi Proyek**
Proyek ini merupakan implementasi komunikasi antara server dan klien menggunakan socket programming. Server berfungsi untuk menerima permintaan HTTP dari klien (metode GET dan POST) dan meresponsnya sesuai dengan data yang dikirimkan. Klien menggunakan file HTML dan JavaScript untuk berkomunikasi dengan server melalui HTTP.

## **Fitur**
- **Metode GET:**
  Klien dapat meminta file `client.html` dari server. File ini akan dikirimkan secara langsung oleh server ke klien.
- **Metode POST:**
  - Klien dapat mengirimkan username untuk melakukan registrasi ke server.
  - Klien dapat mengirimkan pesan setelah username terdaftar.
  - Server akan merespons dengan pesan yang sesuai.

## **Cara Kerja**
1. **Server:**
   - Server mendengarkan koneksi pada port `8080`.
   - Permintaan GET akan memberikan file `client.html`.
   - Permintaan POST akan diproses untuk mendapatkan data seperti username dan pesan.

2. **Klien (client.html):**
   - Klien memiliki antarmuka HTML sederhana untuk mengirimkan username dan pesan ke server.
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
