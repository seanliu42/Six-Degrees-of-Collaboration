/* 
 * SixDegrees.h
 * 
 * CS15 Six Degrees
 *
 * Project 2
 * 
 * by Sean Liu, December 2021
 * 
 * Interface for SixDegrees. Defines SixDegrees as a class that modifies a
 * CollabGraph instance.
 * 
 */


#ifndef _SIX_DEGREES_H_
#define _SIX_DEGREES_H_

#include "Artist.h"
#include "CollabGraph.h"

#include <queue>

using namespace std;

class SixDegrees {
    
public:
    // Constructor, which handles command line arguments
    SixDegrees(int argc, char *argv[]);
    
    // Destructor
    ~SixDegrees();
    
    // Further handles command line arguments and feeds them to driver function
    void run();
    
private:
    // The CollabGraph instance we want to traverse
    CollabGraph CG;
    
    // A flag for the quit function to stop the program without memory leaks
    bool quit = false;
    
    // Variables that make the handling of command line arguments clear
    int numFiles;
    string dataFile;
    string inputFile;
    string outputFile;
    
    // Heler functions to populate the CollabGraph
    void importData();
    vector<Artist> getArtists(istream &input);
    
    // Driver function, which executes the necessary functions when called
    void commandLoop(istream &input, ostream &output);
    
    // Helper function for that gathers data for the traversal functions
    void traversalHelper(string command, istream &input, ostream &output);
    
    // Traversal functions
    void bfs(const Artist& a1, const Artist& a2);
    void dfs(const Artist& a1, const Artist& a2);
    void bfsWrapper(const Artist& a1, const Artist& a2, ostream &output);
    void dfsWrapper(const Artist& a1, const Artist& a2, ostream &output);
    void notWrapper(const Artist& a1, const Artist& a2, istream &input, 
                    ostream &output);
    
    // Helper functions to ensure Artist exists in the CollabGraph
    void errorMessage(const Artist& source, ostream &output);
    bool validArtists(const Artist& a1, const Artist& a2, ostream &output);
    
    // Helper function that prints path from traversal
    void printPath(const Artist& source, const Artist& dest, ostream &output);
};

#endif /* _SIX_DEGREES_H_ */