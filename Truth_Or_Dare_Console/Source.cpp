using namespace std;
#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <time.h>
#include <conio.h>
#include <string>

// Group1 ------------------------------------------------------------ Group1
// Variables controlling active windows/options
int currentOptionIndex = 0;
int currentwindowIndex = 0;
bool listenKeys = true;
int currentOptionsLen = NULL;

// Group1.1
// Variables for text modification or cursor movement.
string inputPos = "\033[H\033[25;4H";
string hidePos = "\033[H\033[30;120H";
string endPos = "\033[H\033[23;4H";
string clearInput = "                                                                    ";
string resetText = "\u001b[0m";
string bold = "\u001b[1m";
string underline = "\u001b[4m";
string reversed = "\u001b[7m";
string dead = "\u001b[31m";

// Group1.2
// General function prototypes:
bool navigate(int asc);
void reprintwindow();
void rotateTurn();
void loadPresets();
void lineAt(int x);
void flatAt(int y);

// Group2 ------------------------------------------------------------ Group2
// Class and Variables for player

class player {
	//Access specifier:
public:
	string name;
	string color = "\u001b[36m";
	string prefix = "|";
	string suffix = "|";
	bool isSelected = false;
	int points = 3;
	int turnIndex;


	//Default Constructor:
	player() {
	}
	//Parameterized Constructor:
	player(string name) {
		this->name = name;
	}
	//First player Constructor:
	player(string name, bool isSelected) {
		this->name = name;
		this->isSelected = isSelected;
	}

	//Methods:
	string fullName() {
		if (isSelected) {
			return name;
		}
		return name;
	}
};

// Group3 ------------------------------------------------------------ Group3
// Class and Variables for playlist (sets of truths and dares)
class playlist {
	//Access specifier:
public:
	string name;
	vector<string> truths{};
	vector<string> dares{};

	//Default Constructor:
	playlist() {
	}

	//Parameterized Constructor:
	playlist(string name, vector<string> truths, vector<string> dares) {
		this->name = name;
		this->truths = truths;
		this->dares = dares;
	}

	//Methods:

};

//-Playlist truths:
vector<string> truths1{
	"When was the last time you lied?",
	"When was the last time you cried?",
	"What's your biggest fear?",
	"What's your biggest fantasy?"
};

//-Playlist dares:
vector<string> dares1{
	"Do 100 squats.",
	"Show the most embarrassing photo on your phone.",
	"Let the rest of the grouW DM someone from your Instagram account.",
	"Eat a banana without using your hands."
};

//Playlist:
playlist playlist1("Playlist1", truths1, dares1);
playlist playlist2("Playlist2", truths1, dares1);

// Group2 ------------------------------------------------------------ Group2
// VisualElement parent and it's child element classes (displaying screen and options to press)
class visualElement {
	//Access specifier:
public:
	string elementType = "visualElement";
	string name;
	string windowName;
	string color;

	virtual void printElement(string x) {
		cout << "Print base element." << endl;
	}
};

class lineStr : public visualElement {
	//Access specifier:
public:
	string str;

	//Default Constructor:
	lineStr() {
	}
	//Parameterized Constructor:
	lineStr(string str) {
		this->str = str;
		this->elementType = "lineStr";
		this->color = "\u001b[34m";
	}
	lineStr(string str, string name) {
		this->str = str;
		this->elementType = "lineStr";
		this->name = name;
		this->color = "\u001b[34m";
	}

	//Methods:
	void printElement(string x) {
		cout << bold << color << "\033[" + x + "C" << str << resetText << endl;
	}
};

class displayBar : public visualElement {
	// Access specifier:
public:
	vector<string>* displayItems;
	int displayIndex = 0;
	int space;

	// Default Constructor:
	displayBar() {
	}
	// Parameterized Constructor:
	displayBar(string name, vector<string>* displayItems, int displayIndex, int space) {
		this->name = name;
		this->displayItems = displayItems;
		this->displayIndex = displayIndex;
		this->space = space;
		this->elementType = "displayBar";
		this->color = "\u001b[35m";
	}
	// Methods:
	void printElement(string x) {
		string prefix = " ";
		string suffix = " ";
		cout << "\033[" + x + "C     ";
		for (int i = 0; i < (*displayItems).size(); i++) {
			if (i == displayIndex) {
				prefix = "|";
				suffix = "|";
			}
			else {
				prefix = " ";
				suffix = " ";
			}
			string tempStr = prefix + (*displayItems)[i] + suffix;

			if (i < (*displayItems).size() - 1) {
				for (int x = 0; x < space; x++) {
					tempStr += " ";
				}
			}
			cout << bold << color << tempStr << resetText;
		}
		cout << endl;
	}
};

