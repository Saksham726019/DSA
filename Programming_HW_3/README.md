# Spell Checker Program

## Overview
The Spell Checker Program is designed to identify misspelled words in a given input and provide suggestions for corrections. 

### Data Structure Used
- **Hash Table**: The program utilizes a hash table to store valid words and employs various algorithms to detect common typographical errors. 

### Error Checks Implemented
The program implements three main error checks:
- **Inverted Adjacent Letters**: Detects and suggests corrections for letters that are inverted.
- **Missing Letters**: Identifies words with missing letters at the start or end and suggests possible completions.
- **Extra Letters**: Recognizes and suggests corrections for words with extra letters at the start or end.

## Requirements
- C Compiler (e.g., GCC)
- Standard C Library

## Compile the program:
gcc -o spell_checker main.c

## Run/Execute the program
./spell_checker words.txt test.txt <argument>

argument: add or ignore

add: if there is a mispelled word in the test.txt, then the program will detect it, suggest the correct words, but also, it will add that misspelled word into the dictionary.
ignore: if there is a mispelled word in the test.txt, then the program will detect it, suggest the correct words, but will not add it to the dictionary.
