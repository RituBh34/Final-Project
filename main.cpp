#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <ctime>
#include <cstdlib>

using namespace std;

// Global variables
const int INITIAL_PLAYER_MONEY = 1000;

// Define card ranks and suits
enum Rank { ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING };
enum Suit { CLUBS, DIAMONDS, HEARTS, SPADES };

// Define card structure
struct Card {
    Rank rank;
    Suit suit;
};

// Function to print card details
void printCard(const Card& card) {
    switch (card.rank) {
    case ACE: cout << "A"; break;
    case TWO: cout << "2"; break;
    case THREE: cout << "3"; break;
    case FOUR: cout << "4"; break;
    case FIVE: cout << "5"; break;
    case SIX: cout << "6"; break;
    case SEVEN: cout << "7"; break;
    case EIGHT: cout << "8"; break;
    case NINE: cout << "9"; break;
    case TEN: cout << "10"; break;
    case JACK: cout << "J"; break;
    case QUEEN: cout << "Q"; break;
    case KING: cout << "K"; break;
    }

    switch (card.suit) {
    case CLUBS: cout << "C"; break;
    case DIAMONDS: cout << "D"; break;
    case HEARTS: cout << "H"; break;
    case SPADES: cout << "S"; break;
    }
}

// Function to calculate total value of a hand
int handValue(const vector<Card>& hand) {
    int total = 0;
    int numAces = 0;

    for (const auto& card : hand) {
        if (card.rank == ACE) {
            numAces++;
        }
        else if (card.rank >= TEN) {
            total += 10;
        }
        else {
            total += card.rank;
        }
    }

    // Calculate the value of aces
    for (int i = 0; i < numAces; ++i) {
        if (total + 11 <= 21) {
            total += 11;
        }
        else {
            total += 1;
        }
    }

    return total;
}

// Template class to manage the deck of cards
template<typename T>
class Deck {
private:
    vector<T> cards;
    queue<T> discardPile;
public:
    Deck() {
        for (int suit = CLUBS; suit <= SPADES; ++suit) {
            for (int rank = ACE; rank <= KING; ++rank) {
                cards.push_back({ static_cast<Rank>(rank), static_cast<Suit>(suit) });
            }
        }
        srand(time(0));
    }

    // Function to shuffle cards
    void shuffle() {
        random_shuffle(cards.begin(), cards.end());
    }

    // Function to deal a card
    T dealCard() {
        if (cards.empty()) {
            if (discardPile.empty()) {
                cout << "Both deck and discard pile are empty!" << endl;
                exit(1);
            }
            else {
                cout << "Shuffling cards from discard pile..." << endl;
                while (!discardPile.empty()) {
                    cards.push_back(discardPile.front());
                    discardPile.pop();
                }
                shuffle();
            }
        }
        T card = cards.back();
        cards.pop_back();
        return card;
    }

    // Function to discard a card
    void discard(const T& card) {
        discardPile.push(card);
    }

    // Function to access the cards vector
    const vector<T>& getCards() const {
        return cards;
    }
};

// Function to print a hand
template<typename T>
void printHand(const vector<T>& hand) {
    for (const auto& card : hand) {
        printCard(card);
        cout << " ";
    }
    cout << endl;
}

// Function to check if the player wants to hit, stand, or split
char playerAction() {
    char choice;
    while (true) {
        cout << "Hit (h), Stand (s), or Split (p)? ";
        cin >> choice;
        if (choice == 'h' || choice == 's' || choice == 'p') {
            return choice;
        }
        else {
            cout << "Please press a valid key (h for hit, s for stand, p for split)." << endl;
        }
    }
}

// Base class to represent a participant in the game
class Participant {
protected:
    string name;
    int money;
public:
    Participant(string name, int money) : name(name), money(money) {}

    string getName() const {
        return name;
    }

    int getMoney() const {
        return money;
    }

    void setMoney(int newMoney) {
        money = newMoney;
    }
};

// Class to represent a player
class Player : public Participant {
public:
    Player(string name, int money) : Participant(name, money) {}

    // Operator overloading for adding money
    Player& operator+=(int amount) {
        money += amount;
        return *this;
    }

    // Prefix increment operator overloading
    Player& operator++() {
        money++;
        return *this;
    }

    // Overloading the << operator to print player details
    friend ostream& operator<<(ostream& os, const Player& player);
};

// Definition of the << operator for Player objects
ostream& operator<<(ostream& os, const Player& player) {
    os << "Player: " << player.name << ", Money: " << player.money;
    return os;
}

// Class to represent a dealer
class Dealer : public Participant {
public:
    Dealer(string name) : Participant(name, 0) {}
};

// Static variable
static int staticVariable = 5;

