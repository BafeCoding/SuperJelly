#include <stdio.h>

/******************\
--------------------
   HEADER FILES
--------------------
\******************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
/******************\
--------------------
   DEFINITIONS
--------------------
\******************/

#define U64 uint64_t
#define starting_postition_fen "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"

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

/*
ENUM for castling rights.
logic goes like this :

   definition                     binary  decimal
white king can castle kingside   = 0001     1
white king can castle queenside = 0010     2
black king can castle kingside   = 0100     4
white king can castle queenside = 1000     8


castling rights for both can be obtained by using bitwise or to combine these bits.
for example, 1001 would mean the white king can castle kingside and the black king can castle queenside.

*/

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
};

enum castling_rights
{
    wk = 1,
    wq = 2,
    bk = 4,
    bq = 8
};

/*
An array providing ascii representation of every piece.
It's important to keep this in the same order as our piece encoding enumeration so we can reference it
by index in order to print our board with the proper pieces.
for example unicode_pieces[P] would return P, which is ideal and convienient
*/
char ascii_pieces[12] = "PNBRQKpnbrqk";

int char_pieces_to_encoded_constant[] = {
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
};
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

U64 bishop_magic_numbers[64] =
    {
        0x8112202042201c44ULL,
        0x20004501401020ULL,
        0x1000629210024010ULL,
        0xc480420850045484ULL,
        0x6210100021000ULL,
        0x201002942c00840ULL,
        0x10090008a20000ULL,
        0xa010400844008208ULL,
        0x80288402448ULL,
        0x454281020912ULL,
        0x4ac0082049010812ULL,
        0xe802906100004ULL,
        0x8048020020060ULL,
        0x801220050044044ULL,
        0x44802003100ULL,
        0x460100820002600aULL,
        0x82000420025200ULL,
        0x950000200800488ULL,
        0x8024000800898808ULL,
        0x8042104000800ULL,
        0x201000484401000ULL,
        0x500880241488ULL,
        0x441018a00840c0ULL,
        0x82040080440141ULL,
        0x460030002202ULL,
        0x8020039480800ULL,
        0x400818008010ULL,
        0x840026002088ULL,
        0x8800800802000ULL,
        0x10023000180010ULL,
        0x1048090890044032ULL,
        0x140020800940500ULL,
        0x4008005012c0ULL,
        0x241002201410400ULL,
        0x1988102044004ULL,
        0x40410080020ULL,
        0x20040400082820ULL,
        0x244a10060001000ULL,
        0x4021010060220900ULL,
        0x200002840410a208ULL,
        0x2002100118040141ULL,
        0x121128c110400046ULL,
        0x100e2228002001a8ULL,
        0x11004988000210ULL,
        0x8000c80100980040ULL,
        0x444448040821ULL,
        0x208a1a4400a040ULL,
        0x4c0840400a802ULL,
        0x138c49004010908ULL,
        0x118022110080800ULL,
        0x180804111901ULL,
        0x10a0400300820640ULL,
        0x2050000889001006ULL,
        0x4102021201082200ULL,
        0x4040440251500ULL,
        0x2304220082020a01ULL,
        0x8000600205158044ULL,
        0x4007002802280ULL,
        0x4088122860440188ULL,
        0x14021aULL,
        0x48013814400a0098ULL,
        0x483000600ULL,
        0x14021810002882ULL,
        0x408241012a701ULL

};

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
// set bit of a square
// Helpful bit macros!!

#define setBit(bitboard, square) ((bitboard) |= (1ULL << (square)))
// retrive state of bit on bitboard
#define getBit(bitboard, square) ((bitboard) & (1ULL << (square)))

// unset bit of a square
#define popBit(bitboard, square) (getBit(bitboard, square) ? (bitboard) ^= (1ULL << (square)) : 0)

// count set bits on a bitboard
#define countBits(bitboard) __builtin_popcountll(bitboard)

// get index of least significant first bit
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
    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if (blockers & (1ULL << (r * 8 + f)))
        {
            break;
        }
    }
    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if (blockers & (1ULL << (r * 8 + f)))
        {
            break;
        }
    }
    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if (blockers & (1ULL << (r * 8 + f)))
        {
            break;
        }
    }
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
        pawn_attacks[black][square] = maskPawnAttacks(white, square);
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

static inline U64 genBishopAttack(U64 occupancy, int square)
{
    occupancy &= bishop_masks[square];
    occupancy *= bishop_magic_numbers[square];
    occupancy >>= 64 - 9;
    return bishop_attacks[square][occupancy];
}
static inline U64 genRookAttacks(U64 occupancy, int square)
{
    occupancy &= rook_masks[square];
    occupancy *= rook_magic_numbers[square];
    occupancy >>= 64 - rook_relevant_bits[square];
    return rook_attacks[square][occupancy];
}
/******************\
--------------------
Piece Bitboards/Flags
--------------------
\******************/

U64 piece_bitboards[12];

U64 occupancy_bitboards[3];

int side = 0;

int en_passant = no_sq;

int castle = 0b1101;

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
    printf("    ");
    for (int i = 97; i < 105; i++) // using ascii codes!!
    {
        printf(" %c ", i);
    }
    printf("\n %s to play\n", (side) ? "black" : "white");
    printf("\n en passant: %s\n", (en_passant != no_sq) ? square_to_coords[en_passant] : "none");
    printf("castling rights : %c%c%c%c\n", (castle & wk) ? 'K' : '-',
           (castle & wq) ? 'Q' : '-',
           (castle & bk) ? 'k' : '-',
           (castle & bq) ? 'q' : '-');
}
/******************\
--------------------
    Initialization
--------------------
\******************/

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
        MAIN
--------------------
\******************/

int main() // entry point
{
    setBit(piece_bitboards[P], a1);
    setBit(piece_bitboards[P], b1);
    setBit(piece_bitboards[P], c1);
    setBit(piece_bitboards[P], d1);
    setBit(piece_bitboards[P], e1);
    setBit(piece_bitboards[P], f1);
    setBit(piece_bitboards[P], g1);
    setBit(piece_bitboards[P], h1);

    printBoard();
}