class option : public visualElement {
	// Access specifier:
public:
	string prefix = " ";
	bool isSelected = false;
	void (*function)();
	void (*detail)();
	//Default Constructor:
	option() {
		this->elementType = "option";
		this->color = "\u001b[32m";
	}
	// Parameterized Constructor:
	option(string name, string windowName, void (*function)(), void (*detail)()) {
		this->name = name;
		this->windowName = windowName;
		this->function = function;
		this->detail = detail;
		this->elementType = "option";
		this->color = "\u001b[32m";
	}

	// Methods
	void printElement(string x) {
		cout << bold << color << "\033[" + x + "C" << prefix << name << resetText << endl;
		if (isSelected) {
			detail();
			isSelected = false;
		}
	}
};

class cache {
	// Cache stores players, playlist, and visualElement child objects.
	// Access specifier:
public:
	vector<player> players = {};
	vector<playlist> playlists = {};
	playlist* gamePlaylist;

	vector<visualElement> elements = {};
	vector<option> options = {};
	vector<lineStr> lineStrs = {};
	//Default Constructor:
	cache() {
	}
};
cache cache1 = cache();

class scoreboard : public visualElement {
	// Access specifier:
public:

	// Default constructor:
	scoreboard() {
		this->color = "\u001b[36m";
	}

	// Methods:
	void printElement(string x) {
		string columnX;
		cout << "\n\n\n";
		cout << bold << color << "\033[88C" << "Scoreboard\n\n" << resetText;
		cout << bold << color << "\033[88C" << underline << "Players" << resetText << endl;
		cout << "\033[A";
		cout << bold << color << "\033[104C" << underline << "Points" << resetText << "\n\n";

		for (int i = 0; i < cache1.players.size(); i++) {
			string if_format = "";
			if (cache1.players[i].points == 0) {
				if_format = dead;
			}
			else if (cache1.players[i].isSelected) {
				if_format = reversed;

			}
			else {
				if_format = "";
			}

			cout << bold << color << if_format << "\033[88C" << cache1.players[i].fullName() << resetText << endl;
			cout << "\033[A";
			cout << bold << color << if_format << "\033[104C" << to_string(cache1.players[i].points) + " points\n" << resetText;
		}
		cout << "\033[H\033[;1H";
	}
};

scoreboard scoreboard_points = scoreboard();

class sideBar {
	// Access specifier:
public:
	vector<string> column1 = {};
	vector<string> column2 = {};
	vector<vector<string>>(*update)();
	vector<vector<string>*> columns = { &column1, &column2 };

	// Default constructor:
	sideBar() {
	}
	// Parameterized constructor:
	sideBar(vector<vector<string>>(*update)()) {
		this->update = update;
	}

	// Methods:
	void updateBar() {
		column1 = update()[0];
		column2 = update()[1];
		columns = { &column1, &column2 };
	}
	void printSideBar() {
		//Reset cursor location.
		cout << "\033[H\033[;1H";
		updateBar();
		string columnX;
		string cursorPos;
		cursorPos = "\033[H\033[4;88H";
		cout << cursorPos << bold << "Scoreboard\n\n" << resetText;
		cout << bold << "\033[88C" << "\u001b[0m\u001b[4m" + (*columns[0])[0] + "\u001b[0m" << resetText << endl;
		cout << "\033[A";
		cout << bold << "\033[104C" << "\u001b[0m\u001b[4m" + (*columns[1])[0] + "\u001b[0m" << resetText << "\n\n";
		for (int i = 0; i < 2; i++) {
			if (i == 0) {
				columnX = "88C";
			}
			else {
				columnX = "104C";
			}
			for (int j = 1; j < columns[i]->size(); j++) {
				cout << "\033[" + columnX;
				cout << bold << (*columns[i])[j] << resetText << endl;
			}
			cout << "\033[" + to_string(columns[i]->size() - 1) + "A";
		}
	}
};

// Group2.1 ----------------------------------------------------------- Group2.1
// Visual Element objects + data members initialization.

// Visual Element displayBars:
vector<string> mainDisplayItems = { "Begin", "Play", "Playlist", "Settings" };
vector<string> playerDisplayItems = { "Players:" };

displayBar mainBar = displayBar("mainDisplayBar", &mainDisplayItems, 0, 8);
displayBar playersBar = displayBar("playersDisplayBar", &playerDisplayItems, -1, 1);
vector<displayBar*> globalBars = { &mainBar, &playersBar };

