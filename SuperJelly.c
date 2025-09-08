#include <stdio.h>

/******************\
--------------------
   HEADER FILES
--------------------
\******************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <windows.h>

/******************\
--------------------
   DEFINITIONS
--------------------
\******************/
/*
we will define our essential "U64" type which stands for "unsigned 64(bits)" and
is the type we utilize to represent our bitboards.
*/

#define U64 uint64_t

/*
we will also define our essential "move" type as an unsigned 16 bit integer to store move information in.
*/
#define move_t uint16_t

/*
we will define some essential FEN strings to initialize our board with
*/
// the starting position of chess.
#define starting_postition_fen "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define empty_board_fen "8/8/8/8/8/8/8/8 w - - 0 1"
// The rest are random positions I obtained from Lichess puzzles.
#define FEN_test_1 "4r3/p7/4p1pp/2Rp4/k7/1R4P1/P4P1P/6K1 b - - 0 37"                    // losing for black
#define FEN_test_2 "5Q2/8/4K3/6r1/8/4k3/8/8 b - - 0 67"                                 // losing for black
#define FEN_test_3 "r3kb1r/1q3ppp/pp2p3/2p1R3/2Pp4/1P1P2P1/PB1N1P1P/R2Q2K1 b kq - 0 17" // losing for black
// extremely bad for black, black should resign if they see this position
#define FEN_test_4 "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"

/******************\
--------------------
   ENUMS
--------------------
\******************/

enum board_squares
{
    a8,
    b8,
    c8,
    d8,
    e8,
    f8,
    g8,
    h8,
    a7,
    b7,
    c7,
    d7,
    e7,
    f7,
    g7,
    h7,
    a6,
    b6,
    c6,
    d6,
    e6,
    f6,
    g6,
    h6,
    a5,
    b5,
    c5,
    d5,
    e5,
    f5,
    g5,
    h5,
    a4,
    b4,
    c4,
    d4,
    e4,
    f4,
    g4,
    h4,
    a3,
    b3,
    c3,
    d3,
    e3,
    f3,
    g3,
    h3,
    a2,
    b2,
    c2,
    d2,
    e2,
    f2,
    g2,
    h2,
    a1,
    b1,
    c1,
    d1,
    e1,
    f1,
    g1,
    h1,
    no_sq
};

int files_to_int[] = {
    ['a'] = 0,
    ['b'] = 1,
    ['c'] = 2,
    ['d'] = 3,
    ['e'] = 4,
    ['f'] = 5,
    ['g'] = 6,
    ['h'] = 7};

/*

ENUM for castling rights.
logic goes like this :

   definition                     binary  decimal
white king can castle kingside   = 0001     1
white king can castle queenside = 0010     2
black king can castle kingside   = 0100     4
white king can castle queenside = 1000     8


castling rights for both can be obtained by using bitwise logic.
for example, 1001 would mean the white king can castle kingside and the black king can castle queenside.

*/
enum castling_rights
{
    wk = 1,
    wq = 2,
    bk = 4,
    bq = 8
};

/*
array for reading castling rights from FEN position.*/
int castle_rights_encoder[] = {
    ['Q'] = wq,
    ['K'] = wk,
    ['q'] = bq,
    ['k'] = bk};
/*
Piece encoding enumeration. The uppercase letters are designated for the white pieces, and the lowercase letters for the black ones.
Notice the pieces are in ascending order of value as knights are worth slightly less than bishops.

*/

enum pieces
{
    P,
    N,
    B,
    R,
    Q,
    K,
    p,
    n,
    b,
    r,
    q,
    k,
    no_piece
};

typedef struct
{
    move_t moves[1028];
    int total_count;
} moves;

/*
An array providing ascii representation of every piece.
It's important to keep this in the same order as our piece encoding enumeration so we can reference it
by index in order to print our board with the proper pieces.
for example unicode_pieces[P] would return P, which is ideal and convienient
*/
char ascii_pieces[12] = "PNBRQKpnbrqk";

int pieces_to_encoded_constant[] = {
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
    ['k'] = k};
enum side_to_move
{
    white,
    black,
    both
};

enum rook_or_bishop
{
    rook,
    bishop
};

const char *square_to_coords[] = {
    "a8",
    "b8",
    "c8",
    "d8",
    "e8",
    "f8",
    "g8",
    "h8",
    "a7",
    "b7",
    "c7",
    "d7",
    "e7",
    "f7",
    "g7",
    "h7",
    "a6",
    "b6",
    "c6",
    "d6",
    "e6",
    "f6",
    "g6",
    "h6",
    "a5",
    "b5",
    "c5",
    "d5",
    "e5",
    "f5",
    "g5",
    "h5",
    "a4",
    "b4",
    "c4",
    "d4",
    "e4",
    "f4",
    "g4",
    "h4",
    "a3",
    "b3",
    "c3",
    "d3",
    "e3",
    "f3",
    "g3",
    "h3",
    "a2",
    "b2",
    "c2",
    "d2",
    "e2",
    "f2",
    "g2",
    "h2",
    "a1",
    "b1",
    "c1",
    "d1",
    "e1",
    "f1",
    "g1",
    "h1",
    "no_sq"};
U64 rook_magic_numbers[64] = {
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
    0x1004081002402ULL};

U64 bishop_magic_numbers[64] = {
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
    0x4010011029020020ULL};

U64 bishop_masks[64];

U64 rook_masks[64];

U64 bishop_attacks[64][512];

U64 rook_attacks[64][4096];
/******************\
--------------------
    BIT BUSINESS
--------------------
\******************/

/* the following constants are derived by printing a bitboatd that excludes the listed files,
then copying the resultant decimal representation of the bitboard using the printBitboard() function.
*/
// not A file
const U64 not_a_file = 18374403900871474942ULL;

// not H file
const U64 not_h_file = 9187201950435737471ULL;

// not HG file
const U64 not_hg_file = 4557430888798830399ULL;

// not AB file
const U64 not_ab_file = 18229723555195321596ULL;

// seventh rank (Used to check if white pawn is on seventh rank for promotion or black pawn can push 2 squares)
const U64 seventh_rank = 65280ULL;

// second rank (Used to check if black pawn can potentially push for promotion or white pawn can push 2 squares)
const U64 second_rank = 71776119061217280ULL;
// eighth rank (Used for white pawn promotion captures)
const U64 eighth_rank = 255ULL;
// first rank (Used for black pawn promotion captures)
const U64 first_rank = 18374686479671623680ULL;

// set bit of a square
// Helpful bit macros!!

#define setBit(bitboard, square) ((bitboard) |= (1ULL << (square)))
// retrive state of bit on bitboard
#define getBit(bitboard, square) ((bitboard) & (1ULL << (square)))

// unset bit of a square
#define popBit(bitboard, square) (getBit(bitboard, square) ? (bitboard) ^= (1ULL << (square)) : 0)

// count set bits on a bitboard
#define countBits(bitboard) __builtin_popcountll(bitboard)

// get index of least significant set bit
#define get_lsb_index(bitboard) __builtin_ctzll(bitboard)

void printBitboard(U64 bitboard) // helpful debugging function to print a given bitboard!
{
    // print files
    printf("    ");
    for (int i = 97; i < 105; i++) // using ascii codes!!
    {
        printf(" %c ", i);
    }
    printf("\n");
    // loop over ranks 1-8
    for (int rank = 0; rank < 8; rank++)
    {
        // loop over files a-h
        for (int file = 0; file < 8; file++)
        {
            // convert rank and file to square index ranging from 0-63
            int square = rank * 8 + file;
            // print ranks

            if (file == 0)
            {
                printf("  %d ", 8 - rank);
            }
            // show bit state
            printf(" %d ", getBit(bitboard, square) ? 1 : 0); // notice the spaces around %d!
        }
        // print newline after each rank
        printf("\n");
    }
    // after loop concludes, print bitboard as decimal for debugging
    printf("  Bitboard as decimal : %llud\n\n\n", bitboard);
}

/******************\
--------------------
     ATTACKS!
--------------------
\******************/

// Pawns

U64 pawn_attacks[2][64]; // we use [2] for the side to move and [64] for the actual bitboard

