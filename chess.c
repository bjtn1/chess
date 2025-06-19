/**
* Copyright (c) 2024 Brandon Jose Tenorio Noguera
*/


#include <stdio.h>
#include <string.h>
// #include <linux/kernel.h>
// #include <linux/module.h>


/**********************************\
==================================
          ANSI Codes
==================================
\**********************************/
// source:
// https://gist.github.com/RabaDabaDoba/145049536f815903c79944599c6f952a
#define BOLD "\e[1m"
#define bold "\e[1m"

#define BBLK "\e[1;30m"
#define BRED "\e[1;31m"
#define BGRN "\e[1;32m"
#define BYEL "\e[1;33m"
#define BBLU "\e[1;34m"
#define BMAG "\e[1;35m"
#define BCYN "\e[1;36m"
#define BWHT "\e[1;37m"
#define reset "\e[0m"
#define CRESET "\e[0m"
#define COLOR_RESET "\e[0m"


// im not typing uint64_t everytime LOL
typedef unsigned long long u64;


/**********************************\
==================================
              Enums
==================================
\**********************************/

// FEN dedug positions
#define empty_board "8/8/8/8/8/8/8/8 b - - "
#define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
#define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define cmk_position "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "


// board squares
enum {
  a8, b8, c8, d8, e8, f8, g8, h8,
  a7, b7, c7, d7, e7, f7, g7, h7,
  a6, b6, c6, d6, e6, f6, g6, h6,
  a5, b5, c5, d5, e5, f5, g5, h5,
  a4, b4, c4, d4, e4, f4, g4, h4,
  a3, b3, c3, d3, e3, f3, g3, h3,
  a2, b2, c2, d2, e2, f2, g2, h2,
  a1, b1, c1, d1, e1, f1, g1, h1, no_square,
};

const char *index_to_board_coordinates[] = {
  "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
  "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
  "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
  "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
  "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
  "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
  "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
  "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"
};

enum {
  white,
  black,
  both,
};

enum {
  rook,
  bishop,
};

enum {
  wk = 1,
  wq = 2,
  bk = 4,
  bq = 8,
};

// piece encoding
// upercase leeters for white, lowercase for black
enum {
  P, N, B, R, Q, K,
  p, n, b, r, q, k,
};

// piece encoding
enum {
  WP, WN, WB, WR, WQ, WK,
  BP, BN, BB, BR, BQ, BK,
};

// ASCII pieces 
char ascii_pieces[12] = "PNBRQKpnbrqk";

// unicode pieces
// src: https://www.alt-codes.net/chess-symbols.php
char *unicode_pieces[12] = {
  "♙", "♘", "♗", "♖", "♕", "♔", "♟", "♞", "♝", "♜", "♛", "♚"
};

// dicts to go from char to ascii piece encoding
int char_to_ascii_pieces[] = {
  ['P'] = P,
  ['N'] = N,
  ['B'] = B,
  ['R'] = R,
  ['Q'] = Q,
  ['K'] = K,

  ['p'] = p,
  ['n'] = n,
  ['b'] = b,
  ['r'] = r,
  ['q'] = q,
  ['k'] = k,
};



/**********************************\
==================================
            Bitboards
==================================
\**********************************/

u64 bitboards[12];

// 1 for black, 1 for black, and 1 for all
u64 occupancies[3];

// keeps track on whose side is it to move
int side;

// en-passant
int enpassant = no_square;

// keeps track of castling rights
int castle;


/**********************************\
==================================
            Bit-macros
==================================
\**********************************/

/*
 * Puts a piece at the specified `square` of a board
 * This is represented as a `1` on the board (visible after calling `print_bitboard`)
 */
#define set_bit(bitboard, square) ((bitboard |= (1ull << square)))
#define place_piece(bitboard, square) ((bitboard |= (1ull << square)))


/*
 * Returns `1` if there is a piece at `square`, `0` otherwise
 */
#define get_bit(bitboard, square) ((bitboard & (1ull << square)))
#define is_square_occupied(bitboard, square) ((bitboard & (1ull << square)))


/*
 * Resets a `square` on the bitboard to `0`
 */
#define pop_bit(bitboard, square) ((bitboard) &= ~(1ull << (square)))
#define reset_square(bitboard, square) ((bitboard) &= ~(1ull << (square)))


/*
 * Counts how many bits are set in a bitboard
 *
 * @param bitboard: u64   bitboard we're intersted in counting the bits from
 * @returns number of bits set in this bitboard
 */
static inline int bit_counter(u64 bitboard) {
  int count;
  count = 0;

  // keep resetting least-significant bit on bitboard
  while (bitboard > 0 ) {
    count++;

    // reset the least significant bit
    bitboard &= bitboard - 1;
  }

  return count;
}


/*
 * Obtains the index of the elast significant bit on a bitboard
 *
 * @param bitboard: u64  bitboard whose least-significant bit index we're interested in
 * @returns -1 if bitboard == 0, index of least-significant bit otherwise
 */
static inline int get_least_significant_bit_index(u64 bitboard) {
  if (bitboard > 0) {
    return bit_counter((bitboard & -bitboard) - 1);
  } else {
    return -1;
  }
}

/**********************************\
==================================
        Input-and-output
==================================
\**********************************/

/**
 * Prints a given bitboard from white's perspective
 *
 * @param bitboard:             bitboard to be printed
 */
void print_white_bitboard(u64 bitboard) {
  printf("\n   Bitboard: "BBLU"%llud\n"COLOR_RESET, bitboard);
  printf("   White's perspective\n");
  printf(BOLD"\n   Rank\n"COLOR_RESET);
  printf("       +---+---+---+---+---+---+---+---+\n");

  for (int rank = 0; rank < 8; rank++) {
    printf(BOLD"    %d  "COLOR_RESET, 8 - rank);

    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      printf("| %s ", is_square_occupied(bitboard, square) ? BGRN"■"COLOR_RESET : " ");
    }
    printf("|\n   ");
    printf("    +---+---+---+---+---+---+---+---+\n");
  }
  printf(BOLD"   File  a   b   c   d   e   f   g   h\n\n"COLOR_RESET);
}


void print_board() {
  int rank, file;

  printf("\n   Rank\n");
  printf("       +---+---+---+---+---+---+---+---+\n");

  for (rank = 0; rank < 8; rank++) {
    printf("    %d  ", 8 - rank);

    for (file = 0; file < 8; file++) {
      int square, piece;

      square = rank * 8 + file;
      piece = -1;

      // figure out which piece we should print out
      int bitboard_piece;
      for (bitboard_piece = P; bitboard_piece <= k; bitboard_piece++) {
        if (get_bit(bitboards[bitboard_piece], square)) {
          piece = bitboard_piece;
        }
      }

      // printf("| %c ", (piece == -1) ? ' ' : ascii_pieces[piece]);
      printf("| %s ", (piece == -1) ? " " : unicode_pieces[piece]);
    }
    printf("|\n   ");
    printf("    +---+---+---+---+---+---+---+---+\n");
  }

  printf("   File  a   b   c   d   e   f   g   h\n\n");
  printf("   Side to move: \t%s\n", (!side) ? "White" : "Black");
  printf("   Enpassant square: \t%s\n", (enpassant != no_square) ? index_to_board_coordinates[enpassant] : "None");
  printf("   Castling rights: \t%c%c%c%c\n",
         (castle & wk) ? 'K' : '-',
         (castle & wq) ? 'Q' : '-',
         (castle & bk) ? 'k' : '-',
         (castle & bq) ? 'q' : '-'
         );
}


