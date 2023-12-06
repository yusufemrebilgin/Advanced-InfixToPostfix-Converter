# Advanced Infix to Postfix Converter

This C program converts infix expressions into postfix expressions and evaluates the converted postfix expressions. It supports floating-point numbers and does not evaluate negative numbers.

## Overview

The program utilizes a generic stack structure for handling both characters and double precision floating-point numbers. It includes functions for converting infix expressions to postfix, evaluating postfix expressions, and validating the input expressions.

## Features

- Conversion of infix expressions to postfix
- Evaluation of postfix expressions with support for floating-point numbers
- Input validation for proper infix expressions
- Dynamic memory allocation for the stack structure
- Cross-platform compatibility for clearing the console screen

## How to Use

1. Run the program.
2. Enter a valid infix expression when prompted.
3. The program will convert the infix expression to postfix and display the result.
4. Optionally, enter a new expression or terminate the program.

## Input Validation

The program validates the entered infix expression for proper syntax, ensuring correct placement of parentheses, operators, and operands. It also checks for matching parentheses.

## Build and Run

### Build

To build the program, you can use a C compiler such as `gcc`:

```bash
gcc InfixToPostfixApplication.c -o InfixToPostfixApplication
```

### Run

After building the program, you can run it:

```bash
./InfixToPostfixApplication
```