U64 maskPawnAttacks(int side, int square)
{
    // create then return a bitboard of the resulting squares attacked by the pawn
    U64 attacks = 0ULL;

    // create a bitboard to place our pawn
    U64 bitboard = 0ULL;

    // put our piece on the board
    setBit(bitboard, square);

    // white to play
    if (side == white)
    {

        if ((bitboard >> 7) & not_a_file) // ensure h-pawns do not attack a-file
        {
            attacks |= (bitboard >> 7);
        }
        if ((bitboard >> 9) & not_h_file) // ensure a-pawns do not attack h-file
        {
            attacks |= (bitboard >> 9);
        }
    }
    // black to play
    else
    {
        if ((bitboard << 7) & not_h_file) // ensure a-pawns do not attack h-file
        {
            attacks |= (bitboard << 7);
        }
        if ((bitboard << 9) & not_a_file) // ensure h-pawns do not attack a-file
        {
            attacks |= (bitboard << 9);
        }
    }

    // return our attacks

    return attacks;
}

// Knights
U64 knight_attacks[64];

U64 maskKnightAttacks(int square)
{
    // create then return a bitboard of the resulting squares attacked by the knight
    U64 attacks = 0ULL;

    // create a bitboard to place our knight
    U64 bitboard = 0ULL;

    // put our piece on the board
    setBit(bitboard, square);

    attacks |= (bitboard >> 17) & not_h_file;
    attacks |= (bitboard >> 10) & not_hg_file;
    attacks |= (bitboard >> 6) & not_ab_file;
    attacks |= (bitboard >> 15) & not_a_file;

    attacks |= (bitboard << 17) & not_a_file;
    attacks |= (bitboard << 10) & not_ab_file;
    attacks |= (bitboard << 6) & not_hg_file;
    attacks |= (bitboard << 15) & not_h_file;

    // return our attacks

    return attacks;
}

// Kings
U64 king_attacks[64];

U64 maskKingAttacks(int square)
{
    // create then return a bitboard of the resulting squares attacked by the king
    U64 attacks = 0ULL;

    // create a bitboard to place our king
    U64 bitboard = 0ULL;

    // put our piece on the board
    setBit(bitboard, square);

    // the king attacks 8 squares given none are off board.
    attacks |= (bitboard >> 8);              // top
    attacks |= (bitboard << 8);              // bottom
    attacks |= (bitboard << 1) & not_a_file; // right
    attacks |= (bitboard >> 1) & not_h_file; // left
    attacks |= (bitboard >> 9) & not_h_file; // top left
    attacks |= (bitboard >> 7) & not_a_file; // top right
    attacks |= (bitboard << 7) & not_h_file; // bottom left
    attacks |= (bitboard << 9) & not_a_file; // bottom right

    // return our attacks

    return attacks;
}

// Bishops

// array of relevant occupancy bits for every square
const int bishop_relevant_bits[64] = {
    6,
    5,
    5,
    5,
    5,
    5,
    5,
    6,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    7,
    7,
    7,
    7,
    5,
    5,
    5,
    5,
    7,
    9,
    9,
    7,
    5,
    5,
    5,
    5,
    7,
    9,
    9,
    7,
    5,
    5,
    5,
    5,
    7,
    7,
    7,
    7,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    6,
    5,
    5,
    5,
    5,
    5,
    5,
    6,
};

U64 maskBishopAttacks(int square)
{

    // create then return a bitboard of the resulting squares attacked by the bishop
    U64 attacks = 0ULL;

    // create rank and file integer variables
    int r, f;
    // create target rank and target file
    int tr = square / 8;
    int tf = square % 8;

    // mask relevant occupancy bits (exclude ranks a and h)
    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
    }
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
    }
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
    }
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
    }

    // return our attacks

    return attacks;
}
U64 maskBishopAttacksInstant(int square, U64 blockers) // this variant will generate a bitboard of all squares the bishop attacks, accounting for blocking pieces.
{

    // create then return a bitboard of the resulting squares attacked by the bishop
    U64 attacks = 0ULL;

    // create rank and file integer variables
    int r, f;
    // create target rank and target file
    int tr = square / 8;
    int tf = square % 8;

    // mask attacked bits, STOP when bishop encounters a piece as it can't attack through it!!!
    // diagonal which goes up and to the right
    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if (blockers & (1ULL << (r * 8 + f)))
        {
            break;
        }
    }
    // diagonal which goes down and to the left
    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if (blockers & (1ULL << (r * 8 + f)))
        {
            break;
        }
    }
    // diagonal which goes down and to the right
    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if (blockers & (1ULL << (r * 8 + f)))
        {
            break;
        }
    }
    // diagonal which goes up and to the left
    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if (blockers & (1ULL << (r * 8 + f)))
        {
            break;
        }
    }

    // return our attacks

    return attacks;
}
// Rooks

// array of relevant rook occupancy bits for every square

const int rook_relevant_bits[64] = {
    12,
    11,
    11,
    11,
    11,
    11,
    11,
    12,
    11,
    10,
    10,
    10,
    10,
    10,
    10,
    11,
    11,
    10,
    10,
    10,
    10,
    10,
    10,
    11,
    11,
    10,
    10,
    10,
    10,
    10,
    10,
    11,
    11,
    10,
    10,
    10,
    10,
    10,
    10,
    11,
    11,
    10,
    10,
    10,
    10,
    10,
    10,
    11,
    11,
    10,
    10,
    10,
    10,
    10,
    10,
    11,
    12,
    11,
    11,
    11,
    11,
    11,
    11,
    12,
};

U64 maskRookAttacks(int square)
{
    // create and return attacks bitboard
    U64 attacks = 0ULL;

    // initialize rank and file
    int r, f;

    // initialize target rank and target file
    int tr = square / 8;
    int tf = square % 8;
    // loopers
    for (f = tf + 1; f <= 6; f++)
    {
        attacks |= (1ULL << (tr * 8 + f));
    }
    for (f = tf - 1; f >= 1; f--)
    {
        attacks |= (1ULL << (tr * 8 + f));
    }
    for (r = tr - 1; r >= 1; r--)
    {
        attacks |= (1ULL << (r * 8 + tf));
    }
    for (r = tr + 1; r <= 6; r++)
    {
        attacks |= (1ULL << (r * 8 + tf));
    }
    // return our attacks bitboard
    return attacks;
}
U64 maskRookAttacksInstant(int square, U64 blockers)
{
    // create and return attacks bitboard
    U64 attacks = 0ULL;

    // initialize rank and file
    int r, f;

    // initialize target rank and target file
    int tr = square / 8;
    int tf = square % 8;
    // loopers
    for (f = tf + 1; f <= 7; f++)
    {
        attacks |= (1ULL << (tr * 8 + f));
        if (blockers & (1ULL << (tr * 8 + f)))
        {
            break;
        }
    }
    for (f = tf - 1; f >= 0; f--)
    {
        attacks |= (1ULL << (tr * 8 + f));
        if (blockers & (1ULL << (tr * 8 + f)))
        {
            break;
        }
    }
    for (r = tr - 1; r >= 0; r--)
    {
        attacks |= (1ULL << (r * 8 + tf));
        if (blockers & (1ULL << (r * 8 + tf)))
        {
            break;
        }
    }
    for (r = tr + 1; r <= 7; r++)
    {
        attacks |= (1ULL << (r * 8 + tf));
        if (blockers & (1ULL << (r * 8 + tf)))
        {
            break;
        }
    }
    // return our attacks bitboard
    return attacks;
}
// Kings, Pawns, Knights
void initLeaperAttacks()
{
    // loop all squares
    for (int square = 0; square < 64; square++)
    {
        // fill pawn attacks array
        pawn_attacks[white][square] = maskPawnAttacks(white, square);
        pawn_attacks[black][square] = maskPawnAttacks(black, square);
        // fill knight attacks array
        knight_attacks[square] = maskKnightAttacks(square);
        // fill king attacks array
        king_attacks[square] = maskKingAttacks(square);
    }
}

// Slider Pieces (Bishops, Rooks, Queens!)

// Generate an occupancy possibility
U64 setOccupancy(int index, int bits_in_mask, U64 attacks)
{
    // create a bitboard for occupancy which will later be returned.
    U64 occupancy = 0ULL;

    for (int i = 0; i < bits_in_mask; i++)
    {
        int square = get_lsb_index(attacks);
        popBit(attacks, square);

        if (index & (1ULL << i))
        {
            occupancy |= (1ULL << square);
        }
    }
    return occupancy;
}

/******************\
--------------------
WHERE MAGIC HAPPENS!
--------------------
\******************/
// generate random numbers
unsigned int rand_state = 1804289383;

unsigned int genRandomNumberU32()
{
    // initialize current state as num
    unsigned int num = rand_state;

    // xor shift. left shift 13, right shift 17, left shift 5. to remember, wave hand in these directions and say numbers out loud
    num ^= num << 13;
    num ^= num >> 17;
    num ^= num << 5;

    rand_state = num;
    return rand_state;
}

