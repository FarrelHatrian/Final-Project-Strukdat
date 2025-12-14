# Final Project Strukdat Athaya Ahmad Farrel Hatrian (5024241068)

# ABOUT
Proyek ini adalah simulasi fisika 2D yang dikembangkan menggunakan C++ dengan library SFML (Simple and Fast Multimedia Library). Tujuan utama program ini adalah mendemonstrasikan dan membandingkan kinerja dua algoritma deteksi tabrakan (Collision Detection) ketika menangani sejumlah besar objek bergerak.

# FITUR
- Menggunakan FPS
- Menggunakan tombol B untuk mode brute force
- Menggunakan tombol Q untuk mode quadtree
- Menggunakan space/mouse untuk menambah bola
- Menggunakan tombol R untuk reset bola

# Algoritma Collision
Brute Force
- Algoritma ini membandingkan setiap bola dengan setiap bola lainnya
- Jika ada 1000 bola, Brute Force harus melakukan sekitar 500.000 pemeriksaan tabrakan dalam satu frame
- Sangat lambat dan tidak skalabel untuk simulasi dengan banyak objek, menyebabkan FPS anjlok

QuadTree
- Menggunakan teknik Spatial Partitioning. QuadTree membagi ruang simulasi (jendela) menjadi empat area kecil (kuadran), kemudian membagi lagi kuadran tersebut
- la hanya dicek tabrakannya dengan bola lain yang berada di kuadran yang sama atau kuadran tetangga dekatnya
- Sangat cepat dan efisien. Meskipun ada 1000 bola, jumlah pemeriksaan tabrakan yang sebenarnya dilakukan tetap rendah, menjaga FPS tetap tinggi.
