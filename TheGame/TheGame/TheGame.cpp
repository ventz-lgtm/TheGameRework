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
	int currentMenu;
	bool running;
	bool gameActive;
	bool debug;
};

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

void endGame(Game* pGame) {
	setActiveMenu(pGame, "Main Menu");
	pGame->gameActive = true;
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

int takeCard(int index, Deck* pDeck) {
	// Todo: create function body
	return 0;
}

int takeCardOnTop(Deck* pDeck) {
	int* pCard = getTopCard(pDeck);
	int card = *pCard;
	*pCard = 0;

	pDeck->deckSize = pDeck->deckSize - 1;

	return card;
}

void placeCardOnTop(Deck* deck, int card) {
	deck->cards[deck->deckSize] = card;
	deck->deckSize = deck->deckSize + 1;
}

bool playerDrawCard(Game* pGame, Player* pPlayer, bool print) {
	if (pPlayer->hand->deckSize >= pPlayer->hand->maxSize) { return false; }

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
		deck->cards[rand] = deck->cards[i] + deck->cards[rand];
		deck->cards[i] = deck->cards[rand] - deck->cards[i];
		deck->cards[rand] = deck->cards[rand] - deck->cards[i];
	}
}

// Display functions // 

void displayActiveMenu(Game* pGame) {
	for (int i = 0; i < pGame->menus[pGame->currentMenu].numOptions; i++) {
		std::cout << "[" << i << "] " << pGame->menus[pGame->currentMenu].options[i].name << std::endl;
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

	std::cout << std::endl << "-- Player Hand --" << std::endl;
	displayDeck(pGame->players[pGame->currentPlayerTurn].hand);

	std::cout << std::endl;
}

// Input functions //

int promptInput(Game* pGame) {
	displayActiveMenu(pGame);

	while (true) {
		int input = 0;
		std::cin >> input;

		if (std::cin.fail()) {
			std::cout << "Please enter a valid number!" << std::endl;
		}else if (input < 0 || input > pGame->menus[pGame->currentMenu].numOptions) {
			std::cout << "Please enter a valid option!" << std::endl;
		}
		else {
			return input;
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

void handlePlayerTurn(Game* pGame) {
	pGame->currentPlayerTurn = pGame->turns % pGame->numPlayers;
	std::cout << "-----[Turn: " << pGame->turns << " | Player " << pGame->currentPlayerTurn << " turn]-----" << std::endl;
	int choice = promptInput(pGame);

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

void addMenuOption(Menu* pMenu, std::string name, std::function<void()> callback) {
	MenuOption* menuOption = initializeMenuOption(name, callback);
	pMenu->options[pMenu->numOptions] = *menuOption;
	pMenu->numOptions = pMenu->numOptions + 1;
}

void initializeMenuOptions(Game* pGame) {
	Menu* pMainMenu = initializeMenu("Main Menu");

	addMenuOption(pMainMenu, "Start Game", [pGame] {
		startGame(pGame);
	});
	addMenuOption(pMainMenu, "Quit", [pGame] {
		pGame->running = false;
	});

	addMenu(pGame, pMainMenu);


	Menu* pGameMenu = initializeMenu("The Game");

	addMenuOption(pGameMenu, "Main Menu", [pGame] {
		endGame(pGame);
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
	Game* pGame = startGame(1);

	while (pGame->running) {
		handlePlayerTurn(pGame);
	}
	//displayGameDecks(game);
}