// Visual Element options: 
void detailTemp();
void detailPlaylist();
void detailAddTruth();
void detailAddDare();
void detailSetName();
void functionTemp();
void functionBegin();
void functionNext();
void functionAddPlayer();
void functionFinished();
void functionNew();
void functionSetName();
void functionAddTruth();
void functionAddDare();
void functionTruth();
void functionDare();
void functionToggleRandomTurns();
void detailRandomTurns();
void functionToggleTallyVoting();
void detailTallyVoting();
void functionToggleTurnDetails();
void detailTurnDetails();
void functionPerformersPerTurn();
void detailPerformersPerTurn();

option option_new = option("New...", "playlistP", &functionNew, &detailTemp);    //New option "new"
option option_setName = option("Set Name", "playlistCreatorP", &functionSetName, &detailSetName);
option option_addTruth = option("Add Truth", "playlistCreatorP", &functionAddTruth, &detailAddTruth);
option option_addDare = option("Add Dare", "playlistCreatorP", &functionAddDare, &detailAddDare);
option option_createPlaylist = option("Create Playlist", "playlistCreatorP", &functionBegin, &detailTemp);
vector<option*> options_playlistCreator = { &option_setName, &option_addTruth, &option_addDare, &option_createPlaylist };

option option_begin = option("Begin", "beginP", &functionBegin, &detailTemp);
vector<option*> options_mainwindow = { &option_begin };
vector<option*> options_playlistwindow = { &option_new };
option option_next = option("Next", "settingsP", &functionNext, &detailTemp);
option option_randomTurns = option("Random Turns", "settingsP", &functionToggleRandomTurns, &detailRandomTurns);
option option_tallyVoting = option("Tally Voting", "settingsP", &functionToggleTallyVoting, &detailTallyVoting);
option option_turnDetails = option("Turn Details", "settingsP", &functionToggleTurnDetails, &detailTurnDetails);
option option_performersPerTurn = option("Performers Per Turn", "settingsP", &functionPerformersPerTurn, &detailPerformersPerTurn);

vector<option*> options_settingswindow = { &option_next, &option_randomTurns, &option_tallyVoting, &option_turnDetails, &option_performersPerTurn };
option option_addPlayer = option("Add Player", "playPlayerInputP", &functionAddPlayer, &detailTemp);
option option_finished = option("Finished", "playPlayerInputP", &functionFinished, &detailTemp);
vector<option*> options_playerInputwindow = { &option_addPlayer, &option_finished };
option option_truth = option("Truth", "playTruthOrDareP", &functionTruth, &detailTemp);
option option_dare = option("Dare", "playTruthOrDareP", &functionDare, &detailTemp);
vector<option*> options_playwindow = { &option_truth, &option_dare };

vector<vector<option*>> globalOptions = { options_playlistwindow, options_mainwindow, options_playlistCreator, options_settingswindow, options_playerInputwindow, options_playwindow };

// Visual element sideBars:
vector<vector<string>> updatePlayers();

sideBar sideBar_none = sideBar();
sideBar sideBar_players = sideBar(&updatePlayers);

class window {
	// Access specifier:
public:
	string name;
	vector<visualElement*> elements;
	sideBar sideBarE;
	bool hasSideBar;
	int printingY;
	string x;
	string y;


	vector<visualElement*> postElements;
	vector<displayBar*> bars;
	vector<option*> options;
	// Default Constructor:
	window() {
	}
	// Parameterized Constructor:
	window(string name, vector<visualElement*> elements) {
		this->name = name;
		this->elements = elements;
		this->bars = bars;
		this->options = options;
		this->hasSideBar = false;
		this->x = "3";
		this->y = "0";
		//Compare elements list vs global options and display bars. .push_back to the respective list.
		for (int i = 0; i < elements.size(); i++) {
			if (elements[i]->elementType == "option") {
				for (int x = 0; x < globalOptions.size(); x++) {
					for (int y = 0; y < globalOptions[x].size(); y++) {
						if (elements[i]->name == globalOptions[x][y]->name && elements[i]->windowName == globalOptions[x][y]->windowName) {
							options.push_back(globalOptions[x][y]);
						}
					}
				}
			}
			if (elements[i]->elementType == "displayBar") {
				for (int x = 0; x < globalBars.size(); x++) {
					if (elements[i]->name == globalBars[x]->name) {
						bars.push_back(globalBars[x]);
					}
				}
			}
		}
	}
	window(string name, vector<visualElement*> elements, sideBar sideBarE) {
		this->name = name;
		this->elements = elements;
		this->bars = bars;
		this->options = options;
		this->sideBarE = sideBarE;
		this->hasSideBar = true;
		this->x = "3";
		this->y = "0";
		//Compare elements list vs global options and display bars. .push_back to the respective list.
		for (int i = 0; i < elements.size(); i++) {
			if (elements[i]->elementType == "option") {
				for (int x = 0; x < globalOptions.size(); x++) {
					for (int y = 0; y < globalOptions[x].size(); y++) {
						if (elements[i]->name == globalOptions[x][y]->name && elements[i]->windowName == globalOptions[x][y]->windowName) {
							options.push_back(globalOptions[x][y]);
						}
					}
				}
			}
			if (elements[i]->elementType == "displayBar") {
				for (int x = 0; x < globalBars.size(); x++) {
					if (elements[i]->name == globalBars[x]->name) {
						bars.push_back(globalBars[x]);
					}
				}
			}
		}
	}