/*
 * Resets game state variables, bitboards, and occupancies
 */
void parse_fen(char *fen) {
  memset(bitboards, 0ull, sizeof(bitboards));
  memset(occupancies, 0ull, sizeof(occupancies));

  side = 0;
  enpassant = no_square;
  castle = 0;

  int rank, file;
  for (rank = 0; rank < 8; rank++) {
    for (file = 0; file <8; file++) {
      int square;
      square = rank * 8 + file;

      if ((*fen >= 'a' && *fen <= 'z') || *fen >= 'A' && *fen <= 'Z') {
        int piece;
        piece = char_to_ascii_pieces[*fen];

        place_piece(bitboards[piece], square);

        fen++;
      }

      if (*fen >= '0' && *fen <= '9') {
        int offset;
        offset = *fen - '0';

        int piece, bitboard_piece;
        piece = -1;
        for (bitboard_piece = P; bitboard_piece <= k; bitboard_piece++) {
          if (get_bit(bitboards[bitboard_piece], square)) {
            piece = bitboard_piece;
          }
        }

        if (piece == -1) {
          file--;
        }

        file += offset;

        fen++;
      }

      if (*fen == '/') {
        fen++;
      }
    }
  }

  // parse side_to_move
  fen++;
  (*fen == 'w') ? (side = white) : (side = black);

  // parse castling rights
  fen += 2;
  while (*fen != ' ') {
    switch (*fen) {
      case 'K': castle |= wk; break;
      case 'Q': castle |= wq; break;
      case 'k': castle |= bk; break;
      case 'q': castle |= bq; break;
      case '-': break;
    }
    fen++;
  }

  // parse enpassant square
  fen++;
  if (*fen != '-') {
    int file, rank;
    file = fen[0] - 'a';
    rank = 8 - (fen[1] - '0');

    enpassant = rank * 8 + file;
  }
  else {
    enpassant = no_square;
  }

  // initialize occupancies for all sides
  int piece;
  for (piece = P; piece <= K; piece++) {
    occupancies[white] |= bitboards[piece];
  }

  for (piece = p; piece <= k; piece++) {
    occupancies[black] |= bitboards[piece];
  }

  occupancies[both] |= occupancies[white];
  occupancies[both] |= occupancies[black];
}


/**********************************\
==================================
        Pre-computed-attacks
==================================
\**********************************/

// used to prevent a piece (pawn, king, or knight) on the h file from seeing squares on the a file
// in other words, the board doesnt loop when you get to the edge
const u64 we_are_on_the_a_file = 18374403900871474942ull;

// used to prevent a piece (pawn, king, or knight) on the a file from seeing squares on the h file
// in other words, the board doesnt loop when you get to the edge
const u64 we_are_on_the_h_file = 9187201950435737471ull;

// used to prevent a piece (pawn, king, or knight) on the gh file from seeing squares on the ab file
// in other words, the board doesnt loop when you get to the edge
const u64 we_are_on_the_gh_file = 4557430888798830399ull;

// used to prevent a piece (pawn, king, or knight) on the ab file from seeing squares on the gh file
// in other words, the board doesnt loop when you get to the edge
const u64 we_are_on_the_ab_file = 18229723555195321596ull;

// array representing the relevant occupancy bit counts for every square on the board for a bishop
const int bishop_relevant_bits[64] = {
  6, 5, 5, 5, 5, 5, 5, 6, 
  5, 5, 5, 5, 5, 5, 5, 5, 
  5, 5, 7, 7, 7, 7, 5, 5, 
  5, 5, 7, 9, 9, 7, 5, 5, 
  5, 5, 7, 9, 9, 7, 5, 5, 
  5, 5, 7, 7, 7, 7, 5, 5, 
  5, 5, 5, 5, 5, 5, 5, 5, 
  6, 5, 5, 5, 5, 5, 5, 6,
};

// array representing the relevant occupancy bit counts for every square on board for a rook
const int rook_relevant_bits[64] = {
  12, 11, 11, 11, 11, 11, 11, 12, 
  11, 10, 10, 10, 10, 10, 10, 11, 
  11, 10, 10, 10, 10, 10, 10, 11, 
  11, 10, 10, 10, 10, 10, 10, 11, 
  11, 10, 10, 10, 10, 10, 10, 11, 
  11, 10, 10, 10, 10, 10, 10, 11, 
  11, 10, 10, 10, 10, 10, 10, 11, 
  12, 11, 11, 11, 11, 11, 11, 12,
};

// rook magic numbers
u64 rook_magic_nums[64] = {
  0x8a80104000800020ULL,
  0x140002000100040ULL,
  0x2801880a0017001ULL,
  0x100081001000420ULL,
  0x200020010080420ULL,
  0x3001c0002010008ULL,
  0x8480008002000100ULL,
  0x2080088004402900ULL,
  0x800098204000ULL,
  0x2024401000200040ULL,
  0x100802000801000ULL,
  0x120800800801000ULL,
  0x208808088000400ULL,
  0x2802200800400ULL,
  0x2200800100020080ULL,
  0x801000060821100ULL,
  0x80044006422000ULL,
  0x100808020004000ULL,
  0x12108a0010204200ULL,
  0x140848010000802ULL,
  0x481828014002800ULL,
  0x8094004002004100ULL,
  0x4010040010010802ULL,
  0x20008806104ULL,
  0x100400080208000ULL,
  0x2040002120081000ULL,
  0x21200680100081ULL,
  0x20100080080080ULL,
  0x2000a00200410ULL,
  0x20080800400ULL,
  0x80088400100102ULL,
  0x80004600042881ULL,
  0x4040008040800020ULL,
  0x440003000200801ULL,
  0x4200011004500ULL,
  0x188020010100100ULL,
  0x14800401802800ULL,
  0x2080040080800200ULL,
  0x124080204001001ULL,
  0x200046502000484ULL,
  0x480400080088020ULL,
  0x1000422010034000ULL,
  0x30200100110040ULL,
  0x100021010009ULL,
  0x2002080100110004ULL,
  0x202008004008002ULL,
  0x20020004010100ULL,
  0x2048440040820001ULL,
  0x101002200408200ULL,
  0x40802000401080ULL,
  0x4008142004410100ULL,
  0x2060820c0120200ULL,
  0x1001004080100ULL,
  0x20c020080040080ULL,
  0x2935610830022400ULL,
  0x44440041009200ULL,
  0x280001040802101ULL,
  0x2100190040002085ULL,
  0x80c0084100102001ULL,
  0x4024081001000421ULL,
  0x20030a0244872ULL,
  0x12001008414402ULL,
  0x2006104900a0804ULL,
  0x1004081002402ULL,
};