// Function to play the game
template<typename T>
void playBlackjack() {
    // Global variable
    static int globalVariable = 10;

    Deck<T> deck;
    vector<T> playerHand;
    vector<T> dealerHand;
    vector<Player> players; // Vector to store multiple players
    int numPlayers;
    int playerBet = 0;

    cout << "Enter the number of players: ";
    cin >> numPlayers;

    // Initialize players
    for (int i = 0; i < numPlayers; ++i) {
        string playerName;
        cout << "Enter the name of player " << i + 1 << ": ";
        cin >> playerName;
        players.push_back(Player(playerName, INITIAL_PLAYER_MONEY));
    }

    while (true) {
        bool allPlayersBroke = true;

        // Check if all players are bankrupt
        for (const auto& player : players) {
            if (player.getMoney() > 0) {
                allPlayersBroke = false;
                break;
            }
        }

        if (allPlayersBroke) {
            cout << "All players are bankrupt!" << endl;
            break;
        }

        // Place bets for each player
        for (auto& player : players) {
            if (player.getMoney() == 0) {
                continue; // Skip if player is bankrupt
            }

            cout << " \nMoney available for " << player.getName() << ": " << player.getMoney() << endl;
            int playerBet; // Declare playerBet here to ensure it's scoped within the loop

            // Loop until valid input is received
            while (true) {
                cout << "Place your bet (0 to skip): ";
                if (!(cin >> playerBet)) {
                    // If user input is not a number
                    cout << "Please enter a valid number. \n" << endl;
                    cin.clear(); // Clear error flags
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
                }
                else if (playerBet < 0) {
                    // If user inputs a negative number
                    cout << "Please enter a non-negative number. \n" << endl;
                }
                else if (playerBet > player.getMoney()) {
                    // If user bets more than they have
                    cout << "You don't have enough money to place this bet." << endl;
                }
                else {
                    // Valid input received
                    break;
                }
            }

            player.setMoney(player.getMoney() - playerBet);

            // Initial deal
            playerHand.push_back(deck.dealCard());
            playerHand.push_back(deck.dealCard());
            dealerHand.push_back(deck.dealCard());
            dealerHand.push_back(deck.dealCard());

            cout << "\nYour hand, " << player.getName() << ": ";
            printHand(playerHand);
            cout << "Dealer's hand: ";
            printCard(dealerHand[0]);
            cout << " ?" << endl;

            // Check for blackjack
            if (handValue(playerHand) == 21) {
                cout << "Blackjack! " << player.getName() << " wins " << playerBet * 3 << "!" << endl;
                player += playerBet * 3; // Initial bet plus 2 times the bet
                playerHand.clear();
                dealerHand.clear();
                continue;
            }

            // Player's turn
            char action = playerAction();
            while (action != 's') {
                if (action == 'h') {
                    T newCard = deck.dealCard();
                    playerHand.push_back(newCard);
                    cout << "\nYour hand: ";
                    printHand(playerHand);
                    if (handValue(playerHand) > 21) {
                        cout << "***    Busted!    *** \n" << endl;
                        deck.discard(newCard); // Discard the busted card
                        break;
                    }
                }
                else if (action == 'p') {
                    cout << "You cannot split in this version." << endl;
                }
                action = playerAction();
            }

            // Dealer's turn
            cout << "Dealer's hand: ";
            printHand(dealerHand);
            while (handValue(dealerHand) < 17) {
                dealerHand.push_back(deck.dealCard());
                cout << "Dealer hits. Dealer's hand: ";
                printHand(dealerHand);
            }

            // Determine the winner
            int playerScore = handValue(playerHand);
            int dealerScore = handValue(dealerHand);

            cout << "Your score, " << player.getName() << ": " << playerScore << endl;
            cout << "Dealer's score: " << dealerScore << endl;

            if ((playerScore <= 21 && playerScore > dealerScore) || dealerScore > 21) {
                cout << "$$$$$$$$$$$$$$$$$" << endl;
                cout << "$               $" << endl;
                cout << "$      WINS     $" << endl;
                cout << "$               $" << endl;
                cout << "$$$$$$$$$$$$$$$$$ \n" << endl;
                cout << player.getName() << " gets " << playerBet * 2 << "! \n" << endl;
                player += playerBet * 2; // Initial bet plus twice the bet
            }
            else if (playerScore == dealerScore) {
                cout << "???????????????????????" << endl;
                cout << "?                     ?" << endl;
                cout << "?     It's a tie!     ?" << endl;
                cout << "?                     ?" << endl;
                cout << "??????????????????????? \n" << endl;
                cout << player.getName() << " gets back the bet of " << playerBet << ". \n" << endl;
                player += playerBet; // Player gets back their bet
            }
            else {
                cout << "XXXXXXXXXXXXXXXXXXXXXXX" << endl;
                cout << "X                     X" << endl;
                cout << "X     Dealer wins     X" << endl;
                cout << "X                     X" << endl;
                cout << "XXXXXXXXXXXXXXXXXXXXXXX \n" << endl;
                cout << player.getName() << " lost " << playerBet << "! \n" << endl;
            }

            playerHand.clear();
            dealerHand.clear();
        }
    }
}

int main() {
    char confirm;
    cout << "Welcome to the Blackjack game!" << endl;
    cout << "Are all players 21 or older? (y/n): ";
    cin >> confirm;

    if (confirm != 'y' && confirm != 'Y') {
        std::cout << "  ________" << std::endl;
        std::cout << " /        \\" << std::endl;
        std::cout << "/          \\" << std::endl;
        std::cout << "|  STOP!   |" << std::endl;
        std::cout << "|          |" << std::endl;
        std::cout << "|          |" << std::endl;
        std::cout << "|          |" << std::endl;
        std::cout << "|__________|" << std::endl;
        cout << "Sorry, this game is only for players who are 21 or older." << endl;
        return 0;
    }

    playBlackjack<Card>(); // No betting function for now

    return 0;
}