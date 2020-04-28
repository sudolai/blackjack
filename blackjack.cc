/*
 * Blackjack
 *
 * William Gherman
 * 2020-04-29
 *
 * Standard game of blackjack on terminal. Uses color escape codes.
 *
 */
#include <iostream>
#include <algorithm>
#include <random>
#include <string>
#include <vector>
#include <numeric>


const bool isfacecard(const int &);
std::string getcard(const int &);
int deal(std::vector<int> &);
void printhands(const std::vector<int> &,
                const std::vector<std::vector<int>> &,
                const std::vector<int> &);
int sumcards(const std::vector<int> &);
int numaces(const std::vector<int> &);
void takeinsurance(const std::vector<int> &, const std::vector<int> &, int &);


const bool isfacecard(const int &card)
{
    int val = card % 13;
    return (val >= 10 || val == 0);
}

std::string getcard(const int &card)
{
    std::string card_string;
    switch(card % 13) {
        case 1:
            card_string = "A";
            break;
        case 10:
            card_string = "10";
            break;
        case 11:
            card_string = "J";
            break;
        case 12:
            card_string = "Q";
            break;
        case 0:
            card_string = "K";
            break;
        default:
            card_string = card % 13 + '0';
            break;
    }
    switch (card % 4) {
        case 0:
            card_string += u8"♤";
            break;
        case 1:
            card_string += u8"♧";
            break;
        case 2:
            card_string += u8"\033[31m♥\033[0m";
            break;
        case 3:
            card_string += u8"\033[31m♦\033[0m";
            break;
        default:
            break;
    }
    return card_string;
}

int deal(std::vector<int> &deck)
{
    int res = deck.back();
    deck.pop_back();
    return res;
}

void printhands(const std::vector<int> &dealer,
                const std::vector<std::vector<int>> &hands,
                const std::vector<int> &bets)
{
    int currhand = 1, sum;
    std::cout << "Dealer: " << getcard(dealer[0])
              << "\nYour hand(s):\n" << std::endl;
    for (auto hand : hands) {
        std::cout << "(" << currhand << "): ";
        for (auto card : hand)
            std::cout << getcard(card) << " ";
        sum = sumcards(hand);
        std::cout << "(";
        if (numaces(hand) == 1 && sum < 21)
            std::cout << sum << "/" << sum-10;
        else if (numaces(hand) > 0 && sum > 21)
            std::cout << sum - (10 * numaces(hand));
        else
            std::cout << sum;
        std::cout << ") $" << bets[currhand-1] << std::endl;
        ++currhand;


    }
}

int numaces(const std::vector<int> &hand)
{
    int aces = 0;
    for (auto card : hand) {
        if (card % 13 == 1)
            ++aces;
    }
    return aces;
}

int sumcards(const std::vector<int> &hand)
{
    int val;
    int sum = 0;
    for (int card : hand) {
        val = card % 13;
        if (val == 1)
            sum += 11;
        else if (isfacecard(card))
            sum += 10;
        else
            sum += val;
    }
    return sum;
}


void takeinsurance(const std::vector<int> &dealer, const std::vector<int> &bets,
                   int &chips)
{
    std::cout << "Dealer: " << getcard(dealer[0]) << " "
              << getcard(dealer[1]) << std::endl;
    int dealersum = sumcards(dealer);
    int sum = std::accumulate(bets.begin(), bets.end(), 0);
    if (dealersum == 21) {
        std::cout << "You won "
                  << sum * 3 / 2
                  << " chips!" << std::endl;
        chips += sum * 3;
    } else {
        std::cout << "You lost the bet." << std::endl;
        chips -= sum;
    }
}

