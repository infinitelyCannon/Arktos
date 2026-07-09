#include <iostream>
#include "Arktos.h"
#include "Board.h"

Arktos::U64 eastMaskEx(Arktos::U64 sq)
{
    const Arktos::U64 one = 1;
    return 2 * ((one << (sq | 7)) - (one << sq));
}

int main(int argc, char** argv)
{
	Arktos::PrintVersion();

	/*Arktos::Board b;
	b.Init("startpos");

    b.CheckFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    std::cout << "No Crash!" << std::endl;*/

    std::cout << eastMaskEx(static_cast<Arktos::U64>(Arktos::ESquare::B2)) << std::endl;
	
	return 0;
}