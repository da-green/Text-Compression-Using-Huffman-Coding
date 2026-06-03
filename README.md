# Huffman Coding (C Implementation)

This project is a from-scratch implementation of **Huffman Coding**, a lossless data compression algorithm. It builds a frequency-based binary tree and uses it to encode and decode text efficiently.

---

## Features

- Builds frequency map for characters (`a-z` + space)
- Constructs a **min-heap** manually (no library priority queue)
- Builds Huffman tree from frequencies
- Generates Huffman codes via tree traversal
- Encodes text into a compact bitstream
- Decodes compressed binary back into original text
- Writes compressed output to a binary file (`compressed.bin`)

---

## Overview

Huffman Coding assigns:
- **shorter bit codes** to frequent characters
- **longer bit codes** to rare characters

This reduces overall storage size while preserving all information (lossless compression).

---

## Algorithm Steps

1. Count frequency of each character in input text
2. Build a min-heap of nodes based on frequency
3. Repeatedly merge two smallest nodes to build Huffman tree
4. Traverse tree to generate binary codes for each character
5. Encode input text into a bitstream using those codes
6. Store encoded data in a binary file
7. Decode by traversing the Huffman tree using bits

---

## Time Complexity

Let:
- `n` = length of input text  
- `k` = number of unique characters (here ≤ 27)

Then:

- Frequency counting: **O(n)**
- Heap construction: **O(k)**
- Building Huffman tree: **O(k log k)**
- Encoding: **O(n)**
- Decoding: **O(n)**

Overall: **O(n + k log k)**

---

## Character Handling (Simplified Model)

This implementation is intentionally simplified for educational purposes.

To reduce complexity and focus on the Huffman Coding algorithm itself, the input character set is restricted:

- Only lowercase alphabetic characters (a–z) are preserved
- All non-alphabetic characters (including punctuation, digits, and symbols) are converted to whitespace (`' '`)

This means multiple different characters (e.g., `,`, `.`, `!`, digits) are treated as a single unified symbol during frequency counting and encoding.

This design choice simplifies:
- The frequency map
- The Huffman tree construction
- The overall encoding/decoding logic

but does not represent a full general-purpose text compression system.

## Author

Implemented by Daniel Green

For learning data compression and low-level algorithm implementation.

## References
- Inspired by course materials and lecture content from **Prof. Arup Guha**
  - [Notes](https://www.cs.ucf.edu/~dmarino/ucf/cop3503/lectures/HuffmanCoding.pdf)

## File Output

- `compressed.bin` → binary encoded output containing:
  - original length of text
  - compressed bitstream

---

## How to Run

Compile:

```bash
gcc huffman.c -o huffman
