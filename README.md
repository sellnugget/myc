Hi i wrote this compiler in my spare time. 

this includes many of the features of the c lanaguge such as functions loops ints and pointers.
main goal was to learn how to lexers parsing and just how compilers work as well as learning some x86 asembly language

you can create arrays by defining int malloc(int amount) then just call the function and making a int equal to it.

int ptr = malloc(1000);

then just dereference it *(ptr + 0) = ptr[0], *(ptr + 4) = ptr[1]. although the dereferce is quite broken and i have to fix it.


other then that almost everything you'd expect works. binary operations, + / - * % & | > < etc... are supported. 


how to run.

1. first run the program (must be compiled) 
2. then paste your source file directory. it will create a .s file with the same name.
3. then compile the .s asembly file with gcc using gcc -m32 <filename.s> -o <your desired output name>