U64 genRandomNumberU64()
{
    U64 num1, num2, num3, num4;
    num1 = (U64)(genRandomNumberU32()) & 0xFFFF;
    num2 = (U64)(genRandomNumberU32()) & 0xFFFF;
    num3 = (U64)(genRandomNumberU32()) & 0xFFFF;
    num4 = (U64)(genRandomNumberU32()) & 0xFFFF;
    return num1 | (num2 << 16) | (num3 << 32) | (num4 << 48);
}

U64 genRandomMagicNumber()
{
    return genRandomNumberU64() & genRandomNumberU64() & genRandomNumberU64();
}

U64 findMagicNumber(int square, int relevant_bits, int bishop) // a function to generate magic numbers
{
    // createoccupancy, attacks,and used attacks arrays
    U64 occupancies[4096];

    U64 attacks[4096];
    U64 used_attacks[4096];

    // is the piece a bishop or a rook, that is, is bishop equal to 1(true) or 0(false)? use the appropriate mask generation function.
    U64 attack_mask = bishop ? maskBishopAttacks(square) : maskRookAttacks(square);

    // the amount of occupancy indicies we will need corresponds to the count of blockers!
    // notice the max amount of blockers a rook can have is 12, and 2^12 = 4096. This is why the arrays cap out at 4096.
    int occupancy_indicies_count = 1 << relevant_bits;

    // fill occupancies and attacks!
    for (int i = 0; i < occupancy_indicies_count; i++)
    {
        occupancies[i] = setOccupancy(i, relevant_bits, attack_mask);

        attacks[i] = bishop ? maskBishopAttacksInstant(square, occupancies[i]) : maskRookAttacksInstant(square, occupancies[i]);
    }

    // test magic numbers
    for (int i = 0; i < 100000000; i++)
    {
        U64 magic_num = genRandomMagicNumber();

        /*
        this is a heuristic test to speed up finding magics!
        we skip magic numbers which will have less than 6 bits for the index
        these numbers tend to be too sparse and bad candidates
        */
        if (countBits((attack_mask * magic_num) & 0xFF00000000000000) < 6)
            continue;
        // fill used attacks with 0's of type unsigned long long(which is also our U64) using memset() from <string.h>
        memset(used_attacks, 0ULL, sizeof(used_attacks));

        // create flags for index and failure
        int index, fail;

        // loop to test magic index

        for (index = 0, fail = 0; !fail && index < occupancy_indicies_count; index++)
        {
            // create the magic index using multiply right shift.
            int magic_index = (int)((occupancies[index] * magic_num) >> (64 - relevant_bits));

            // if the index works?
            if (used_attacks[magic_index] == 0ULL) // notice this means there is no collision since used_attacks is initialized with 0
            {
                // fill used attacks with appropriate entry from attacks
                used_attacks[magic_index] = attacks[index];
            }
            else if (used_attacks[magic_index] != attacks[index]) // magic index didn't work :(
            {
                fail = 1; // we have failed... end the loop...
            }
        }
        if (!fail) // the loop concluded and we never failed. the magic number is found!
        {
            return magic_num;
        }
    }
    // if all of that happened and we got no magic number whatsoever
    printf("Magic number was a fail :(\n )");
    return 0ULL; // keep in mind this function returns U64, which is the same as unsigned long long!
}

void initMagicNumbers()
{
    for (int square = 0; square < 64; square++)
    {
        rook_magic_numbers[square] = findMagicNumber(square, rook_relevant_bits[square], rook);
        bishop_magic_numbers[square] = findMagicNumber(square, rook_relevant_bits[square], bishop);
    }
}

void initSliderAttacks(int bishop)
{
    for (int square = 0; square < 64; square++)
    {
        bishop_masks[square] = maskBishopAttacks(square);
        rook_masks[square] = maskRookAttacks(square);

        U64 attack_mask = bishop ? bishop_masks[square] : rook_masks[square];

        int relevant_bits_count = countBits(attack_mask);

        int occupancy_indicies_count = (1 << relevant_bits_count);
        for (int index = 0; index < occupancy_indicies_count; index++)
        {
            // bishop
            if (bishop)
            {
                // init current occupancy variation
                U64 occupancy = setOccupancy(index, relevant_bits_count, attack_mask);

                // init magic index
                int magic_index = (occupancy * bishop_magic_numbers[square]) >> (64 - bishop_relevant_bits[square]);

                // init bishop attacks
                bishop_attacks[square][magic_index] = maskBishopAttacksInstant(square, occupancy);
            }

            // rook
            else
            {
                // init current occupancy variation
                U64 occupancy = setOccupancy(index, relevant_bits_count, attack_mask);

                // init magic index
                int magic_index = (occupancy * rook_magic_numbers[square]) >> (64 - rook_relevant_bits[square]);

                // init rook attacks
                rook_attacks[square][magic_index] = maskRookAttacksInstant(square, occupancy);
            }
        }
    }
}

static inline U64 genBishopAttacks(U64 occupancy, int square)
{
    occupancy &= bishop_masks[square];
    occupancy *= bishop_magic_numbers[square];
    occupancy >>= 64 - bishop_relevant_bits[square];
    return bishop_attacks[square][occupancy];
}
static inline U64 genRookAttacks(U64 occupancy, int square)
{
    occupancy &= rook_masks[square];
    occupancy *= rook_magic_numbers[square];
    occupancy >>= 64 - rook_relevant_bits[square];
    return rook_attacks[square][occupancy];
}

static inline U64 genQueenAttacks(U64 occupancy, int square)
{
    return (genRookAttacks(occupancy, square)) | (genBishopAttacks(occupancy, square));
}
/******************\
--------------------
Piece Bitboards/Flags
--------------------
\******************/

U64 piece_bitboards[12];

U64 occupancy_bitboards[3];

int piece_on_square[64]; // an array I implemented later on to use a lookup table for find a piece for making moves.

int side = 0;

int en_passant = no_sq;

int castle = 0;

int half_moves = 0;

int full_moves = 0;

typedef struct
{
    U64 piece_bitboards_undo[12];
    U64 occupancy_bitboards_undo[3];
    int piece_on_square_undo[64];
    int side_undo;
    int en_passant_undo;
    int castle_undo;
    int half_moves_undo;
    int full_moves_undo;
} Undo;

Undo undo_move; // create an instance of the struct type defined above

void printBoard()
{
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            int square = rank * 8 + file;
            int piece = -1;
            // print rank
            if (file == 0)
            {
                printf("  %d ", 8 - rank);
            }
            for (int piece_on_bitboard = P; piece_on_bitboard < 12; piece_on_bitboard++)
            {
                if (getBit(piece_bitboards[piece_on_bitboard], square))
                    piece = piece_on_bitboard;
            }
            printf("  %c", (piece == -1) ? '.' : ascii_pieces[piece]);
        }
        printf("\n");
    }
    // print files
    printf("     ");
    for (int i = 97; i < 105; i++) // using ascii codes!!
    {
        printf(" %c ", i);
    }

    printf("\n      %s to play\n", (!side) ? "white" : "black");
    printf("\n      en passant: %s\n", (en_passant != no_sq) ? square_to_coords[en_passant] : "none");
    printf("      castling rights : %c%c%c%c\n", (castle & wk) ? 'K' : '-',
           (castle & wq) ? 'Q' : '-',
           (castle & bk) ? 'k' : '-',
           (castle & bq) ? 'q' : '-');
    printf("      half moves : %d\n", half_moves);
    printf("      full moves: %d\n", full_moves);
}
static inline int isSquareAttacked(int square, int side)
{
    // white pieces
    if (side == white)
    {
        // check if white pawn attacks the square
        if (pawn_attacks[black][square] & piece_bitboards[P])
            return 1;
        // check if white knight attacks the square
        if (knight_attacks[square] & piece_bitboards[N])
            return 1;
        // check if white bishop attacks the square
        if ((genBishopAttacks(occupancy_bitboards[both], square)) & (piece_bitboards[B]))
            return 1;
        // check if white rook attacks the square
        if ((genRookAttacks(occupancy_bitboards[both], square)) & piece_bitboards[R])
            return 1;
        // check if white queen attacks the square
        if ((genQueenAttacks(occupancy_bitboards[both], square)) & piece_bitboards[Q])
            return 1;
    }
    else
    {
        // check if black pawn attacks the square
        if (pawn_attacks[white][square] & piece_bitboards[p])
            return 1;
        // check if black knight attacks the square
        if (knight_attacks[square] & piece_bitboards[n])
            return 1;
        // check if black bishop attacks the square
        if ((genBishopAttacks(occupancy_bitboards[both], square)) & (piece_bitboards[b]))
            return 1;
        // check if black rook attacks the square
        if ((genRookAttacks(occupancy_bitboards[both], square)) & piece_bitboards[r])
            return 1;
        // check if black queen attacks the square
        if ((genQueenAttacks(occupancy_bitboards[both], square)) & piece_bitboards[q])
            return 1;
    }
    return 0; // default return value
}
void printAttackedSquares(int side)
{
    // print files
    printf("    ");
    for (int i = 97; i < 105; i++) // using ascii codes!!
    {
        printf(" %c ", i);
    }
    printf("\n");
    // loop over ranks 1-8
    for (int rank = 0; rank < 8; rank++)
    {
        // loop over files a-h
        for (int file = 0; file < 8; file++)
        {
            // convert rank and file to square index ranging from 0-63
            int square = rank * 8 + file;
            // print ranks

            if (file == 0)
            {
                printf("  %d ", 8 - rank);
            }
            // show bit state
            printf(" %d ", isSquareAttacked(square, side)); // notice the spaces around %d!
        }
        // print newline after each rank
        printf("\n");
    }
}

