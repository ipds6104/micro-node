# Micro-Node 🚀
> Membangun Runtime JavaScript dari Nol (*Start from Scratch*) untuk Menelusuri Sejarah & Arsitektur Asli Ryan Dahl (2009).

Repositori ini adalah proyek edukasi reka ulang (*reverse-engineering & reconstruction*) untuk menjawab satu pertanyaan mendasar:
**"Ketika Ryan Dahl menggunakan V8 dan libuv, apa sebenarnya yang dia koding?"**

---

## 📖 Latar Belakang: Masalah yang Dihadapi Ryan Dahl (2009)

Pada tahun 2008–2009, Ryan Dahl mengamati fitur **upload progress bar** di Flickr. Ia menyadari web server konvensional saat itu (seperti Apache HTTP Server dengan model *thread-per-connection*) sangat tidak efisien dalam menangani koneksi lambat (*slow connections*):
1. **Thread Blocking**: Setiap koneksi yang lambat memblokir satu thread sistem operasi.
2. **C10K Problem**: Ribuan koneksi bersamaan menghabiskan memori RAM secara masif (2–8 MB per thread) dan membebani CPU karena *thread context switching*.
3. **Pustaka Sinkron di Bahasa Lain**: Di Python (Twisted) atau Ruby (EventMachine), mayoritas library ekosistem ditulis secara sinkron/blocking (`file.read()`, query database). Sekali developer memanggil fungsi blocking, seluruh event loop lumpuh.

**Pilihan ke JavaScript:**
Google merilis V8 (2008) yang sangat cepat. Selain itu, **JavaScript di browser tidak memiliki API I/O bawaan** sama sekali (tidak ada file system API atau network socket API). JavaScript adalah kanvas bersih di mana Ryan Dahl bisa mendesain lingkungan baru yang **100% I/O-nya non-blocking secara default**.

---

## 🧩 Anatomi Runtime: Apa yang Sebenarnya Dikoding?

Banyak orang mengira V8 atau Engine JS sudah bisa melakukan segalanya. Faktanya:
- **V8 / JS Engine**: Hanya parser dan compiler matematika/logika (`1 + 1`, `function() {}`, `Array`, `Object`). Engine **TIDAK TAHU** apa itu file di disk, terminal output, atau kartu jaringan.
- **Sistem Operasi (Linux Kernel)**: Memiliki syscall (`open`, `read`, `socket`, `bind`, `epoll`), tetapi hanya mengerti kode native C/mesin.
- **Event Loop (`libuv`)**: Mengelola antrian event I/O non-blocking.

**Yang dikoding Ryan Dahl adalah lapisan lem (*glue layer*) C++ dan modul JavaScript:**
1. **Host Executable**: Inisialisasi engine dan loop.
2. **C++ Bindings (Jembatan)**: Mengekspos kemampuan sistem operasi (file descriptor, socket) ke dalam objek global JavaScript.
3. **Event Loop Marshalling**: Menjembatani callback JS saat event I/O pada kernel Linux selesai.
4. **Standard Library JS (`fs.js`, `net.js`, `http.js`)**: Membungkus binding C++ mentah menjadi API yang elegan.

```
   ┌─────────────────────────────────────────────────────────┐
   │                  KODE JAVASCRIPT USER                   │
   │      console.log(...) / fs.readFile(...)                │
   └────────────────────────────┬────────────────────────────┘
                                │
   ═════════════════════════════╪═════════════════════════════
   YANG DIBANGUN DI PROYEK INI (MICRO-NODE CORE):
   ───────────────────────────────────────────────────────────
   1. JS Standard Library (lib/*.js)
   ───────────────────────────────────────────────────────────
   2. C Bindings Layer (src/bindings/*.c)
      - Mengekspos console.log, process, setTimeout, fs, socket
      - Menerjemahkan tipe data C <-> JavaScript Value
   ───────────────────────────────────────────────────────────
   3. Host Program & Event Loop Integration (src/main.c)
      - Inisialisasi JS Engine Runtime & Context
      - Integrasi Loop Event (libuv)
   ═════════════════════════════╪═════════════════════════════
                                │
   ┌────────────────────────────┴────────────────────────────┐
   │         JS Engine         │      libuv / OS Kernel      │
   │  (QuickJS / V8 Standar)   │   (epoll, socket, timers)   │
   └───────────────────────────┴─────────────────────────────┘
```

---

## 🗺️ Peta Pembangunan (*Phase-by-Phase Roadmap*)

- [x] **Fase 1: The JS Embedder & Native Console Binding**
  - Menginisialisasi JS Engine di dalam C.
  - Membaca file JavaScript dan mengevaluasinya.
  - Menginjeksi fungsi native C pertama: `console.log`, `console.warn`, `console.error`.
  - Menginjeksi objek `process` (`process.version`, `process.platform`, `process.argv`, `process.cwd()`, `process.exit()`).
  - Menangkap *uncaught exception* dan mencetak stack trace.
- [ ] **Fase 2: The Non-Blocking Timer & Event Loop (`setTimeout`)**
  - Menghubungkan engine JS dengan Event Loop `libuv`.
  - Mendaftarkan timer callback ke C tanpa memblokir thread utama.
- [ ] **Fase 3: Asynchronous File I/O (`fs.readFile`)**
  - Membaca file di thread pool latar belakang dan memicu callback JS setelah data siap.
- [ ] **Fase 4: Non-Blocking Network Socket & Web Server (`http.createServer`)**
  - Membuka TCP socket server dan melayani request HTTP secara non-blocking murni dalam 1 thread.
- [ ] **Fase 5: Modular Standard Library**
  - Memisahkan library internal JS dari user code (`lib/fs.js`, dll.).

---

## 🚀 Memulai (Fase 1)

### Prasyarat
- Kompiler C (`gcc` atau `clang`)
- `make`
- `libquickjs` & `libuv1-dev`

### Kompilasi
```bash
make
```

### Menjalankan Contoh
1. **Hello World & Console**:
   ```bash
   ./bin/micro-node examples/01_hello.js
   ```

2. **Inspeksi Objek `process`**:
   ```bash
   ./bin/micro-node examples/02_process.js hello world 123
   ```

3. **Demonstrasi Error & Stack Trace**:
   ```bash
   ./bin/micro-node examples/03_error.js
   ```

4. **Menjalankan Seluruh Test Otomatis**:
   ```bash
   make test
   ```

---

## 🔬 Kode Kunci Fase 1: Bagaimana C Menginjeksi `console.log`?

Di file `src/bindings/console.c`, kita mendefinisikan fungsi dalam bahasa C:
```c
static JSValue js_console_log(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    for (int i = 0; i < argc; i++) {
        const char *str = JS_ToCString(ctx, argv[i]);
        printf("%s%s", i > 0 ? " " : "", str);
        JS_FreeCString(ctx, str);
    }
    printf("\n");
    return JS_UNDEFINED;
}
```

Lalu kita daftarkan ke JavaScript Engine:
```c
JSValue global = JS_GetGlobalObject(ctx);
JSValue console = JS_NewObject(ctx);

// Pasang fungsi C js_console_log sebagai properti "log" pada objek console
JS_SetPropertyStr(ctx, console, "log", JS_NewCFunction(ctx, js_console_log, "log", 1));

// Pasang objek console ke global scope JavaScript
JS_SetPropertyStr(ctx, global, "console", console);
```

Dengan beberapa baris ini, kode JavaScript apa pun yang dijalankan oleh runtime kita sekarang bisa memanggil `console.log("Halo!")`!
