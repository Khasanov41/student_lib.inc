;; File student_lib.inc for Linux OS.
;; Simple set of NASM macros for NASM assembler.
;; made by (c) Khasanov Islam (Khasanov41), SUSU student, in 2022.
;; The author hereby grant everyone the right to use this
;; file for any purpose, in any manner, in it's original or
;; modified form.
;;
;; GitHub: https://github.com/Khasanov41/student_lib.inc.git 
;;
;;							V 2.1 Beta

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Terminate the program with the specified code.
; Arguments:	%1 - exit code.

%macro EXIT 1

	%ifdef x86_64
		mov	rax, 60	; System call number (sys_exit).
		mov	rdi, %1	; Specify exit code.
		syscall		; Initialize program interrupt.

	%elifdef i386
		mov	eax, 1	; System call number (sys_exit).
		mov	ebx, %1	; Specify exit code.
		int	80h		; Initialize program interrupt.
	%else
		%error Please define architecture: x86_64, i386.
	%endif

%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Push the values of the used registers to the top of stack.
; Arguments:	%* - register(s).

%macro SAVE 1-*
	%rep %0
		push %1
		%rotate 1
	%endrep
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Pop the values of the used registers from the top of stack.
; Arguments:	%* - register(s).

%macro RESTORE 1-*
	%rep %0
		pop %1
		%rotate 1
	%endrep
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Print the string from memory area to STDOUT.
; The string must end with a null byte '\0'. 
; Arguments:	%1 - address to a memory area with the string;
;				%2 - (optional) the length of the string.

%macro PRINT 1-2

	%ifdef x86_64
		SAVE rax, rdi, rsi, rdx		
		
		mov	rsi, %1		; Specify the string address.
		mov rdx, %0		; Get the number of arguments.
		cmp rdx, 2		; Check for the presence of the second argument.
		je	%%second_is_exist	; Skip string character count. 
		mov	rdx, rsi	; Copy the string address to count its characters. 

%%lp0:	cmp [rdx], byte 0	; Is this the end of the string?
		je short %%p0		; Yes -> Fix the number of characters.
		inc rdx				; No  -> Continue counting.
		jmp short %%lp0

%%p0:	sub rdx, rsi	; The string length is the address difference.   
		jmp short %%end0

%%second_is_exist:	
		%rotate 1		; Shift arguments to the left.
		mov	rdx, %1		; The length of the string from the second argument.

%%end0:	mov	rax, 1		; System call number (sys_write).
		mov	rdi, 1		; File descriptor (stdout).
		syscall
		RESTORE rdx, rsi, rdi, rax

	%elifdef i386
		SAVE eax, ebx, ecx, edx		
		
		mov	ecx, %1		; Specify the string address.
		mov edx, %0		; Get the number of arguments.
		cmp edx, 2		; Check for the presence of the second argument.
		je	%%second_is_exist	; Skip string character count. 
		mov	edx, ecx	; Copy the string address to count its characters. 

%%lp0:	cmp [edx], byte 0	; Is this the end of the string?
		je short %%p0		; Yes -> Fix the number of characters.
		inc edx				; No  -> Continue counting.
		jmp short %%lp0

%%p0:	sub edx, ecx	; The string length is the address difference.   
		jmp short %%end0

%%second_is_exist:	
		%rotate 1		; Shift arguments to the left.
		mov	edx, %1		; The length of the string from the second argument.

%%end0:	mov	eax, 4		; System call number (sys_write).
		mov	ebx, 1		; File descriptor (stdout).
		int 80h			; Initialize program interrupt.
		RESTORE edx, ecx, ebx, eax
	%endif

%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Print immediate string to STDOUT.
; Arguments:	%1 - immediate string operand or ASCII code of character.

%macro	IPRINT 1
		jmp short %%iprint	; Escape segmentation fault.
%%str	db %1				; Put the string into the memory area. 
%%strln equ $-%%str			; Calculate the length of the input string. 

%%iprint:
		PRINT %%str, %%strln
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Read data from STDIN.
; The rax register will store the number of characters read 
; (including the null byte): rax == 0 sign EOF; rax < 0 sign ERROR.
; Arguments:	%1 - the area of memory to store the input;
;				%2 - the number of bytes to read.
%macro INPUT 2
	%ifdef x86_64
		SAVE rax, rdi, rsi, rdx

		mov rax, 0		; System call number (sys_read).
		mov rdi, 0		; File descriptor (stdin).
		mov rsi, %1		; Set the address of the memory area to be written.
		mov rdx, %2		; Set number of bytes to read.
		syscall			; Initialize program interrupt.
		mov [rsi+rax-1], byte 0	; Close the string with a null byte.
		RESTORE rdx, rsi, rdi, rax
	
	%elifdef i386
		SAVE eax, ebx, ecx, edx

		mov eax, 3		; System call number (sys_read).
		mov ebx, 0		; File descriptor (stdin).
		mov ecx, %1		; Set the address of the memory area to be written.
		mov edx, %2		; Set number of bytes to read.
		int 80h			; Initialize program interrupt.
		mov [ecx+eax-1], byte 0	; Close the string with a null byte.
		RESTORE edx, ecx, ebx, eax
	%endif