/******************\
--------------------
   Move Generation
--------------------
\******************/

// macros
/*
    To encode moves, I decided to use 16 bit move representation as it was shown to me on chessprogramming.org
    0000 0000 00[00 0000]
                these bits encode the source square of the move, 6 bits means 2^6 = 64 possible squares which matches perfectly


    0000 [0000 00]00 0000
        these bits encode the target square of the move


    [0000] 0000 0000 0000
    special flag bits
     Bit 1                   Bit 2          Bit 3               Bit 4
    Promotion               Capture     Special Bit 1         Special Bit 2
    full details can be found on https://www.chessprogramming.org/Encoding_Moves#From-To_Based
*/
#define encodeMove(from, to, promo, capture, s2, s1) (from) | ((to) << 6) | ((s1) << 12) | ((s2) << 13) | ((capture) << 14) | ((promo) << 15)
/*
The following macros get information from a given move using bitwise logic

Bitwise and to extract the only relevant bits in the given context

Right shift to reduce the bits to the proper amount
*/
#define getSourceSq(move) (move & 0b0000000000111111)
#define getTargetSq(move) ((move & 0b0000111111000000) >> 6)
#define getFlags(move) ((move & 0b1111000000000000) >> 12)

void addMove(move_t move, moves *move_list)
{

    move_list->moves[move_list->total_count] = move;

    move_list->total_count++;
}

/*
Like it has usually been done some other times within this code, I decided to create a printMove() function for debugging purposes
*/
void printMoveUCI(move_t move) // print just the UCI-compliant move string (not yet supporting promotion)
{
    printf("\nString to send to UCI: %s%s\n", square_to_coords[getSourceSq(move)], square_to_coords[getTargetSq(move)]);
}
void printMove(move_t move)
{
    // print source square of move
    printf("Source square : %s\n", square_to_coords[getSourceSq(move)]);
    // print target square of move
    printf("Target square : %s\n", square_to_coords[getTargetSq(move)]);
    // print whether move was capture or not
    printf("Capture : %s\n", (getFlags(move) & 0b0100) ? "yes" : "no");
    // print whether move was en-passant capture or not
    printf("En-passant capture : %s\n", ((getFlags(move) & 0b0100) && (getFlags(move) & 0b0001)) ? "yes" : "no");
    // print whether or not move is a promotion
    printf("Promotion : %s\n", (move & 0b1000) ? "yes" : "no");
    // print move as decimal and binary
    printf("Move as decimal : %d\n", move);
    printf("Move as binary :  ");
    for (int i = 15; i >= 0; i--)
    {
        printf("%d", (getBit(move, i)) ? 1 : 0);
    }
    printf("\nString to send to UCI: %s%s\n", square_to_coords[getSourceSq(move)], square_to_coords[getTargetSq(move)]);
}
// created a function to print movelists for debugging aswell.
void printMoveList(moves *move_list)
{
    printf("list of moves : [");
    for (int i = 0; i < move_list->total_count; i++)
    {
        printf(" %d : %s%s, ", i, square_to_coords[getSourceSq(move_list->moves[i])], square_to_coords[getTargetSq(move_list->moves[i])]);
        if (((i + 1) % 17) == 0) // seperate lines every 16 moves
        {
            printf("\n");
        }
    }
    printf("]\n");
}

