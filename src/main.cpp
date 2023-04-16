#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>

using namespace std;

struct Card {
    char suit;
    int value;

    Card(char suit, int value) : suit(suit), value(value) {}

    friend bool operator==(const Card &lhs, const Card &rhs) {
        return lhs.suit == rhs.suit && lhs.value == rhs.value;
    }
};

void displayCard(const Card &card) {
    cout << card.value << card.suit;
}

vector<Card> createEuchreDeck() {
    vector<Card> deck;
    const char suits[] = {'C', 'D', 'H', 'S'};

    for (const char &suit : suits) {
        for (int value = 9; value <= 14; ++value) {
            deck.push_back({suit, value});
        }
    }
    return deck;
}

void shuffleDeck(vector<Card> &deck) {
    srand(time(0));
    random_shuffle(deck.begin(), deck.end());
}

void dealCards(vector<vector<Card>> &players, vector<Card> &deck, int cardsToDeal) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < cardsToDeal; ++j) {
            players[i].push_back(deck.back());
            deck.pop_back();
        }
    }
}

void displayBotCards(const vector<vector<Card>> &players) {
    for (int i = 0; i < 4; ++i) {
        cout << "Bot " << i + 1 << ": ";
        for (const Card &card : players[i]) {
            displayCard(card);
            cout << " ";
        }
        cout << endl;
    }
}

void displayScore(int team1Points, int team2Points) {
    cout << "Team 1 Points: " << team1Points << endl;
    cout << "Team 2 Points: " << team2Points << endl;
}

bool canBeat(const Card &card, const Card &winningCard, char trump) {
    if (card.suit == winningCard.suit) {
        return card.value > winningCard.value;
    } else if (card.suit == trump) {
        return true;
    }
    return false;
}

Card chooseCard(const vector<Card> &hand, const Card &winningCard, char trump, bool lead) {
    Card chosenCard = hand[0];
    for (const Card &card : hand) {
        if (lead) {
            if (card.suit == trump) {
                chosenCard = card;
                break;
            }
        } else {
            if (canBeat(card, winningCard, trump)) {
                chosenCard = card;
                break;
            }
        }
    }
    return chosenCard;
}

int bidding(const vector<vector<Card>> &players) {
    int trumpCaller = -1;
    char trumpSuit = 'H';

    for (int i = 0; i < 4; ++i) {
        if (players[i][0].suit == trumpSuit || players[i][1].suit == trumpSuit) {
            trumpCaller = i;
            cout << "Bot " << trumpCaller + 1 << " calls trump: " << trumpSuit << endl;
            break;
        }
    }

    return trumpCaller;
}

void playRound(vector<vector<Card>> &players, int trumpCaller, char trumpSuit, int &team1Points, int &team2Points, int roundNumber) {
    int leadPlayer = (trumpCaller + 1) % 4;
    int winningPlayer = leadPlayer;
    Card winningCard = players[leadPlayer][0];
    players[leadPlayer].erase(players[leadPlayer].begin());
    cout << "Round " << roundNumber << ":\n";
    cout << "Bot " << leadPlayer + 1 << " plays: ";
    displayCard(winningCard);
    cout << endl;
    for (int i = 1; i < 4; ++i) {
        int currentPlayer = (leadPlayer + i) % 4;
        Card playedCard = chooseCard(players[currentPlayer], winningCard, trumpSuit, i == 0);
        auto cardIt = find(players[currentPlayer].begin(), players[currentPlayer].end(), playedCard);
        players[currentPlayer].erase(cardIt);

        cout << "Bot " << currentPlayer + 1 << " plays: ";
        displayCard(playedCard);
        cout << endl;

        if (canBeat(playedCard, winningCard, trumpSuit)) {
            winningPlayer = currentPlayer;
            winningCard = playedCard;
        }
    }

    cout << "Bot " << winningPlayer + 1 << " wins the trick" << endl;

    if (winningPlayer % 2 == 0) {
        team1Points += 1;
    } else {
        team2Points += 1;
    }

    if (trumpCaller == winningPlayer) {
        team1Points += trumpCaller % 2 == 0 ? -1 : 1;
        team2Points += trumpCaller % 2 == 0 ? 1 : -1;
    }

}

int main() {
    vector<vector<Card>> players(4);
    vector<Card> euchreDeck = createEuchreDeck();
    shuffleDeck(euchreDeck);
    int team1Points = 0;
    int team2Points = 0;
    bool gameOver = false;
    int roundNumber = 1;

    while (!gameOver) {
        cout << "Press any key to deal cards...";
        cin.get();

        dealCards(players, euchreDeck, 2);
        dealCards(players, euchreDeck, 3);

        displayBotCards(players);

        int trumpCaller = bidding(players);

        if (trumpCaller == -1) {
            cout << "No player called trump. Redealing..." << endl;
            for (vector<Card> &player : players) {
                player.clear();
            }
            euchreDeck = createEuchreDeck();
            shuffleDeck(euchreDeck);
            continue;
        }

        char trumpSuit = 'H';

        playRound(players, trumpCaller, trumpSuit, team1Points, team2Points, roundNumber);
        roundNumber++;

        if (team1Points >= 10 || team2Points >= 10) {
            gameOver = true;
        } else {
            displayScore(team1Points, team2Points);
        }

        for (vector<Card> &player : players) {
            player.clear();
        }

        euchreDeck = createEuchreDeck();
        shuffleDeck(euchreDeck);
    }

    if (team1Points >= 10) {
        cout << "Team 1 wins!" << endl;
    } else {
        cout << "Team 2 wins!" << endl;
    }

    return 0;
}