%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Convert integer to string.
; Input integer value must be stored in rax.
; Arguments:	%1 - the area of memory to store the result;

%macro ITOS 1
	%ifdef x86_64
		SAVE rcx, rsi, rdx, rbx
				
		mov rsi, %1		; Get the result memory area.
		xor rcx, rcx	; Clean the counter.
		
		cmp rax, 0		; It is a negative integer?
		jge %%lp1		; No -> Go to the converting.
		mov [rsi], byte '-' ; Yes -> Add negative sign to string and
		neg rax			; change integer sign to positive.
		inc rsi			; Skip first character in string.
	
%%lp1:	test rax, rax	; Check dividend for zero.
		jz %%p1			; Zero -> go to make the string.
		xor rdx, rdx	; Else -> go to the next digit.	
		mov rbx, 10		; Update the divisor.
		div rbx			; Divide by 10 and convert the
		add rdx, '0'	; digit to appropriate ASCII code.
		push rdx		; Push reminder to the top of stack.
		inc rcx			; Increase counter.
		jmp short %%lp1

%%p1:	jrcxz %%end1	; End converting if there's nothing to convert.
%%lp2:	pop rdx			; Else get each character from stack
		mov [rsi], rdx	; and put it to the next string area.
		inc rsi			
		loop %%lp2
		mov [rsi], byte 0	; Close string with null byte (\0)
%%end1:
		RESTORE	rbx, rdx, rsi, rcx
	%elifdef i386
		SAVE ecx, esi, edx, ebx
				
		mov esi, %1		; Get the result memory area.
		xor ecx, ecx	; Clean the counter.
		
		cmp eax, 0		; It is a negative integer?
		jge %%lp1		; No -> Go to the converting.
		mov [esi], byte '-' ; Yes -> Add negative sign to string and
		neg eax			; change integer sign to positive.
		inc esi			; Skip first character in string.
	
%%lp1:	test eax, eax	; Check dividend for zero.
		jz %%p1			; Zero -> go to make the string.
		xor edx, edx	; Else -> go to the next digit.	
		mov ebx, 10		; Update the divisor.
		div ebx			; Divide by 10 and convert the
		add edx, '0'	; digit to appropriate ASCII code.
		push edx		; Push reminder to the top of stack.
		inc ecx			; Increase counter.
		jmp short %%lp1

%%p1:	jecxz %%end1	; End converting if there's nothing to convert.
%%lp2:	pop edx			; Else get each character from stack
		mov [esi], edx	; and put it to the next string area.
		inc esi			
		loop %%lp2
		mov [esi], byte 0	; Close string with null byte (\0)
%%end1:
		RESTORE	ebx, edx, esi, ecx
	%endif
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Convert string to integer.
; Result will store in rax register.
; Arguments:	%1 - the area of memory with the input string;

%macro STOI 1	
	%ifdef x86_64
		SAVE rbx, rcx, rdx

		mov rbx, 1		; Sign flag.
		xor rax,rax		; Clean initial area.
		mov rdx, %1		; Get source string address.
	
		movzx rcx, byte [rdx]
		cmp rcx, '-'	; Is it a negative sign?
		jne	%%lp3		; No  -> Skip setting flag. 
		mov rbx, -1		; Yes -> Set a sign flag and
		inc rdx			; skip first character in the string.

%%lp3:	movzx rcx, byte [rdx]   ; Get the current character.
		test rcx, rcx   ; Is it the end of the string?
		je %%end3		; Yes -> Finish converting.
		sub rcx, '0'	; No  -> Convert ASCII code to decimal.
		imul rax, 10    ; Multiply total by 10.
		add rax, rcx    ; Add current digit to total.
		inc rdx         ; Get the address of the next character.
		jmp %%lp3

%%end3:	imul rax, rbx	; Set the sign.
		
		RESTORE rdx, rcx, rbx
	%elifdef i386
		SAVE ebx, ecx, edx

		mov ebx, 1		; Sign flag.
		xor eax,eax		; Clean initial area.
		mov edx, %1		; Get source string address.
	
		movzx ecx, byte [edx]
		cmp ecx, '-'	; Is it a negative sign?
		jne	%%lp3		; No  -> Skip setting flag. 
		mov ebx, -1		; Yes -> Set a sign flag and
		inc edx			; skip first character in the string.

%%lp3:	movzx ecx, byte [edx]   ; Get the current character.
		test ecx, ecx   ; Is it the end of the string?
		je %%end3		; Yes -> Finish converting.
		sub ecx, '0'	; No  -> Convert ASCII code to decimal.
		imul eax, 10    ; Multiply total by 10.
		add eax, ecx    ; Add current digit to total.
		inc edx         ; Get the address of the next character.
		jmp %%lp3

%%end3:	imul eax, ebx	; Set the sign.
		RESTORE edx, ecx, ebx
	%endif
%endmacro
;; Use string commands to optimize code.
;; Write system call macro
;; Adaptive registers
;; Memory usage optimizations