	//Methods:
	void coutW(string str) {
		printingY++;
		cout << "\033[H\033[" + to_string(printingY) + ";1H";
		cout << "\033[" + x + "C" << str << endl;
	}
	void optionsToPostElements() {
		for (int i = 0; i < options.size(); i++) {
			postElements.push_back(options[i]);
		}
	}
	//Assign prefix for options as an indicator for user selection.
	void assignPrefixes() {
		int optionIndex = 0;
		int testIndex = 0;
		vector<vector<visualElement*>> allElements = { elements, postElements };
		for (int h = 0; h < allElements.size(); h++) {
			for (int i = 0; i < allElements[h].size(); i++) {
				if (allElements[h][i]->elementType == "option") {
					if (optionIndex == currentOptionIndex) {
						options[optionIndex]->prefix = ">";
						options[optionIndex]->isSelected = true;
					}
					else {
						options[optionIndex]->prefix = ":";
					}
					optionIndex++;
				}
				testIndex++;
			}
		}
	}
	void printwindow() {
		system("CLS");
		printingY = 0;
		//Reset cursor location.
		cout << "\033[H\033[;1H";
		assignPrefixes();
		for (int i = 0; i < elements.size(); i++) {
			elements[i]->printElement(x);
			printingY++;
		}
		for (int i = 0; i < postElements.size(); i++) {
			postElements[i]->printElement(x);
			printingY++;
		}
		if (hasSideBar) {
			sideBarE.printSideBar();
		}
		lineAt(3);
		lineAt(72);
		lineAt(86);
		lineAt(118);
		flatAt(3);
		flatAt(5);
		flatAt(24);
		flatAt(26);
		cout << hidePos;
	}
};

//-window prompts:
lineStr ____space("");
lineStr ____line("--------------------------------------------------------------------               -------------------------------");
lineStr str_mainPrompt("Welcome to Truth or Dare! Use the arrows and enter to navigate.");
lineStr str_playlistPrompt("Playlist Selector. Choose an existing playlist or create a new set.");
lineStr str_settingsPrompt("Settings. Hit enter on an option to toggle.");
lineStr str_playerInputPrompt("Play - Character Input. Add players to the game (2-4 players).");
lineStr str_playlistCreatorPrompt("Playlist Creator. Create a your own playlist.");
lineStr str_playwindowPrompt("Truth or Dare!");
lineStr str_currentActivePlayers("", "str_currentActivePlayers");

//windows:
vector<visualElement*> mainElements = {
	&str_mainPrompt,
	&____space,
	&____line,
	&mainBar,
	&____line,
	&____space,
	&option_begin
};
window mainW = window("beginP", mainElements);

vector<visualElement*> playlistElements = {
	&str_playlistPrompt,
	&____space,
	&____line,
	&mainBar,
	&____line,
	&____space,
};

window playlistW = window("playlistP", playlistElements);

vector<visualElement*> playlistCreatorElements = {
	&str_playlistCreatorPrompt,
	&____space,
	&____line,
	&mainBar,
	&____line,
	&____space,
	&option_setName,
	&option_addTruth,
	&option_addDare,
	&option_createPlaylist
};
window playlistCreatorW = window("playlistCreatorP", playlistCreatorElements);

vector<visualElement*> settingsElements = {
	&str_settingsPrompt,
	&____space,
	&____line,
	&mainBar,
	&____line,
	&____space,
	&option_randomTurns,
	&option_tallyVoting,
	&option_turnDetails,
	&option_performersPerTurn,
	&option_next
};
window settingsW = window("settingsP", settingsElements);

