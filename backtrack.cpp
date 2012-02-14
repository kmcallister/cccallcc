#include <iostream>
#include <boost/optional.hpp>

#include "cccallcc_fork.hpp"


////////////////////////////////////////////////
// Backtracking implemented using continuations.
//
// Conceptually, guess() only returns true if doing so results in
// a program that never calls fail().
//
// In reality, guess() returns true and saves a continuation.
// fail() will invoke that continuation and return false instead.

boost::optional< cont<bool> > checkpoint;

bool guess() {
    return call_cc<bool>( [](cont<bool> k) {
        checkpoint = k;
        return true;
    } );
}

void fail() NORETURN;
void fail() {
    if (checkpoint) {
        (*checkpoint)(false);
    } else {
        std::cerr << "Nothing to be done." << std::endl;
        exit(1);
    }
}

// Commit to this path; prevent backtracking before this point.
void cut() {
    checkpoint = boost::none;
}


/////////////////////////////////////////////
// Backtracking example: factoring an integer

int integer(int m, int n) {
    for (int i=m; i<=n; i++) {
        if (guess())
            return i;
    }
    fail();
}

void factor(int n) {
    const int i = integer(2, 100);
    const int j = integer(2, 100);

    if (i*j != n)
        fail();

    std::cout << i << " * " << j << " = " << n << std::endl;
}


///////////////////////////////////////
// Backtracking example: solving a maze

const int maze_size = 15;
char maze[] =
    "X-------------+\n"
    "        |     |\n"
    "|--+  | |   | |\n"
    "|  |  | | --+ |\n"
    "|     |     | |\n"
    "|-+---+--+- | |\n"
    "| |      |    |\n"
    "| | | ---+-+- |\n"
    "|   |      |  |\n"
    "| +-+-+--|    |\n"
    "| |   |  |--- |\n"
    "|     |       |\n"
    "|--- -+-------|\n"
    "|              \n"
    "+------------- \n";

void solve_maze() {
    // Start at the upper-left corner.
    int x=0, y=0;

    // As long as we haven't reached the lower-right corner:
    while ((x != maze_size-1)
        || (y != maze_size-1)) {

        // Guess a direction to move.
             if (guess()) x++;
        else if (guess()) x--;
        else if (guess()) y++;
        else              y--;

        // Fail if we go off the edge.
        if (  (x < 0) || (x >= maze_size) ||
              (y < 0) || (y >= maze_size)  )
            fail();

        // Fail if we run into a wall, or find ourselves on
        // a square we already visited.
        const int i = y*(maze_size+1) + x;
        if (maze[i] != ' ')
            fail();

        // Mark our new position.
        maze[i] = 'X';
    }

    // Print out the completed maze, using some ANSI color
    // codes.
    for (char c : maze) {
        if (c == 'X')
            std::cout << "\e[1;32mX\e[0m";
        else
            std::cout << c;
    }
}


/////////////////////
// Run both examples.

int main() {
    factor(391);
    cut();
    solve_maze();
    return 0;
}