static inline void genMoves(moves *move_list)
{
    move_list->total_count = 0;
    int start_square, target_square, double_pawn_push_square, attacked_square;
    U64 bitboard, attacks;
    for (int piece = P; piece <= k; piece++)
    {
        bitboard = piece_bitboards[piece];
        // generating white pawn moves and white king castling
        if (side == white)
        {

            if (piece == P) // generate pawn moves
            {
                // generate quiet(non capture ) pawn moves
                while (bitboard)
                {
                    start_square = get_lsb_index(bitboard);
                    target_square = start_square - 8;
                    double_pawn_push_square = start_square - 16;
                    if ((1ULL << start_square) & seventh_rank && !((1ULL << target_square) & occupancy_bitboards[both])) // white pawn is on rank 7 and can therefore promote
                    {

                        move_t knight_promo = encodeMove(start_square, target_square, 1, 0, 0, 0);
                        move_t bishop_promo = encodeMove(start_square, target_square, 1, 0, 0, 1);
                        move_t rook_promo = encodeMove(start_square, target_square, 1, 0, 1, 0);
                        move_t queen_promo = encodeMove(start_square, target_square, 1, 0, 1, 1);
                        addMove(knight_promo, move_list);
                        addMove(bishop_promo, move_list);
                        addMove(rook_promo, move_list);
                        addMove(queen_promo, move_list);
                    }
                    else if ((1ULL << start_square) & second_rank && !((1ULL << target_square) & occupancy_bitboards[both]))
                    {
                        // add double pawn push if double push square is not occupied
                        if (!((1ULL << double_pawn_push_square) & occupancy_bitboards[both]))
                        {
                            move_t double_push = encodeMove(start_square, double_pawn_push_square, 0, 0, 0, 1);
                            addMove(double_push, move_list);
                        }

                        // add single pawn push
                        move_t single_push = encodeMove(start_square, target_square, 0, 0, 0, 0);
                        addMove(single_push, move_list);
                    }
                    else if (!((1ULL << target_square) & occupancy_bitboards[both]))
                    {
                        move_t single_push = encodeMove(start_square, target_square, 0, 0, 0, 0);
                        addMove(single_push, move_list);
                    }

                    attacks = pawn_attacks[white][start_square] & (occupancy_bitboards[black] | (en_passant != no_sq ? (1ULL << en_passant) : 0ULL));
                    // generate white pawn attacks
                    while (attacks)
                    {
                        attacked_square = get_lsb_index(attacks);
                        if ((1ULL << attacked_square) & eighth_rank)
                        {
                            move_t knight_promo_capture = encodeMove(start_square, attacked_square, 1, 1, 0, 0);
                            move_t bishop_promo_capture = encodeMove(start_square, attacked_square, 1, 1, 0, 1);
                            move_t rook_promo_capture = encodeMove(start_square, attacked_square, 1, 1, 1, 0);
                            move_t queen_promo_capture = encodeMove(start_square, attacked_square, 1, 1, 1, 1);
                            addMove(knight_promo_capture, move_list);
                            addMove(bishop_promo_capture, move_list);
                            addMove(rook_promo_capture, move_list);
                            addMove(queen_promo_capture, move_list);
                        }
                        else if (attacked_square == en_passant)
                        {
                            move_t ep_capture = encodeMove(start_square, en_passant, 0, 1, 0, 1);
                            addMove(ep_capture, move_list);
                        }

                        else
                        {
                            move_t capture = encodeMove(start_square, attacked_square, 0, 1, 0, 0);
                            addMove(capture, move_list);
                        }
                        popBit(attacks, attacked_square);
                    }
                    popBit(bitboard, start_square);
                }
            }
            if (piece == K) // generate castling
            {
                if (castle & wk)
                {
                    if (!getBit(occupancy_bitboards[both], f1) && !(getBit(occupancy_bitboards[both], g1)))
                    {
                        if (!isSquareAttacked(e1, black) && !isSquareAttacked(f1, black) && !isSquareAttacked(g1, black))
                        {
                            move_t white_kingside_castle = encodeMove(e1, g1, 0, 0, 1, 0);
                            addMove(white_kingside_castle, move_list);
                        }
                    }
                }
                if (castle & wq)
                {
                    if (!getBit(occupancy_bitboards[both], d1) && !(getBit(occupancy_bitboards[both], c1)) && !(getBit(occupancy_bitboards[both], b1)))
                    {
                        if (!isSquareAttacked(e1, black) && !isSquareAttacked(d1, black) && !isSquareAttacked(c1, black) && !isSquareAttacked(b1, black))
                        {
                            move_t white_queenside_castle = encodeMove(e1, c1, 0, 0, 1, 1);
                            addMove(white_queenside_castle, move_list);
                        }
                    }
                }
            }
        }
        else // generating black pawn moves and black king castling moves
        {
            if (piece == p) // generate pawn moves
            {
                // generate quiet(non capture ) pawn moves
                while (bitboard)
                {
                    start_square = get_lsb_index(bitboard);
                    target_square = start_square + 8;
                    double_pawn_push_square = start_square + 16;
                    if ((1ULL << start_square) & second_rank && !((1ULL << target_square) & occupancy_bitboards[both])) // black pawn is on rank 2 and can therefore promote
                    {

                        move_t knight_promo = encodeMove(start_square, target_square, 1, 0, 0, 0);
                        move_t bishop_promo = encodeMove(start_square, target_square, 1, 0, 0, 1);
                        move_t rook_promo = encodeMove(start_square, target_square, 1, 0, 1, 0);
                        move_t queen_promo = encodeMove(start_square, target_square, 1, 0, 1, 1);
                        addMove(knight_promo, move_list);
                        addMove(bishop_promo, move_list);
                        addMove(rook_promo, move_list);
                        addMove(queen_promo, move_list);
                    }
                    else if ((1ULL << start_square) & seventh_rank && !((1ULL << target_square) & occupancy_bitboards[both])) // black pawn is on second rank and can double push
                    {
                        // add double pawn push if double push square is not occupied
                        if (!((1ULL << double_pawn_push_square) & occupancy_bitboards[both]))
                        {
                            move_t double_push = encodeMove(start_square, double_pawn_push_square, 0, 0, 0, 1);
                            addMove(double_push, move_list);
                        }

                        // add single pawn push
                        move_t single_push = encodeMove(start_square, target_square, 0, 0, 0, 0);
                        addMove(single_push, move_list);
                    }
                    else if (!((1ULL << target_square) & occupancy_bitboards[both]))
                    {
                        move_t single_push = encodeMove(start_square, target_square, 0, 0, 0, 0);
                        addMove(single_push, move_list);
                    }

                    attacks = pawn_attacks[black][start_square] & (occupancy_bitboards[white] | (en_passant != no_sq ? (1ULL << en_passant) : 0ULL));
                    // generate white pawn attacks
                    while (attacks)
                    {
                        attacked_square = get_lsb_index(attacks);
                        if ((1ULL << attacked_square) & first_rank) // promo capture
                        {
                            move_t knight_promo_capture = encodeMove(start_square, attacked_square, 1, 1, 0, 0);
                            move_t bishop_promo_capture = encodeMove(start_square, attacked_square, 1, 1, 0, 1);
                            move_t rook_promo_capture = encodeMove(start_square, attacked_square, 1, 1, 1, 0);
                            move_t queen_promo_capture = encodeMove(start_square, attacked_square, 1, 1, 1, 1);
                            addMove(knight_promo_capture, move_list);
                            addMove(bishop_promo_capture, move_list);
                            addMove(rook_promo_capture, move_list);
                            addMove(queen_promo_capture, move_list);
                        }
                        else if (attacked_square == en_passant)
                        {
                            move_t ep_capture = encodeMove(start_square, en_passant, 0, 1, 0, 1);
                            addMove(ep_capture, move_list);
                        }

                        else
                        {
                            move_t capture = encodeMove(start_square, attacked_square, 0, 1, 0, 0);
                            addMove(capture, move_list);
                        }
                        popBit(attacks, attacked_square);
                    }
                    popBit(bitboard, start_square);
                }
            }
            if (piece == k) // generate castling
            {
                if (castle & bk)
                {
                    if (!getBit(occupancy_bitboards[both], f8) && !(getBit(occupancy_bitboards[both], g8)))
                    {
                        if (!isSquareAttacked(e8, white) && !isSquareAttacked(f8, white) && !isSquareAttacked(g8, white))
                        {
                            move_t black_kingside_castle = encodeMove(e8, g8, 0, 0, 1, 0);
                            addMove(black_kingside_castle, move_list);
                        }
                    }
                }
                if (castle & bq)
                {
                    if (!getBit(occupancy_bitboards[both], d8) && !(getBit(occupancy_bitboards[both], c8)) && !(getBit(occupancy_bitboards[both], b8)))
                    {
                        if (!isSquareAttacked(e8, white) && !isSquareAttacked(d8, white) && !isSquareAttacked(c8, white) && !isSquareAttacked(b8, white))
                        {
                            move_t black_queenside_castle = encodeMove(e8, c8, 0, 0, 1, 1);
                            addMove(black_queenside_castle, move_list);
                        }
                    }
                }
            }
        }
        if ((side == white) ? piece == N : piece == n) // gen knight moves
        {
            bitboard = (side == white) ? piece_bitboards[N] : piece_bitboards[n];
            while (bitboard)
            {
                start_square = get_lsb_index(bitboard);
                attacks = knight_attacks[start_square] & ~((side == white) ? occupancy_bitboards[white] : occupancy_bitboards[black]);
                while (attacks)
                {
                    attacked_square = get_lsb_index(attacks);
                    if ((1ULL << attacked_square) & ((side == white) ? occupancy_bitboards[black] : occupancy_bitboards[white])) // move is a capture
                    {
                        move_t knight_capture = encodeMove(start_square, attacked_square, 0, 1, 0, 0);
                        addMove(knight_capture, move_list);
                    }
                    else // move is a quiet move
                    {
                        move_t knight_move = encodeMove(start_square, attacked_square, 0, 0, 0, 0);
                        addMove(knight_move, move_list);
                    }
                    popBit(attacks, attacked_square);
                }
                popBit(bitboard, start_square);
            }
        }
        if ((side == white) ? piece == B : piece == b) // generating bishop moves
        {
            bitboard = (side == white) ? piece_bitboards[B] : piece_bitboards[b];
            while (bitboard)
            {
                start_square = get_lsb_index(bitboard);
                attacks = genBishopAttacks(occupancy_bitboards[both], start_square) & ~((side == white) ? occupancy_bitboards[white] : occupancy_bitboards[black]);
                while (attacks)
                {
                    attacked_square = get_lsb_index(attacks);
                    if ((1ULL << attacked_square) & ((side == white) ? occupancy_bitboards[black] : occupancy_bitboards[white]))
                    {
                        move_t bishop_capture = encodeMove(start_square, attacked_square, 0, 1, 0, 0);
                        addMove(bishop_capture, move_list);
                    }
                    else
                    {
                        move_t bishop_move = encodeMove(start_square, attacked_square, 0, 0, 0, 0);
                        addMove(bishop_move, move_list);
                    }
                    popBit(attacks, attacked_square);
                }
                popBit(bitboard, start_square);
            }
        }

        if ((side == white) ? piece == R : piece == r) // generating rook moves
        {
            bitboard = (side == white) ? piece_bitboards[R] : piece_bitboards[r];
            while (bitboard)
            {
                start_square = get_lsb_index(bitboard);
                attacks = genRookAttacks(occupancy_bitboards[both], start_square) & ~((side == white) ? occupancy_bitboards[white] : occupancy_bitboards[black]);
                while (attacks)
                {
                    attacked_square = get_lsb_index(attacks);
                    if ((1ULL << attacked_square) & ((side == white) ? occupancy_bitboards[black] : occupancy_bitboards[white]))
                    {
                        move_t rook_capture = encodeMove(start_square, attacked_square, 0, 1, 0, 0);
                        addMove(rook_capture, move_list);
                    }
                    else
                    {
                        move_t rook_move = encodeMove(start_square, attacked_square, 0, 0, 0, 0);
                        addMove(rook_move, move_list);
                    }
                    popBit(attacks, attacked_square);
                }
                popBit(bitboard, start_square);
            }
        }

        // generating queen moves
        if ((side == white) ? piece == Q : piece == q)
        {
            bitboard = (side == white) ? piece_bitboards[Q] : piece_bitboards[q];
            while (bitboard)
            {
                start_square = get_lsb_index(bitboard);
                attacks = genQueenAttacks(occupancy_bitboards[both], start_square) & ~((side == white) ? occupancy_bitboards[white] : occupancy_bitboards[black]);
                while (attacks)
                {
                    attacked_square = get_lsb_index(attacks);
                    if ((1ULL << attacked_square) & ((side == white) ? occupancy_bitboards[black] : occupancy_bitboards[white]))
                    {
                        move_t queen_capture = encodeMove(start_square, attacked_square, 0, 1, 0, 0);
                        addMove(queen_capture, move_list);
                    }
                    else
                    {
                        move_t queen_move = encodeMove(start_square, attacked_square, 0, 0, 0, 0);
                        addMove(queen_move, move_list);
                    }
                    popBit(attacks, attacked_square);
                }
                popBit(bitboard, start_square);
            }
        }

        // generating king moves
        if ((side == white) ? piece == K : piece == k)
        {
            bitboard = (side == white) ? piece_bitboards[K] : piece_bitboards[k];
            while (bitboard)
            {
                start_square = get_lsb_index(bitboard);
                attacks = king_attacks[start_square] & ~((side == white) ? occupancy_bitboards[white] : occupancy_bitboards[black]);
                while (attacks)
                {
                    attacked_square = get_lsb_index(attacks);
                    if ((1ULL << attacked_square) & ((side == white) ? occupancy_bitboards[black] : occupancy_bitboards[white]))
                    {
                        move_t king_capture = encodeMove(start_square, attacked_square, 0, 1, 0, 0);
                        addMove(king_capture, move_list);
                    }
                    else
                    {
                        move_t king_move = encodeMove(start_square, attacked_square, 0, 0, 0, 0);
                        addMove(king_move, move_list);
                    }
                    popBit(attacks, attacked_square);
                }
                popBit(bitboard, start_square);
            }
        }
    }
}
/******************\
--------------------
   Make/Unmake Moves
--------------------
\******************/
/*
In this code section, functions will be created for making and unmaking moves on the board so we can later implement them within the
search function.
*/
int isIllegalPosition() // helper function to check if current board position is illegal.
{

    if (!countBits(piece_bitboards[k]) || !countBits(piece_bitboards[K]))
    {
        return 1;
    }
    int enemyKingSquare = get_lsb_index((side == white) ? piece_bitboards[k] : piece_bitboards[K]);
    if (isSquareAttacked(enemyKingSquare, side))
    {
        return 1;
    }
    return 0;
}
void unmakeMove()
{
    // use the saved variables in the undo struct to restore saved game state.
    memcpy(piece_bitboards, undo_move.piece_bitboards_undo, sizeof(piece_bitboards));
    memcpy(occupancy_bitboards, undo_move.occupancy_bitboards_undo, sizeof(occupancy_bitboards));
    memcpy(piece_on_square, undo_move.piece_on_square_undo, sizeof(piece_on_square));
    castle = undo_move.castle_undo;
    en_passant = undo_move.en_passant_undo;
    full_moves = undo_move.full_moves_undo;
    half_moves = undo_move.half_moves_undo;
    side = undo_move.side_undo;
}

