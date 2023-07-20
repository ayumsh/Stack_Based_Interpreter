#include <stdio.h>
#include <stdlib.h>

#define MEM_SIZE 4096        // bytecode loads in memory at 0 address, and 0 address takes control
#define ST_SIZE 2048         // separated stack to make calculations

#define COMMAND_LEN 2

int mem[MEM_SIZE];
int c_cur;                  // code cursor.
int stack[ST_SIZE];         // real size in bytes is ST_SIZE * sizeof(int)
int st_cur;                 // stack cursor. x86 architecture don't have a similar concept
long long int tacts_counter;

void die(char* message)
{
    printf("Error: %s at 0x%x, exiting\n", message, c_cur*COMMAND_LEN);
    exit(1);
}

void inc_c_cur()
{
    if (c_cur + 1 >= MEM_SIZE)
        die("unexpected end of code");
    c_cur++;
}

void inc_st_cur()
{
    if (st_cur == ST_SIZE - 1)
        die("stack overflow");
    st_cur++;
}

void dec_st_cur()
{
    if (st_cur == -1)
        die("pop from empty stack");
    st_cur--;
}

void push()
{
    inc_c_cur();
    inc_st_cur();
    stack[st_cur] = mem[c_cur];
}

void pop()
{
    dec_st_cur();
}

void print()
{
    if (st_cur == -1)
        printf("Stack is empty, continue...\n");
    else
        printf("stack top Value: %d \n", stack[st_cur]);
}

void add()
{
    if (st_cur < 1)
        die("not enough elements in stack for addition");
    stack[st_cur - 1] = stack[st_cur - 1] + stack[st_cur];
    dec_st_cur();
}

void sub()
{
    if (st_cur < 1)
        die("not enough elements in stack for subtraction");
    stack[st_cur - 1] = stack[st_cur - 1] - stack[st_cur];
    dec_st_cur();
}

void mult()
{

    if (st_cur < 1)
        die("not enough elements in stack for mult");
    stack[st_cur - 1] = stack[st_cur - 1] * stack[st_cur];
    dec_st_cur();
}

void divide()
{

    if (st_cur < 1)
        die("not enough elements in stack for divide");
    stack[st_cur - 1] = stack[st_cur - 1] / stack[st_cur];
    dec_st_cur();
}

void succ_exit()
{
    printf("End of program, success\nNumber of instruction: %I64i \n",tacts_counter);
    exit(0);
}

void swap()
{
    if (st_cur < 1)
        die("not enough elements in stack to swap");
    int t = stack[st_cur];
    stack[st_cur] = stack[st_cur - 1];
    stack[st_cur - 1] = t;
}

void inc()
{
    if (st_cur < 0)
        die("cannot inc because stack is empty");
    stack[st_cur]++;
}

void dec()
{
    if (st_cur < 0)
        die("cannot dec because stack is empty");
    stack[st_cur]--;
}

void nop()
{
    ;
}

void quit()
{
    succ_exit();
}

#include "opcodes.h"
int main(int argc, char ** argv)
{
    FILE * bfile;
    if (argc < 2)
        die("bytecode file not specified");
    printf("Bytecode file is: %s\n", argv[1]);
    bfile = fopen(argv[1], "r");
    if (bfile == NULL)
        die("cannot open bytecode file");
    fread(mem, COMMAND_LEN, MEM_SIZE, bfile);
    if (!feof(bfile))
        die("bytecode file too big");
    st_cur = -1;
    c_cur = 0;
    tacts_counter = 0;
    while (1)
    {
        tacts_counter++;
        int instruction = mem[c_cur];
        if (instruction < 0 || instruction >= OPCODES_COUNT)
            die("Unknown instruction");
        handlers[instruction]();
        inc_c_cur();
    }
}
