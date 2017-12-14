#include "TheGame.h"

struct Deck {
	int* cards;
	int deckSize;
	int maxSize;
	bool ascending;
};

struct Player {
	Deck* hand;
};


struct MenuOption {
	std::string name;
	std::function<void()> callback;
	bool confirm;
};

struct Menu {
	std::string name;
	MenuOption options[10];
	int numOptions;
};

struct Game {
	Menu menus[10];
	Deck drawDeck;
	Deck playPiles[4];
	Player players[5];
	int numMenus;
	int numPlayers;
	int playerHandSize;
	int currentPlayerTurn;
	int turns;
	int cardsPlaced;
	int currentMenu;
	bool running;
	bool gameActive;
	bool debug;
};

void waitForConfirm() {
	std::cout << "Press enter to continue..." << std::endl;
	std::cin.clear();
	std::cin.ignore();
	std::cin.ignore();
}

void printGameOverview(Game* pGame) {
	std::cout << std::endl << "---[GAME OVERVIEW]---" << std::endl;
	std::cout << "Turns taken: " << pGame->turns << std::endl;
	std::cout << "Cards placed: " << pGame->cardsPlaced << std::endl;
	std::cout << "Draw deck cards remaining: " << pGame->drawDeck.deckSize << std::endl;
	std::cout << "---------------------" << std::endl << std::endl;
}

// Management functions //

void setActiveMenu(Game* pGame, std::string name) {
	for (int i = 0; i < pGame->numMenus; i++) {
		if (pGame->menus[i].name == name) {
			pGame->currentMenu = i;
			return;
		}
	}

	std::cout << "Error: no menu '" << name << "' exists!" << std::endl;
}

void startGame(Game* pGame) {
	setActiveMenu(pGame, "The Game");
	pGame->gameActive = true;
}

void endGame(Game* pGame, int status) {
	std::cout << "Game over!" << std::endl;
	printGameOverview(pGame);

	if (status != 0) {
		if (status == 1) {
			std::cout << "The game was won! All cards placed in play piles." << std::endl;
		}
		else {
			std::cout << "The game was lost! No possible moves remaining." << std::endl;
		}
		waitForConfirm();
	}

	setActiveMenu(pGame, "Main Menu");
	pGame->gameActive = false;
}

int randomRange(int min, int max) {
	return min + (std::rand() % static_cast<int>(max - min + 1));
}

int* getTopCard(Deck* pDeck) {
	return pDeck->cards + pDeck->deckSize - 1;
}

void addCard(int index, int value, Deck* pDeck) {
	if (pDeck->cards[index] == 0) {
		pDeck->deckSize = pDeck->deckSize + 1;
	}

	pDeck->cards[index] = value;
}

int takeCardOnTop(Deck* pDeck) {
	int* pCard = getTopCard(pDeck);
	int card = *pCard;
	*pCard = 0;

	pDeck->deckSize = pDeck->deckSize - 1;

	return card;

}
int takeCard(int index, Deck* pDeck) {
	int card = pDeck->cards[index];

	// If the index is not the final value, For each index greater than the provided index, shift backwards one to override removed card
	if (index < pDeck->deckSize - 1) {
		for (int i = 0; i < pDeck->deckSize - index; i++) {
			pDeck->cards[index + i] = pDeck->cards[index + i + 1];
		}
	}

	// Then take the top card to replace it with value 0, and adjust the deck size variable
	takeCardOnTop(pDeck);

	return card;
}

void placeCardOnTop(Deck* deck, int card) {
	deck->cards[deck->deckSize] = card;
	deck->deckSize = deck->deckSize + 1;
}

bool canPlayerPlaceCard(Game* pGame, Player* pPlayer, int playPile, int deckCard, bool print) {
	int topPileCard = *getTopCard(&pGame->playPiles[playPile]);
	int playerCard = pPlayer->hand->cards[deckCard];

	if (pGame->playPiles[playPile].ascending) {
		if (playerCard < topPileCard && playerCard != topPileCard - 10) {
			if (print) {
				std::cout << "You must pick a card higher than " << topPileCard;
				if (topPileCard - 10 >= 2) {
					std::cout << " or equal to " << topPileCard - 10;
				}
				std::cout << std::endl;
				waitForConfirm();
			}
			return false;
		}
	}
	else {
		if (playerCard > topPileCard && playerCard != topPileCard + 10) {
			if (print) {
				std::cout << "You must pick a card lower than " << topPileCard;
				if (topPileCard + 10 <= 99) {
					std::cout << " or equal to " << topPileCard + 10;
				}
				std::cout << std::endl;
				waitForConfirm();
			}
			return false;
		}
	}

	return true;
}

