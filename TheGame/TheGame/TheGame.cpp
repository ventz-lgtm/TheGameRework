#include "TheGame.h"

struct Deck {
	int* cards;
	int deckSize;
	int maxSize;
	bool ascending;
};

struct Player {
	Deck* hand;
	bool isAI;
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
	int cardsPlacedThisTurn;
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
	// Loop through all the menus, and compare their names with the provided name, when it matches, set the active menu to it
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
	// Here i index a decks top card by returning the arrays index plus the size of the deck
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
		// If the chosen deck is ascending, check that the chosen card is either higher than the top card, or equal to the top card minus one
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
			// If the chosen deck is descending, check that the chosen card is either lower than the top card, or equal to the top card plus one
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
	// This function allows me to call canPlayerPlaceCard without explicitly providing false as a final argument if i dont want it to print output
	return canPlayerPlaceCard(pGame, pPlayer, playPile, deckCard, false);
}

bool playerPlaceCard(Game* pGame, Player* pPlayer, int playPile, int deckCard, bool print) {
	if (!canPlayerPlaceCard(pGame, pPlayer, playPile, deckCard, true)) {
		return false;
	}

	// If the player is allowed, first copy the card which they chose from their deck, and remove it from their deck
	// Then, place that card on top of the chosen play pile

	int playerCard = takeCard(deckCard, pPlayer->hand);
	placeCardOnTop(&pGame->playPiles[playPile], playerCard);

	// Next, increment the cardsPlaced counter variables
	pGame->cardsPlaced = pGame->cardsPlaced + 1;
	pGame->cardsPlacedThisTurn = pGame->cardsPlacedThisTurn + 1;

	if (print) {
		std::cout << "You placed card " << playerCard << " on play pile " << playPile + 1 << std::endl;
		waitForConfirm();
	}

	return true;
}

bool playerPlaceCard(Game* pGame, Player* pPlayer, int playPile, int deckCard) {
	// This function allows me to call playerPlaceCard without explicitly providing false as a final parameter if i dont want it to print output
	return playerPlaceCard(pGame, pPlayer, playPile, deckCard, false);
}

int isGameOver(Game* pGame) {
	int cardsRemaining = 0;

	for (int i = 0; i < pGame->numPlayers; i++) {
		cardsRemaining += pGame->players[i].hand->deckSize;
	}

	cardsRemaining += pGame->drawDeck.deckSize;

	// If the number of cards remaining is 0, end the game with a success message (1)
	if (cardsRemaining == 0) {
		return 1;
	}
	else {
		Player* pPlayer = &pGame->players[pGame->currentPlayerTurn];

		// For each current player card, for each play pile, return continue state (0) if a play is possible
		for (int c = 0; c < pPlayer->hand->deckSize; c++) {
			for (int p = 0; p < 4; p++) {
				if (canPlayerPlaceCard(pGame, pPlayer, p, c)) {
					return 0;
				}
			}
		}

		// If the current player has drawn enough cards to end their turn, return continue state (0) as next player can possibly place a card
		if (pGame->cardsPlacedThisTurn >= 2) {
			return 0;
		}

		// If the function has not returned by now, assume the game is failed as the current player cannot play any more cards
		return 2;
	}
}

bool playerDrawCard(Game* pGame, Player* pPlayer, bool print) {
	// If the players hand is full, do not allow them to take more cards, and tell them about it
	if (pPlayer->hand->deckSize >= pPlayer->hand->maxSize) { 
		if (print) {
			std::cout << "Your hand is full!" << std::endl;
		}
		waitForConfirm();
		return false; 
	}

	// If the player is allowed, take the top card from the draw deck, and place it on top of the player hand deck
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
	// For numCards, place a new card into the provided deck, the startCard is used to fetch the value of each card, and int i is used to index the deck
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

	std::cout << std::endl << "-----[Player " << pGame->currentPlayerTurn + 1 << " Hand]-----" << std::endl;
	displayDeck(pGame->players[pGame->currentPlayerTurn].hand);

	std::cout << std::endl;
}

// AI Functions

