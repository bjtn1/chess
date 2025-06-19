# Chess Engine in C

This is a simple chess engine implementation written in C, featuring bitboard representation, move generation, and basic game state management.

<!-- TOC start (generated with https://github.com/derlin/bitdowntoc) -->

* [Acknowledgements](#ack)
* [Features](#features)
* [Board Representation](#board-representation)
* [Move Generation](#move-generation)
* [Usage](#usage)
    * [Sample FEN Strings](#sample-fen-strings)
* [Building and Running](#building-and-running)
* [Implementation Details](#implementation-details)
* [Limitations](#limitations)

<!-- TOC end -->


<!-- TOC --><a name="ack"></a>
## Acknowledgements
Thank you to [chessprogramming591](https://www.youtube.com/@chessprogramming591) for the help with this

<!-- TOC --><a name="features"></a>
## Features

- Bitboard-based board representation
- FEN string parsing for game state initialization
- Move generation for all piece types
- Legal move validation
- Castling rights tracking
- En passant capture handling
- Pawn promotion
- Attack detection and checking

<!-- TOC --><a name="board-representation"></a>
## Board Representation

The engine uses bitboards (64-bit integers) to represent the position of each piece type. This allows for efficient move generation and attack detection using bitwise operations.

<!-- TOC --><a name="move-generation"></a>
## Move Generation

The engine generates all legal moves for the current position, including:
- Pawn moves (single push, double push, captures, en passant, promotion)
- Knight moves
- Bishop moves
- Rook moves
- Queen moves
- King moves (including castling)

<!-- TOC --><a name="usage"></a>
## Usage

The main program initializes the engine with a sample FEN string and demonstrates move generation and execution.

<!-- TOC --><a name="sample-fen-strings"></a>
### Sample FEN Strings

Several test positions are predefined:
- `empty_board`: Empty board
- `start_position`: Standard chess starting position
- `tricky_position`: A complex test position
- `killer_position`: Another test position
- `cmk_position`: Yet another test position

<!-- TOC --><a name="building-and-running"></a>
## Building and Running

To compile and run the engine:
```bash
gcc chess_engine.c -o chess_engine
./chess_engine
```

<!-- TOC --><a name="implementation-details"></a>
## Implementation Details
- Uses magic bitboards for sliding piece attack generation
- Implements efficient bit manipulation macros for board operations
- Tracks castling rights and en passant targets
- Validates moves to ensure they don't leave the king in check

<!-- TOC --><a name="limitations"></a>
## Limitations

This is a basic implementation and does not yet include:

- Advanced search algorithms
- Evaluation function
- UCI protocol support
- Time management
- Opening book or endgame tablebases