int makeMove(move_t move)
{
    // save current game state in undo struct
    memcpy(undo_move.piece_bitboards_undo, piece_bitboards, sizeof(piece_bitboards));
    memcpy(undo_move.occupancy_bitboards_undo, occupancy_bitboards, sizeof(occupancy_bitboards));
    memcpy(undo_move.piece_on_square_undo, piece_on_square, sizeof(piece_on_square));
    undo_move.castle_undo = castle;
    undo_move.en_passant_undo = en_passant;
    undo_move.full_moves_undo = full_moves;
    undo_move.half_moves_undo = half_moves;
    undo_move.side_undo = side;

    // extract neccesary info from move passed to function
    int from = getSourceSq(move);
    int piece = piece_on_square[from];
    // printf("piece in question : %d\n", piece);
    int to = getTargetSq(move);
    int captured_piece = piece_on_square[to];
    int flags = getFlags(move);
    int capture = 0;
    if (piece == no_piece) // no piece is on from square which doesn't make sense, makeMove fails.
    {
        return 0;
    }

    // update piece bitboards according to move
    if (piece_on_square[to] != no_piece) // move is a capture, need to pop bit from opposing side piece and occupancy bitboard
    {
        capture = 1;
        popBit(piece_bitboards[piece], from);

        popBit(occupancy_bitboards[side], from);
        piece_on_square[from] = no_piece;
        setBit(piece_bitboards[piece], to);
        setBit(occupancy_bitboards[side], to);
        popBit(occupancy_bitboards[!side], to);
        piece_on_square[to] = piece;
        popBit(piece_bitboards[captured_piece], to);
        occupancy_bitboards[both] = occupancy_bitboards[white] | occupancy_bitboards[black];
    }
    else // move is a quiet move or en-passant
    {
        // use XOR to unset bit of square piece moves from on both piece and occupancy bitboard
        popBit(piece_bitboards[piece], from);
        popBit(occupancy_bitboards[side], from);
        // update the square to have no_piece on it in piece_on_square[64] array
        piece_on_square[from] = no_piece;
        // use OR to set bit of to square on in both the piece bitboard of the piece and the occupancy bitboard
        setBit(piece_bitboards[piece], to);
        setBit(occupancy_bitboards[side], to);
        // update piece_on_square[to] to be the piece which moves
        piece_on_square[to] = piece;
        if (flags == 0b0101) // move is an en-passant capture, need to remove enemy pawn which was captured.
        {
            int captured_pawn_square = (side == white) ? to + 8 : to - 8;
            popBit(occupancy_bitboards[!side], captured_pawn_square);
            popBit(piece_bitboards[(side == white) ? p : P], captured_pawn_square);
        }
        if (flags == 0b0010) // move is a kingside castle, king already moved. need to move the rook aswell
        {

            // initialize variables for the rook based on side to move
            int castle_piece = (side == white) ? R : r;
            int castle_from = (side == white) ? h1 : h8;
            int castle_to = (side == white) ? f1 : f8;
            // unset bits of rook from piece and occupancy bitboards, and update piece_on_square aswell
            popBit(piece_bitboards[castle_piece], castle_from);
            popBit(occupancy_bitboards[side], castle_from);
            piece_on_square[castle_from] = no_piece;
            // set bits of to square
            setBit(piece_bitboards[castle_piece], castle_to);
            setBit(occupancy_bitboards[side], castle_to);

            piece_on_square[castle_to] = castle_piece;
        }
        else if (flags == 0b0011) // queenside castle
        {
            // same process as kingside castling, just for queenside
            int castle_piece = (side == white) ? R : r;
            int castle_from = (side == white) ? a1 : a8;
            int castle_to = (side == white) ? d1 : d8;
            popBit(piece_bitboards[castle_piece], castle_from);
            popBit(occupancy_bitboards[side], castle_from);

            piece_on_square[castle_from] = no_piece;

            setBit(piece_bitboards[castle_piece], castle_to);
            setBit(occupancy_bitboards[side], castle_to);

            piece_on_square[castle_to] = castle_piece;
        }
        // update occupancies of both with OR
        occupancy_bitboards[both] = occupancy_bitboards[white] | occupancy_bitboards[black];
    }
    if (flags & 0b1000) // promotion occured, extra handling is needed
    {
        int promo_piece;
        switch (flags)
        {
        case 0b1000:
            promo_piece = (side == white) ? N : n;
            break;
        case 0b1001:
            promo_piece = (side == white) ? B : b;
            break;
        case 0b1010:
            promo_piece = (side == white) ? R : r;
            break;
        case 0b1011:
            promo_piece = (side == white) ? Q : q;
            break;
        case 0b1100:
            promo_piece = (side == white) ? N : n;
            break;
        case 0b1101:
            promo_piece = (side == white) ? B : b;
            break;
        case 0b1110:
            promo_piece = (side == white) ? R : r;
            break;
        case 0b1111:
            promo_piece = (side == white) ? Q : q;
            break;
        }
        popBit(piece_bitboards[(side == white) ? P : p], to);
        piece_on_square[from] = no_piece;
        setBit(piece_bitboards[promo_piece], to);
        piece_on_square[to] = promo_piece;
    }
    // update game state variables according to move

    // update castling
    /*
     Personal thinking here :
     Castling rights can only be lost, not gained
     The four ways in which they can be lost are :
     1. The act of castling itself
     2. Moving your king
     3. Moving a rook, losing you the castling rights of the corresponding side the rook was on
     4. Your rook being captured
     All of these checks should be implemented, updating castling rights using XOR.
    */
    if (castle)
    {
        if (side == white)
        {
            // this checks for 1 and 2 since castling is encoded as the king moving from e1 to either g1 or c1
            if (piece == K) // king was moved, remove rights for both kingside and queenside
            {
                castle ^= wk;
                castle ^= wq;
            }
            // check for 3
            if (piece == R) // rook moved
            {
                if (from == h1) // kingside rook moved, remove kingside castling rights
                {
                    castle ^= wk;
                }
                else if (from == a1) // queenside rook moved, remove queenside castling rights
                {
                    castle ^= wq;
                }
            }
            // check for 4
            if (to == h8 && captured_piece == r)
            {
                castle ^= bk;
            }
            else if (to == a8 && captured_piece == r)
            {
                castle ^= bq;
            }
        }
        else
        {

            if (piece == k) // king was moved, remove rights for both kingside and queenside
            {
                castle ^= bk;
                castle ^= bq;
            }
            // check for 3
            if (piece == r) // rook moved
            {
                if (from == h8) // kingside rook moved, remove kingside castling rights
                {
                    castle ^= bk;
                }
                else if (from == a8) // queenside rook moved, remove queenside castling rights
                {
                    castle ^= bq;
                }
            }
            // check for 4
            if (to == h1 && captured_piece == R)
            {
                castle ^= wk;
            }
            else if (to == a1 && captured_piece == R)
            {
                castle ^= wq;
            }
        }
    }
    // update en passant

    if ((piece == P || piece == p) && (flags == 0b0001))
    {

        en_passant = (side == white) ? from - 8 : from + 8;
    }
    else if (en_passant != no_sq)
    {
        en_passant = no_sq;
    }

    // update fullmoves
    if (side == black)
    {
        full_moves++;
    }

    // update halfmoves
    if (piece == P || piece == p || capture)
    {
        half_moves = 0;
    }
    else
    {
        half_moves++;
    }

    // flip side to move
    side ^= 1;
    if (isIllegalPosition()) // resulting position from making move is illegal, undo the move and return 0.
    {
        unmakeMove();
        return 0;
    }
    return 1;
}

