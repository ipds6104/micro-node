// examples/02_process.js
console.log("=========================================");
console.log("   Inspeksi Global Objek 'process'       ");
console.log("=========================================");

console.log("Node Version :", process.version);
console.log("Platform     :", process.platform);
console.log("Architecture :", process.arch);
console.log("Process PID  :", process.pid);
console.log("Working Dir  :", process.cwd());
console.log("Argumen CLI  :", JSON.stringify(process.argv));

console.log("\nSimulasi selesai dengan sukses.");
