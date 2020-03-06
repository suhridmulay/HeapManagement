# HEAPS

## Description
This program provides the means to emulate heap memory management including simple allocation and deallocation of memory in C

## Process:
The process is supposed to be emulated as follows

1. Memory is an array of chars. Chars are used because of byte structure
2. Memory is structured as in real life into blocks
3. A block mask (number of bits to be used for fetching a byte in a block) is defined in code
4. The heap is to be indexed as a binary tree of spaces where two spaces can be merged into the one above should they both be free

## Implementation
Memory spaces and information regarding them is to be stored in a tree. It starts from a node containing the tree start address, size of tree and no adjacent nodes. When a block is requested, the tree is split until a block of required size is found. And the relevant block is allocated.
When freeing a particular block the requisite block status us set to free and if it has any adjacent block it is merged with it and the process continues until no further blocks can be merged

## Checklist
The following steps are necessary for the project:

- [-] Create definitions
- [-] Implement fetch mechanisms for blocks
- [-] Implement split and free mechanism for records
