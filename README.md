# Blackjack Game
This project is meant to be a tool in learning more about blackjack and how much one might expect to win from playing it. Details on compiling and running are found at the bottom.
## Functionality
The program has 2 different purposes: practicing and simulating, both of which are configurable.
### Practice
Practice mode allows the user to play rounds of blackjack against an autonomous dealer. One can enter a different bet for each round, and the total winnings are displayed at the end. During a round, one may hit, stand, double, or split depending on their hand. Though unecessary, splitting works indefinitely and an indicator appears to show which hand is in action. Evaluations of one's decisions are displayed if the setting is enabled.
### Simulate
Simulation mode allows the user to simulate many rounds of blackjack with a high level of configurability. The amount of players, their bets and betting configurations can all be modified in runtime. Additionally, one can load and save players to or from a .csv file. If a player string is malformed in the file during loading, it is skipped over, and the remaining players are still read in. Each simulated player makes their decisions based on a standard Blackjack hitting table, which is present in a global array. It cannot be changed during runtime (yet) but can be modified in the code. The details for each betting variable can be found in their respective menus.
### Settings
The program has settings that can be edited in the menu. They are loaded from and saved to `settings.bin` which must exist in the same directory as the executable.
## Behaviors
The program will print the same screen until a valid input is entered. It should be obvious what is valid depending on the situation.
## Implementation Decisions
I wanted to make the program function as similarly as possible to an actual game of blackjack. For this reason, I chose different structures of lists for each data type. Linked lists are used a lot because one of my goals for this project was to practice their implementation and use.
- The deck is an array of pointers to cards. Having just an array of cards was problematic for edge cases when shuffling. Memory is allocated in the `generate_deck()` method.
- Each hand has an array of card pointers. Originally, new cards were actually allocated when they were drawn, which is not a very good solution. I feel that it is better to just have references to cards stored in each hand, even if the memory savings is not very much. Thus, drawing a card to a hand simply extends the array to add a new pointer. In the future, this implementation could be changed so that there is a predetermined card array size, like 3, that only extends if it needed to get rid of the overhead caused by allocating a new array and copying over values.
- Each player has a linked list of hands. It might seem like hands should be stored in an array, but having linked lists makes it much easier to handle splits, especially if the first hand is being split (you would have to move the next hand over or have the hands be out of order).
- The players are stored in a linked list. The only reason for this is that when loading players from a file, the correctly-formatted players do not need to be counted first. If players were stored in an array, the file would have to be traversed over twice so that the appropriate amount of memory can be allocated.
## Compilation
To compile and run the program, run the following commands while in the source directory. Use gcc or clang.

`gcc -c Blackjack.c` followed by `gcc -c Main.c` to generate the object files.

`gcc -o EXEC_NAME Blackjack.o Main.o` to link the object files. Replace `EXEC_NAME` with the desired name of the executable.

`./EXEC_NAME` runs the program.
