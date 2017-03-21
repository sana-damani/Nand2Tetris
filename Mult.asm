// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

// initialize i
@i
M=0

// initialize product (R2)
@R2
M=0

(LOOP)

// break if i >= R1
@i
D=M
@R1
D=M-D;
@END
D; JLE

// accumulate product
@R0
D=M
@R2
M=M+D

// increment i
@i
M=M+1

@LOOP
0;JMP

// infinite loop to END
(END)
@END
0;JMP
