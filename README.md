# student_lib.inc
#### Simple macros for NASM assembler.
> Warning: These macros are written solely for the purpose of learning 
about the capabilities of the NASM assembler. They do not contain any 
safety measures. For example, a macro does not validate input, so the 
result may be unpredictable. 


Supported architecture:
* x86_64 Linux

## Usage
Place this macro in the same directory as your .asm files.
To import it into the project, type `%include "student_lib.inc"` before calling the macro. 

**PRINT** - Print the string from memory area to STDOUT.
```
section .data			; Data section for initialized variables.
msg:	db "Hello, world!", 10, 0	; Null ended string for printing.

section .text			; Code section.
_start:	PRINT msg		; Print the entire message.
	PRINT msg, 5		; Print the first five bytes.

OUTPUT:
Hello, world!
Hello
```

**IPRINT** - Print immediate string to STDOUT.
> The difference from the `PRINT` is that `IPRINT` does 
not need to reserve memory, but the macro operands must be immediate, 
i.e. known at the stage of macroprocessing. 
```
section .text			; Code section.
_start:	IPRINT "Hello, world!"	; Print massage.
	IPRINT 10		; Print '\n' ("line feed" ASCII code).

OUTPUT:
Hello, world!

```

**INPUT** - Read data from STDIN.
```
section .bss			; Section of uninitialized data.
buf		resb 256	; Buffer for input data.

section .data			; Data section for initialized variables.
buf_size equ 256		; Input buffer size.

section .text			; Code section.
_start: INPUT buf, buf_size	; Place the entered data at buf address,
				; where three is the maximum
				; number of bytes to be read. 
```

**STOI** - Convert string to integer.
```
section .data			; Data section for initialized variables.
buf		db "55", 0	; Create address to "55" string.
section .text
_start: STOI buf		; Convert "55" string to 55 integer
				; and place it to rax register.
	mov [buf], rax		; Copy 55 to reserved memory area
	PRINT buf, 1		; Print 55 ASCII code == "7"

OUTPUT:
7
```

**ITOS** - Convert integer to string.
```
section .bss			; Section of uninitialized data.
buf		resb 256	; Buffer for input data.

section .text			; Code section.
_start: mov rax, -100		; Place -100 integer to rax register
	ITOS buf		; Convert -100 to "-100"
				; and plase it to reserved area.
	PRINT buf		; Print "-100".

OUTPUT:
-100
```

**SAVE** - Push the values of the used registers to the top of stack.
```
SAVE rax, rdx, rsi
```
is equal to
```
push rax
push rdx
push rsi
```

**RESTORE** - Pop the values of the used registers from the top of stack.
```
RESTORE rax, rdx, rsi
```
is equal to
```
pop rax
pop rdx
pop rsi
```
