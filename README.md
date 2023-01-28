# Polynomial-Solver
 
This is my first C++ Project. It is a polynomial solver. It is capable of handling any polynomial of the form ax^4 + bx^3 + cx^2 + dx + e where coefficients a - d are values between 0 and 9 and constant e is a value between 0 and 1000. The program can handle the omission of terms beginning with 0. Coefficients of 1 do not need to be explicit. Minus signs (-) can be used in place of plus signs, but not alongside, and can also proceed coefficient a. Exponents should be 0 - 4. The program can handle and seems to derive sequences accurately for exponents 5 - 9 but this behaviour is outside the scope of the program and should not be relied upon. If handed a sequence of numbers from which it must derive a sequence it will assume the sequence starts at 0, if this does not work it will make a best guess and if that still doesn't work it will shift along 10 numbers in either direciton of the best guess. If it still fails to reach an answer it will tell the user that, though it doesn't tell the user if that result is because an answer doesn't exist within the bounds of if there simply is no answer.

## How to Use

Option 0: ./Polynomial.exe 0 "function in quotes or speech marks" start_num end_num out_filename
This generates a sequence from an input function, displays to user then writes to file

Option 1: ./Polynomial.exe 1 in_filename
This reads in a sequence and derives an equation

Option 2: ./Polynomial.exe 2 in_filename out_filename
This option 2 reads in several sequences, derives their equations and writes them to an output file

## Built With

Visual Studio 2019