bool canPlayerPlaceCard(Game* pGame, Player* pPlayer, int playPile, int deckCard) {
	return canPlayerPlaceCard(pGame, pPlayer, playPile, deckCard, false);
}

bool playerPlaceCard(Game* pGame, Player* pPlayer, int playPile, int deckCard) {
	if (!canPlayerPlaceCard(pGame, pPlayer, playPile, deckCard, true)) {
		return false;
	}

	int playerCard = takeCard(deckCard, pPlayer->hand);
	placeCardOnTop(&pGame->playPiles[playPile], playerCard);

	pGame->cardsPlaced = pGame->cardsPlaced + 1;

	std::cout << "You placed card " << playerCard << " on play pile " << playPile + 1 << std::endl;
	waitForConfirm();
}

bool placeIsPossible(Game* pGame) {
	for (int p = 0; p < pGame->numPlayers; p++) {
		Player* pPlayer = &pGame->players[p];

		for (int i = 0; i < pPlayer->hand->deckSize; i++) {
			if (canPlayerPlaceCard(pGame, pPlayer, p, i)) {
				return true;
			}
		}
	}

	// If there are still possible cards to draw from the deck, do not end the game until the players have drawn them
	if (pGame->drawDeck.deckSize > 0) {
		return true;
	}

	return false;
}

int isGameOver(Game* pGame) {
	int cardsRemaining = 0;
	bool allDecksFull = true;

	for (int i = 0; i < pGame->numPlayers; i++) {
		if (pGame->players[i].hand->deckSize < pGame->playerHandSize) {
			allDecksFull = false;
		}

		cardsRemaining += pGame->players[i].hand->deckSize;
	}

	cardsRemaining += pGame->drawDeck.deckSize;

	if (cardsRemaining == 0) {
		return 1;
	}
	else {
		if (placeIsPossible(pGame)) {
			return 0;
		}
		else {
			// Keep game running if some players have room in their deck to draw new cards
			if (!allDecksFull) {
				return 0;
			}

			return 2;
		}
	}
}

bool playerDrawCard(Game* pGame, Player* pPlayer, bool print) {
	if (pPlayer->hand->deckSize >= pPlayer->hand->maxSize) { 
		if (print) {
			std::cout << "Your hand is full!" << std::endl;
		}
		waitForConfirm();
		return false; 
	}

	int card = takeCardOnTop(&pGame->drawDeck);
	
	if (print) {
		std::cout << "You drew the card: " << card << std::endl;
	}

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
	// Source a random seed before using the randomRange function
	std::srand(std::time(0));

	// For each value in the decks cards, swap it with a random different card in the deck, do this once for each index
	for (int i = 0; i < deck->deckSize; i++) {
		int rand = randomRange(0, deck->deckSize - 1);
		int temp = deck->cards[rand];
		deck->cards[rand] = deck->cards[i];
		deck->cards[i] = temp;
	}
}

// Display functions // 

void displayActiveMenu(Game* pGame) {
	std::cout << std::endl;
	std::cout << "[Menu Options]" << std::endl;
	for (int i = 0; i < pGame->menus[pGame->currentMenu].numOptions; i++) {
		std::cout << "[" << i + 1 << "] " << pGame->menus[pGame->currentMenu].options[i].name << std::endl;
	}
}

void displayDeckTopCard(Deck* pDeck) {
	std::cout << *getTopCard(pDeck) << std::endl;
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
	displayDeckTopCard(&pGame->playPiles[0]);
	std::cout << "Ascending deck 2: ";
	displayDeckTopCard(&pGame->playPiles[1]);
	std::cout << "Descending deck 1: ";
	displayDeckTopCard(&pGame->playPiles[2]);
	std::cout << "Descending deck 2: ";
	displayDeckTopCard(&pGame->playPiles[3]);

	std::cout << std::endl << "-- Draw Deck --" << std::endl;
	std::cout << pGame->drawDeck.deckSize << " cards remaining" << std::endl;

	std::cout << std::endl << "-----[Player " << pGame->numPlayers << " Hand]-----" << std::endl;
	displayDeck(pGame->players[pGame->currentPlayerTurn].hand);

	std::cout << std::endl;
}

