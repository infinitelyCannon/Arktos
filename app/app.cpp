#include <iostream>
#include "Arktos.h"
#include "Board.h"

int main(int argc, char** argv)
{
	Arktos::PrintVersion();

	Arktos::Board b;
	b.Init("startpos");

    b.CheckFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    std::cout << "No Crash!" << std::endl;
	
	return 0;
}