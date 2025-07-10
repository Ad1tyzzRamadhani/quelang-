---

### ✅ **2. Versi HTML + CSS (misal untuk website, Notion, atau GitHub Pages)**

```html
<!DOCTYPE html>
<html lang="id">
<head>
  <meta charset="UTF-8">
  <title>QueLang — A Minimalist Systems Language</title>
  <style>
    body { font-family: 'Segoe UI', sans-serif; max-width: 800px; margin: auto; padding: 2rem; line-height: 1.6; }
    h1, h2, h3 { color: #2c3e50; }
    code { background: #f4f4f4; padding: 2px 5px; border-radius: 3px; }
    pre { background: #272822; color: #f8f8f2; padding: 1em; overflow-x: auto; border-radius: 6px; }
    table { width: 100%; border-collapse: collapse; margin: 1em 0; }
    th, td { border: 1px solid #ccc; padding: 0.5em; text-align: left; }
    .highlight { background: #fffae6; padding: 0.5em; border-left: 4px solid #f39c12; }
  </style>
</head>
<body>

<h1>🌱 QueLang</h1>
<h3>A Minimalist Systems Language</h3>

<p class="highlight">Bahasa pemrograman baru yang dirancang untuk kesederhanaan, eksplisit, dan dapat dipelajari dalam hitungan menit.</p>

<h2>✨ Mengapa Memilih QueLang?</h2>
<ul>
  <li>🧑‍🎓 Cocok untuk pelajar & pembelajar sistem</li>
  <li>⚙️ Pengembang bare-metal atau sistem ARM</li>
  <li>👩‍🔬 Eksperimen compiler & bahasa</li>
</ul>

<h3>✅ Kelebihan</h3>
<ul>
  <li>Bahasa ringan, eksplisit, dan modular</li>
  <li>Mudah dipahami</li>
</ul>

<h3>⚠️ Kekurangan</h3>
<ul>
  <li>Hanya untuk AArch64</li>
  <li>Belum ada memory safety</li>
</ul>

<h2>🛠️ Kompilasi</h2>
<pre><code>./quelang input.q output.s
aarch64-linux-gnu-as -o out.o output.s
aarch64-linux-gnu-ld -o binary out.o</code></pre>

<h2>🔍 Fitur Bahasa</h2>
<table>
  <tr><th>Fitur</th><th>Contoh</th></tr>
  <tr><td><code>def</code></td><td><code>def tambah(a: u16, b: u16): u16 { return a + b }</code></td></tr>
  <tr><td><code>def struct</code></td><td><code>def struct Pair { x: u16 y: u16 }</code></td></tr>
  <tr><td><code>init type</code></td><td><code>init type Age = u16</code></td></tr>
  <tr><td>Pointer</td><td><code>*x = val</code>, <code>&x</code></td></tr>
  <tr><td><code>inj</code></td><td><code>inj raw { "mov x0, #42" }</code></td></tr>
</table>

<h2>🧪 Contoh Program</h2>
<pre><code>def struct Pair {
    a: u16
    b: u16
}

def set(x: *u16, y: u16) {
    *x = y
}

def main() {
    let a: u16 = 10
    let b: u16 = 20
    return a + b
}</code></pre>

<h2>📦 Struktur File</h2>
<ul>
  <li><b>main.cpp</b> - Pipeline utama</li>
  <li><b>token.cpp</b> - Tokenizer</li>
  <li><b>parser.cpp</b> - Parser + AST</li>
  <li><b>codegen.cpp</b> - Generator AArch64</li>
  <li><b>ast.hpp</b> - AST Node</li>
</ul>

<h2>📚 Lisensi & Kontribusi</h2>
<p>QueLang adalah proyek open-source. Versi: <code>v0.1 alpha</code></p>
<p>© 2025 QueLang Contributors</p>

</body>
</html>
