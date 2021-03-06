#include "Game.hpp"
#include "GameState.hpp"
#include "GameStateSplash.hpp"
#include "GameStatePlaying.hpp"

#include <iostream>

// default Game object constructor
Game::Game() : 	Screen_Width(800),
				Screen_Height(600),
				GameRunning(false) {
}

//*****************************************
//Initialize Game and GameStates, main game loop
//*****************************************
int Game::Execute() {
	// check if everything initialized
	if (Init(Screen_Width, Screen_Height) == false) {
		std::cout << "Somethin don goofed with initialization" << std::endl;
		return -1;
	}
	
	// create game states
	GameStateSplash * Splash_Ptr = new GameStateSplash();
	GameStatePlaying * Play_Ptr = new GameStatePlaying();
	
	// insert game states into state map
	GameStates.insert(std::make_pair("Splash", Splash_Ptr ));
	GameStates.insert(std::make_pair("Playing", Play_Ptr ));
	
	
	// load first state
	ChangeState("Splash");
	
	SDL_Event Event;
	
	//main game loop
	while (GameRunning) {
		
		// process events
		while(SDL_PollEvent(&Event)) {
			CurrentState->HandleEvent(&Event, GameRunning);
		}
	
		CurrentState->Update();
		
		CurrentState->Render(RenderWindow);
	}
	
	Exit();
	
	return 0;
}

//*****************************************
//Change from the current state to a new one
//by calling old->exit, new->enter and new->load
//*****************************************
void Game::ChangeState(const std::string State_Name) {
	if (CurrentState != NULL) {
		CurrentState->Exit();
	}
	
	// find matching state ptr to state_name
	auto state_itr = GameStates.find(State_Name);
	
	// check if state was found...
	if (state_itr != GameStates.end() ) {
		CurrentState = state_itr->second;
		CurrentState->Enter();
		CurrentState->LoadMedia(RenderWindow);
	}
	else {
		std::cout << "Error, state " << State_Name << " not found." << std::endl;
	}

}

//*****************************************
//Initialize SDL and the game window
//*****************************************
bool Game::Init(int ScrWidth, int scrHeight) {
	// initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0 ) {
		return false;
	}
		
	// open SDL window
	if((MainWindow = SDL_CreateWindow("Tic Tac Toe",
										SDL_WINDOWPOS_UNDEFINED,
										SDL_WINDOWPOS_UNDEFINED,
										scrWidth, scrHeight, 
										SDL_WINDOW_OPENGL)
										) == NULL) {
		return false;
	}
	
	if ( (RenderWindow = SDL_CreateRenderer(MainWindow, -1, 0)) == NULL) {
		return false;
	}
		
	// successfully initialized
	std::cout << "Game initialized." << std::endl;
	GameRunning = true;
	return true;
	
}

//*****************************************
// Clean up after ourselves
// by deleting GameStates/SDL stuff
//*****************************************
void Game::Exit() {

	// exit current state
	CurrentState->Exit();
	
	
	// clean up state ptrs
	for (auto state_itr = GameStates.begin(); state_itr != GameStates.end(); ++state_itr) {
		delete state_itr->second;
		std::cout << "Deleted state: " << state_itr->first << std::endl;
	}
	
	
	// clean up SDL related resources
	SDL_DestroyWindow( MainWindow);
	SDL_Quit();
	
	std::cout << "Game exiting." << std::endl;
}

// static member declarations
SDL_Window* Game::MainWindow = NULL;
SDL_Renderer* Game::RenderWindow = NULL;
GameState* Game::CurrentState = NULL;
std::map<std::string, GameState*> Game::GameStates;
