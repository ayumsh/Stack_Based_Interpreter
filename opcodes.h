#define OPCODES_COUNT 12
void (*handlers[OPCODES_COUNT])(void) = {
	nop,
	push,
	pop,
	print,
	add,
	sub,
	mult,
	divide,
	quit,
	swap,
	inc,
	dec,
};