vector<visualElement*> playerInputElements = {
	&str_playerInputPrompt,
	&____space,
	&____line,
	&playersBar,
	&____line,
	&____space,
	&option_addPlayer,
	&option_finished
};
window playerInputW = window("playPlayerInputP", playerInputElements);

vector<visualElement*> playElements = {
	&scoreboard_points,
	&str_playwindowPrompt,
	&____space,
	&____line,
	&mainBar,
	&____line,
	&str_currentActivePlayers,
	&____space,
	&option_truth,	&option_dare

};
window playW = window("playTruthOrDareP", playElements);

vector<visualElement*> blankElements = {};
window blankW = window("blankP", blankElements);
vector<window*> windows = { &mainW, &playlistW, &settingsW, &playerInputW, &playlistCreatorW, &playW };

class page {
	// Access specifier:
public:
	string x;
	string y;
	window window_left;
	window window_right;
	// Default constructor:
	page() {
	}
	// Parameterized constructor:
	page(window window_left, window window_right) {
		this->window_left = window_left;
		this->window_right = window_right;
	}
	// Methods:
	void printPage() {
		window_left.printwindow();
		window_right.printwindow();
	}
};

// Global Variables
int* pIndex = &playersBar.displayIndex;
string playerInput;
string groupInput;

// Group5 ------------------------------------------------------------ Group5
// Classes and Variables for game controlling

void activatewindow(window* p);

class gameController {
	// Access specifier:
	string str_activePlayers;
public:
	int performersPerTurn = 1;
	bool turnRandom = false;
	bool tallyVoting = false;
	bool detailsPrompt = false;
	int turnIndex = 0;
	vector<player> performingPlayers;

	// Default constructor:
	gameController() {
	}
	// Parameterized constructor:
	gameController(int performersPerTurn, bool turnRandom, bool tallyVoting, bool detailsPrompt) {
		this->performersPerTurn = performersPerTurn;
		this->turnRandom = turnRandom;
		this->tallyVoting = tallyVoting;
		this->detailsPrompt = detailsPrompt;
	}

