//
//  Blackjack.h
//  Blackjack Game
//
//  Created by CmChi on 3/8/23.
//

#ifndef Blackjack_h
#define Blackjack_h

#include "Deck.h"

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Constant declarations */

#define MAX_ID_LEN   (21)
#define MAX_PLAYER   (10)
#define MAX_FILE_LEN (100)
#define MAX_RATIO    (10)
#define MAX_BET      (1000)

/* Error declarations */

#define OK            (0)
#define FILE_ERROR   (-1)
#define INPUT_ERROR  (-2)
#define MEMORY_ERROR (-3)

/* Macro declarations */

#define ABS(x) ( (x < 0) ? -(x) : (x) )

/* Constant declarations */

extern const char reg_decision_matrix[][10];
extern const char ace_decision_matrix[][10];
extern const char split_decision_matrix[][10];

enum action {
    EXIT,
    PRACTICE = 1,
    SIMULATE,
    CONFIGURE,
    CHANGE_DECK = 1,
    CHANGE_PASSES,
    CHANGE_PERCENT,
    CHANGE_SOFT,
    CHANGE_INSURANCE,
    CHANGE_EVAL,
    CHANGE_ID = 1,
    CHANGE_ALGORITHM,
    CHANGE_RATIO,
    CHANGE_BET,
    CHANGE_INTENSITY
};

enum algorithm {
    CONSTANT = 1,
    LINEAR,
    EXPONENTIAL
};

enum decision {
    HIT = 1,
    STAND,
    DOUBLE,
    SPLIT
};

enum evaluation {
    OPTIMAL,
    GOOD,
    DECENT,
    BAD,
    AWFUL
};

enum outcome {
    LOSE,
    PUSH,
    WIN
};

/* Struct declarations */

typedef struct settings {
    // How many decks are used
    unsigned short deck_size;
    // How many times the shuffle algorithm executes
    unsigned short shuffle_passes;
    // When the deck must be shuffled
    unsigned short shuffle_percent;
    // Does dealer hit on soft 17
    bool soft_rule;
    // Does dealer offer insurance
    bool insurance_rule;
    // Are decision evaluations shown
    bool show_eval;
} settings;

typedef struct bet {
    enum algorithm algorithm;
    float multiplier;
    // The ratio applies to LINEAR and EXPONENTIAL differently.
    // Example: ratio = 0.5, bet = 10. LINEAR would add 5 each
    // time whereas EXPONENTIAL would multiply by 1.5 each time.
    float ratio;
    short amount;
    short intensity;
    
} bet;

typedef struct hand {
    card ** cards;
    struct hand * next;
    short aces;
    short card_count;
    short hand_total;
    bool is_bj;
    bool is_double;
} hand;

typedef struct player {
    bet bet;
    hand * hands;
    struct player * next;
    char * id;
    int winnings;
    long wins;
    long pushes;
    long losses;
    long hand_count;
    short win_streak;
} player;

typedef struct dealer {
    hand hand;
    //short aces;
} dealer;

/* Global variable declarations */

extern settings g_settings;

/* Function prototypes */

void add_to_hand(hand *, card *);
void adjust_bet(player *);
player * create_player(char *, enum algorithm, float, short, short);
short create_players(int, player **);
void change_settings(char *);
void deal(player *, dealer *);
enum evaluation evaluate_decision(hand *, short, enum decision);
void evaluate_winnings(player *, hand *);
void free_dealer(dealer *);
void free_players(player *, bool);
float get_input(void);
short get_hand_total(hand *);
void hit_players(player *, short);
void hit_dealer(dealer *);
void load_settings(char *);
short load_players(char *, player **);
enum decision make_decision(hand *, short);
void print_hand(hand *, bool);
char * print_hand_str(hand *, bool);
void print_player_hands(player *, bool, short);
void reset_dealer(dealer *);
void split_hand(hand *, bool);
short write_players(char *, player *);

#endif /* Blackjack_h */
