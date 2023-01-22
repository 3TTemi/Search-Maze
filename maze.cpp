// Maze Problem | July 7th 2022 | Temi Adebowale

#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "grid.h"
#include "maze.h"
#include "mazegraphics.h"
#include "queue.h"
#include "set.h"
#include "stack.h"
#include "vector.h"
#include "testing/SimpleTest.h"
using namespace std;


/* Functions takes in a Grid of booleans  and a gridLocation of the current endig lcoation in the maze
 * From there the function calculates one step away from the current locaiton in North,South, East, and West
 * It makes sure that each of these steps is within bounds and that they are open corridor
 *The funciton returns a set of grid locations with the valid one step away locations
 */
Set<GridLocation> generateValidMoves(Grid<bool>& maze, GridLocation cur) {
    Set<GridLocation> neighbors;

    GridLocation northCur {cur.row, cur.col + 1};
    GridLocation southCur {cur.row, cur.col - 1};
    GridLocation eastCur {cur.row + 1, cur.col};
    GridLocation westCur {cur.row - 1, cur.col};
    Vector<GridLocation> directions = {northCur,southCur,eastCur,westCur};

    for (int i = 0; i < 4; i++)
    {
        if (maze.inBounds(directions[i]) && maze[directions[i]])
        {
            neighbors.add(directions[i]);
        }
    }

    return neighbors;
}

/* Function takes in a Grid of booleans and a stack of grid locations that represents the path
 * The function validates that the end of the stack is at maze entry, and that the top of the stack is at the exit
 * Then it checks each location in the stack to see if it is a vlaid step from the previous using generateValidMoves
 * Lastly if checks that no two same locations are  in the stack using a set variable
 * Nothing is returned as an error will be sent if any of these paramters are not met
 */
void validatePath(Grid<bool>& maze, Stack<GridLocation> path) {

    GridLocation mazeExit = {maze.numRows()-1,  maze.numCols()-1};
    GridLocation mazeStart = {0,0};
    Set<GridLocation> uniqueLocations;

    // path must end at exit
    if (path.peek() != mazeExit) {
        error("Path does not end at maze exit");
    }

    if (path.isEmpty())
    {
        error("Path has no locations");
    }
    for(int i = 0; i <  path.size(); i++)
    {
        GridLocation cur = path.pop();
        if (path.isEmpty())
        {
            break;
        }
        else
        {
            uniqueLocations.add(cur);
            if (uniqueLocations.contains(path.peek()))
            {
            error("Location has already been seen in path");
            }

            GridLocation previousLocation = path.peek();

            // path must start at entry
            if (i == path.size() - 1)
            {
                if(cur != mazeStart)
                {
                error("Path does not start at mazeStart");
                }
            }

            Set<GridLocation> validPaths = generateValidMoves(maze, previousLocation);
            if (!validPaths.contains(cur))
            {
                error("Location in path is not valid move from previous location");
            }


        }
    }



}

/* Takes in argument of boolean grid and returns the stack of grid locxation with the path to exit
 * Uses Breadth-first search to continually move outward from each path, removing each path that has no further valid moves
 * Unique locations are accounted for using set
 * First path enqueued is the entry path and then the function loops through all remianing paths
 * In loop, it deques the ucrrent path to check if it is the exit, if not then the valid moves are found and added as their own paths usign enqueue
 * Function runs until there are no paths left or if the end is found
 */
Stack<GridLocation> solveMaze(Grid<bool>& maze) {
    MazeGraphics::drawGrid(maze);
    Stack<GridLocation> path = {{0,0}};
    Stack<GridLocation> entryPath;
    Set<GridLocation> uniqueLocations;
    Set<GridLocation> viableMoves;

    GridLocation exit = {maze.numRows() - 1, maze.numCols() - 1};


    Queue<Stack<GridLocation>> allPaths;
    entryPath = {{0,0}};
    allPaths.enqueue(entryPath);

    while(allPaths.size() > 0)
    {

        Stack<GridLocation> curPath = allPaths.dequeue();
        MazeGraphics::highlightPath(curPath,"blue",0);

        Stack<GridLocation> curPathReference = curPath;
        for(int i = 0; i < curPath.size(); i++)
        {
            uniqueLocations += curPathReference.pop();
        }

        if (curPath.peek() == exit)
        {
            return curPath;
        }

        else{

            Set<GridLocation> uncensoredMoves = generateValidMoves(maze, curPath.peek());
            for (GridLocation currElem: uncensoredMoves)
            {
                if (!uniqueLocations.contains(currElem))
                {
                        viableMoves.add(currElem);

                }
            }
            for (GridLocation move: viableMoves)
            {
                Stack<GridLocation> curPathCopy = curPath;
                curPathCopy.push(move);
                allPaths.enqueue(curPathCopy);
            }
                viableMoves.clear();
            }

        }

    return path;
}

/*
 * The given readMazeFile function correctly reads a well-formed
 * maze from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readMazeFile(string filename, Grid<bool>& maze) {
    /* The following code reads data from the file into a Vector
     * of strings representing the lines of the file.
     */
    ifstream in;

    if (!openFile(in, filename))
        error("Cannot open file named " + filename);

    Vector<string> lines;
    readEntireFile(in, lines);

    /* Now that the file data has been read into the Vector, populate
     * the maze grid.
     */
    int numRows = lines.size();        // rows is count of lines
    int numCols = lines[0].length();   // cols is length of line
    maze.resize(numRows, numCols);     // resize grid dimensions

    for (int r = 0; r < numRows; r++) {
        if (lines[r].length() != numCols) {
            error("Maze row has inconsistent number of columns");
        }
        for (int c = 0; c < numCols; c++) {
            char ch = lines[r][c];
            if (ch == '@') {        // wall
                maze[r][c] = false;
            } else if (ch == '-') { // corridor
                maze[r][c] = true;
            } else {
                error("Maze location has invalid character: '" + charToString(ch) + "'");
            }
        }
    }
}