	int secondIndex() {
		if (turnIndex == cache1.players.size() - 1) {
			return 0;
		}
		else {
			return turnIndex + 1;
		}
	}
	void selectPerformingPlayers() {
		for (int i = 0; i < cache1.players.size(); i++) {
			if (cache1.players[i].isSelected) {
				cache1.players[i].isSelected = false;
			}
		}

		cache1.players[turnIndex].isSelected = true;
		str_activePlayers = cache1.players[turnIndex].name;
		str_currentActivePlayers.str = str_activePlayers + " is currently performing. Choose the performance type:";

		if (performersPerTurn == 2) {
			str_activePlayers = cache1.players[turnIndex].name + " and ";
			cache1.players[secondIndex()].isSelected = true;
			str_activePlayers.append(cache1.players[secondIndex()].name);
			str_currentActivePlayers.str = str_activePlayers + " are currently performing.";
		}
	}
	void updatePlayPage() {
		selectPerformingPlayers();
	}
	// sequence step 1
	void roundStage1(bool pressedTruth) {
		if (pressedTruth) {
			int tIndex = rand() % cache1.gamePlaylist->truths.size();
			playW.coutW("");
			playW.coutW("Truth: " + cache1.gamePlaylist->truths[tIndex] + "\u001b[0m");
		}
		else {
			int dIndex = rand() % cache1.gamePlaylist->dares.size();
			playW.coutW("");
			playW.coutW("Dare: " + cache1.gamePlaylist->dares[dIndex] + "\u001b[0m");
		}
	}
	// sequence step 2
	void roundStage2() {
		if (detailsPrompt) {
			string tempInput;
			cout << inputPos << "Summary:";
			cin.ignore();
			getline(cin, tempInput);
			cout << inputPos << clearInput;
			playW.coutW(tempInput);
		}
	}
	// sequence step 3
	void roundStage3() {
		if (tallyVoting) {
			playW.coutW("");
			playW.coutW("You will be asked to vote on each player.");
			int tempInput = -1;
			while (tempInput < 0 || tempInput > cache1.players.size() - 1) {
				cout << inputPos << "Votes for " + cache1.players[turnIndex].name + " passing. One vote per audience player: ";
				cin >> tempInput;
				cout << inputPos << clearInput;
			}
			playW.coutW(cache1.players[turnIndex].name + " received \u001b[36m+" + to_string(tempInput) + "pts" + resetText + " votes for and\u001b[36m - " + to_string(cache1.players.size() - 1 - tempInput) + "pts" + resetText + " votes against.");
			cache1.players[turnIndex].points += tempInput;
			cache1.players[turnIndex].points -= cache1.players.size() - 1 - tempInput;
			if (performersPerTurn == 2) {
				tempInput = -1;
				while (tempInput < 0 || tempInput > cache1.players.size() - 1) {
					cout << inputPos << "Votes for " + cache1.players[secondIndex()].name + " passing. One vote per audience player: ";
					cin >> tempInput;
					cout << inputPos << clearInput;
				}
				playW.coutW(cache1.players[secondIndex()].name + " received \u001b[36m+" + to_string(tempInput) + "pts" + resetText + " votes for and\u001b[36m - " + to_string(cache1.players.size() - 1 - tempInput) + "pts" + resetText + " votes against.");
				cache1.players[secondIndex()].points += tempInput;
				cache1.players[secondIndex()].points -= cache1.players.size() - 1 - tempInput;
			}
		}
		else {
			// Binary Pass Fail rating.
			playW.coutW("");
			playW.coutW("Did the performing player(s) pass?");
			string tempInput;
			while (tempInput != "passed" && tempInput != "failed") {
				cout << inputPos << "Enter if " + cache1.players[turnIndex].name + " (passed) or (failed) : ";
				cin >> tempInput;
				cout << inputPos << clearInput;
			}
			if (tempInput == "passed") {
				playW.coutW(cache1.players[turnIndex].name + " " + tempInput + " \u001b[36m+1pt" + resetText + ".");
				cache1.players[turnIndex].points++;
			}
			else if (tempInput == "failed") {
				playW.coutW(cache1.players[turnIndex].name + " " + tempInput + " \u001b[36m-1pt" + resetText + ".");
				cache1.players[turnIndex].points--;
			}
			if (performersPerTurn == 2) {
				tempInput = " ";
				while (tempInput != "passed" && tempInput != "failed") {
					cout << inputPos << "Enter if " + cache1.players[secondIndex()].name + " (passed) or (failed) : ";
					cin >> tempInput;
					cout << inputPos << clearInput;
				}
				if (tempInput == "passed") {
					playW.coutW(cache1.players[secondIndex()].name + " " + tempInput + " \u001b[36m+1pt" + resetText + ".");
					cache1.players[secondIndex()].points++;
				}
				else if (tempInput == "failed") {
					playW.coutW(cache1.players[secondIndex()].name + " " + tempInput + " \u001b[36m-1pt" + resetText + ".");
					cache1.players[secondIndex()].points--;
				}
			}
		}
	}
	void roundStage4() {
		playW.coutW("");
		playW.coutW("Press any key to continue.");
		cout << inputPos << "Continue:";
		char c = _getch();
	}
	void roundStage5() {
		int activePlayerCount = 0;
		for (int i = 0; i < cache1.players.size(); i++) {
			if (cache1.players[i].points > 0) {
				activePlayerCount++;
			}
		}
		if (activePlayerCount <= 1) {
			for (int i = 0; i < cache1.players.size(); i++) {
				if (cache1.players[i].points > 0) {
					system("CLS");
					cout << cache1.players[i].name + " has won the game!" << endl;
					exit(0);
				}
			}
		}

		else {
			if (turnRandom) {
				turnIndex = rand() % cache1.players.size();
			}
			else {
				turnIndex = secondIndex();
			}
			selectPerformingPlayers();
		}
	}
	void roundSequence(bool pressedTruth) {
		// Print Random Truth or Dare.
		roundStage1(pressedTruth);
		// If detailsPrompt, request performance summary.
		roundStage2();
		// Request pass/fail input, or tally vote.
		roundStage3();
		// Enter to continue.
		roundStage4();
		// Rotate turns.
		roundStage5();
	}

};
gameController game = gameController();

// GroupMain ------------------------------------------------------ GroupMain

int main() {
	srand(time(NULL));
	activatewindow(&mainW);
	bool runOnce = true;
	if (runOnce) {
		loadPresets();
		runOnce = false;
	}

	while (listenKeys) {
		char c = _getch();
		int ascii = c;
		//Use arrows to change currentOptionIndex.
		if (navigate(ascii)) {

		}
		if (ascii == 13) {
			//When enter is pressed, run the function of the currentOption.
			window currentwindow = *windows[currentwindowIndex];
			option currentOption = *currentwindow.options[currentOptionIndex];
			currentOption.function();
		}
	}
}