int isIllegalMove(move_t move) // try a move and test for legality
{
    if (!makeMove(move))
    {
        return 1;
    }
    unmakeMove();
    return 0;
}
/******************\
--------------------
    Evaluation
--------------------
\******************/
int pieceValue[12] = {
    1,
    3,
    3,
    5,
    9,
    10000,
    1,
    3,
    3,
    5,
    9,
    10000};
int pieceScore()
{
    int score = 0;
    for (int piece = P; piece <= K; piece++)
    {
        score += countBits(piece_bitboards[piece]) * pieceValue[piece];
    }
    for (int piece = p; piece <= k; piece++)
    {
        score -= countBits(piece_bitboards[piece]) * pieceValue[piece];
    }
    return score;
}
int evaluate()
{
    int eval = 0;
    eval += pieceScore();
    //  printf("evaluation %d negative evaluation %d\n", eval, -eval); //debug line
    return (side == white) ? eval : -eval;
}
/******************\
--------------------
       Search
--------------------
\******************/

/*
    the main function implementing the search algorithm, based off the negamax algorithm
    with alpha beta enhancements.

    My understanding of the parameters :
    alpha : the minimum score that the maximizing player is assured of
    beta : the maximum score that the minimizing player is assured of
    depth : the maximum depth of the search tree

    Typically, at the grandmaster level of play, white is the maximizing player playing for the win
    and black is the minimizing player playing to equalize the position.

    Therefore, lets assume white is the maximizing player in the algorithm and black is the minimizer.

    Alpha is the minimizing score white is assured of, so if we find a better score for white, we must
    update our minimum score for our maximizing player.

    On the other hand, if we are searching moves for black and find a move that produces a higher score
    for white (i.e +5 vs +3), we already know the maximum score we can get is +3 as that's better than +5
    for us as black. So we cutoff the branch and search no further.
*/
int negaMax(int alpha, int beta, int depth) // main function described above
{
    if (depth == 0)
    {
        return evaluate();
    }
    int best_score = -INT_MAX;

    moves move_list;
    genMoves(&move_list);
    for (int i = 0; i < move_list.total_count; i++)
    {

        if (!makeMove(move_list.moves[i])) // illegal move, try next move
        {
            continue;
        }

        int score = -negaMax(-beta, -alpha, depth - 1); // recurse
        unmakeMove();
        if (score > best_score)
        {
            best_score = score;
            if (score > alpha)
            { // found better minimum score for maximizing player, update alpha
                alpha = score;
            }
        }
        if (score >= beta)
        {
            return best_score; // score is worse than maximum score minimizing player can get, stop search
        }
    }
    return best_score;
}

/*
function which will print the best move so it can be sent through the UCI protocol
*/
void searchPos(int depth) // the function which will provide the lichess-bot api with the best string.
{
    moves move_list;
    genMoves(&move_list);
    int best_score = -INT_MAX;
    move_t best_move = 0;

    for (int i = 0; i < move_list.total_count; i++)
    {
        if (!makeMove(move_list.moves[i]))
        {
            continue;
        }

        int score = -negaMax(-INT_MAX, INT_MAX, depth - 1);
        unmakeMove();

        if (score > best_score)
        {
            best_score = score;
            best_move = move_list.moves[i];
        }
    }

    if (best_move)
    {
        int flags = getFlags(best_move);
        if (flags & 0b1000) // If move is a promotion
        {
            char promo_char = 'q'; // Default to queen
            switch (flags & 0b1111)
            {
            case 0b1000:
            case 0b1100:
                promo_char = 'n';
                break;
            case 0b1001:
            case 0b1101:
                promo_char = 'b';
                break;
            case 0b1010:
            case 0b1110:
                promo_char = 'r';
                break;
            case 0b1011:
            case 0b1111:
                promo_char = 'q';
                break;
            }
            printf("bestmove %s%s%c\n",
                   square_to_coords[getSourceSq(best_move)],
                   square_to_coords[getTargetSq(best_move)],
                   promo_char);
        }
        else
        {
            printf("bestmove %s%s\n",
                   square_to_coords[getSourceSq(best_move)],
                   square_to_coords[getTargetSq(best_move)]);
        }
    }
    else
    {
        printf("bestmove 0000\n");
    }
}
/******************\
--------------------
        Perft
--------------------
\******************/
int getTime()
{
    return GetTickCount();
}

long long perft(int depth)
{
    if (depth == 0)
    {
        return 1;
    }
    long long nodes = 0;
    moves move_list;
    genMoves(&move_list);
    for (int i = 0; i < move_list.total_count; i++)
    {
        if (!makeMove(move_list.moves[i]))
        {
            continue;
        }
        nodes += perft(depth - 1);
        unmakeMove();
    }
    return nodes;
}

/******************\
--------------------
    Initialization
--------------------
\******************/

