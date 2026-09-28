// examples/01_hello.js
console.log("=========================================");
console.log("   Halo dari Micro-Node Runtime (Fase 1) ");
console.log("=========================================");

// 1. Eksekusi JavaScript standar
const a = 10;
const b = 32;
console.log("1. Perhitungan JS:", a, "+", b, "=", a + b);

// 2. Tipe data modern (Object, Array, Arrow Function)
const user = {
    name: "Ryan Dahl",
    created: "Node.js",
    year: 2009,
    features: ["V8", "Event Loop", "Non-blocking I/O"]
};

console.log("2. Halo,", user.name, "- pembuat", user.created, "di tahun", user.year);
console.log("   Fitur utama:", user.features.join(", "));

// 3. Testing console.warn dan console.error
console.warn("3. Peringatan: ini dicetak via stream stderr!");
console.error("4. Error simulasi: dicetak via stream stderr dengan highlight!");
