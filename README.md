<div align="center">
  <img width="1440" height="1080" alt="Image" src="https://github.com/user-attachments/assets/d59c1b0d-55ff-4926-8dce-ed4985188ae5" />
  <h1>🌱 QueLang — A Minimalist Systems Language</h1>
  <p><em>Bahasa sistem baru yang eksplisit, minimalis, dan mendekati perangkat keras.</em></p>
  <p>
    <strong>Sintaks ringan</strong> · 
    <strong>Kompilasi ke CRU ( Cuick Representation Utils )</strong> · 
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
| Modular                              | Belum mendukung x86/x64              |
| Mudah dipelajari & disusun           | Belum ada tooling modern (pkg mgr)   |

---

## 🛠️ Kompilasi & Build Pipeline

Build Compiler: 
```sh
chmod +x build.sh
bash build.sh
```

```sh
Dalam Pengembangan*
```

Aktifkan debug:
```sh
Dalam Pengembangan*
```

---

## 🔍 Contoh Fitur Bahasa

### 🧩 Fungsi (`def`)
```quelang
int tambah(int a, int b) {
    return a + b
}
```

### 🧱 Struct (`def struct`)
```quelang
struct Vector3 {
    int x,
    int y,
    int z,
    static struct Vector3 new() {
        return struct Vector3{0,0,0}
    }
```

### 🏷️ Alias Tipe (`init type`)
```quelang
@def Age = u16
```

### 🎯 Pointer (`*`, `&`)
```quelang
*ptr = val     # dereference & assign
&x             # ambil alamat x
```

### 🔥 Injeksi ASM (`inj`)
```quelang
inj("mov x0, #42\nret")
```

### 🔁 Kontrol Alur
```quelang
if x == 1 {
    inj("mov x0, #1")
} else {
    inj("mov x0, #0")
}

while x < 5 {
    inj("add x0, x0, #1")
}
```

### 📦 Variabel & Return
```quelang
int age = 10
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
| `codegen.cpp`  | Codegen ke CRU01                           |
| `ast.hpp`      | Struktur node AST (Abstract Syntax Tree)   |
| `grammar.txt`  | Grammar formal dalam BNF                   |

---

## 🧪 Contoh Program

```quelang
struct Vector3 {
    int x,
    int y,
    int z,
    static struct Vector3 new() {
        return struct Vector3{0,0,0}
    }
}

struct Player {
    string name,
    int id,
    struct Vector3 position,
    static struct Player new(struct Player self) {
        pos = self.position.new()
        return struct Player{"",0,pos}
    }
}

struct Player NewPlayer(int id, string name) {
    struct Player player = struct Player.new(struct Player{name,id,struct Vector3.new()})
}

int main() {
    struct Player p1 = NewPlayer(1,"player1)
    struct Player p2 = NewPlayer(2,"player2")
    return 0
}
        
        
```

---

## 📚 Lisensi & Kontribusi

QueLang adalah proyek open-source.  
Silakan kontribusi melalui GitHub.
Versi saat ini masih eksperimental, Masih saya Kembangkan :)

Versi: `v0.4 alpha`  
© 2025 QueLang Contributors — Dibuat dengan ❤️ untuk pembelajar sistem low-level.

---

<!-- 
Keywords: QueLang, que lang, bahasa pemrograman embedded, sistem minimalis, sistem ARM, AArch64, bahasa sistem baru, sistem bare-metal, compiler modern, bahasa eksplisit, riset PL, sistem programming language, toy compiler, minimalist systems language, low-level language education, bahasa untuk riset compiler, sistem operasi embedded, ARMv8 AArch64 compiler, compiler education tools
-->

Topics: compiler, aarch64, systems-programming, programming-language, toy-language, embedded, risc, arm64