// Input functions //

int promptInput(Game* pGame, int min, int max) {
	while (true) {
		int input = 0;
		std::cin >> input;

		if (std::cin.fail()) {
			std::cout << "Please enter a valid number!" << std::endl;
		}else if (input - 1 < min || input - 1 > max) {
			std::cout << "Please enter a valid option!" << std::endl;
		}
		else {
			return input - 1;
		}
	}
}

void runMenuOptionCallback(MenuOption* item) {
	if (item->confirm) {
		while (true) {
			std::cout << "Confirm '" << item->name << "' [Y/N]" << std::endl;
			std::string input;

			std::cin >> input;

			if (input == "y" || input == "Y") {
				break;
			}
			else if (input == "n" || input == "N") {
				return;
			}
			else {
				std::cout << "Y or N please.." << std::endl;
			}
		}
	}

	item->callback();
}

void playerChosePlaceCard(Game* pGame, Player* pPlayer) {
	while (true) {
		std::cout << "Choose pile to place card on:" << std::endl;
		std::cout << "[1] Descending deck 2 (" << *getTopCard(&pGame->playPiles[0]) << ")" << std::endl;
		std::cout << "[2] Descending deck 2 (" << *getTopCard(&pGame->playPiles[1]) << ")" << std::endl;
		std::cout << "[3] Descending deck 2 (" << *getTopCard(&pGame->playPiles[2]) << ")" << std::endl;
		std::cout << "[4] Descending deck 2 (" << *getTopCard(&pGame->playPiles[3]) << ")" << std::endl;
		std::cout << "[5] Cancel" << std::endl;

		int pileIndex = promptInput(pGame, 0, 4);

		if (pileIndex == 4) {
			break;
		}

		for (int i = 0; i < pPlayer->hand->deckSize; i++) {
			std::cout << "[" << i + 1 << "] " << pPlayer->hand->cards[i] << std::endl;
		}

		std::cout << "[" << pPlayer->hand->deckSize + 1 << "] Cancel" << std::endl;

		int cardIndex = promptInput(pGame, 0, pPlayer->hand->deckSize);

		if (cardIndex == pPlayer->hand->deckSize) {
			break;
		}

		if (playerPlaceCard(pGame, pPlayer, pileIndex, cardIndex)) {
			break;
		}
	}
}

void playerEndTurn(Game* pGame) {
	Player* pPlayer = &pGame->players[pGame->currentPlayerTurn];
	if (pPlayer->hand->deckSize < pGame->playerHandSize) {
		int numTakes = pGame->playerHandSize - pPlayer->hand->deckSize;
		for (int i = 0; i < numTakes; i++){
			playerDrawCard(pGame, &pGame->players[pGame->currentPlayerTurn], true);
		}
		waitForConfirm();
	}

	pGame->currentPlayerTurn = pGame->turns % pGame->numPlayers;

	pGame->turns = pGame->turns + 1;
	std::cout << "Turn ended! Next player: " << pGame->currentPlayerTurn + 1 << " / " << pGame->numPlayers << std::endl;
	waitForConfirm();
}

void handlePlayerTurn(Game* pGame) {
	int gameStatus = isGameOver(pGame);
	if (gameStatus != 0) {
		endGame(pGame, gameStatus);
	}

	std::cout << "[Game Status: " << gameStatus << "]" << std::endl;

	pGame->currentPlayerTurn = pGame->turns % pGame->numPlayers;

	if (pGame->gameActive) {
		std::cout << "-----[Turn: " << pGame->turns << " | Player " << pGame->currentPlayerTurn << " / " << pGame->numPlayers << " turn]-----" << std::endl;
		displayGameDecks(pGame);
	}
	else {
		pGame->turns = 1;
	}

	displayActiveMenu(pGame);
	int choice = promptInput(pGame, 0, pGame->menus[pGame->currentMenu].numOptions);

	runMenuOptionCallback(&pGame->menus[pGame->currentMenu].options[choice]);
}

