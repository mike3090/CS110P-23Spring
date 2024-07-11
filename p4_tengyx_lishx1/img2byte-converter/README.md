Use the gcc compiler to compile the `converter.c`, with option `-lm`. Then you will get a executable, e.g. on Windows, it will be `a.exe`.

Then we could 
```
./a.exe <the path of your image>
```
and the program would transform your program into an array of pixels. The result would be stored in a new file `result.h`.s

Thanks @billhu0 for his proj4, 22Spring, which provides me this converter and saves me tons of time.