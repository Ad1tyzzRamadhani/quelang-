<div align="center">
  <img width="1440" height="1080" alt="Image" src="https://github.com/user-attachments/assets/d59c1b0d-55ff-4926-8dce-ed4985188ae5" />
  <h1>🌱 QueLang — A Minimalist Systems Language</h1>
  <p><em>Bahasa sistem baru yang eksplisit, minimalis, dan mendekati perangkat keras.</em></p>
  <p>
    <strong>Sintaks ringan</strong> · 
    <strong>Kompilasi ke AArch64</strong> · 
    <strong>Pointer & Struct eksplisit</strong>
  </p>
</div>

---

> ⚠️ **PERINGATAN**: QueLang masih dalam versi awal (`v0.4-alpha`) dan belum stabil.  
> Harap tidak digunakan untuk produksi besar. Masukan dan eksperimen sangat diapresiasi!
>

## ✨ Mengapa Memilih QueLang?

**QueLang cocok untuk:**

- 🧑‍🎓 Pelajar yang ingin belajar sistem low-level dengan gaya modern
- ⚙️ Pengembang mikrokernel, bare-metal ARM, atau sistem embedded
- 👩‍🔬 Eksperimen bahasa / compiler enthusiast / riset PL

| ✅ Kelebihan                          | ⚠️ Kekurangan                        |
|--------------------------------------|--------------------------------------|
| Minimalis dan eksplisit              | Belum ada memory safety / GC         |
| Modular: tokenizer → parser → codegen| Belum mendukung x86/x64              |
| Mudah dipelajari & disusun           | Belum ada tooling modern (pkg mgr)   |

---

## 🛠️ Kompilasi & Build Pipeline

Build Compiler: 
```sh
chmod +x build.sh
bash build.sh
```

```sh
./quelang input.q output.s         # Compile ke AArch64 Assembly
aarch64-linux-gnu-as -o out.o output.s
aarch64-linux-gnu-ld -o binary out.o
```

Aktifkan debug:
```sh
./quelang --debug input.q output.s
```

---

## 🔍 Contoh Fitur Bahasa

### 🧩 Fungsi (`def`)
```quelang
def tambah(a: u16, b: u16): u16 {
    return a + b
}
```

### 🧱 Struct (`def struct`)
```quelang
def struct Pair {x: u16 y: u16}
```

### 🏷️ Alias Tipe (`init type`)
```quelang
init type Age = u16
```

### 🎯 Pointer (`*`, `&`)
```quelang
*ptr = val     # dereference & assign
&x             # ambil alamat x
```

### 🔥 Injeksi ASM (`inj`)
```quelang
inj raw {mov x0, #42\nret"}
```

### 🔁 Kontrol Alur
```quelang
if x == 1 {
    inj raw { "mov x0, #1" }
} else {
    inj raw { "mov x0, #0" }
}

while x < 5 {
    inj raw { "add x0, x0, #1" }
}
```

### 📦 Variabel & Return
```quelang
let age: u16 = 10
return age + 5
```

---

## 📁 Struktur File Proyek

| File           | Fungsi                                    |
|----------------|--------------------------------------------|
| `main.cpp`     | Pipeline: tokenizer → parser → codegen ASM |
| `linker.hpp`   | Linker statis untuk file `load "..."`      |
| `token.cpp`    | Lexer / tokenizer                          |
| `parser.cpp`   | Parser + pembentukan AST                   |
| `checker.cpp`  | Semantic checker minimal                   |
| `codegen.cpp`  | Codegen ke AArch64                         |
| `ast.hpp`      | Struktur node AST (Abstract Syntax Tree)   |
| `grammar.txt`  | Grammar formal dalam BNF                   |

---

## 🧪 Contoh Program

```quelang
def struct Pair {a: u16 b: u16}

def set(x: u16, y: u16) {
    *x = y
}

def main() {
    let a: u16 = 10
    let b: u16 = 20
    return a + b
}
```

---

## 📚 Lisensi & Kontribusi

QueLang adalah proyek open-source.  
Silakan kontribusi melalui GitHub.
Versi saat ini masih belum stabil , Untuk kedepannya akan saya perbaiki lagi :)

Versi: `v0.4 alpha`  
© 2025 QueLang Contributors — Dibuat dengan ❤️ untuk pembelajar sistem low-level.

---

<!-- 
Keywords: QueLang, que lang, bahasa pemrograman embedded, sistem minimalis, sistem ARM, AArch64, bahasa sistem baru, sistem bare-metal, compiler modern, bahasa eksplisit, riset PL, sistem programming language, toy compiler, minimalist systems language, low-level language education, bahasa untuk riset compiler, sistem operasi embedded, ARMv8 AArch64 compiler, compiler education tools
-->

Topics: compiler, aarch64, systems-programming, programming-language, toy-language, embedded, risc, arm64
