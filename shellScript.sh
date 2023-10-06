# Compile your C code (if not already compiled)
gcc myio.c test.c -o test
# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful!"
else
    echo "Compilation failed. Check for errors in your code."
    exit 1
fi

# Run the test program
./test

# Cleanup - Remove the compiled executable and object files
rm -f test *.o 