// bishop magic nums
u64 bishop_magic_nums[64] = {
  0x40040844404084ULL,
  0x2004208a004208ULL,
  0x10190041080202ULL,
  0x108060845042010ULL,
  0x581104180800210ULL,
  0x2112080446200010ULL,
  0x1080820820060210ULL,
  0x3c0808410220200ULL,
  0x4050404440404ULL,
  0x21001420088ULL,
  0x24d0080801082102ULL,
  0x1020a0a020400ULL,
  0x40308200402ULL,
  0x4011002100800ULL,
  0x401484104104005ULL,
  0x801010402020200ULL,
  0x400210c3880100ULL,
  0x404022024108200ULL,
  0x810018200204102ULL,
  0x4002801a02003ULL,
  0x85040820080400ULL,
  0x810102c808880400ULL,
  0xe900410884800ULL,
  0x8002020480840102ULL,
  0x220200865090201ULL,
  0x2010100a02021202ULL,
  0x152048408022401ULL,
  0x20080002081110ULL,
  0x4001001021004000ULL,
  0x800040400a011002ULL,
  0xe4004081011002ULL,
  0x1c004001012080ULL,
  0x8004200962a00220ULL,
  0x8422100208500202ULL,
  0x2000402200300c08ULL,
  0x8646020080080080ULL,
  0x80020a0200100808ULL,
  0x2010004880111000ULL,
  0x623000a080011400ULL,
  0x42008c0340209202ULL,
  0x209188240001000ULL,
  0x400408a884001800ULL,
  0x110400a6080400ULL,
  0x1840060a44020800ULL,
  0x90080104000041ULL,
  0x201011000808101ULL,
  0x1a2208080504f080ULL,
  0x8012020600211212ULL,
  0x500861011240000ULL,
  0x180806108200800ULL,
  0x4000020e01040044ULL,
  0x300000261044000aULL,
  0x802241102020002ULL,
  0x20906061210001ULL,
  0x5a84841004010310ULL,
  0x4010801011c04ULL,
  0xa010109502200ULL,
  0x4a02012000ULL,
  0x500201010098b028ULL,
  0x8040002811040900ULL,
  0x28000010020204ULL,
  0x6000020202d0240ULL,
  0x8918844842082200ULL,
  0x4010011029020020ULL,
};

// u64 pawn_attacks[side_to_move][available_squares]
u64 pawn_attacks[2][64];

// u64 knight_attacks[square]
u64 knight_attacks[64];

// king_attacks[square]
u64 king_attacks[64];

// bishop attaks mask
u64 bishop_masks[64];

// rook attacks masks
u64 rook_masks[64];

// bishop attacks table [square][occupancies]
// number of bytes representing the max num of occupancies = 512
u64 bishop_attacks[64][512];

// rook attacks table [square][occupancies]
u64 rook_attacks[64][4096];


/*
 * Generates every possible pawn attack when the pawn moves to `square`
 *
 * @param side_to_move: int         `0` if it's white's turn to move, `1` otherwise
 * @param square: int               the square this pawn is moving to   
 * @returns                         a u64 bitboard of every square attacked after the piece moves
 */
u64 generate_pawn_attacks(int side_to_move, int square) {
  // resulting attacks bitboard
  u64 attacks;
  attacks = 0ull;

  // piece bitboard
  u64 pawn_bitboard;
  pawn_bitboard = 0ull;

  // place piece on bitboard
  place_piece(pawn_bitboard, square);

  // if white to move, what squares can this pawn attack/see?
  if (side_to_move == white) {
    if ((pawn_bitboard >> 7) & we_are_on_the_a_file) attacks |= (pawn_bitboard >> 7);
    if ((pawn_bitboard >> 9) & we_are_on_the_h_file) attacks |= (pawn_bitboard >> 9);

  // if black to move, what squares can this pawn attack/see?
  } else {
    if ((pawn_bitboard << 7) & we_are_on_the_h_file) attacks |= (pawn_bitboard << 7);
    if ((pawn_bitboard << 9) & we_are_on_the_a_file) attacks |= (pawn_bitboard << 9);
  }

  // return attack map
  return attacks;
}


/*
 * Generates every possible knight attack when the knight is positioned at `square`
 *
 * @param square: int               the square this piece is moving to   
 * @returns                         a u64 bitboard of every square attacked after the piece moves
 */
u64 generate_knight_attacks(int square) {
  // resulting attacks bitboard
  u64 attacks;
  attacks = 0ull;

  // piece bitboard
  u64 bitboard;
  bitboard = 0ull;

  // place piece on board
  place_piece(bitboard, square);

  // what squares can this knight attack?
  if ((bitboard >> 17) & we_are_on_the_h_file) attacks |= (bitboard >> 17);
  if ((bitboard >> 15) & we_are_on_the_a_file) attacks |= (bitboard >> 15);
  if ((bitboard >> 10) & we_are_on_the_gh_file) attacks |= (bitboard >> 10);
  if ((bitboard >> 6) & we_are_on_the_ab_file) attacks |= (bitboard >> 6);

  // now we do the same for the other side (essentially mirroring)
  if ((bitboard << 17) & we_are_on_the_a_file) attacks |= (bitboard << 17);
  if ((bitboard << 15) & we_are_on_the_h_file) attacks |= (bitboard << 15);
  if ((bitboard << 10) & we_are_on_the_ab_file) attacks |= (bitboard << 10);
  if ((bitboard << 6) & we_are_on_the_gh_file) attacks |= (bitboard << 6);

  return attacks;
}


/*
 * Generates every possible knight attack when the knight is positioned at `square`
 *
 * @param square: int               the square this piece is moving to   
 * @returns                         a u64 bitboard of every square attacked after the piece moves
 */
u64 generate_king_attacks(int square) {
  // resulting attacks bitboard
  u64 attacks;
  attacks = 0ull;

  // piece bitboard
  u64 bitboard;
  bitboard = 0ull;

  // place piece on board
  place_piece(bitboard, square);

  // what squares can this king attack/see?
  if ((bitboard >> 9) & we_are_on_the_h_file) attacks |= (bitboard >> 9);
  if (bitboard >> 8) attacks |= (bitboard >> 8);
  if ((bitboard >> 7) & we_are_on_the_a_file) attacks |= (bitboard >> 7);
  if ((bitboard >> 1) & we_are_on_the_h_file) attacks |= (bitboard >> 1);

  // now we do the same for the other side (essentially mirroring)
  if ((bitboard << 9) & we_are_on_the_a_file) attacks |= (bitboard << 9);
  if (bitboard << 8) attacks |= (bitboard << 8);
  if ((bitboard << 7) & we_are_on_the_h_file) attacks |= (bitboard << 7);
  if ((bitboard << 1) & we_are_on_the_a_file) attacks |= (bitboard << 1);

  return attacks;
}