int main()
{
    std::random_device rd;
    const int &DECK_SIZE = 52 * 4;
    int card, numhands, bet, chips = 1000;
    std::string c;

    std::cout << "Welcome to Blackjack. Blackjack pays 3:2. Insurance pays "
                 "2:1.\nDealer stands on 17." << std::endl;

    while (true) {
        std::vector<int> deck, dealer, bets;
        std::vector<std::vector<int>> hands;
        std::cout << "\n\nChips: " << chips
                  << "\nEnter number of hands (10 min. each): " << std::flush;
        std::cin >> numhands;
        if (numhands * 10 > chips) {
            std::cout << "You do not have enough chips." << std::endl;
            continue;
        }
        if (numhands < 1 || chips < 10)
            continue;
        for (int i = 1; i <= numhands; ++i) {
            std::cout << "Enter bet amount for hand " << i << ": "
                      << std::flush;
            std::cin >> bet;
            if (chips - bet < (numhands - i) * 10) {
                std::cout << "The max bet is " << chips - ((numhands - i) * 10)
                          << ". Try again." << std::endl;
                --i;
                continue;
            }
            bets.push_back(bet);
            chips -= bet;
        }


        for (int i = 0; i < DECK_SIZE; ++i)
            deck.push_back(i % 52);
        std::shuffle(std::begin(deck), std::end(deck), rd);

        // burn
        deck.pop_back();

        // deal two cards to all hands
        for (int i = 0; i < numhands; ++i)
            hands.push_back(std::vector<int>());
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < numhands; ++j)
                hands[j].push_back(deal(deck));
            dealer.push_back(deal(deck));
        }

        printhands(dealer, hands, bets);
        for (int i = 0; i < numhands; ++i) {
            if (sumcards(hands[i]) == 21) {
                std::cout << "Hand " << i << " has Blackjack!\n"
                             "You won " << bets[i] * 5 / 2 << " chips!"
                          << std::endl;
                chips += bets[i] * 5 / 2;
            }
        }
        if (dealer[0] % 13 == 1) {
            std::cout << "\nTake insurance? [y/N]: " << std::flush;
            std::cin >> c;
            if (c[0] == 'y' || c[0] == 'Y') {
                std::cout << "You chose to take insurance." << std::endl;
                takeinsurance(dealer, bets, chips);
                continue;
            } else
                std::cout << "You chose not to take insurance." << std::endl;
        }

        for (int i = 0; i < numhands; ++i) {
            if (sumcards(hands[i]) == 21)
                continue;
            std::cout << "Hand " << i+1 << ":" << std::endl;
            if ((hands[i][0] % 13 == hands[i][1] % 13) && chips >= bets[i]) {
                std::cout << "Split hand " << i+1 << "? (cost: "
                          << bets[i] << ") [y/N]: " << std::flush;
                std::cin >> c;
                if (c[0] == 'y' || c[0] == 'Y') {
                    chips -= bets[i];
                    ++numhands;
                    hands.insert(hands.cbegin() + i, hands[i]);
                    hands[i].pop_back();
                    hands[i+1].pop_back();
                    hands[i].push_back(deal(deck));
                    hands[i+1].push_back(deal(deck));
                    bets.insert(bets.cbegin() + i, bets[i]);
                } else {
                    std::cout << "You chose not to split hand " << i+1
                              << "." << std::endl;
                }
            }
            while (sumcards(hands[i]) < 21) {
                printhands(dealer, hands, bets);
                if (hands[i].size() == 2 && chips >= bets[i]) {
                    std::cout << "Hit, stand or double? [h/S/d]: "
                              << std::flush;
                } else {
                    std::cout << "Hit or stand? [h/S]: " << std::flush;
                }
                std::cin >> c;
                if ((c[0] == 'd' || c[0] == 'D') && hands[i].size() == 2
                     && chips >= bets[i]) {
                    chips -= bets[i];
                    bets[i] *= 2;
                    hands[i].push_back(deal(deck));
                } else if (c[0] == 'h' || c[0] == 'H') {
                    hands[i].push_back(deal(deck));
                } else {
                    break;
                }
            }
            if (sumcards(hands[i]) > 21)
                std::cout << "Hand " << i+1 << " busted!" << std::endl;
        }
        while (sumcards(dealer) < 17 ||
               (sumcards(dealer) == 17 && numaces(dealer) != 0))
            dealer.push_back(deal(deck));
        printhands(dealer, hands, bets);
        std::cout << "Dealer's full hand: ";
        for (auto card : dealer)
            std::cout << getcard(card) << " ";
        std::cout << "(" << sumcards(dealer) << ")" << std::endl;
        for (int i = 0; i < hands.size(); ++i) {
            if (sumcards(hands[i]) > 21)
                continue;
            std::cout << "Hand " << i+1;
            if (sumcards(dealer) > 21 ||
                     sumcards(hands[i]) > sumcards(dealer)) {
                std::cout << " won " << bets[i] << " chips!" << std::endl;
                chips += bets[i] * 2;
            } else if (sumcards(dealer) < 21 &&
                       sumcards(hands[i]) < sumcards(dealer)) {
                std::cout << " lost!" << std::endl;
            } else if (sumcards(dealer) < 21 &&
                       sumcards(hands[i]) == sumcards(dealer)) {
                std::cout << " pushes." << std::endl;
                chips += bets[i];
            }
        }
    }
    std::cout << "Thank you for playing!\nTotal score: " << chips
              << "Cash return: $" << (double)chips/100 << std::endl;

    return 0;
}
