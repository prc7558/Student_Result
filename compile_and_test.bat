@echo off
echo ========================================
echo Compiling C++ Backend...
echo ========================================
echo.

g++ backend_server.cpp -o Student_Result_Management_Enhanced.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo ✓ Compilation Successful!
    echo ========================================
    echo.
    echo Testing the program...
    echo.
    
    REM Test GET_ALL command
    echo GET_ALL | Student_Result_Management_Enhanced.exe --web
    
    echo.
    echo ========================================
    echo If you see JSON output above, it works!
    echo ========================================
) else (
    echo.
    echo ========================================
    echo ✗ Compilation Failed!
    echo ========================================
    echo.
    echo Make sure you have MinGW/GCC installed
)

echo.
pause