/*
 * Generates every possible bishop attack when the bishop is positioned at `square`
 * mask_bishop_attacks
 *
 * @param square: int               the square this piece is moving to   
 * @returns                         a u64 bitboard of every square attacked after the piece moves
 */
u64 generate_bishop_attacks(int square) {
  // resulting attacks bitboard
  u64 attacks = 0ull;

  // init ranks and files
  int rank;
  int file;

  // init target ranks and files
  int target_rank = square / 8;
  int target_file = square % 8;

  for (rank = target_rank + 1, file = target_file + 1; rank <= 6 && file <= 6; rank++, file++) attacks |= (1ull << (rank * 8 + file));
  for (rank = target_rank - 1, file = target_file + 1; rank >= 1 && file <= 6; rank--, file++) attacks |= (1ull << (rank * 8 + file));
  for (rank = target_rank + 1, file = target_file - 1; rank <= 6 && file >= 1; rank++, file--) attacks |= (1ull << (rank * 8 + file));
  for (rank = target_rank - 1, file = target_file - 1; rank >= 1 && file >= 1; rank--, file--) attacks |= (1ull << (rank * 8 + file));

  return attacks;
}


/*
  * Generates the path of a bishop given a bitboard of pieces on its way
  * @param square int:          square this piece is moving to
  * @param blocker_piece u64:   a bitboard of pieces that may or may not be blocking the path
  */
u64 bishop_attacks_generator(int square, u64 blocker_piece) {
  // resulting attacks bitboard
  u64 attacks;
  attacks = 0ull;

  // init ranks and files
  int rank;
  int file;

  // init target ranks and files
  int target_rank;
  target_rank = square / 8;

  int target_file;
  target_file = square % 8;

  for (rank = target_rank + 1, file = target_file + 1; rank <= 7 && file <= 7; rank++, file++) {
    attacks |= (1ull << (rank * 8 + file));
    if ((1ull << (rank * 8 + file)) & blocker_piece) break;
  }

  for (rank = target_rank - 1, file = target_file + 1; rank >= 0 && file <= 7; rank--, file++) {
    attacks |= (1ull << (rank * 8 + file));
    if ((1ull << (rank * 8 + file)) & blocker_piece) break;
  }

  for (rank = target_rank + 1, file = target_file - 1; rank <= 7 && file >= 0; rank++, file--) {
    attacks |= (1ull << (rank * 8 + file));
    if ((1ull << (rank * 8 + file)) & blocker_piece) break;
  }

  for (rank = target_rank - 1, file = target_file - 1; rank >= 0 && file >= 0; rank--, file--) {
    attacks |= (1ull << (rank * 8 + file));
    if ((1ull << (rank * 8 + file)) & blocker_piece) break;
  }

  return attacks;
}


/*
 * Generates every possible rook attack when the rook is positioned at `square`
 *
 * mask_bishop_attacks
 *
 * @param square: int               the square this piece is moving to
 * @returns                         a u64 bitboard of every square attacked after the piece moves
 */
u64 generate_rook_attacks(int square) {
  // resulting attacks bitboard
  u64 attacks;
  attacks = 0ull;

  // init ranks and files
  int rank;
  int file;

  // init target ranks and files
  int target_rank;
  target_rank = square / 8;

  int target_file;
  target_file = square % 8;

  for (rank = target_rank + 1; rank <= 6; rank++)  attacks |= (1ull << (rank * 8 + target_file));
  for (rank = target_rank - 1; rank >= 1; rank--) attacks |= (1ull << (rank * 8 + target_file));
  for (file = target_file + 1; file <= 6; file++) attacks |= (1ull << (target_rank * 8 + file));
  for (file = target_file - 1; file >= 1; file--) attacks |= (1ull << (target_rank * 8 + file));

  return attacks;
}

/*
  * Generates the path of a rook given a bitboard of pieces on its way
  * @param square int:          square this piece is moving to
  * @param blocker_piece u64:   a bitboard of pieces that may or may not be blocking the path
  */
u64 rook_attacks_generator(int square, u64 blocker_piece) {
  // resulting attacks bitboard
  u64 attacks;
  attacks = 0ull;

  // init ranks and files
  int rank;
  int file;

  // init target ranks and files
  int target_rank;
  target_rank = square / 8;

  int target_file;
  target_file = square % 8;

  for (rank = target_rank + 1; rank <= 7; rank++){
    attacks |= (1ull << (rank * 8 + target_file));
    if ((1ull << (rank * 8 + target_file)) & blocker_piece) break;
  }

  for (rank = target_rank - 1; rank >= 0; rank--) {
    attacks |= (1ull << (rank * 8 + target_file));
    if ((1ull << (rank * 8 + target_file)) & blocker_piece) break;
  }

  for (file = target_file + 1; file <= 7; file++) {
    attacks |= (1ull << (target_rank * 8 + file));
    if ((1ull << (target_rank * 8 + file)) & blocker_piece) break;
  }

  for (file = target_file - 1; file >= 0; file--) {
    attacks |= (1ull << (target_rank * 8 + file));
    if ((1ull << (target_rank * 8 + file)) & blocker_piece) break;
  }

  return attacks;
}


void init_precomputed_attacks() {
  int square;

  for (square = 0; square < 64; square++) {
    // initialize pawn attacks
    pawn_attacks[white][square] = generate_pawn_attacks(white, square);
    pawn_attacks[black][square] = generate_pawn_attacks(black, square);

    // initialize knight atacks
    knight_attacks[square] = generate_knight_attacks(square);

    // init king attacks
    king_attacks[square] = generate_king_attacks(square);
  }
}


u64 set_occupancy(int index, int bits_in_generated_attack, u64 attacks_generated) {
  u64 occupancy;
  occupancy = 0ull;

  int count;
  int square;

  for (count = 0; count < bits_in_generated_attack; count++) {
    square = get_least_significant_bit_index(attacks_generated);

    reset_square(attacks_generated, square);

    if (index & (1 << count)) {
      occupancy |= (1ull << square);
    }
  }

  return occupancy;
}


// 1804289383
// 1574098257
unsigned int rand_state = 1804289383;

/*
* Generates a pseudo random 32 bit number using the xorshift32 algorithm
*/
unsigned int get_rand_u32_num() {
  unsigned int num = rand_state;

  // xorshift32 prng algorithm 
  num ^= num << 13;
  num ^= num >> 17;
  num ^= num << 5;

  rand_state = num;

  return num;
}