bool AI_shouldEndTurn(Game* pGame) {
	// Tell the AI they can end their turn if they have placed 2 or more cards
	if (pGame->cardsPlacedThisTurn >= 2) { return true; }

	return false;
}

void AI_pickCardToPlace(Game* pGame, Player* pPlayer) {

	// First, initialize some variables that will be used to keep track of the best possible place, and the index of the pile and card which is best
	int minDifference = INT_MAX;
	int bestPlayPile = -1;
	int bestCard = -1;

	// First, search for any opportunities to place card in the reverse direction in the ascending piles
	// For each ascending pile, for each player card, check if it is equal to the top card - 10
	for (int i = 0; i < 2; i++) {
		for (int c = 0; c < pPlayer->hand->deckSize; c++) {
			if (pPlayer->hand->cards[c] == *getTopCard(&pGame->playPiles[i]) - 10) {
				minDifference = 0;
				bestPlayPile = i;
				bestCard = c;
			}
		}
	}

	// First, search for any opportunities to place card in the reverse direction in the descending piles
	// For each ascending pile, for each player card, check if it is equal to the top card + 10
	for (int i = 2; i < 4; i++) {
		for (int c = 0; c < pPlayer->hand->deckSize; c++) {
			if (pPlayer->hand->cards[c] == *getTopCard(&pGame->playPiles[i]) + 10) {
				minDifference = 0;
				bestPlayPile = i;
				bestCard = c;
			}
		}
	}

	if (minDifference != 0) {
		// Next, if a card has not already been chosen, find the card which will have the smallest difference from the current top card in a play pile
		// For each ascending play pile, for each player card larger then the top card, check if the difference is smaller than the best current
		for (int i = 0; i < 2; i++) {
			for (int c = 0; c < pPlayer->hand->deckSize; c++) {
				if (pPlayer->hand->cards[c] < *getTopCard(&pGame->playPiles[i])) { continue; }

				int difference = pPlayer->hand->cards[c] - *getTopCard(&pGame->playPiles[i]);
				if (difference < minDifference) {
					minDifference = difference;
					bestPlayPile = i;
					bestCard = c;
				}
			}
		}

		// For each ascending play pile, for each player card smaller then the top card, check if the difference is smaller than the best current
		for (int i = 2; i < 4; i++) {
			for (int c = 0; c < pPlayer->hand->deckSize; c++) {
				if (pPlayer->hand->cards[c] > *getTopCard(&pGame->playPiles[i])) { continue; }

				int difference = *getTopCard(&pGame->playPiles[i]) - pPlayer->hand->cards[c];
				if (difference < minDifference) {
					minDifference = difference;
					bestPlayPile = i;
					bestCard = c;
				}
			}
		}
	}

	// Now that we know the best possible card to play, simply play the card
	std::cout << "Computer (player " << pGame->currentPlayerTurn + 1 << ") placed card '" << pPlayer->hand->cards[bestCard] << " on pile " << bestPlayPile << std::endl;

	bool placed = playerPlaceCard(pGame, pPlayer, bestPlayPile, bestCard);
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

	// This is the function used to run the menu callback lambda expression when an option is chosen
	// Unless the item is marked to ask for Y/N confirmation, this simply executes the callback function of the menu item

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

	// This is the function used to prompt the player to chose a card to place
	// This loops until the player either choses a valid deck and card combination to place, or choses to cancel

	while (true) {
		std::cout << "Choose pile to place card on:" << std::endl;
		std::cout << "[1] Ascending deck 1 (" << *getTopCard(&pGame->playPiles[0]) << ")" << std::endl;
		std::cout << "[2] Ascending deck 2 (" << *getTopCard(&pGame->playPiles[1]) << ")" << std::endl;
		std::cout << "[3] Descending deck 1 (" << *getTopCard(&pGame->playPiles[2]) << ")" << std::endl;
		std::cout << "[4] Descending deck 2 (" << *getTopCard(&pGame->playPiles[3]) << ")" << std::endl;
		std::cout << "[5] Cancel" << std::endl;

		// Prompt the user to chose a pile to place a card in, or to cancel
		int pileIndex = promptInput(pGame, 0, 4);

		if (pileIndex == 4) {
			break;
		}

		for (int i = 0; i < pPlayer->hand->deckSize; i++) {
			std::cout << "[" << i + 1 << "] " << pPlayer->hand->cards[i] << std::endl;
		}

		std::cout << "[" << pPlayer->hand->deckSize + 1 << "] Cancel" << std::endl;

		// Prompt the user to pick a card to place from their hand, or cancel
		int cardIndex = promptInput(pGame, 0, pPlayer->hand->deckSize);

		// The Cancel option index is equal to the deck size, so break if it is chosen
		if (cardIndex == pPlayer->hand->deckSize) {
			break;
		}

		// The playerPlaceCard function returns whether the placement was successful, if it was not, we do not break and repeat the loop
		if (playerPlaceCard(pGame, pPlayer, pileIndex, cardIndex, true)) {
			break;
		}
	}
}

