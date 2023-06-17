//
//  Blackjack.c
//  Blackjack Game
//
//  Created by CmChi on 3/8/23.
//

#include "Blackjack.h"

/* Constant declarations */

const char reg_decision_matrix[][10] = {
    {'h', 'h', 'h', 'h', 'h', 'h', 'h', 'h', 'h', 'h'}, // 9
    {'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'h', 'h'}, //10
    {'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'h'}, //11
    {'h', 'h', 's', 's', 's', 'h', 'h', 'h', 'h', 'h'}, //12
    {'s', 's', 's', 's', 's', 'h', 'h', 'h', 'h', 'h'}, //13
    {'s', 's', 's', 's', 's', 'h', 'h', 'h', 'h', 'h'}, //14
    {'s', 's', 's', 's', 's', 'h', 'h', 'h', 'h', 'h'}, //15
    {'s', 's', 's', 's', 's', 'h', 'h', 'h', 'h', 'h'}  //16
};

const char ace_decision_matrix[][10] = {
    {'h', 'h', 'h', 'd', 'd', 'h', 'h', 'h', 'h', 'h'}, //A2
    {'h', 'h', 'h', 'd', 'd', 'h', 'h', 'h', 'h', 'h'}, //A3
    {'h', 'h', 'd', 'd', 'd', 'h', 'h', 'h', 'h', 'h'}, //A4
    {'h', 'h', 'd', 'd', 'd', 'h', 'h', 'h', 'h', 'h'}, //A5
    {'h', 'd', 'd', 'd', 'd', 'h', 'h', 'h', 'h', 'h'}, //A6
    {'s', 'd', 'd', 'd', 'd', 's', 's', 'h', 'h', 'h'}  //A7
};

const char split_decision_matrix[][10] = {
    {'x', 'x', 'x', 'x', 'x', 'x', 'h', 'h', 'h', 'h'}, // 2
    {'x', 'x', 'x', 'x', 'x', 'x', 'h', 'h', 'h', 'h'}, // 3
    {'h', 'h', 'h', 'x', 'x', 'h', 'h', 'h', 'h', 'h'}, // 4
    {'d', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'h', 'h'}, // 5
    {'x', 'x', 'x', 'x', 'x', 'h', 'h', 'h', 'h', 'h'}, // 6
    {'x', 'x', 'x', 'x', 'x', 'x', 'h', 'h', 'h', 'h'}, // 7
    {'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'}, // 8
    {'x', 'x', 'x', 'x', 'x', 's', 'x', 'x', 's', 's'}, // 9
    {'s', 's', 's', 's', 's', 's', 's', 's', 's', 's'}, //10
    {'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'}  //11
};


// add_to_hand()
//
// This method adds a card to a hand's list of cards
// while updating the hand's number of aces.

void add_to_hand(hand * hand, card * card_ptr) {
    
    // Check if the card being added is an ace
    if (card_ptr->number == 11) {
        hand->aces++;
    }
    
    // Allocate a larger array of cards, check for memory error
    card ** new_list = malloc(sizeof(card *) * (hand->card_count + 1));
    if (new_list == NULL) {
        exit(MEMORY_ERROR);
    }
    
    // Populate new array with cards
    for (int i = 0; i < hand->card_count; i++) {
        new_list[i] = hand->cards[i];
    }
    new_list[(int) hand->card_count] = card_ptr;
    
    // Adjust hand_total based on aces in the hand
    hand->hand_total += card_ptr->number;
    if ((hand->hand_total > 21) && (hand->aces > 0)) {
        hand->hand_total -= 10;
        hand->aces--;
    }

    // Free old array and assign to new_list
    free(hand->cards);
    hand->card_count++;
    hand->cards = new_list;
    
} // add_to_hand() //

// adjust_bet()
//
// This method adjusts the betting multiplier of the players
// in the list by comparing the winstreak of each player
// to their betting intensity value.

void adjust_bet(player * player_list) {

    while (player_list != NULL) {
    
        // Reset multiplier if winstreak was broken
        if (player_list->win_streak == 0) {
            player_list->bet.multiplier = 1;
        }
        
        // Adjust bet if intensity requirement is met
        else if (player_list->win_streak >= player_list->bet.intensity) {
            if (player_list->bet.algorithm == LINEAR) {
                player_list->bet.multiplier += player_list->bet.ratio;
            }
            else if (player_list->bet.algorithm == EXPONENTIAL) {
                player_list->bet.multiplier *= (player_list->bet.ratio + 1);
            }
            player_list->win_streak = 0;
        }
        player_list = player_list->next;
    }
    
} // adjust_bet() //

// calc_avdev()
//
// This method is for testing purposes only and returns the
// average deviation of the cards in the deck. For example,
// a 2 card which is where the 9s were generated would have
// a deviation of 7. This metric helps understand how
// shuffled the deck is.

float calc_avdev(void) {
    
    float deviation = 0;
    for (int i = 0; i < g_card_count; i++) {
        short rank_chunk = i / (g_settings.deck_size * 4) + 2;
        short card_rank = atoi(g_deck[i].rank);
        if (strcmp(g_deck[i].rank, "J") == 0) {
            card_rank = 11;
        } else if (strcmp(g_deck[i].rank, "Q") == 0) {
            card_rank = 12;
        } else if (strcmp(g_deck[i].rank, "K") == 0) {
            card_rank = 13;
        } else if (strcmp(g_deck[i].rank, "A") == 0) {
            card_rank = 14;
        }
        
        deviation += ABS(rank_chunk - card_rank);
    }
    return (deviation / g_card_count);
    
} // calc_avdev() //

// change_settings()
//
// This method writes the current settings to the specified
// binary file (settings.bin).

void change_settings(char * file) {
    
    FILE *f = fopen(file, "wb");
    if (f == NULL) {
        printf("There was an error accessing the settings file.\n");
        return;
    }
    fwrite(&g_settings, sizeof(settings), 1, f);
    fclose(f);
    
} // change_settings() //

// create_hand()
//
// This method allocates memory and returns a pointer for a
// new player.

hand * create_hand(void) {
    
    // Allocate memory for hand
    hand * hand_ptr = malloc(sizeof(hand));
    if (hand_ptr == NULL) {
        return NULL;
    }
    
    // Set hand attributes
    hand_ptr->aces = 0;
    hand_ptr->card_count = 0;
    hand_ptr->cards = NULL;
    hand_ptr->hand_total = 0;
    hand_ptr->is_bj = false;
    hand_ptr->is_double = false;
    hand_ptr->next = NULL;
    
    return hand_ptr;
    
} // create_hand() //

// create_player()
//
// This method allocates memory and returns a pointer for a
// new player, or null if there was an issue.

player * create_player(char * id, enum algorithm algorithm, float ratio, short intensity, short bet_amount) {
    
    // Allocate memory for player
    player * player_ptr = malloc(sizeof(player));
    hand * hand_ptr = create_hand();
    player_ptr->id = malloc(strlen(id) + 1);
    
    // Check for memory errors
    if ((player_ptr == NULL) || (hand_ptr == NULL) || (player_ptr->id == NULL)) {
        return NULL;
    }
    
    // Set player attributes
    player_ptr->hands = hand_ptr;
    player_ptr->next = NULL;
    player_ptr->win_streak = 0;
    player_ptr->winnings = 0;
    strcpy(player_ptr->id, id);
    
    // Set player betting configuration
    player_ptr->bet.amount = bet_amount;
    player_ptr->bet.algorithm = algorithm;
    player_ptr->bet.intensity = intensity;
    player_ptr->bet.multiplier = 1;
    player_ptr->bet.ratio = ratio;
    
    return player_ptr;
    
} // create_player() //

// create_players()
//
// This method creates a linked-list of players, the amount of which
// is passed in. It returns the number of players that were
// successfully allocated in case of memory errors.

short create_players(int num, player ** player_list) {
    
    player * prev = NULL;
    
    for (int i = 0; i < num; i++) {
        
        // Create buffer for player ID
        char str[MAX_ID_LEN];
        sprintf(str, "Player %d", i + 1);
        
        // Create a non-configured player
        player * player_ptr = create_player(str, CONSTANT, 0, 0, 10);
        
        // If player is not allocated successfully
        if (player_ptr == NULL) {
            return i;
        }
        
        // Set the head of the list to the first player or update previous player
        if ((*player_list) == NULL) {
            (*player_list) = player_ptr;
        } else {
            prev->next = player_ptr;
        }
        
        prev = player_ptr;
    }
    
    return num;
    
} // create_players() //

// deal()
//
// This method deals cards in an orderly fashion to the dealer
// as well as each player in the list.

void deal(player * player_list, dealer * dealer_ptr) {
    
    for (int i = 0; i < 2; i++) {
        
        // Hit dealer first
        add_to_hand(&(dealer_ptr->hand), draw_card());
        
        // Hit each player
        player * player_ptr = player_list;
        while (player_ptr != NULL) {
            add_to_hand(player_ptr->hands, draw_card());
            player_ptr = player_ptr->next;
        }
    }
    
} // deal() //

// draw_card()
//
// This method returns the next card in the deck and updates
// g_deck_index.

card * draw_card(void) {
    
    if (g_deck_index == g_card_count) {
        shuffle_deck();
    }
    g_deck_index++;
    return &g_deck[g_deck_index - 1];
    
} // draw_card() //

// evaluate_decision()
//
// This method returns an enumerated evaluation of the decision
// passed in based on the hand's cards and the showing dealer card.
// The evaluations do not adhere to a certain scale, but are
// personal opinion. For this reason this method probably should
// not be used or trusted yet.

enum evaluation evaluate_decision(hand * hand_ptr, short dealer_card, enum decision decision) {
    
    if (decision == make_decision(hand_ptr, dealer_card)) {
        return OPTIMAL;
    }
    short total = hand_ptr->hand_total;
    bool possible_split = ((hand_ptr->cards[0]->number == hand_ptr->cards[1]->number) && (hand_ptr->card_count == 2));
    switch (decision) {
        case HIT:
            if (possible_split) {
                short card_number = hand_ptr->cards[0]->number;
                if (card_number == 11 || card_number == 8) {
                    if (dealer_card <= 6) {
                        return BAD;
                    } else {
                        return AWFUL;
                    }
                }
                if (card_number >= 7 && card_number <= 10) {
                    return BAD;
                } else if (card_number == 6) {
                    return DECENT;
                } else {
                    return GOOD;
                }
            }
            else {
                if (hand_ptr->aces > 0) {
                    if (total >= 19 && total <= 20) {
                        return AWFUL;
                    } else if (total == 18) {
                        if (dealer_card == 2) {
                            return BAD;
                        }
                        return (dealer_card <= 6) ? DECENT : BAD;
                    } else {
                        return GOOD;
                    }
                }
                else {
                    if (total >= 17) {
                        return (dealer_card >= 7) ? BAD : AWFUL;
                    } else if (total >= 13 && total <= 16) {
                        return BAD;
                    } else if (total == 12) {
                        return DECENT;
                    } else {
                        return GOOD;
                    }
                }
            }
            break;
        case DOUBLE:
            if (possible_split) {
                short card_number = hand_ptr->cards[0]->number;
                if (card_number == 5) {
                    return BAD;
                } else {
                    return AWFUL;
                }
            }
            else {
                if (hand_ptr->aces > 0) {
                    if (total >= 19 && total <= 20) {
                        return AWFUL;
                    } else if (total == 18) {
                        if (dealer_card <= 6) {
                            return AWFUL;
                        } else {
                            return BAD;
                        }
                    }
                    if (dealer_card >= 7) {
                        return BAD;
                    } else {
                        return DECENT;
                    }
                }
                else {
                    if (total >= 13) {
                        return AWFUL;
                    } else if (total == 12) {
                        return BAD;
                    } else if (total >= 8) {
                        return DECENT;
                    } else {
                        return BAD;
                    }
                }
            }
            break;
        case SPLIT:
        {
            short card_number = hand_ptr->cards[0]->number;
            if (card_number == 10) {
                return AWFUL;
            } else {
                return BAD;
            }
        }
            break;
        case STAND:
            if (possible_split) {
                short card_number = hand_ptr->cards[0]->number;
                if (card_number == 11 || card_number == 8) {
                    if (dealer_card <= 6) {
                        return BAD;
                    } else {
                        return AWFUL;
                    }
                }
                if (card_number == 9) {
                    return GOOD;
                } else if (card_number == 7 || card_number == 6) {
                    if (dealer_card >= 6) {
                        return BAD;
                    } else {
                        return DECENT;
                    }
                } else {
                    return AWFUL;
                }
            }
            else {
                if (hand_ptr->aces > 0) {
                    if (total == 18) {
                        return DECENT;
                    } else {
                        if (dealer_card <= 6) {
                            return AWFUL;
                        } else {
                            return BAD;
                        }
                    }
                }
                else {
                    if (total >= 13) {
                        return BAD;
                    } else if (total == 12) {
                        if (dealer_card <= 3) {
                            return BAD;
                        }
                        return AWFUL;
                    } else {
                        return AWFUL;
                    }
                }
            }
    }
    return 0;
    
} // evaluate_decision() //

// evaluate_winnings()
//
// This method evaluates the outcome of each player's hands and pays out
// accordingly. Winstreaks are also updated.

void evaluate_winnings(player * player_list, hand * dealer_hand) {
    
    player * player_ptr = player_list;
    
    while (player_ptr != NULL) {
        hand * hand_ptr = player_ptr->hands;
        
        while (hand_ptr != NULL) {
            printf("%s: ", player_ptr->id);

            short winnings = player_ptr->bet.amount * player_ptr->bet.multiplier;
            enum outcome outcome = 0;
            winnings *= (hand_ptr->is_bj) ? 1.5 : 1;
            winnings *= (hand_ptr->is_double) ? 2 : 1;
            
            // Evaluate the outcome of each hand
            if (hand_ptr->hand_total > 21) {
                outcome = LOSE;
            } else if (hand_ptr->hand_total == dealer_hand->hand_total) {
                if (dealer_hand->is_bj) {
                    outcome = PUSH;
                } else if (hand_ptr->is_bj) {
                    outcome = WIN;
                } else {
                    outcome = PUSH;
                }
            } else if ((dealer_hand->hand_total > 21) || (hand_ptr->hand_total > dealer_hand->hand_total)) {
                outcome = WIN;
            } else {
                outcome = LOSE;
            }
            
            // Update player winnings and win streaks;
            switch (outcome) {
                case LOSE:
                    printf("Lose %hd.\n", winnings);
                    winnings *= -1;
                    player_ptr->win_streak = 0;
                    break;
                case PUSH:
                    printf("Push.\n");
                    winnings = 0;
                    player_ptr->win_streak = 0;
                    break;
                case WIN:
                    printf("%sWin %hd.\n", (hand_ptr->is_bj) ? "Blackjack! " : "", winnings);
                    player_ptr->win_streak++;
            }
            
            FILE *f = fopen("File.txt", "aw");
            
            fprintf(f, "%s %s %hd -> %hd\n", print_hand_str(dealer_hand, false), print_hand_str(hand_ptr, false), (short) (player_ptr->bet.amount * player_ptr->bet.multiplier), winnings);
             
            //fprintf(f, "%hd,", winnings);
            fclose(f);
            player_ptr->winnings += winnings;
            hand_ptr = hand_ptr->next;
        }
        player_ptr = player_ptr->next;
    }
    
} // evaluate_winnings() //

// free_player()
//
// This method deallocates the memory for a player, which includes
// freeing each hand and it's cards, the id, and the player itself.

void free_players(player * player_list, bool reset) {
    
    player * player_ptr = player_list;
    
    while (player_ptr != NULL) {
        hand * hand_ptr = player_ptr->hands;
        
        // Free each hand and its card array
        while (hand_ptr != NULL) {
            free(hand_ptr->cards);
            hand_ptr->cards = NULL;
            hand * temp = hand_ptr;
            hand_ptr = hand_ptr->next;
            free(temp);
            temp = NULL;
        }
        
        player_ptr->hands = NULL;
        player * temp = player_ptr;
        player_ptr = player_ptr->next;
        
        if (reset) {
            // If only resetting, we need to reallocate a hand to the player
            temp->hands = create_hand();
            if (temp->hands == NULL) {
                exit(MEMORY_ERROR);
            }
        } else {
            // If not resetting, free all player memory
            free(temp->id);
            free(temp);
            temp = NULL;
        }
    }
    
} // free_player() //

// generate_deck()
//
// This method allocates the memory for the deck (array of cards)
// and fills the array with cards which are not yet shuffled.

void generate_deck(void) {
    
    g_card_count = g_settings.deck_size * 52;
    g_deck = malloc(sizeof(card) * g_card_count);
    if (g_deck == NULL) {
        exit(MEMORY_ERROR);
    }
    int index = 0;
    for (int card_num = 2; card_num <= 11; card_num++) {
        
        // 16 10s, 4 of everything else will be generated
        int passes = g_settings.deck_size * ((card_num == 10) ? 16 : 4);
        for (int j = 0; j < passes; j++) {
            
            // Adjust card rank if there is a face card
            if (card_num == 10) {
                switch (j / (g_settings.deck_size * 4)) {
                    case 0:
                        sprintf(g_deck[index].rank, "%d", 10);
                        break;
                    case 1:
                        strcpy(g_deck[index].rank, "J");
                        break;
                    case 2:
                        strcpy(g_deck[index].rank, "Q");
                        break;
                    default:
                        strcpy(g_deck[index].rank, "K");
                }
            }
            else if (card_num == 11) {
                strcpy(g_deck[index].rank, "A");
            }
            else {
                sprintf(g_deck[index].rank, "%d", card_num);
            }
            g_deck[index].suit = j % 4;
            g_deck[index].number = card_num;
            index++;
        }
    }
    
} // generate_deck() //

// get_input()
//
// This method takes a number input from the user and returns INPUT_ERROR
// if there was a problem.

float get_input(void) {
    
    float choice = 0;
    if (scanf("%f", &choice) == 0) {
        scanf("%*c");
        choice = INPUT_ERROR;
    }
    return choice;
    
} // get_input() //

// hit_dealer()
//
// This method hits the dealer until it has reached 17, and hits
// again if there is a soft 17 and the rule is enabled in settings.

void hit_dealer(dealer * dealer) {
    
    // Hit dealer until cards total 17 or more
    while (dealer->hand.hand_total < 17) {
        add_to_hand(&(dealer->hand), draw_card());
    }
    
    // Hit soft 17 if it is enabled
    if (g_settings.soft_rule) {
        while ((dealer->hand.hand_total == 17) && (dealer->hand.aces > 0)) {
            add_to_hand(&dealer->hand, draw_card());
        }
    }
    
} // hit_dealer() //

// hit_player()
//
// This method autonomously hits a player based on the result of the
// make_decision() method.

void hit_players(player * player_ptr, short dealer_card) {
    
    while (player_ptr != NULL) {
        
        // Hit hand by hand
        hand * hand_ptr = player_ptr->hands;
        while (hand_ptr != NULL) {
            
            // Get decision and act accordingly
            enum decision decision = make_decision(hand_ptr, dealer_card);
            while (decision != STAND) {
                if (decision == SPLIT) {
                    split_hand(hand_ptr, true);
                } else {
                    
                    // Double if necessary then hit regardless
                    if (decision == DOUBLE && (hand_ptr->card_count == 2)) {
                        hand_ptr->is_double = true;
                    }
                    add_to_hand(hand_ptr, draw_card());
                }
                if (hand_ptr->is_double) {
                    break;
                }
                decision = make_decision(hand_ptr, dealer_card);
            }
            hand_ptr = hand_ptr->next;
        }
        player_ptr = player_ptr->next;
    }
    
} // hit_player() //

// load_players()
//
// This method reads the specified file path and creates a list of
// players from it. If there is an error, the method will return
// how many players were read successfully. A file error will return
// FILE_ERROR to be handled. This method will successfully read
// players both with a specified ID and without.

short load_players(char * file_name, player ** player_head) {
    
    // Open file and handle an error
    FILE *f = fopen(file_name, "r");
    if (f == NULL) {
        return FILE_ERROR;
    }
    
    short player_counter = 0;
    player * prev_player = NULL;
    
    // Read player strings line by line
    while (!feof(f)) {
        
        // Buffer variable declarations
        char id[MAX_ID_LEN] = { 0 };
        char algorithm = 0;
        float ratio = 0.0;
        short bet_amount = 0;
        short intensity = 0;
        long position = ftell(f);
        
        // Check if there is a specified ID string
        int read_values = fscanf(f, "%19[^,],%c,%f,%hd,%hd\n", id, &algorithm, &ratio, &bet_amount, &intensity);
        if (read_values != 5) {
            fseek(f, position, SEEK_SET);
            read_values = fscanf(f, "%c,%f,%hd,%hd\n", &algorithm, &ratio, &bet_amount, &intensity);
            
            // Stop reading if there is a problem with a player string
            if (read_values != 4) {
                continue;
            }
            sprintf(id, "Player %d", player_counter + 1);
        }
        
        // Create new player and convert char to enum
        player * new_player_ptr;
        if (algorithm == 'C') {
            new_player_ptr = create_player(id, CONSTANT, ratio, intensity, bet_amount);
        } else if (algorithm == 'L') {
            new_player_ptr = create_player(id, LINEAR, ratio, intensity, bet_amount);
        } else if (algorithm == 'E') {
            new_player_ptr = create_player(id, EXPONENTIAL, ratio, intensity, bet_amount);
        } else {
            continue;
        }
        
        // Set head or fill next attribute
        if (prev_player == NULL) {
            (*player_head) = new_player_ptr;
        }
        else {
            prev_player->next = new_player_ptr;
        }
        prev_player = new_player_ptr;
        player_counter++;
        
    }
    fclose(f);
    return player_counter;
    
} // load_players() //

// load_settings()
//
// This method reads the specified file (settings.bin) and populates
// g_settings.

void load_settings(char * file) {

    FILE *f = fopen(file, "rb");
    if (f == NULL) {
        printf("There was an error accessing the settings file.\n");
        return;
    }
    fread(&g_settings, sizeof(settings), 1, f);
    fclose(f);
    
} // load_settings() //

// make_decision()
//
// This method returns an enumerated decision based on the hand's
// cards and the dealer's showing card. The by-case decisions are
// found in the decision matrices.

enum decision make_decision(hand * hand, short dealer_card) {
    
    char decision = 0;

    // Check if the hand has the same two cards for split
    if ((hand->cards[0]->number == hand->cards[1]->number) && (hand->card_count == 2)) {
        decision = split_decision_matrix[hand->cards[0]->number - 2][dealer_card - 2];
    }
    
    // Check if the hand contains an ace
    else if (hand->aces > 0) {
        
        short not_ace = hand->hand_total - 11;
        if (not_ace >= 8) {
            return STAND;
        }
        decision = ace_decision_matrix[not_ace - 2][dealer_card - 2];
        
    }
    
    else {
        
        if (hand->hand_total >= 17) {
            return STAND;
        } else if (hand->hand_total <= 8) {
            return HIT;
        }
        decision = reg_decision_matrix[hand->hand_total - 9][dealer_card - 2];
        
    }
    
    // Convert decision character to enumerated decision
    switch (decision) {
        case 'h':
            return HIT;
        case 'd':
            return DOUBLE;
        case 'x':
            return SPLIT;
        default:
            return STAND;
    }
    
} // make_decision() //

// print_deck()
//
// This method is used for debugging purposes and prints out the
// deck in an easy-to-read fashion.

void print_deck(void) {
    
    printf("|");
    for (int i = 0; i < g_card_count; i++) {
        printf("%s|", g_deck[i].rank);
    }
    printf("\n");
    
} // print_deck() //

// print_hands()
//
// This method prints out each hand that is in the specified list
// of hands. The 'hide' parameter is used to hide the first card of
// the hand for the dealer.

void print_hand(hand * hand_head, bool hide) {
    
    bool first_card = true;
    hand * hand_ptr = hand_head;
    while (hand_ptr != NULL) {
        printf("|");
        
        for (int i = 0; i < hand_ptr->card_count; i++) {
            if (first_card && hide) {
                printf("?");
                first_card = false;
            } else {
                printf("%s", hand_ptr->cards[i]->rank);
            }
            printf("|");
        }
        
        if (!hide) {
            printf(" (%hd) ", hand_ptr->hand_total);
        }
        
        hand_ptr = hand_ptr->next;
        break;
    }
    
} // print_hand() //

// print_hand_str()
//
// This method is an alternate version of print_hand() that
// returns a string of formatted hands to print. It is currently
// not in use and causes memory leaks if not dealt with
// properly.

char * print_hand_str(hand * hand_head, bool hide) {
    
    char * result_str = malloc(200);
    bool first_card = true;
    hand * hand_ptr = hand_head;
    while (hand_ptr != NULL) {
        strcat(result_str, "|");
        
        for (int i = 0; i < hand_ptr->card_count; i++) {
            if (first_card && hide) {
                strcat(result_str, "?");
                first_card = false;
            } else {
                strcat(result_str, hand_ptr->cards[i]->rank);
            }
            strcat(result_str, "|");
        }
        
        if (!hide) {
            char total_str[7];
            sprintf(total_str, " (%hd) ", hand_ptr->hand_total);
            strcat(result_str, total_str);
        }
        hand_ptr = hand_ptr->next;
        if (hand_ptr != NULL) {
            strcat(result_str, "\n");
        }
    }
    return result_str;
    
} // print_hand_str() //

// print_player_hands()
//
// This method prints out player hands with an indicator as
// to which will be acted upon in case of a split.

void print_player_hands(player * player_ptr, bool is_split, short counter) {
    
    short hand_counter = 1;
    hand * hand_ptr = player_ptr->hands;
    while (hand_ptr != NULL) {
        
        printf("%s hand", player_ptr->id);
        
        if (is_split) {
            printf(" (%hd): ", hand_counter);
        }
        else {
            printf(": ");
        }
        
        print_hand(hand_ptr, false);
        
        // Add hand indicator
        if ((hand_counter == counter) && is_split) {
            printf(" <");
        }
        printf("\n");
        hand_ptr = hand_ptr->next;
        hand_counter++;
    }
    
} // print_player_hands() //

// reset_dealer()
//
// This method deallocates the memory for the dealer, which includes
// freeing the dealer's array of cards. Dealers are not allocated
// dynamically so we need not free a dealer struct.

void reset_dealer(dealer * dealer) {
    
    hand * hand_ptr = &dealer->hand;
    free(hand_ptr->cards);
    hand_ptr->cards = NULL;
    
    // Reset dealer values
    dealer->hand.aces = 0;
    dealer->hand.card_count = 0;
    dealer->hand.is_bj = false;
    dealer->hand.hand_total = 0;
    
} // reset_dealer() //

// split_hand()
//
// This method splits a hand into two different hands by creating
// another linked node in the specified list of hands. The 'recurse'
// parameter is used to recursively split hands until a
// differently-numbered card is drawn. This method can successfully
// run until a stack-overflow, theoretically splitting hands indefinitely.

void split_hand(hand * hand_ptr, bool recurse) {
    
    // Create new hand
    hand * new_hand_ptr = create_hand();
    if (new_hand_ptr == NULL) {
        exit(MEMORY_ERROR);
    }
    add_to_hand(new_hand_ptr, hand_ptr->cards[1]);
    new_hand_ptr->next = hand_ptr->next;
    
    // Modify aces variable for each hand: 2 -> 1, 0 -> 0
    hand_ptr->aces /= 2;
    new_hand_ptr->aces = hand_ptr->aces / 2;
    
    // Update and hit the original hand. Since we are replacing the second card,
    // we must manually change the pointer instead of using add_to_hand().
    card * new_card = draw_card();
    hand_ptr->cards[1] = new_card;
    hand_ptr->hand_total = hand_ptr->cards[0]->number + new_card->number;
    if (new_card->number == 11) {
        hand_ptr->aces++;
    }
    hand_ptr->next = new_hand_ptr;
    
    // Check for another split and recurse if specified
    if (new_card->number == hand_ptr->cards[0]->number && recurse) {
        split_hand(hand_ptr, true);
    }
    
    // Draw a card to the new hand
    new_card = draw_card();
    add_to_hand(new_hand_ptr, new_card);
    
    // Check for another split for new hand
    if (new_card->number == hand_ptr->cards[0]->number && recurse) {
        split_hand(new_hand_ptr, true);
    }
    
} // split_hand() //

// shuffle_deck()
//
// This method shuffles the deck by iterating over it and swapping
// each card with a randomly-selected card. It will do this a number
// of times according to the 'shuffle_passes' setting.

void shuffle_deck(void) {
    
    // Offset variable ensures that the seed changes regardless of how
    // fast time() is called.
    static unsigned int offset = 0;
    srand((unsigned int) time(NULL) + offset);
    offset++;
    
    for (int i = 0; i < g_settings.shuffle_passes; i++) {
        for (int j = 0; j < g_card_count; j++) {
            int rand_index = rand() % g_card_count;
            
            // Swap cards
            card temp = g_deck[rand_index];
            g_deck[rand_index] = g_deck[j];
            g_deck[j]= temp;
        }
    }
    
    g_deck_index = 0;
    
} // shuffle_deck() //

// write_players()
//
// This method writes player configurations to a specified
// file, and returns the number of players that were
// successfully written to the file.

short write_players(char * file_path, player * player_list) {
    
    FILE *f = fopen(file_path, "w");
    if (f == NULL) {
        return FILE_ERROR;
    }
    
    short player_counter = 0;
    while (player_list != NULL) {
        
        // Convert enumerated algorithm to char
        char algorithm = 0;
        switch (player_list->bet.algorithm) {
            case CONSTANT:
                algorithm = 'C';
                break;
            case LINEAR:
                algorithm = 'L';
                break;
            case EXPONENTIAL:
                algorithm = 'E';
        }
        
        // Write to file and check for errors
        int ret_val = fprintf(f, "%s,%c,%f,%hd,%hd\n", player_list->id, algorithm, player_list->bet.ratio, player_list->bet.amount, player_list->bet.intensity);
        if (ret_val < 0) {
            break;
        }
        
        player_counter++;
        player_list = player_list->next;
    }
    fclose(f);
    return player_counter;
    
} // write_players() //
