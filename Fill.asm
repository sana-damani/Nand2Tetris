// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

(INF_LOOP)

@KBD
D=M;

// if no key is pressed
@NEGATIVE
D; JEQ

// if a key is pressed, fill with black 
@color
M=-1
@FILL
0; JMP

// if no key is pressed, fill with white
(NEGATIVE)
@color
M=0

(FILL)

// initialize arr
@SCREEN
D=A
@arr
M=D

// initialize screen_size
@8192
D=A
@screen_size
M=D

// initialize i
@i
M=0

(LOOP)

// write color
@color
D=M
@arr
A=M
M=D

// increment arr
@arr
M=M+1

// increment i
@i
M=M+1

// loop
@i
D=M
@screen_size
D=M-D;
@LOOP
D;JNE

@INF_LOOP
0;JMP
