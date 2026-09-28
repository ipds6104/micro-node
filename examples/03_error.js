// examples/03_error.js
function a() {
    b();
}
function b() {
    c();
}
function c() {
    throw new Error("Oops! Terjadi kesalahan fatal di fungsi c().");
}

console.log("Memulai eksekusi...");
a();
console.log("Baris ini tidak akan pernah tercapai.");