/*
* Generates a pseudo random 64 bit number using the xorshift32 algorithm
*/
u64 get_rand_u64_num() {
  u64 n1, n2, n3, n4;

  // slice 16 bits from left side (msb side)
  n1 = (u64)(get_rand_u32_num()) & 0xFFFF;
  n2 = (u64)(get_rand_u32_num()) & 0xFFFF;
  n3 = (u64)(get_rand_u32_num()) & 0xFFFF;
  n4 = (u64)(get_rand_u32_num()) & 0xFFFF;

  // making number even more random
  return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}


/*
* Generates a candidate for a magic number
*/
u64 generate_magic_num() {
  return get_rand_u64_num() & get_rand_u64_num() & get_rand_u64_num();
}


/**********************************\
==================================
        Magic-number-stuff
==================================
\**********************************/

/*
* Finds the appropriate magic number for a bitboard
*/
u64 find_magic_num(int square, int relevant_bits, int bishop) {
  int rand_count;
  int magic_index;

  u64 occupancies[4096];
  u64 attacks[4096];
  u64 used_attacks[4096];

  u64 generated_attacks = bishop ? generate_bishop_attacks(square) : generate_rook_attacks(square);

  int occupancy_indices = 1 << relevant_bits;

  int index;
  for (index = 0; index < occupancy_indices; index++) {
    occupancies[index] = set_occupancy(index, relevant_bits, generated_attacks);

    attacks[index] = bishop ? bishop_attacks_generator(square, occupancies[index]) : rook_attacks_generator(square, occupancies[index]);
  }

  // this is where we find the appropriate magic nums
  for (rand_count = 0; rand_count < 100000000; rand_count++)
  {
    u64 magic_num_candidate = generate_magic_num();
    if (bit_counter((generated_attacks * magic_num_candidate) & 0xFF00000000000000) < 6) {
      continue;
    }

    memset(used_attacks, 0ull, sizeof(used_attacks));

    int index, fail;
    for (index = 0, fail = 0; !fail && index < occupancy_indices; index++) {
      magic_index = (int)((occupancies[index] * magic_num_candidate) >> (64 - relevant_bits));

      if (used_attacks[magic_index] == 0ull) used_attacks[magic_index] = attacks[index];
      else if (used_attacks[magic_index] != attacks[index]) fail = 1;
    }
    if (!fail) return magic_num_candidate;
  }
  // printf("    Magic number failed\n");
  printf("    Magic number failed\n");
  return 0ull;
}


/*
* Initializes a magic numbers
*/
void init_magic_nums() {
  int square;
  for (square = 0; square < 64; square++) rook_magic_nums[square] = find_magic_num(square, rook_relevant_bits[square], rook);
  for (square = 0; square < 64; square++) bishop_magic_nums[square] = find_magic_num(square, bishop_relevant_bits[square], bishop);
}


// initialize the attack table for rook and bishop
void init_rook_and_bishop_attacks(int bishop) {
  int square;

  for (square = 0; square < 64; square++) {
    bishop_masks[square] = generate_bishop_attacks(square);
    rook_masks[square] = generate_rook_attacks(square);

    u64 attack_mask = bishop ? bishop_masks[square] : rook_masks[square];

    int relevant_bits_count = bit_counter(attack_mask);

    int occupancy_indices = (1 << relevant_bits_count);

    int index;
    for (index = 0; index < occupancy_indices; index++) {
      if (bishop) {
        u64 occupancy = set_occupancy(index, relevant_bits_count, attack_mask);

        int magic_index = (occupancy * bishop_magic_nums[square]) >> (64 - bishop_relevant_bits[square]);

        bishop_attacks[square][magic_index] = bishop_attacks_generator(square, occupancy);
      }
      else {
        u64 occupancy = set_occupancy(index, relevant_bits_count, attack_mask);

        int magic_index = (occupancy * rook_magic_nums[square]) >> (64 - rook_relevant_bits[square]);

        rook_attacks[square][magic_index] = rook_attacks_generator(square, occupancy);
      }
    }
  }
}


static inline u64 get_bishop_attacks(int square, u64 occupancy) {
  occupancy &= bishop_masks[square];
  occupancy *= bishop_magic_nums[square];
  occupancy >>= (64 - bishop_relevant_bits[square]);

  return bishop_attacks[square][occupancy];
}


static inline u64 get_rook_attacks(int square, u64 occupancy) {
  occupancy &= rook_masks[square];
  occupancy *= rook_magic_nums[square];
  occupancy >>= (64 - rook_relevant_bits[square]);

  return rook_attacks[square][occupancy];
}


static inline u64 get_queen_attacks(int square, u64 occupancy) {
  u64 queen_attacks = 0ull;

  u64 bishop_occupancy = occupancy;
  u64 rook_occupancy = occupancy;

  bishop_occupancy &= bishop_masks[square];
  bishop_occupancy *= bishop_magic_nums[square];
  bishop_occupancy >>= (64 - bishop_relevant_bits[square]);

  queen_attacks = bishop_attacks[square][bishop_occupancy];

  rook_occupancy &= rook_masks[square];
  rook_occupancy *= rook_magic_nums[square];
  rook_occupancy >>= (64 - rook_relevant_bits[square]);

  queen_attacks |= rook_attacks[square][rook_occupancy];

  return queen_attacks;
}


/**********************************\
==================================
        Moves-generation
==================================
\**********************************/


/*
 * Checks if a square is being attacked by a given side
 */
static inline int is_square_attacked(int square, int side) {
  // squares attacked by white pawns
  if ((side == white) && (pawn_attacks[black][square] & bitboards[P])) {
    return 1;
  }

  // squares attacked by black pawns
  if ((side == black) && (pawn_attacks[white][square] & bitboards[p])) {
    return 1;
  }

  // squares attacked by all knights
  if (knight_attacks[square] & ((side == white) ? bitboards[N] : bitboards[n])) {
    return 1;
  }

  // squares attacked by all bishops
  if (get_bishop_attacks(square, occupancies[both]) & ((side == white) ? bitboards[B] : bitboards[b])) {
    return 1;
  }

  // squares attacked by all rooks
  if (get_rook_attacks(square, occupancies[both]) & ((side == white) ? bitboards[R] : bitboards[r])) {
    return 1;
  }

  // squares attacked by all queens
  if (get_queen_attacks(square, occupancies[both]) & ((side == white) ? bitboards[Q] : bitboards[q])) {
    return 1;
  }

  // squares attacked by all kings
  if (king_attacks[square] & ((side == white) ? bitboards[K] : bitboards[k])) {
    return 1;
  }

  // return False = return 0;
  // return True = return 1;
  return 0;
}


/*
 * Prints the board from white's perspective showing which pieces are being attacked
 */
