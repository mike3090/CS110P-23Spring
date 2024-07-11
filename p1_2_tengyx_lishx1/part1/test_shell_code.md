## before testing

`make`

and create the `out` folder if not exists


## p1

### simple
./riscv assembler-p1 test/part1/pass1/in/simple.s -d test/part1/pass1/out/simple.data -s test/part1/pass1/out/simple.symbol -o test/part1/pass1/out/simple.original 

### p1-error
./riscv assembler-p1 test/part1/pass1/in/p1_error.s -d test/part1/pass1/out/p1_error.data -s test/part1/pass1/out/p1_error.symbol -o test/part1/pass1/out/p1_error.original  -l test/part1/pass1/out/p1_error.log

./run-valgrind -s ./riscv assembler-p1 test/part1/pass1/in/simple.s -d test/part1/pass1/out/simple.data -s test/part1/pass1/out/simple.symbol -o simple.original 

### p2-error
./riscv assembler-p1 test/part1/pass1/in/p2_error.s -d test/part1/pass1/out/p2_error.data -s test/part1/pass1/out/p2_error.symbol -o test/part1/pass1/out/p2_error.original  -l test/part1/pass1/out/p2_error.log


## p2

### no errors; if running normally
./riscv assembler-p2 test/part1/pass2/in/simple.symbol test/part1/pass2/in/simple.original -b test/part1/pass2/out/simple.basic -t test/part1/pass2/out/simple.text

### errors;
./riscv assembler-p2 test/part1/pass2/in/p2_error.symbol test/part1/pass2/in/p2_error.original -l test/part1/pass2/out/p2_error.log

### valgrind; using "no-errors" version
./run-valgrind ./riscv assembler-p2 test/part1/pass2/in/simple.symbol test/part1/pass2/in/simple.original -b test/part1/pass2/out/simple.basic -t test/part1/pass2/out/simple.text

(The above valgrind part: see piazza may help)