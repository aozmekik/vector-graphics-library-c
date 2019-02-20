target:	test_program

test_program:	test_program.c epslib.h epslib.c 
		gcc test_program.c epslib.c -lm -o test_program

