---

🌱 QueLang — A Minimalist Systems Language

QueLang adalah bahasa pemrograman baru yang dirancang untuk kesederhanaan, eksplisit, dan mudah dipahami. Fokusnya:

✅ Sintaks ringan & familiar (terinspirasi Python + C)

✅ Kompilasi langsung ke AArch64 Assembly

✅ Kendali penuh atas memori dan struktur data



---

✨ Mengapa Memilih QueLang?

QueLang cocok untuk:

🧑‍🎓 Pelajar yang ingin belajar sistem low-level dengan gaya modern

⚙️ Pengembang OS / bare-metal ARM / sistem embedded

👩‍🔬 Eksperimen bahasa / compiler enthusiast / riset PL


✅ Kelebihan

Bahasa minimalis dan eksplisit (semua aturan terlihat jelas)

Arsitektur modular: tokenizer → parser → AST → codegen

Cepat dipelajari: seluruh bahasa bisa dikuasai dalam <1 jam


⚠️ Kekurangan

Belum memiliki sistem memori aman / GC

Kompilasi hanya untuk AArch64 (belum ada x86)

Belum ada package manager atau tooling lanjutan



---

🛠️ Kompilasi & Build Pipeline

./quelang input.q output.s         # Compile ke AArch64 Assembly
aarch64-linux-gnu-as -o out.o output.s
aarch64-linux-gnu-ld -o binary out.o

Gunakan mode debug:

./quelang --debug input.q output.s


---

🔍 Penjelasan Fitur Bahasa

🧩 1. Fungsi (def)

Digunakan untuk mendefinisikan fungsi, mirip Python:

def tambah(a: u16, b: u16): u16 {
    return a + b
}

🧱 2. Struct (def struct)

Tipe data buatan, mirip struct di C:

def struct Pair {x: u16 y: u16}

🏷️ 3. Alias Tipe (init type)

Alias atau typedef:

init type Age = u16

🎯 4. Pointer (*, &)

*ptr = val → assign ke alamat

&x → ambil alamat variabel

*x → dereferensi pointer


🔥 5. Injeksi Assembler (inj)

Langsung sisipkan ASM ke output:

inj raw {
    "mov x0, #42"
    "ret"
}

🔁 6. Kontrol Alur

if, elseif, else

if a == 1 {
    inj raw { "mov x0, #1" }
} else {
    inj raw { "mov x0, #0" }
}

while

while a < 5 {
    inj raw { "add x0, x0, #1" }
}

📦 7. Deklarasi Variabel (let)

let age: u16 = 10

🔂 8. Return, Break, Continue

return a + b
break
continue


---

📁 Struktur File Proyek

File	Fungsi

main.cpp	Pipeline utama (token → AST → asm)
token.cpp	Tokenizer (lexer)
parser.cpp	Parser + pembentuk AST
codegen.cpp	Generator AArch64 ASM
ast.hpp	Definisi AST node
grammar.txt	BNF grammar formal (opsional referensi)



---

🧪 Contoh Program

def struct Pair {a: u16 b: u16}

def set(x: *u16, y: u16) {
    *x = y
}

def main() {
    let a: u16 = 10
    let b: u16 = 20
    return a + b
}


---

📚 Lisensi & Kontribusi

QueLang adalah proyek open-source.
Silakan kontribusi melalui GitHub.
Versi saat ini masih belum stabil.
Versi saat ini: v0.1 alpha

© 2025 QueLang Contributors — Dibuat dengan ❤️ untuk pembelajar sistem low-level.


---