void print_attacked_squares(int side) {
  int rank, file;

  printf("\n   Squares attacked by %s", (side == white) ? "WHITE" : "BLACK");
  printf("\n   Rank\n");
  printf("       +---+---+---+---+---+---+---+---+\n");

  for (rank = 0; rank < 8; rank++) {
    printf("    %d  ", 8 - rank);

    for (file = 0; file < 8; file++) {
      int square, piece;

      square = rank * 8 + file;
      piece = -1;

      printf("| %s ",is_square_attacked(square, side) ? BGRN"■"COLOR_RESET : " ");      
      // printf("| %c ", (piece == -1) ? ' ' : ascii_pieces[piece]);
      // printf("| %s ", (piece == -1) ? " " : unicode_pieces[piece]);
    }
    printf("|\n   ");
    printf("    +---+---+---+---+---+---+---+---+\n");
  }

  printf("   File  a   b   c   d   e   f   g   h\n\n");
}


// encoding binary macro for moves
#define encode_move(source, target, piece, promoted, capture, double_pawn_push, enpassant, castling) \
    (source) |          \
    (target << 6) |     \
    (piece << 12) |     \
    (promoted << 16) |  \
    (capture << 20) |   \
    (double_pawn_push << 21) |    \
    (enpassant << 22) | \
    (castling << 23)    \


// Gets the source square from a move
#define get_move_source(move) (move & 0x3f)

// Gets the target sqaure from an encoded move
#define get_move_target(move) ((move & 0xfc0) >> 6)

// Gets the piece from encoded move
#define get_move_piece(move) ((move & 0xf000) >> 12)

// Gets the promoted piece
#define get_move_promoted(move) ((move & 0xf0000) >> 16)

// Gets the capture flag 
#define get_move_capture(move) ((move & 0x100000))

// Gets the double pawn push flag
#define get_move_double(move) (move & 0x200000)

// Gets the enpassant flag
#define get_move_enpassant(move) (move & 0x400000)

// Gets the castling flag
#define get_move_castling(move) (move & 0x800000)


// move list structure
typedef struct {
  int moves[256];
  int count;
} moves;


static inline void add_move(moves *move_list, int move) {
  move_list->moves[move_list->count] = move;

  move_list->count++;
}


char promoted_pieces[] = {
  [Q] = 'q',
  [R] = 'r',
  [B] = 'b',
  [N] = 'n',

  [q] = 'q',
  [r] = 'r',
  [b] = 'b',
  [n] = 'n',
};



void print_move(int move) {
  printf("%s%s%c\n", 
         index_to_board_coordinates[get_move_source(move)],
         index_to_board_coordinates[get_move_target(move)],
         promoted_pieces[get_move_promoted(move)]
         );
}


void print_move_list(moves *move_list) {
  // do nothing on empty move list
  if (!move_list->count)
  {
    printf("\n     No move in the move list!\n");
    return;
  }

  printf("\n     move    piece     capture   double    enpass    castling\n\n");

  // loop over moves within a move list
  for (int move_count = 0; move_count < move_list->count; move_count++)
  {
    // init move
    int move = move_list->moves[move_count];

    // print move
    printf("     %s%s%c   %s         %d         %d         %d         %d\n", index_to_board_coordinates[get_move_source(move)],
           index_to_board_coordinates[get_move_target(move)],
           get_move_promoted(move) ? promoted_pieces[get_move_promoted(move)] : ' ',
           unicode_pieces[get_move_piece(move)],
           get_move_capture(move) ? 1 : 0,
           get_move_double(move) ? 1 : 0,
           get_move_enpassant(move) ? 1 : 0,
           get_move_castling(move) ? 1 : 0);
  }

  // print total number of moves
  printf("\n\n     Total number of moves: %d\n\n", move_list->count);

}


// preserve board state
#define copy_board()                                                      \
    u64 bitboards_copy[12], occupancies_copy[3];                          \
    int side_copy, enpassant_copy, castle_copy;                           \
    memcpy(bitboards_copy, bitboards, 96);                                \
    memcpy(occupancies_copy, occupancies, 24);                            \
    side_copy = side, enpassant_copy = enpassant, castle_copy = castle;   \


// restore board state
#define take_back()                                                       \
    memcpy(bitboards, bitboards_copy, 96);                                \
    memcpy(occupancies, occupancies_copy, 24);                            \
    side = side_copy, enpassant = enpassant_copy, castle = castle_copy;   \

enum {
  all_moves,
  only_captures,
};


// castling rights update
const int castling_rights[64] = {
  7, 15, 15, 15,  3, 15, 15, 11,
  15, 15, 15, 15, 15, 15, 15, 15,
  15, 15, 15, 15, 15, 15, 15, 15,
  15, 15, 15, 15, 15, 15, 15, 15,
  15, 15, 15, 15, 15, 15, 15, 15,
  15, 15, 15, 15, 15, 15, 15, 15,
  15, 15, 15, 15, 15, 15, 15, 15,
  13, 15, 15, 15, 12, 15, 15, 14
};


