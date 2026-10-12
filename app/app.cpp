#include <iostream>
#include "Arktos.h"
#include "Board.h"

int main(int argc, char** argv)
{
	Arktos::PrintVersion();

	Arktos::Board b;
	b.Init();
	b.SetFEN("startpos");

    //b.CheckFEN("");

	Arktos::Move move = b.ParseMove("d2d4");
	b.MakeMove(move);

    std::cout << "No Crash!" << std::endl;
	
	return 0;
}