/* The given readSolutionFile correctly reads a path from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readSolutionFile(string filename, Stack<GridLocation>& soln) {
    ifstream in;

    if (!openFile(in, filename)) {
        error("Cannot open file named " + filename);
    }

    if (!(in >> soln)) {// if not successfully read
        error("Maze solution did not have the correct format.");
    }
}



TEST("generateValidMoves on location in the center of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation center = {1, 1};
    Set<GridLocation> expected = {{0, 1}, {1, 0}, {1, 2}, {2, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}

TEST("generateValidMoves on location on the side of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation side = {0, 1};
    Set<GridLocation> expected = {{0,0}, {0,2}, {1, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, side), expected);
}

TEST("generateValidMoves on corner of 2x2 grid with walls") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    GridLocation corner = {0, 0};
    Set<GridLocation> expected = {{1, 0}};

    EXPECT_EQUAL(generateValidMoves(maze, corner), expected);
}

TEST("generateValidmoves on location with only walls")
{
    Grid<bool> maze = {{true, false, false},
                       {true, false, true},
                       {true, false, false}};
    GridLocation edge = {1, 2};
    Set<GridLocation> expected = {};

    EXPECT_EQUAL(generateValidMoves(maze, edge), expected);
}
TEST("generateValidmoves on location that only has ")
{
    Grid<bool> maze = {{true}};

    GridLocation single = {0, 0};
    Set<GridLocation> expected = {};

    EXPECT_EQUAL(generateValidMoves(maze, single), expected);
}

TEST("generateValidmoves on 3 x 3 grid with only one slot open")
{
    Grid<bool> maze = {{true, false, false},
                       {false, true, false},
                       {true, true, false}};

    GridLocation center = {1, 1};
    Set<GridLocation> expected = {{2,1}};

    EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}


TEST("validatePath on correct solution") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Stack<GridLocation> soln = { {0 ,0}, {1, 0}, {1, 1} };

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

TEST("validatePath on correct solution loaded from file for medium maze") {
    Grid<bool> maze;
    Stack<GridLocation> soln;
    readMazeFile("res/5x7.maze", maze);
    readSolutionFile("res/5x7.soln", soln);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

TEST("validatePath on correct solution loaded from file for large maze") {
    Grid<bool> maze;
    Stack<GridLocation> soln;
    readMazeFile("res/25x33.maze", maze);
    readSolutionFile("res/25x33.soln", soln);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

TEST("validatePath on invalid path should raise error") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Stack<GridLocation> not_end_at_exit = { {1, 0}, {0, 0} };
    Stack<GridLocation> not_begin_at_entry = { {1, 0}, {1, 1} };
    Stack<GridLocation> go_through_wall = { {0 ,0}, {0, 1}, {1, 1} };
    Stack<GridLocation> teleport = { {0 ,0}, {1, 1} };
    Stack<GridLocation> revisit = { {0 ,0}, {1, 0}, {0, 0}, {1, 0}, {1, 1} };

    EXPECT_ERROR(validatePath(maze, not_end_at_exit));
    EXPECT_ERROR(validatePath(maze, not_begin_at_entry));
    EXPECT_ERROR(validatePath(maze, go_through_wall));
    EXPECT_ERROR(validatePath(maze, teleport));
    EXPECT_ERROR(validatePath(maze, revisit));
}

TEST("validatePath on invalid path that contains a loop on the exit") {
    Grid<bool> maze = {
                       {true, false, false},
                       {true, true, true},
                       {false, true, true}
                       };

    Stack<GridLocation> endCopy = { {0 ,0}, {1, 0}, {1, 1}, {1, 2}, {2, 2}, {2,1}, {2,2}};


    EXPECT_ERROR(validatePath(maze, endCopy));
}

TEST("validatePath on invalid path that contains an invalid move that is out of bounds") {
    Grid<bool> maze = {
                       {true, false, false},
                       {true, true, true},
                       {false, true, true}
                       };

    Stack<GridLocation> endCopy = { {0 ,0}, {1, 0}, {1, 1}, {1, 2}, {1,3}, {2,3}, {2,2} };


    EXPECT_ERROR(validatePath(maze, endCopy));
}

TEST("validatePath on invalid path that starts at end and then goes to start") {
    Grid<bool> maze = {
                       {true, false, false},
                       {true, true, true},
                       {false, true, true}
                       };

    Stack<GridLocation> endCopy = { {2 ,2}, {1, 2}, {1, 1}, {1, 0}, {0,0} };


    EXPECT_ERROR(validatePath(maze, endCopy));
}

TEST("validatePath on invalid path that only has one element") {
    Grid<bool> maze = {
                       {true}
                       };

    Stack<GridLocation> endCopy = {{0,0}};


    EXPECT_NO_ERROR(validatePath(maze, endCopy));
}



TEST("solveMaze on file 5x7") {
    Grid<bool> maze;
    readMazeFile("res/5x7.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

TEST("solveMaze on file 21x23") {
    Grid<bool> maze;
    readMazeFile("res/21x23.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

TEST("solveMaze on file 25x33") {
    Grid<bool> maze;
    readMazeFile("res/25x33.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}
TEST("solveMaze on file 25x33") {
    Grid<bool> maze;
    readMazeFile("res/25x33.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

TEST("solveMaze on invalid file 8x6") {
    Grid<bool> maze;
    readMazeFile("res/8x6.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_ERROR(validatePath(maze, soln));
}