// returns 1 for legal moves
// returns 0 for illegal moves
static inline int make_move(int move, int move_flag) {
  // non-capture moves
  if (move_flag == all_moves) {
    // preserve board if we're in check
    copy_board();

    // parse the move
    int src_sq;
    src_sq = get_move_source(move);

    int tgt_sq;
    tgt_sq = get_move_target(move);

    int piece;
    piece = get_move_piece(move);

    int promoted_piece;
    promoted_piece = get_move_promoted(move);

    int capture;
    capture = get_move_capture(move);

    int double_pawn_push;
    double_pawn_push = get_move_double(move);

    int enpass;
    enpass = get_move_enpassant(move);

    int castling;
    castling = get_move_castling(move);

    // move piece
    pop_bit(bitboards[piece], src_sq);
    place_piece(bitboards[piece], tgt_sq);

    // handle captures
    if (capture) {
      // get bitboard piece indixes for each side
      int start_piece, end_piece;

      // white to move
      if (side == white) {
        start_piece = p;
        end_piece = k;
      }
      // black to move
      else {
        start_piece = P;
        end_piece = K;
      }

      // iterate bitboards opposite current side to move
      int bitboard_piece;
      for (bitboard_piece = start_piece; bitboard_piece <= end_piece; bitboard_piece++) {
        // remove piece on target_square from the bitboard
        if (get_bit(bitboards[bitboard_piece], tgt_sq)) {
          pop_bit(bitboards[bitboard_piece], tgt_sq);
          break;
        }
      }
    }

    // handle pawn promotions
    if (promoted_piece) {
      // erase pawn from target square
      pop_bit(bitboards[(side == white) ? P : p], tgt_sq);

      // put promoted piece on the board
      place_piece(bitboards[promoted_piece], tgt_sq);
    }

    // handle enapssant
    if (enpass) {
      // erase pawn that got captured
      (side == white) ? pop_bit(bitboards[p], (tgt_sq + 8))
                      : pop_bit(bitboards[P], (tgt_sq - 8));
    }

    // reset enpassant square (enpassant can only happen on a double push)
    enpassant = no_square;

    // handle double pawn push
    if (double_pawn_push) {
      // set up enpassant sq based on side to move
      (side == white) ? (enpassant = (tgt_sq + 8))
                      : (enpassant = (tgt_sq - 8));
    }

    // handle castling
    if (castling) {
      // find target square for rooks
      switch (tgt_sq) {
        // white castles king-side
        case (g1): {
          pop_bit(bitboards[R], h1);
          place_piece(bitboards[R], f1);
          break;
        }

        // white castles queenside
        case (c1): {
          pop_bit(bitboards[R], a1);
          place_piece(bitboards[R], d1);
          break;
        }

        // black kingside
        case (g8): {
          pop_bit(bitboards[r], h8);
          place_piece(bitboards[r], f8);
          break;
        }

        // black queenside
        case (c8): {
          pop_bit(bitboards[r], a8);
          place_piece(bitboards[r], d8);
          break;
        }
      }
    }

    // update castling rights string
    castle &= castling_rights[src_sq];
    castle &= castling_rights[tgt_sq];

    // update occupancy boards
    memset(occupancies, 0ull, 24);

    int bb_piece;
    for (bb_piece = P; bb_piece <= K; bb_piece++) {
      // update white occupancy board
      occupancies[white] |= bitboards[bb_piece];
    }

    for (bb_piece = p; bb_piece <= k; bb_piece++) {
      // update white occupancy board
      occupancies[black] |= bitboards[bb_piece];
    }

    // update both occupancies
    occupancies[both] |= occupancies[white];
    occupancies[both] |= occupancies[black];

    // change side
    side ^= 1;

    // is king in check
    if (is_square_attacked((side == white) ? get_least_significant_bit_index(bitboards[k])
                                           : get_least_significant_bit_index(bitboards[K]),
                                           side)) {
      // move is illegal
      take_back();
      return 0;
    }

    else {
      return 1;
    }
  }

  // capture moves
  else {
    // make sure move is the capture
    if (get_move_capture(move)) {
      make_move(move, all_moves);
    }
    else {
      // move is not a capture, dont make a move
      return 0;
    }
  }

  return 0;
}


/*
 * Generates all moves
 */
