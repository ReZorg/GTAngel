@echo off
setlocal
cd /d "%~dp0"
set "PATH=C:\msys64\mingw64\bin;%PATH%"
node generate_v56.mjs || exit /b 1
node emit_v56_components.mjs || exit /b 1
node emit_v56_src_tree.mjs || exit /b 1
g++.exe -std=c++17 -O2 -static -static-libgcc -static-libstdc++ -mwindows -Isrc -I. -Igneural-net\include -I..\gneural-net\include src\main.cpp gnn_obj\*.o -o gtangel_agi_v56.exe -lwinhttp -lgdi32 -luser32 -lshell32 -lole32 -lcomdlg32 || exit /b 1
g++.exe -std=c++17 -O2 -static -static-libgcc -static-libstdc++ -Igneural-net\include -I..\gneural-net\include verify_v56_learning.cpp gnn_obj\*.o -o verify_v56_learning.exe -lgdi32 -luser32 || exit /b 1
.\verify_v56_learning.exe || exit /b 1
.\gtangel_agi_v56.exe --selftest || exit /b 1
node -e "const fs=require('fs');const p='V27_RUNTIME_VERIFICATION.json';const r=JSON.parse(fs.readFileSync(p,'utf8'));if(fs.statSync(p).mtimeMs<fs.statSync('gtangel_agi_v56.exe').mtimeMs||r.checks<=0||r.passed!==r.checks||r.nn_training_steps<=0) {console.error('Runtime verification failed',r);process.exit(1);}console.log('Runtime verification: '+r.passed+'/'+r.checks);" || exit /b 1
echo GTAngel v56 build and DTE learning tests passed.