/*
here, we create a function which will accept a FEN string as input and initialize the
current position into our piece bitboard.
*/
void initFENPosition(char *FEN)
{
    // erase data from piece and occupancy bitboards, and also reset piece_on_square board
    for (int square = 0; square < 64; square++)
    {
        piece_on_square[square] = no_piece;
    }
    memset(piece_bitboards, 0ULL, sizeof(piece_bitboards));
    memset(occupancy_bitboards, 0ULL, sizeof(occupancy_bitboards));
    int current_index = 0;
    int square;

    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            square = rank * 8 + file;
            if (FEN[current_index] == '/')
            {
                current_index++;
            }

            if (FEN[current_index] >= '1' && FEN[current_index] <= '8')
            {
                file += FEN[current_index] - '0' - 1;
            }
            else
            {
                setBit(piece_bitboards[pieces_to_encoded_constant[FEN[current_index]]], square);
                piece_on_square[square] = pieces_to_encoded_constant[FEN[current_index]];
            }
            current_index++;
        }
    }
    current_index++;
    /*
    I decided to use strncpy() and sscanf() to read the rest of the FEN string to initialize game state variables.
    One advantage of this approach is that its more compact compared to manually incrementing current_index and reading the string
    at each step with if/else statements

    One disadvantage (which had me debugging for a bit) is that it will initialize extremely incorrect game state variables when
    given an incomplete FEN string, for example :
     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"
     The starting position, but without any of the game state information and only the board information
     When debugging, I'd see that half-moves was set to something like -81555201...
     So overall, this function depends on being given a complete and well-formatted string, or else bugs will surely be encountered.
    */
    char rest_of_FEN[64];
    strncpy(rest_of_FEN, &FEN[current_index], sizeof(rest_of_FEN) - 1);
    rest_of_FEN[sizeof(rest_of_FEN) - 1] = '\0';

    char side_from_FEN;
    char castle_from_FEN[5];
    char en_passant_str[3];
    int half_moves_from_FEN;
    int full_moves_from_FEN;

    sscanf(rest_of_FEN, "%c %4s %2s %d %d", &side_from_FEN, castle_from_FEN,
           en_passant_str, &half_moves_from_FEN, &full_moves_from_FEN);

    /*
    debug print statements
    printf("side %c\n", side_from_FEN);
    printf("castle %s\n", castle_from_FEN);
    printf("ep %s\n", en_passant_str);
    printf("half %d\n", half_moves_from_FEN);
    printf("full %d\n", full_moves_from_FEN);
    */

    side = (side_from_FEN == 'w') ? white : black;

    for (int index = 0; index < 4; index++)
    {
        char curr = castle_from_FEN[index];
        if (curr != 'Q' && curr != 'K' && curr != 'k' && curr != 'q')
        {
            break;
        }
        castle |= castle_rights_encoder[curr];
    }

    if (strcmp(en_passant_str, "-") == 0)
    {
        en_passant = no_sq;
    }
    else
    {
        int file = files_to_int[en_passant_str[0]];
        int rank = 8 - (en_passant_str[1] - '0');
        en_passant = rank * 8 + file;
    }

    half_moves = half_moves_from_FEN;
    full_moves = full_moves_from_FEN;
    for (int piece = P; piece <= K; piece++)
    {
        occupancy_bitboards[white] |= piece_bitboards[piece];
    }
    for (int piece = p; piece <= k; piece++)
    {
        occupancy_bitboards[black] |= piece_bitboards[piece];
    }
    occupancy_bitboards[both] |= occupancy_bitboards[white] | occupancy_bitboards[black];
}

void initEverything()
{
    // initialize leaper (pawn, knight, king) attacks
    initLeaperAttacks();

    // initialize slider attacks
    initSliderAttacks(bishop);
    initSliderAttacks(rook);
    // initalize magic numbers
    // initMagicNumbers();
}

/******************\
--------------------
        UCI
--------------------
\******************/
/*
This section will define functions used for implementing the UCI protocol.
*/
// convert a move string sent by the engine (e.g "e2e4" or "h7h8q") to a move.
move_t parseMove(char *move_string)
{
    // extract source square and target square from string
    // printf("move sent : %s\n", move_string);  //debug line
    // printBoard(); // debug line
    int from = (move_string[0] - 'a') + (8 - (move_string[1] - '0')) * 8;
    int to = (move_string[2] - 'a') + (8 - (move_string[3] - '0')) * 8;
    // printf("move parsed : %s%s", square_to_coords[from], square_to_coords[to]); //debug line

    // initialize movelist and fill it with moves
    moves move_list;
    int flags;
    genMoves(&move_list);
    move_t move;
    // printf("total move count for move %s : %d\n", move_string, move_list.total_count); //debug line
    for (int i = 0; i < move_list.total_count; i++)
    { // loop through generated moves
        move = move_list.moves[i];
        // printMoveUCI(move); //debug line
        flags = getFlags(move);
        // check if source and target squares match the move string
        if (getSourceSq(move) == from && getTargetSq(move) == to)
        {

            /* printf("Parsed move: from %s (%d), to %s (%d)\n", //debug lines
                    square_to_coords[getSourceSq(move)],
                    getSourceSq(move),
                    square_to_coords[getTargetSq(move)],
                    getTargetSq(move));
                    */
            int promo = getFlags(move);
            if (promo & 0b1000)
            { // promotion is available
                if ((promo == 0b1011 || promo == 0b1111) && (move_string[4] == 'q'))
                {
                    if (!isIllegalMove(move))
                    {
                        return move;
                    }
                }
                if ((promo == 0b1000 || promo == 0b1001) && (move_string[4] == 'n'))
                {
                    if (!isIllegalMove(move))
                    {
                        return move;
                    }
                }
                if ((promo == 0b1001 || promo == 0b1011) && (move_string[4] == 'b'))
                {
                    if (!isIllegalMove(move))
                    {
                        return move;
                    }
                }
                if ((promo == 0b1010 || promo == 0b1110) && (move_string[4] == 'r'))
                {
                    if (!isIllegalMove(move))
                    {
                        return move;
                    }
                }

                continue;
            }

            return move;
        }
    }

    // all checks failed, move is illegal.
    return 0;
}

void parsePosition(char *input)
{
    // move pointer forward to skip "position" text
    input += 9;
    if ((strncmp(input, "startpos", 8)) == 0)
    {
        initFENPosition(starting_postition_fen);
    }
    else if ((strncmp(input, "fen", 3)) == 0)
    {
        input += 4; // move pointer forward to skip "fen" text
        initFENPosition(input);
    }
    // check for "moves" in input string to see if moves need to be made.
    char *moves_ptr = strstr(input, "moves");
    if (moves_ptr != NULL)
    {
        moves_ptr += 6; // move pointer forward to skip "moves" text
        char move_string[6];
        while (sscanf(moves_ptr, "%s", move_string) == 1)
        {
            move_t move = parseMove(move_string);
            if (!isIllegalMove(move))
            {
                makeMove(move);
            }
            else
            {
                printf("Illegal move sent by UCI : %s", move_string);
                break;
            }
            moves_ptr += strlen(move_string) + 1; // move pointer forward to next move
        }
    }
}

void parseGo(char *input) // a function to parse the "go" command sent by GUI to engine
{
    int depth = 1; // default depth
    char *depth_ptr = strstr(input, "depth");
    if (depth_ptr != NULL)
    {
        depth_ptr += 6; // move pointer forward to skip "depth" text
        depth = atoi(depth_ptr);
    }
    searchPos(depth);
}

void uciLoop()
{
    // reset input and output buffers
    setbuf(stdout, NULL);
    // setbuf(stdin, NULL);
    // create buffer for input
    char buffer[2000];
    // send GUI info about engine and "uciok" command to begin UCI communcation
    printf("id name Superjelly\n");
    printf("uciok\n");
    fflush(stdout);
    while (1)
    {

        // flush the output stream to ensure output is sent to the GUI

        // use fgets to get input
        if (!(fgets(buffer, 2000, stdin)))
        {
            continue;
        }
        if (buffer[0] == '\n') // ensure first char is not a newline resulting from possibly hitting the enter key, meaning input is available for parsing.
        {
            continue;
        }
        /*
        the next command sent to the engine after "uci" will be "isready", to which our engine must reply with "readyok".
         */
        if (strncmp(buffer, "isready", 7) == 0)
        {
            printf("readyok\n");
            fflush(stdout);
            continue;
        }

        // handle "quit" cmd by ending loop
        else if (strncmp(buffer, "quit", 4) == 0)
        {
            break;
        }
        // handle "ucinewgame" command by resetting board to starting position
        else if (strncmp(buffer, "ucinewgame", 10) == 0)
        {
            initFENPosition(starting_postition_fen);
        }
        // handle "position" command by setting up position sent by GUI
        else if (strncmp(buffer, "position", 8) == 0)
        {
            parsePosition(buffer);
            printBoard();
            fflush(stdout);
        }
        // handle "go" command by starting search for best move
        else if (strncmp(buffer, "go", 2) == 0)
        {
            parseGo(buffer);
        }

        // provide info requested by "uci" command
        else if (strncmp(buffer, "uci", 3) == 0)
        {

            printf("id name Superjelly\n");
            printf("uciok\n");
            fflush(stdout);
        }
    }
}

/******************\
--------------------
        MAIN
--------------------
\******************/

int main() // entry point
{

    initEverything();
    int debug = 0;
    if (debug) // run debug code
    {
        // printf("side before code executiton %s", (side == white) ? "white" : "black");

        initFENPosition("rnbqkbnr/ppp1ppp1/7p/3pP3/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3");
        move_t ep = parseMove("e5d6");
        makeMove(ep);
        printBoard();
        printBitboard(occupancy_bitboards[white]);
        printBitboard(occupancy_bitboards[black]);
        printBitboard(occupancy_bitboards[both]);
    }
    else
    {
        uciLoop();
    }
}