static inline void generate_moves(moves *move_list) {
  // initialise move count
  move_list->count = 0;

  int from_square, target_square;

  u64 bitboard, attacks;

  int piece;
  for (piece = P; piece <= k; piece++) {
    bitboard = bitboards[piece];

    // white pawn moves
    if (side == white) {
      if (piece == P) {
        while (bitboard) {
          from_square = get_least_significant_bit_index(bitboard);

          target_square = from_square - 8;

          if (!(target_square < a8) && !get_bit(occupancies[both], target_square)) {
            if (from_square >= a7 && from_square <= h7) {
              // add to move list
              add_move(move_list, encode_move(from_square, target_square, piece, Q, 0, 0, 0, 0));
              add_move(move_list, encode_move(from_square, target_square, piece, R, 0, 0, 0, 0));
              add_move(move_list, encode_move(from_square, target_square, piece, B, 0, 0, 0, 0));
              add_move(move_list, encode_move(from_square, target_square, piece, N, 0, 0, 0, 0));
            }

            else {
              add_move(move_list, encode_move(from_square, target_square, piece, 0, 0, 0, 0, 0));

              if ((from_square >= a2 && from_square <= h2) && !get_bit(occupancies[both], (target_square - 8))) {
              add_move(move_list, encode_move(from_square, (target_square - 8), piece, 0, 0, 1, 0, 0));
              }
            }
          }

          // white pawn captures
          attacks = pawn_attacks[side][from_square] & occupancies[black];

          while (attacks) {
            target_square = get_least_significant_bit_index(attacks);

            if (from_square >= a7 && from_square <= h7) {
              // add to move list
              add_move(move_list, encode_move(from_square, target_square, piece, Q, 1, 0, 0, 0));
              add_move(move_list, encode_move(from_square, target_square, piece, R, 1, 0, 0, 0));
              add_move(move_list, encode_move(from_square, target_square, piece, B, 1, 0, 0, 0));
              add_move(move_list, encode_move(from_square, target_square, piece, N, 1, 0, 0, 0));
            }

            else {
              add_move(move_list, encode_move(from_square, target_square, piece, 0, 1, 0, 0, 0));
            }

            pop_bit(attacks, target_square);
          }

          // white enpassant
          if (enpassant != no_square) {
            u64 enpassant_attacks = pawn_attacks[side][from_square] & (1ull << enpassant);

            if (enpassant_attacks)  {
              int target_enpassant = get_least_significant_bit_index(enpassant_attacks);
              add_move(move_list, encode_move(from_square, target_enpassant, piece, 0, 1, 0, 1, 0));
            }
          }

          pop_bit(bitboard, from_square);
        }
      }
      
      // castling
      if (piece == K) {
        // white castles king side
        if (castle & wk) {
          if ((!get_bit(occupancies[both], f1)) && (!get_bit(occupancies[both], g1))) {
            if ((!is_square_attacked(e1, black)) && (!is_square_attacked(g1, black))) {
              add_move(move_list, encode_move(e1, g1, piece, 0, 0, 0, 0, 1));
            }
          }
        }

        // white castle queen side
        if (castle & wq) {
          if ((!get_bit(occupancies[both], d1)) && (!get_bit(occupancies[both], c1)) && (!get_bit(occupancies[both], b1))) {
            if ((!is_square_attacked(d1, black)) && (!is_square_attacked(c1, black)) && (!is_square_attacked(b1, black))) {
              add_move(move_list, encode_move(e1, c1, piece, 0, 0, 0, 0, 1));
            }
          }
        }
      }
    }

    // smae as above but for black
    else {
      if (piece == p) {
        while (bitboard) {
          from_square = get_least_significant_bit_index(bitboard);

          target_square = from_square + 8;

          if (!(target_square > h1) && !get_bit(occupancies[both], target_square)) {
            if (from_square >= a2 && from_square <= h2) {
              // add to move list
              add_move(move_list, encode_move(from_square, target_square, piece, q, 0, 0, 0, 0));
              add_move(move_list, encode_move(from_square, target_square, piece, r, 0, 0, 0, 0));
              add_move(move_list, encode_move(from_square, target_square, piece, b, 0, 0, 0, 0));
              add_move(move_list, encode_move(from_square, target_square, piece, n, 0, 0, 0, 0));
            }

            else {
              add_move(move_list, encode_move(from_square, target_square, piece, 0, 0, 0, 0, 0));

              if ((from_square >= a7 && from_square <= h7) && !get_bit(occupancies[both], (target_square + 8))) {
                add_move(move_list, encode_move(from_square, (target_square + 8), piece, 0, 0, 1, 0, 0));
              }
            }
          }

          attacks = pawn_attacks[side][from_square] & occupancies[white];

          while (attacks) {
            target_square = get_least_significant_bit_index(attacks);

            if (from_square >= a2 && from_square <= h2) {
              // add to move list
              add_move(move_list, encode_move(from_square, target_square, piece, q, 1, 0, 0, 0));
              add_move(move_list, encode_move(from_square, target_square, piece, r, 1, 0, 0, 0));
              add_move(move_list, encode_move(from_square, target_square, piece, b, 1, 0, 0, 0));
              add_move(move_list, encode_move(from_square, target_square, piece, n, 1, 0, 0, 0));
            }

            else {
              add_move(move_list, encode_move(from_square, target_square, piece, 0, 1, 0, 0, 0));
            }

            pop_bit(attacks, target_square);
          }

          // white enpassant
          if (enpassant != no_square) {
            u64 enpassant_attacks = pawn_attacks[side][from_square] & (1ull << enpassant);

            if (enpassant_attacks)  {
              int target_enpassant = get_least_significant_bit_index(enpassant_attacks);
              add_move(move_list, encode_move(from_square, target_enpassant, piece, 0, 1, 0, 1, 0));
            }
          }

          pop_bit(bitboard, from_square);
        }
      }

      // castling
      if (piece == k) {
        // white castles king side
        if (castle & bk) {
          if (!get_bit(occupancies[both], f8) && !get_bit(occupancies[both], g8)) {
            if (!is_square_attacked(e8, white) && !is_square_attacked(f8, white)) {
              add_move(move_list, encode_move(e8, g8, piece, 0, 0, 0, 0, 1));
            }
          }
        }

        // white castle queen side
        if (castle & bq) {
          if (!get_bit(occupancies[both], d8) && !get_bit(occupancies[both], c8) && !get_bit(occupancies[both], b8)) {
            if ((!is_square_attacked(d8, white)) && (!is_square_attacked(c8, white)) && (!is_square_attacked(b8, white))) {
              add_move(move_list, encode_move(e8, c8, piece, 0, 0, 0, 0, 1));
            }
          }
        }
      }
    }

    // knights
    if ((side == white) ? piece == N : piece == n) {
      while (bitboard) {
        from_square = get_least_significant_bit_index(bitboard);

        attacks = knight_attacks[from_square] & ((side == white) ? ~occupancies[white] : ~occupancies[black]);

        while (attacks) {
          target_square = get_least_significant_bit_index(attacks);

          if (!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_square)) {
            add_move(move_list, encode_move(from_square, target_square, piece, 0, 0, 0, 0, 0));
          }

          else {
            add_move(move_list, encode_move(from_square, target_square, piece, 0, 1, 0, 0, 0));
          }

          pop_bit(attacks, target_square);
        }

        pop_bit(bitboard, from_square);
      }
    }

    // bishops
    if ((side == white) ? piece == B : piece == b) {
      while (bitboard) {
        from_square = get_least_significant_bit_index(bitboard);

        attacks = get_bishop_attacks(from_square, occupancies[both]) & ((side == white) ? ~occupancies[white] : ~occupancies[black]);

        while (attacks) {
          target_square = get_least_significant_bit_index(attacks);

          if (!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_square)) {
            add_move(move_list, encode_move(from_square, target_square, piece, 0, 0, 0, 0, 0));
          }

          else {
            add_move(move_list, encode_move(from_square, target_square, piece, 0, 1, 0, 0, 0));
          }

          pop_bit(attacks, target_square);
        }

        pop_bit(bitboard, from_square);
      }
    }

    // rooks
    if ((side == white) ? piece == R : piece == r) {
      while (bitboard) {
        from_square = get_least_significant_bit_index(bitboard);

        attacks = get_rook_attacks(from_square, occupancies[both]) & ((side == white) ? ~occupancies[white] : ~occupancies[black]);

        while (attacks) {
          target_square = get_least_significant_bit_index(attacks);

          if (!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_square)) {
            add_move(move_list, encode_move(from_square, target_square, piece, 0, 0, 0, 0, 0));
          }

          else {
            add_move(move_list, encode_move(from_square, target_square, piece, 0, 1, 0, 0, 0));
          }

          pop_bit(attacks, target_square);
        }

        pop_bit(bitboard, from_square);
      }
    }

    // queens
    if ((side == white) ? piece == Q : piece == q) {
      while (bitboard) {
        from_square = get_least_significant_bit_index(bitboard);

        attacks = get_queen_attacks(from_square, occupancies[both]) & ((side == white) ? ~occupancies[white] : ~occupancies[black]);

        while (attacks) {
          target_square = get_least_significant_bit_index(attacks);

          if (!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_square)) {
            add_move(move_list, encode_move(from_square, target_square, piece, 0, 0, 0, 0, 0));
          }

          else {
            add_move(move_list, encode_move(from_square, target_square, piece, 0, 1, 0, 0, 0));
          }

          pop_bit(attacks, target_square);
        }

        pop_bit(bitboard, from_square);
      }
    }

    // kings
    if ((side == white) ? piece == K : piece == k) {
      while (bitboard) {
        from_square = get_least_significant_bit_index(bitboard);

        attacks = king_attacks[from_square] & ((side == white) ? ~occupancies[white] : ~occupancies[black]);

        while (attacks) {
          target_square = get_least_significant_bit_index(attacks);

          if (!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_square)) {
            add_move(move_list, encode_move(from_square, target_square, piece, 0, 0, 0, 0, 0));
          }

          else {
            add_move(move_list, encode_move(from_square, target_square, piece, 0, 1, 0, 0, 0));
          }

          pop_bit(attacks, target_square);
        }

        pop_bit(bitboard, from_square);
      }
    }
  }
}


/**********************************\
==================================
    Initializing stuff-and-main
==================================
\**********************************/



/* 
 * Initializes all variables
*/
void init_all(void) {
  init_precomputed_attacks();
  init_rook_and_bishop_attacks(bishop);
  init_rook_and_bishop_attacks(rook);
}


int main(void) {
  init_all();

  // parse fen
  parse_fen("r3k2r/p1ppQpb1/1n2pnp1/3PN3/1p2P3/2N2Q1p/PPPBqPPP/R3K2R w KQkq - 0 1 ");
  print_board();

  // create move list
  moves move_list[1];

  generate_moves(move_list);

  // loop over generated moves
  int move_count;
  for (move_count = 0; move_count < move_list->count; move_count++) {
    // init move
    int move;
    move = move_list->moves[move_count];

    // preserve board state
    copy_board();

    if (!make_move(move, all_moves)) {
      continue;
    }

    // make move
    make_move(move, all_moves);
    print_board();
    // print_white_bitboard(occupancies[both]);
    getchar();

    // take back
    take_back();
    print_board();
    // print_white_bitboard(occupancies[both]);

    getchar();

  }


  return 0;
}