// General
bool navigate(int asc) {

	if (asc == 72 && currentOptionIndex > 0) {
		currentOptionIndex--;
		(*windows[currentwindowIndex]).printwindow();
		return true;
	}
	if (asc == 80 && currentOptionIndex < currentOptionsLen - 1) {
		currentOptionIndex++;
		(*windows[currentwindowIndex]).printwindow();
		return true;
	}
	return false;
}
void reprintwindow() {
	windows.at(currentwindowIndex)->printwindow();
}
void activatewindow(window* p) {
	for (int i = 0; i < windows.size(); i++) {
		if ((p->name).compare((*windows[i]).name) == 0) {
			currentwindowIndex = i;
			break;
		}
	}
	currentOptionsLen = p->options.size();
	currentOptionIndex = 0;
	p->printwindow();
}
void rotateTurn() {
	if (*pIndex < playerDisplayItems.size() - 1) {
		*pIndex = *pIndex + 1;
	}
	else {
		*pIndex = 1;
	}
	reprintwindow();
}
void loadPresets() {
	cache1.playlists.push_back(playlist1);
	cache1.playlists.push_back(playlist2);
}
void lineAt(int x) {
	for (int i = 0; i < 22; i++) {
		if (i != 1) {
			string cursorPos = "\033[H\033[" + to_string(i + 4) + ";" + to_string(x) + "H";
			cout << cursorPos << bold << "|" << resetText;
		}
	}
	//Reset cursor location.
	cout << "\033[H\033[;1H";
}
void flatAt(int y) {
	string cursorPos = "\033[H\033[" + to_string(y) + ";3H";
	cout << cursorPos << bold << " --------------------------------------------------------------------               -------------------------------" << resetText;
}

// Option details
void detailTemp() {
	cout << "";
}
void detailPlaylist() {
	cout << "\033[3C" << "  Truths\n";
	for (int i = 0; i < cache1.playlists[currentOptionIndex].truths.size(); i++) {
		string truth = cache1.playlists[currentOptionIndex].truths[i];
		cout << "\033[3C" << "  -" << truth << "\n";
	}
	cout << "\n" << "\033[3C" << "  Dares\n";
	for (int i = 0; i < cache1.playlists[currentOptionIndex].dares.size(); i++) {
		string dare = cache1.playlists[currentOptionIndex].dares[i];
		cout << "\033[3C" << "  -" << dare << "\n";
	}
	cout << "\n";
}
void detailAddTruth() {
	if (cache1.playlists[cache1.playlists.size() - 1].truths.size() > 0) {
		cout << "\033[3C" << "  Truths:\n";
		for (int i = 0; i < cache1.playlists[cache1.playlists.size() - 1].truths.size(); i++) {
			string truth = cache1.playlists[cache1.playlists.size() - 1].truths[i];
			cout << "\033[3C" << "  -" << truth << "\n";
		}
	}
}
void detailAddDare() {
	if (cache1.playlists[cache1.playlists.size() - 1].dares.size() > 0) {
		cout << "\033[3C" << "  Dare:\n";
		for (int i = 0; i < cache1.playlists[cache1.playlists.size() - 1].dares.size(); i++) {
			string dare = cache1.playlists[cache1.playlists.size() - 1].dares[i];
			cout << "\033[3C" << "  -" << dare << "\n";
		}
	}
}
void detailSetName() {
	if (cache1.playlists[cache1.playlists.size() - 1].name.length() > 0) {
		cout << "\033[3C" << "  Playlist Name: " << cache1.playlists[cache1.playlists.size() - 1].name << endl;
	}
}
string trueFalse;
void detailRandomTurns() {
	trueFalse = game.turnRandom ? "(True)" : "(False)";
	cout << "\033[3C" << "  " << trueFalse << " This setting dictates whether turns will cycle randomly, " << endl;
	cout << "\033[3C" << "  or in an order." << endl;
}
void detailTallyVoting() {
	trueFalse = game.tallyVoting ? "(True)" : "(False)";
	cout << "\033[3C" << "  " << trueFalse << " Tally voting is where each audience player can vote on" << endl;
	cout << "\033[3C" << "  the performer(s). A vote for is +1pt. No vote is -1pt." << endl;
	cout << "\033[3C" << "  Alternatively for False, the audience will cast one collective" << endl;
	cout << "\033[3C" << "pass or fail vote." << endl;
}
void detailTurnDetails() {
	trueFalse = game.detailsPrompt ? "(True)" : "(False)";
	cout << "\033[3C" << "  " << trueFalse << " True means following each turn, the audience will input " << endl;
	cout << "\033[3C" << "  a summary of the performance. A game summary will be made." << endl;
}
void detailPerformersPerTurn() {
	cout << "\033[3C" << "  (" << game.performersPerTurn << ") This setting dictates how many players will perform per turn." << endl;
	cout << "\033[3C" << "  " << endl;
}

