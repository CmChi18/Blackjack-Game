//
//  Deck.h
//  Blackjack Game
//
//  Created by CmChi on 3/8/23.
//

#ifndef Deck_h
#define Deck_h

#include <stdlib.h>
#include <time.h>

enum SUIT {
    CLUBS,
    DIAMONDS,
    HEARTS,
    SPADES
};

typedef struct card {
    enum SUIT suit;
    short number;
    char rank[3];
} card;

extern card ** g_deck;
extern int g_deck_index;
extern int g_card_count;

float calc_avdev(void);
void generate_deck(void);
void shuffle_deck(void);
card * draw_card(void);
void print_deck(void);

#endif /* Deck_h */