// Initialization functions //

Deck* initializeDeck(int numCards, bool ascending) {
	Deck* deck = new Deck();

	deck->cards = new int[numCards];
	deck->deckSize = 0;
	deck->maxSize = numCards;
	deck->ascending = ascending;

	for (int i = 0; i < numCards; i++) {
		deck->cards[i] = 0;
	}

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
	shuffleDeck(drawDeck);
	
	pGame->drawDeck = *drawDeck;

	Deck* playPile1 = initializeDeck(100, true);
	addCard(0, 1, playPile1);

	Deck* playPile2 = initializeDeck(100, true);
	addCard(0, 1, playPile2);

	Deck* playPile3 = initializeDeck(100, false);
	addCard(0, 100, playPile3);

	Deck* playPile4 = initializeDeck(100, false);
	addCard(0, 100, playPile4);

	pGame->playPiles[0] = *playPile1;
	pGame->playPiles[1] = *playPile2;
	pGame->playPiles[2] = *playPile3;
	pGame->playPiles[3] = *playPile4;

	pGame->currentMenu = 0;
	pGame->turns = 0;
	pGame->cardsPlaced = 0;
	pGame->currentPlayerTurn = 0;
	pGame->numMenus = 0;

	pGame->numPlayers = numPlayers;
	// Todo: Calculate hand size based on player count
	pGame->playerHandSize = 8;

	for (int i = 0; i < numPlayers; i++) {
		pGame->players[i] = *createPlayer(pGame);
	}

	pGame->debug = false;
	pGame->running = true;
	pGame->gameActive = false;
}

MenuOption* initializeMenuOption(std::string name, std::function<void()> callback, bool confirm) {
	MenuOption* menuOption = new MenuOption();

	menuOption->name = name;
	menuOption->callback = callback;
	menuOption->confirm = confirm;

	return menuOption;
}

MenuOption* initializeMenuOption(std::string name, std::function<void()> callback) {
	return initializeMenuOption(name, callback, false);
}

Menu* initializeMenu(std::string name) {
	Menu* pMenu = new Menu();

	pMenu->name = name;
	pMenu->numOptions = 0;

	return pMenu;
}

void addMenu(Game* pGame, Menu* pMenu) {
	pGame->menus[pGame->numMenus] = *pMenu;
	pGame->numMenus = pGame->numMenus + 1;
}

void addMenuOption(Menu* pMenu, std::string name, std::function<void()> callback, bool confirm) {
	MenuOption* menuOption = initializeMenuOption(name, callback, confirm);
	pMenu->options[pMenu->numOptions] = *menuOption;
	pMenu->numOptions = pMenu->numOptions + 1;
}

void addMenuOption(Menu* pMenu, std::string name, std::function<void()> callback) {
	addMenuOption(pMenu, name, callback, false);
}

void initializeMenuOptions(Game* pGame) {
	Menu* pMainMenu = initializeMenu("Main Menu");

	addMenuOption(pMainMenu, "Start Game", [pGame] {
		startGame(pGame);
	});
	addMenuOption(pMainMenu, "Quit", [pGame] {
		pGame->running = false;
	}, true);

	addMenu(pGame, pMainMenu);


	Menu* pGameMenu = initializeMenu("The Game");

	addMenuOption(pGameMenu, "Place Card", [pGame] {
		playerChosePlaceCard(pGame, &pGame->players[pGame->currentPlayerTurn]);
	});
	addMenuOption(pGameMenu, "End Turn", [pGame] {
		playerEndTurn(pGame);
	});
	addMenuOption(pGameMenu, "Main Menu", [pGame] {
		endGame(pGame, 0);
	});

	addMenu(pGame, pGameMenu);
}

Game* startGame(int playerCount) {
	Game* pGame = new Game();

	initializeGame(pGame, playerCount);
	pGame->debug = true;
	initializeMenuOptions(pGame);

	return pGame;
}

int main(int argc, char** argv) {
	Game* pGame = startGame(5);

	while (pGame->running) {
		handlePlayerTurn(pGame);
	}
}