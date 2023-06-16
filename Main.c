//
//  Main.c
//  Blackjack Game
//
//  Created by CmChi on 6/16/23.
//

#include "Blackjack.h"

card * g_deck = NULL;
int g_deck_index = 0;
int g_card_count = 0;
settings g_settings = { 0 };

int main(void) {
    
    load_settings("settings.bin");
    generate_deck();
    
Master:
    
    while (true) {
        
        printf("==================> Menu <==================\n\n");
        printf("1. Practice\n");
        printf("2. Simulate\n");
        printf("3. Change settings\n");
        printf("0. Exit\n");
        printf("\n============================================\n");
        
        enum action action = get_input();
        if (action == INPUT_ERROR) {
            continue;
        }
        
        if (action == PRACTICE) {
            
            shuffle_deck();
            dealer dealer = { 0 };
            player * player = create_player("Player", 0, 0, 0, 0);
            
            while (true) {
                
                // Check if deck needs to be shuffled
                float deck_percent = g_deck_index / (float) g_card_count;
                if (deck_percent * 100 > g_settings.shuffle_percent) {
                    shuffle_deck();
                }
                
                // Get the desired betting amount
                short bet = 0;
                printf("============================================\n");
                while (true) {
                    printf("Enter bet amount: ");
                    bet = get_input();
                    if (bet == INPUT_ERROR) {
                        continue;
                    } else if (bet < 0) {
                        printf("Your bet must be a positive number.\n");
                        printf("============================================\n");
                        continue;
                    } else if (bet > MAX_BET) {
                        printf("Your bet must not exeed 1000.\n");
                        printf("============================================\n");
                        continue;
                    }
                    break;
                }
                player->bet.amount = bet;
                
                deal(player, &dealer);
                hand * player_hand_ptr = player->hands;
                
                // Declare variables necessary for splits
                short counter = 1;
                bool is_split = false;
                
                // Check for blackjacks (only first hand)
                player_hand_ptr->is_bj = (player_hand_ptr->hand_total == 21);
                dealer.hand.is_bj = (dealer.hand.hand_total == 21);
                
                // Iterate through player's hands
                while (player_hand_ptr != NULL && !player_hand_ptr->is_bj && !dealer.hand.is_bj) {
                    
                    // If hand has 21 or bust go to the next hand
                    if (player_hand_ptr->hand_total >= 21) {
                        player_hand_ptr = player_hand_ptr->next;
                        counter++;
                        continue;
                    }
                    
                    enum decision decision = 0;
                    while (true) {
                        
                        bool can_split = ( (player_hand_ptr->card_count == 2) &&
                                           (player_hand_ptr->cards[0]->number ==
                                            player_hand_ptr->cards[1]->number) );
                        
                        printf("================> Practice <================\n\n");
                        printf("Dealer's hand:%s", (is_split) ? "     " : " ");
                        print_hand(&(dealer.hand), true);
                        printf("\n");
                        print_player_hands(player, is_split, counter);
                        printf("\n");
                        printf("1. Hit\n");
                        printf("2. Stand\n");
                        printf("%s", (player_hand_ptr->card_count == 2) ? "3. Double\n" : "");
                        printf("%s", (can_split) ? "4. Split\n" : "");
                        printf("\n============================================\n");
                        
                        printf("Enter your choice: ");
                        decision = get_input();
                        if (can_split) {
                            if (decision != INPUT_ERROR && decision >= 0 && decision <= 4) {
                                break;
                            }
                        }
                        else if (player_hand_ptr->card_count == 2) {
                            if (decision != INPUT_ERROR && decision >= 0 && decision <= 3) {
                                break;
                            }
                        }
                        else {
                            if (decision != INPUT_ERROR && decision >= 0 && decision <= 2) {
                                break;
                            }
                        }
                        
                    }
                    
                    // Print evaluations if they are enabled
                    if (g_settings.show_eval == true) {
                        switch (evaluate_decision(player_hand_ptr, dealer.hand.cards[1]->number, decision)) {
                            case OPTIMAL:
                                printf("OPTIMAL\n");
                                break;
                            case GOOD:
                                printf("GOOD\n");
                                break;
                            case DECENT:
                                printf("DECENT\n");
                                break;
                            case BAD:
                                printf("BAD\n");
                                break;
                            case AWFUL:
                                printf("AWFUL\n");
                        }
                    }
                    
                    // Handle player's decision
                    switch (decision) {
                        case SPLIT:
                            split_hand(player_hand_ptr, false);
                            is_split = true;
                            break;
                        case DOUBLE:
                            player_hand_ptr->is_double = true;
                            add_to_hand(player_hand_ptr, draw_card());
                        case STAND:
                            player_hand_ptr = player_hand_ptr->next;
                            counter++;
                            break;
                        case HIT:
                            add_to_hand(player_hand_ptr, draw_card());
                            break;
                    }
                    
                }
                
                hit_dealer(&dealer);
                
                short cont = 0;
                while (true) {
                    
                    printf("============================================\n\n");
                    printf("Dealer's hand:%s", (is_split) ? "     " : " ");
                    print_hand(&(dealer.hand), false);
                    printf("\n");
                    print_player_hands(player, is_split, 0);
                    printf("\n");
                    evaluate_winnings(player, &dealer.hand);
                    printf("\n============================================\n");
                    printf("Enter 1 to continue or 0 to quit: ");
                    cont = get_input();
                    if (cont == INPUT_ERROR || cont < 0 || cont > 1) {
                        continue;
                    }
                    break;
                    
                }
                
                // Reset dealer and players
                reset_dealer(&dealer);
                free_players(player, true);
                if (!cont) {
                    printf("Your total winnings were %d.\n", player->winnings);
                    // Fully deallocate player memory
                    free_players(player, false);
                    break;
                }
            }
            
        }
        
        else if (action == SIMULATE) {
            
            short player_count = 0;
            player * player_list = NULL;
            
            while (true) {
                
                printf("================> Simulate <================\n\n");
                printf("1. Create players with default configurations\n");
                printf("2. Load configured players from a .csv file\n");
                printf("0. Go back\n");
                printf("\n============================================\n");
                action = (short) get_input();
                if (action == 0) {
                    // Must use goto because there is no loop to break from
                    goto Master;
                }
                if (action == INPUT_ERROR || action < 1 || action > 2) {
                    continue;
                }
                break;
                
            }
            
            printf("============================================\n");
            if (action == EXIT) {
                goto Master;
            }
            else if (action == 1) {
                
                short created_players = 0;
                while (true) {
                    
                    printf("Enter the number of players to simulate: ");
                    player_count = (short) get_input();
                    if (player_count == INPUT_ERROR || player_count <= 0) {
                        continue;
                    } else if (player_count > MAX_PLAYER) {
                        printf("You cannot simulate more than %d players.\n", MAX_PLAYER);
                        continue;
                    }
                    break;
                    
                }
                
                created_players = create_players(player_count, &player_list);
                printf("Created %hd players successfully.\n", created_players);
                player_count = created_players;
                
            }
            
            else if (action == 2) {
                
                while (true) {
                    
                    char file_name[MAX_FILE_LEN] = { 0 };
                    printf("Enter a file name: ");
                    scanf("%99s", file_name);
                    
                    short read_players = load_players(file_name, &player_list);
                    if (read_players == FILE_ERROR) {
                        
                        printf("The file is not accessible or does not exist.\n");
                        printf("============================================\n");
                        printf("Enter 1 to try again or 0 to exit: ");
                        
                        
                        while (true) {
                            action = (short) get_input();
                            if (action == INPUT_ERROR || action < 0 || action > 1) {
                                continue;
                            }
                            break;
                        }
                        
                        if (action == 0) {
                            goto Master;
                        } else {
                            continue;
                        }
                        
                    } else if (read_players == 0) {
                        printf("There were no players to read in.\n");
                        goto Master;
                    } else {
                        printf("Loaded %hd players successfully.\n", read_players);
                        player_count = read_players;
                        break;
                    }
                    
                }
                
            }

            while (true) {
                
                printf("================> Simulate <================\n\n");
                printf("1. Begin simulation\n");
                printf("2. Edit player configurations\n");
                printf("3. Save players to a .csv file\n");
                printf("0. Exit simulation\n");
                printf("\n============================================\n");
                
                action = (short) get_input();
                if (action == FILE_ERROR || action < 0 || action > 3) {
                    continue;
                }
                
                if (action == EXIT) {
                    goto Master;
                } else if (action == 1) {
                    break;
                } else if (action == 2) {
                    
                    while (true) {
                        
                        short player_choice = 0;
                        player * player_ptr = player_list;
                        
                        while (true) {
                            
                            printf("==================> Edit <==================\n\n");
                            for (int i = 1; i <= player_count; i++) {
                                printf("%d. %s: ", i, player_ptr->id);
                                switch (player_ptr->bet.algorithm) {
                                    case CONSTANT:
                                        printf("Constant");
                                        break;
                                    case LINEAR:
                                        printf("Linear");
                                        break;
                                    case EXPONENTIAL:
                                        printf("Exponential");
                                }
                                printf("|%.2f|%hd|%hd\n", player_ptr->bet.ratio, player_ptr->bet.amount, player_ptr->bet.intensity);
                                player_ptr = player_ptr->next;
                            }
                            printf("0. Go back\n");
                            printf("\n============================================\n");
                            
                            player_choice = (short) get_input();
                            if (player_choice == INPUT_ERROR || player_choice < 0 || player_choice > player_count) {
                                continue;
                            }
                            break;
                            
                        }
                        
                        if (player_choice == 0) {
                            break;
                        } else {
                            
                            // Iterate to select player
                            player_ptr = player_list;
                            for (int i = 1; i < player_choice; i++) {
                                player_ptr = player_ptr->next;
                            }
                            
                            printf("==================> Edit <==================\n\n");
                            printf("1. ID : %s\n", player_ptr->id);
                            printf("2. Betting algorithm: ");
                            switch (player_ptr->bet.algorithm) {
                                case CONSTANT:
                                    printf("Constant\n");
                                    break;
                                case LINEAR:
                                    printf("Linear\n");
                                    break;
                                case EXPONENTIAL:
                                    printf("Exponential\n");
                            }
                            printf("3. Betting ratio: %.2f\n", player_ptr->bet.ratio);
                            printf("4. Betting amount: %hd\n", player_ptr->bet.amount);
                            printf("5. Betting intensity: %hd\n", player_ptr->bet.intensity);
                            printf("0. Go back\n");
                            printf("\n============================================\n");
                            
                            while (true) {
                                action = get_input();
                                if (action == INPUT_ERROR || action < 0 || action > CHANGE_INTENSITY) {
                                    continue;
                                }
                                break;
                            }
                            
                            if (action == CHANGE_ID) {
                                
                                char new_id[MAX_ID_LEN] = { 0 };
                                
                                printf("===================> ID <===================\n\n");
                                printf("The ID is the name of the player that is\n");
                                printf("printed when results are shown.\n");
                                printf("Max 20 characters.\n");
                                printf("\n============================================\n");
                                printf("Enter a new ID: ");
                                
                                while (true) {
                                    if (scanf("%20s", new_id) != 1) {
                                        continue;
                                    }
                                    break;
                                }
                                
                                strcpy(player_ptr->id, new_id);
                                printf("This player's ID is now %s.\n", player_ptr->id);
                                
                            } else if (action == CHANGE_ALGORITHM) {
                                
                                enum algorithm new_alg = 0;
                                
                                printf("===============> Algorithm <================\n\n");
                                printf("The algorithm is the shape at which a\n");
                                printf("player's bet is incremented.\n\n");
                                printf("1. Constant\n");
                                printf("2. Linear\n");
                                printf("3. Exponential\n");
                                printf("\n============================================\n");
                                
                                while (true) {
                                    printf("Enter a new algorithm: ");
                                    new_alg = get_input();
                                    if (new_alg == INPUT_ERROR || new_alg < CONSTANT || new_alg > EXPONENTIAL) {
                                        continue;
                                    }
                                    break;
                                }
                                
                                player_ptr->bet.algorithm = new_alg;
                                printf("%s's betting algorithm is now\n", player_ptr->id);
                                switch (player_ptr->bet.algorithm) {
                                    case CONSTANT:
                                        printf("constant.\n");
                                        break;
                                    case LINEAR:
                                        printf("linear.\n");
                                        break;
                                    case EXPONENTIAL:
                                        printf("exponential.\n");
                                }
                                
                            } else if (action == CHANGE_RATIO) {
                                
                                float new_ratio = 0;
                                
                                printf("==================> Ratio <=================\n\n");
                                printf("The ratio is the percentage that affects how\n");
                                printf("much a player's bet is incremented. A ratio\n");
                                printf("of 0.5 would increment a bet by half of the\n");
                                printf("original or multiply it by 1.5 every time.\n");
                                printf("The limit is a ratio of 10.\n");
                                printf("\n============================================\n");
                                
                                while (true) {
                                    printf("Enter a new ratio: ");
                                    new_ratio = get_input();
                                    if (new_ratio == INPUT_ERROR || new_ratio < 0 || new_ratio > MAX_RATIO) {
                                        continue;
                                    }
                                    break;
                                }
                                
                                player_ptr->bet.ratio = new_ratio;
                                printf("%s's betting ratio is now %f.\n", player_ptr->id, player_ptr->bet.ratio);
                                
                            } else if (action == CHANGE_BET) {
                                
                                float new_amount = 0;
                                
                                printf("=================> Amount <=================\n\n");
                                printf("The betting amount is the base amount that a\n");
                                printf("player bets in a round, and is incremented\n");
                                printf("according to the betting algorithm and ratio.\n");
                                printf("The limit is %d.\n", MAX_BET);
                                printf("\n============================================\n");
                                
                                while (true) {
                                    printf("Enter a new betting amount: ");
                                    new_amount = (short) get_input();
                                    if (new_amount == INPUT_ERROR || new_amount < 0 || new_amount > MAX_BET) {
                                        continue;
                                    }
                                    break;
                                }
                                
                                player_ptr->bet.amount = new_amount;
                                printf("%s's betting amount is now %hd.\n", player_ptr->id, player_ptr->bet.amount);
                            } else if (action == CHANGE_INTENSITY) {
                                
                                float new_intensity = 0;
                                
                                printf("================> Intensity <===============\n\n");
                                printf("The betting intensity is how many rounds it\n");
                                printf("takes for the player to increase their bet.\n");
                                printf("A lower intensity is more aggessive.\n");
                                printf("\n============================================\n");
                                
                                while (true) {
                                    printf("Enter a new betting intensity: ");
                                    new_intensity = (short) get_input();
                                    if (new_intensity == INPUT_ERROR || new_intensity < 0 || new_intensity > SHRT_MAX) {
                                        continue;
                                    }
                                    break;
                                }
                                
                                player_ptr->bet.intensity = new_intensity;
                                printf("%s's betting intensity is now %hd.\n", player_ptr->id, player_ptr->bet.intensity);
                                
                            } else {
                                break;
                            }
                        }
                    }
                    
                } else if (action == 3) {
                    
                    char file_path[MAX_FILE_LEN] = { 0 };
                    printf("Enter a file path: ");
                    scanf("%99s", file_path);
                    
                    short written_players = write_players(file_path, player_list);
                    if (written_players == FILE_ERROR) {
                        
                        printf("The file is not accessible or does not exist.\n");
                        printf("Enter 1 to try again or 0 to exit: ");
                        
                        while (true) {
                            action = (short) get_input();
                            if (action == INPUT_ERROR || action < 0 || action > 1) {
                                continue;
                            }
                            break;
                        }
                        
                        if (action == 0) {
                            goto Master;
                        }
                        else {
                            continue;
                        }
                        
                    } else if (written_players == 0) {
                        printf("There were no players to write.\n");
                        continue;
                    } else {
                        printf("Wrote %hd players successfully.\n", written_players);
                        player_count = written_players;
                    }
                }
                
            }
            
            printf("============================================\n");
            
            int iterations = 0;
            while (true) {
                printf("Enter times to simulate: ");
                iterations = (int) get_input();
                if (iterations == INPUT_ERROR || iterations < 0 || iterations > INT_MAX) {
                    continue;
                }
                break;
            }
            
            // Begin simulation
            shuffle_deck();
            dealer dealer = { 0 };
            for (int i = 0; i < iterations; i++) {
                
                printf("============================================\n\n");
                player * player_ptr = player_list;
                
                // Check if deck needs to be shuffled
                float deck_percent = g_deck_index / (float) g_card_count;
                if (deck_percent * 100 > g_settings.shuffle_percent) {
                    shuffle_deck();
                }
                
                deal(player_list, &dealer);
                
                // Check if dealer and players have blackjack
                dealer.hand.is_bj = (dealer.hand.hand_total == 21);
                while (player_ptr != NULL) {
                    player_ptr->hands->is_bj = (player_ptr->hands->hand_total == 21);
                    player_ptr->bet.amount = 10;
                    player_ptr = player_ptr->next;
                }
                
                // Hit players only if dealer does not have blackjack
                if (!dealer.hand.is_bj) {
                    
                    hit_players(player_list, dealer.hand.cards[1]->number);
                    hit_dealer(&dealer);
                    printf("Dealer's hand: ");
                    print_hand(&dealer.hand, false);
                    printf("\n");
                    player_ptr = player_list;
                    while (player_ptr != NULL) {
                        print_player_hands(player_ptr, false, 0);
                        player_ptr = player_ptr->next;
                    }
                    printf("\n");
                    
                }
                
                evaluate_winnings(player_list, &dealer.hand);
                printf("\n");
                adjust_bet(player_list);
                
                // Reset dealer and players
                reset_dealer(&dealer);
                free_players(player_list, true);
                
            }
            
            printf("================> Winnings <================\n");
            player * player_ptr = player_list;
            while (player_ptr != NULL) {
                printf("%s: %d\n", player_ptr->id, player_ptr->winnings);
                player_ptr = player_ptr->next;
            }
            
            // Deallocate all player memory
            free_players(player_list, false);
            
        } else if (action == CONFIGURE) {
            
            action = 0;
            while (true) {
                
                while (true) {
                    
                    printf("============================================\n\n");
                    printf("1. Deck Size: %hi\n", g_settings.deck_size);
                    printf("2. Shuffle Passes: %hi\n", g_settings.shuffle_passes);
                    printf("3. Shuffle Percent: %hi\n", g_settings.shuffle_percent);
                    printf("4. Dealer hits a soft 17: %s\n", (g_settings.soft_rule) ? "True" : "False");
                    printf("5. Insurance (not implemented): %s\n", (g_settings.insurance_rule) ? "Enabled" : "Disabled");
                    printf("6. Evaluations: %s\n", (g_settings.show_eval) ? "Enabled" : "Disabled");
                    printf("0. Go back\n");
                    printf("\n============================================\n");
                    
                    action = get_input();
                    if (action != INPUT_ERROR && action >= 0 && action <= CHANGE_EVAL) {
                        break;
                    }
                    
                }
                
                if (action == CHANGE_DECK) {
                    
                    short new_size = 0;
                    
                    printf("============================================\n\n");
                    printf("The size of the deck will be 52 x deck size.\n");
                    printf("Enter the new deck size below.\n");
                    printf("\n============================================\n");
                    
                    while (true) {
                        printf("Enter a positive integer: ");
                        new_size = (short) get_input();
                        if (new_size == INPUT_ERROR || new_size < 0) {
                            continue;
                        }
                        break;
                    }
                    
                    g_settings.deck_size = new_size;
                    change_settings("settings.bin");
                    printf("Deck Size has been updated to %hi\n", g_settings.deck_size);
                    
                    // Free and reconstruct the deck
                    free(g_deck);
                    generate_deck();
                    
                } else if (action == CHANGE_PASSES) {
                    
                    short new_passes = 0;
                    
                    printf("============================================\n\n");
                    printf("Passes determines how many times the\n");
                    printf("shuffling algorithm executes.\n");
                    printf("Enter the new number of passes below.\n");
                    printf("\n============================================\n");
                    
                    while (true) {
                        printf("Enter a positive integer: ");
                        new_passes = (short) get_input();
                        if (new_passes == INPUT_ERROR || new_passes < 0) {
                            continue;
                        }
                        break;
                    }
                    
                    g_settings.shuffle_passes = new_passes;
                    change_settings("settings.bin");
                    printf("Shuffle Passes has been updated to %hi\n", g_settings.shuffle_passes);
                    
                } else if (action == CHANGE_PERCENT) {
                    
                    short new_percent = 0;
                    
                    printf("============================================\n\n");
                    printf("Percent is the percent of the deck at which it must\n");
                    printf("be shuffled. Usually, it is 50. If the deck runs\n");
                    printf("out during a round, it will be shuffled automatically.");
                    printf("Enter the new percentage below.\n");
                    printf("\n============================================\n");
                    
                    while (true) {
                        printf("Enter a positive integer: ");
                        new_percent = (short) get_input();
                        if (new_percent == INPUT_ERROR || new_percent < 0 || new_percent > 100) {
                            continue;
                        }
                        break;
                    }
                    g_settings.shuffle_percent = new_percent;
                    change_settings("settings.bin");
                    printf("Shuffle Percent has been updated to %hi\n", g_settings.shuffle_percent);
                    
                } else if (action == CHANGE_SOFT) {
                    
                    short new_soft = 0;
                    
                    printf("============================================\n\n");
                    printf("A soft 17 is an Ace and a 6. Usually,\n");
                    printf("the dealer hits this.\n");
                    printf("Enter 1 for true or 0 for false below.\n");
                    printf("\n============================================\n");
                    
                    while (true) {
                        printf("Enter 1 or 0: ");
                        new_soft = (short) get_input();
                        if (new_soft == INPUT_ERROR || new_soft < 0 || new_soft > 1) {
                            continue;
                        }
                        break;
                    }
                    
                    g_settings.soft_rule = (new_soft == 1);
                    change_settings("settings.bin");
                    printf("Hit a soft 17 has been updated to %s\n", (g_settings.soft_rule) ? "true" : "false");
                    
                } else if (action == CHANGE_INSURANCE) {
                    
                    short new_insurance = 0;
                    
                    printf("============================================\n\n");
                    printf("Change whether the dealer offers insurance.\n");
                    printf("Enter 1 to enable or 0 to disable below.\n");
                    printf("\n============================================\n");
                    
                    while (true) {
                        printf("Enter 1 or 0: ");
                        new_insurance = (short) get_input();
                        if (new_insurance == INPUT_ERROR || new_insurance < 0 || new_insurance > 1) {
                            continue;
                        }
                        break;
                    }
                    
                    g_settings.insurance_rule = (new_insurance == 1);
                    change_settings("settings.bin");
                    printf("Insurance has been %s\n", (g_settings.insurance_rule) ? "enabled" : "disabled");
                    
                } else if (action == CHANGE_EVAL) {
                    
                    short new_eval = 0;
                    
                    printf("============================================\n\n");
                    printf("Change where decision evaluations are shown.\n");
                    printf("Enter 1 to enable or 0 to disable below.\n");
                    printf("\n============================================\n");
                    
                    while (true) {
                        printf("Enter 1 or 0: ");
                        new_eval = (short) get_input();
                        if (new_eval == INPUT_ERROR || new_eval < 0 || new_eval > 1) {
                            continue;
                        }
                        break;
                    }
                    
                    g_settings.show_eval = (new_eval == 1);
                    change_settings("settings.bin");
                    printf("Evaluations have been %s\n", (g_settings.show_eval) ? "enabled" : "disabled");
                   
                } else if (action == EXIT) {
                    break;
                }
                
            }
            
        } else if (action == EXIT) {
            printf("Goodbye.\n");
            break;
        }
    }
    
    free(g_deck);
    return 0;
    
}