// Option Functions
void functionTemp() {
	cout << "Option Pressed";
}
void functionPlaylistN() {
	mainBar.displayIndex = 3;
	cache1.gamePlaylist = &cache1.playlists[currentOptionIndex];
	mainDisplayItems[2] = cache1.gamePlaylist->name;
	activatewindow(&settingsW);
}
void functionBegin() {
	mainBar.displayIndex = 2;
	cache1.options.clear();
	for (int i = 0; i < cache1.playlists.size(); i++) {
		cache1.options.push_back(option());
		cache1.options[i].name = cache1.playlists[i].name;
		cache1.options[i].windowName = "playlistP";
		cache1.options[i].function = &functionPlaylistN;
		cache1.options[i].detail = &detailPlaylist;
	}
	playlistW.options.clear();
	for (int i = 0; i < cache1.options.size(); i++) {
		playlistW.options.push_back(&cache1.options[i]);
	}
	playlistW.options.push_back(&option_new);
	playlistW.postElements.clear();
	playlistW.optionsToPostElements();
	activatewindow(&playlistW);
}
void functionNext() {
	mainBar.displayIndex = 1;
	activatewindow(&playerInputW);
}
void functionAddPlayer() {
	string tempName;

	do {
		reprintwindow();
		cout << inputPos << "Enter player name (3-8 Characters): ";
		cin >> tempName;
	} while (tempName.length() > 8 || tempName.length() < 3);

	playerDisplayItems.push_back(tempName);
	cache1.players.push_back(player(tempName));

	reprintwindow();
	if (cache1.players.size() >= 4) {
		functionFinished();
	}
}
void functionFinished() {
	game.updatePlayPage();
	if (cache1.players.size() >= 2) {
		playerDisplayItems.erase(playerDisplayItems.begin());
		playersBar.displayIndex = 0;
		activatewindow(&playW);
	}
}
void functionNew() {
	cache1.playlists.push_back(playlist());
	activatewindow(&playlistCreatorW);
}
void functionSetName() {
	string tempSetName;
	cout << inputPos << "Enter playlist name: ";
	cin >> tempSetName;
	cache1.playlists[cache1.playlists.size() - 1].name = tempSetName;
	reprintwindow();
}
void functionAddDare() {
	string tempAddDare;
	cout << inputPos << "Enter dare: ";
	cin.ignore();
	getline(cin, tempAddDare);
	cache1.playlists[cache1.playlists.size() - 1].dares.push_back(tempAddDare);
	reprintwindow();
}
void functionAddTruth() {
	string tempTruth;
	cout << inputPos << "Enter Truth: ";
	cin.ignore();
	getline(cin, tempTruth);
	cache1.playlists[cache1.playlists.size() - 1].truths.push_back(tempTruth);
	reprintwindow();
}
void functionTruth() {
	game.roundSequence(true);
	activatewindow(&playW);
}
void functionDare() {
	game.roundSequence(false);
	activatewindow(&playW);
}
void functionToggleRandomTurns() {
	game.turnRandom = game.turnRandom ? false : true;
	reprintwindow();
}
void functionToggleTallyVoting() {
	game.tallyVoting = game.tallyVoting ? false : true;
	reprintwindow();
}
void functionToggleTurnDetails() {
	game.detailsPrompt = game.detailsPrompt ? false : true;
	reprintwindow();
}
void functionPerformersPerTurn() {
	game.performersPerTurn = game.performersPerTurn == 1 ? 2 : 1;
	reprintwindow();
}

// For original Scoreboard function.
vector<vector<string>> updatePlayers() {
	vector<string> tempColumn1;
	vector<string> tempColumn2;
	tempColumn1.push_back("Players ");
	tempColumn2.push_back("Points  ");

	for (int i = 0; i < cache1.players.size(); i++) {
		tempColumn1.push_back("" + cache1.players[i].fullName());
		tempColumn2.push_back("" + to_string(cache1.players[i].points) + " points");
	}

	vector<vector<string>> columns = { tempColumn1, tempColumn2 };
	return columns;
}
