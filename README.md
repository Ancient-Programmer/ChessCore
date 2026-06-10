# ChessCore

**ChessCore** is a modular C++ chess core library designed for chess board representation, move generation, debugging, testing, hashing, and future GUI or engine integration.

The project is built around a clean architecture, efficient bitboard-based logic, reusable chess modules, and readable C++ code. It can be used as the backend for a Qt chess application, command-line chess tool, testing environment, or future full chess engine.

---

## Features

- Bitboard-based board representation
- Precomputed attack tables
- Legal and pseudo-legal move generation
- Chess move validation utilities
- Zobrist hashing support
- Debugging and logging utilities
- Basic chess clock support
- Test utilities for validation and development
- UCI-style helper utilities
- Modular structure for easy maintenance and expansion
- Designed for GUI or CLI integration
- Suitable for future chess engine development

---

## Project Structure

```text
ChessCore/
│
├── attackTables/
│   └── Precomputed attack masks for chess pieces
│
├── bitboard/
│   └── Core board representation using bitboards
│
├── bitutils/
│   └── Low-level bit manipulation helpers
│
├── ChessClock/
│   └── Chess timer and clock-related logic
│
├── ChessDebug/
│   └── Debugging, logging, and diagnostic tools
│
├── ChessTest/
│   └── Test utilities and validation code
│
├── ChessTypes/
│   └── Core enums, structs, move types, piece types, and board constants
│
├── MoveGeneration/
│   └── Move generation logic for legal and pseudo-legal moves
│
├── UciUtils/
│   └── Utilities for UCI-compatible formatting and parsing
│
├── Zobrist/
│   └── Zobrist hashing for board state identification
│
└── README.md
