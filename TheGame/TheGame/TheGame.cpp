#pragma once
#include <string>
#include <iostream>

struct Deck {
	int* cards;
	int deckSize;
	int maxSize;
	bool ascending;
};

struct Player {
	Deck* hand;
};

struct Game {
	Deck* drawDeck;
	Deck* playPiles[4];
	Player* players[5];
	int numPlayers;
	int playerHandSize;
	int currentPlayerTurn;
	int turns;
};

// Display functions // 

void displayDeckTopCard(Deck* pDeck) {
	std::cout << pDeck->cards[pDeck->deckSize] << std::endl;
}

void displayDeck(Deck* pDeck) {
	for (int i = 0; i < pDeck->deckSize; i++) {
		std::cout << "[" << pDeck->cards[i] << "]";
	}
	std::cout << std::endl;
}

void displayGameDecks(Game* pGame) {
	std::cout << std::endl << "-- Play Piles --" << std::endl;
	
	std::cout << "Ascending deck 1: ";
	displayDeckTopCard(pGame->playPiles[0]);
	std::cout << "Ascending deck 2: ";
	displayDeckTopCard(pGame->playPiles[1]);
	std::cout << "Descending deck 1: ";
	displayDeckTopCard(pGame->playPiles[2]);
	std::cout << "Descending deck 2: ";
	displayDeckTopCard(pGame->playPiles[3]);

	std::cout << std::endl << "-- Draw Deck --" << std::endl;
	std::cout << pGame->drawDeck->deckSize << " cards remaining" << std::endl;

	std::cout << std::endl << "-- Player Hand --" << std::endl;
	displayDeck(pGame->players[pGame->currentPlayerTurn]->hand);

	std::cout << std::endl;
}

// Management functions //

int* getTopCard(Deck* pDeck) {
	return pDeck->cards + pDeck->deckSize;
}

void addCard(int index, int value, Deck* pDeck) {
	pDeck->cards[index] = value;
}

int takeCard(int index, Deck* pDeck) {
	// Todo: create function body
	return 0;
}

int takeCardOnTop(Deck* pDeck) {
	int* pCard = getTopCard(pDeck);
	int card = *pCard;
	pCard = 0;

	pDeck->deckSize = pDeck->deckSize - 1;

	return card;
}

void placeCardOnTop(Deck* deck, int card) {
	deck->cards[deck->deckSize] = card;
	deck->deckSize = deck->deckSize + 1;
}

bool playerDrawCard(Game* pGame, Player* pPlayer, bool print) {
	if (pPlayer->hand->deckSize >= pPlayer->hand->maxSize) { return false; }

	int card = takeCardOnTop(pGame->drawDeck);
	std::cout << "Draw card" << card << std::endl;

	placeCardOnTop(pPlayer->hand, card);

	return true;
}

bool playerDrawCard(Game* pGame, Player* pPlayer) {
	return playerDrawCard(pGame, pPlayer, false);
}

void populateDeck(int startCard, int numCards, Deck* pDeck) {
	int counter = 0;
	for (int i = startCard; i < numCards + startCard; i++) {
		addCard(counter, i, pDeck);
		counter++;
	}
}

void shuffleDeck(Deck* deck) {

}

// Initialization functions //

Deck* initializeDeck(int numCards, bool ascending) {
	Deck* deck = new Deck();

	deck->cards = new int[numCards];
	deck->deckSize = 0;
	deck->maxSize = numCards;
	deck->ascending = ascending;

	return deck;
}

Player* createPlayer(Game* pGame) {
	Player* pPlayer = new Player();

	Deck* playerHand = initializeDeck(pGame->playerHandSize, true);
	pPlayer->hand = playerHand;

	for (int i = 0; i < pGame->playerHandSize; i++) {
		playerDrawCard(pGame, pPlayer);
	}

	return pPlayer;
}

void initializeGame(Game* pGame, int numPlayers) {
	Deck* drawDeck = initializeDeck(98, true);
	populateDeck(2, 98, drawDeck);
	pGame->drawDeck = drawDeck;

	Deck* playPile1 = initializeDeck(100, true);
	addCard(0, 1, playPile1);

	Deck* playPile2 = initializeDeck(100, true);
	addCard(0, 1, playPile2);

	Deck* playPile3 = initializeDeck(100, false);
	addCard(0, 100, playPile3);

	Deck* playPile4 = initializeDeck(100, false);
	addCard(0, 100, playPile4);

	pGame->playPiles[0] = playPile1;
	pGame->playPiles[1] = playPile2;
	pGame->playPiles[2] = playPile3;
	pGame->playPiles[3] = playPile4;

	pGame->turns = 0;
	pGame->currentPlayerTurn = 0;
	pGame->numPlayers = numPlayers;
	// Todo: Calculate hand size based on player count
	pGame->playerHandSize = 8;

	for (int i = 0; i < numPlayers; i++) {
		pGame->players[i] = createPlayer(pGame);
	}
}

Game* startGame(int playerCount) {
	Game* pGame = new Game();

	initializeGame(pGame, playerCount);

	return pGame;
}

int main(int argc, char** argv) {
	Game* game = startGame(1);
	displayGameDecks(game);
}