void playerEndTurn(Game* pGame) {
	if (pGame->cardsPlacedThisTurn < 2) {
		std::cout << "You must place " << 2 - pGame->cardsPlacedThisTurn << " more cards before ending your turn." << std::endl;
		waitForConfirm();
		return;
	}

	Player* pPlayer = &pGame->players[pGame->currentPlayerTurn];
	if (pPlayer->hand->deckSize < pGame->playerHandSize) {
		int numTakes = pGame->playerHandSize - pPlayer->hand->deckSize;
		for (int i = 0; i < numTakes; i++){
			playerDrawCard(pGame, &pGame->players[pGame->currentPlayerTurn], true);
		}
		waitForConfirm();
	}

	pGame->turns = pGame->turns + 1;
	pGame->currentPlayerTurn = pGame->turns % pGame->numPlayers;
	pGame->cardsPlacedThisTurn = 0;

	std::cout << "Turn ended! Next player: " << pGame->currentPlayerTurn + 1 << " / " << pGame->numPlayers;
	if (pGame->players[pGame->currentPlayerTurn].isAI) {
		std::cout << " (Computer Player)";
	}
	std::cout << std::endl;

	waitForConfirm();

	system("cls");
}

void handlePlayerTurn(Game* pGame) {
	int gameStatus = isGameOver(pGame);
	if (gameStatus != 0) {
		endGame(pGame, gameStatus);
	}

	pGame->currentPlayerTurn = pGame->turns % pGame->numPlayers;

	Player* pPlayer = &pGame->players[pGame->currentPlayerTurn];
	if (pPlayer->isAI) {
		if (AI_shouldEndTurn(pGame)) {
			playerEndTurn(pGame);
		}
		else {
			AI_pickCardToPlace(pGame, pPlayer);
		}
	}
	else {
		std::cout << "[Game Status: " << gameStatus << "]" << std::endl;

		if (pGame->gameActive) {
			std::cout << "-----[Turn: " << pGame->turns << " | Player " << pGame->currentPlayerTurn + 1 << " / " << pGame->numPlayers << " turn]-----" << std::endl;
			displayGameDecks(pGame);
		}
		else {
			pGame->turns = 0;
		}

		displayActiveMenu(pGame);
		int choice = promptInput(pGame, 0, pGame->menus[pGame->currentMenu].numOptions);

		runMenuOptionCallback(&pGame->menus[pGame->currentMenu].options[choice]);
	}
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

Player* createPlayer(Game* pGame, bool isAI) {
	// A new player object is created AND initialized here
	Player* pPlayer = new Player();

	Deck* playerHand = initializeDeck(pGame->playerHandSize, true);
	pPlayer->hand = playerHand;
	pPlayer->isAI = isAI;

	// draw the hand size number of cards into the new players deck, to provide them with initial cards
	for (int i = 0; i < pGame->playerHandSize; i++) {
		playerDrawCard(pGame, pPlayer);
	}

	return pPlayer;
}

MenuOption* initializeMenuOption(std::string name, std::function<void()> callback, bool confirm) {
	// Here, MenuOption objects are created and their variables are initialized
	MenuOption* menuOption = new MenuOption();

	menuOption->name = name;
	menuOption->callback = callback;
	menuOption->confirm = confirm;

	return menuOption;
}

MenuOption* initializeMenuOption(std::string name, std::function<void()> callback) {
	// This allows me to call this function without explicitly providing false as the final argument
	return initializeMenuOption(name, callback, false);
}

Menu* initializeMenu(std::string name) {
	Menu* pMenu = new Menu();

	pMenu->name = name;
	pMenu->numOptions = 0;

	return pMenu;
}

void addMenu(Game* pGame, Menu* pMenu) {
	// Append a new menu object on to the end of the menu list, and increment the menu counter
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

void configGame(Game* pGame, int numPlayers, int numAI) {
	
	// This function is used to set how many players and AI will be in the game, and initialize the Player objects for the game
	// It takes an already initialized Game object, and sets the numPlayers variable in it.
	// It also creates new player objects equal to the number of players wanted, and sets whether they are AI players or not

	pGame->numPlayers = numPlayers + numAI;

	if (pGame->numPlayers == 1) {
		pGame->playerHandSize = 8;
	}
	else if (pGame->numPlayers == 2) {
		pGame->playerHandSize = 7;
	}
	else {
		pGame->playerHandSize = 6;
	}

	for (int i = 0; i < numPlayers; i++) {
		pGame->players[i] = *createPlayer(pGame, false);
	}
	for (int i = numPlayers; i < numAI + numPlayers; i++) {
		pGame->players[i] = *createPlayer(pGame, true);
	}
	
}

void playerConfigGame(Game* pGame) {

	// This function is used to allow the player to input the number of players and AI they want in the game
	// It collects input, then configs the game with the provided numbers requested

	std::cout << "How many players do you want? (5 Maximum)" << std::endl;
	int numPlayers = promptInput(pGame, 0, 4) + 1;
	int numAI = 0;

	if (numPlayers == 5) {
		std::cout << "There is no space to add computer players!" << std::endl;
		waitForConfirm();
	}
	else {
		std::cout << "How many computer players do you want? (" << 5 - numPlayers << " Maximum)" << std::endl;
		numAI = promptInput(pGame, -1, 4 - numPlayers) + 1;
	}

	configGame(pGame, numPlayers, numAI);
	startGame(pGame);
}

void initializeMenuOptions(Game* pGame) {

	// This function creates the Menu objects which will be in the game, and initialized all of their options
	Menu* pMainMenu = initializeMenu("Main Menu");

	// I chose to use lambda using the standard library functions to create callbacks for my menu options
	// This is because it is quick and neat to write, and can be easily changed in future
	// I pass in the name of the menu option, and a lambda expression which will be called when the user choses this menu option

	addMenuOption(pMainMenu, "Start Game", [pGame] {
		playerConfigGame(pGame);
	});
	addMenuOption(pMainMenu, "Quit", [pGame] {
		pGame->running = false;
	}, true);

	// After the menu options have been initialized, i add the Menu object to the games Menu list
	addMenu(pGame, pMainMenu);

	// Do the same for the Game menu

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

void initializeGame(Game* pGame, int numPlayers, int numAI) {

	// This function is where the Game object is first initialized
	// It created all the Deck objects for the game, including the play piles and draw deck
	// Player decks are not created here, because they are instead created later on, after the user chooses how many players and AI they want

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
	pGame->cardsPlacedThisTurn = 0;
	pGame->currentPlayerTurn = 0;
	pGame->numMenus = 0;

	configGame(pGame, numPlayers, numAI);

	pGame->debug = false;
	pGame->running = true;
	pGame->gameActive = false;

	// After the Game object has been initialized, i then initialize all the game menus
	initializeMenuOptions(pGame);
}

Game* createGame(int numPlayers, int numAI) {
	Game* pGame = new Game();

	initializeGame(pGame, numPlayers, numAI);
	pGame->debug = true;

	return pGame;
}

int main(int argc, char** argv) {
	Game* pGame = createGame(5, 0);

	while (pGame->running) {
		handlePlayerTurn(pGame);
	}
}