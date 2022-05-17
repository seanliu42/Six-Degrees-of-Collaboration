/* 
 * Six Degrees.cpp
 * 
 * CS15 Six Degrees
 *
 * Project 2
 * 
 * by Sean Liu, December 2021
 * 
 * Implementation for the SixDegrees interface. SixDegrees is a class that
 * has on a CollabGraph instance. Its functions traverse the graph, print the 
 * graph, and updates the vertices' information.
 * 
 */

#include "Artist.h"
#include "CollabGraph.h"
#include "SixDegrees.h"
#include <iostream>
#include <fstream>
#include <queue>

using namespace std;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: constructor
 * @purpose: initialize a SixDegrees instance
 *
 * @preconditions: none
 * @postconditions: our numFiles, inputFile, outputFile variables are updated
 *
 * @parameters: number of command line arguments and their positions
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
SixDegrees::SixDegrees(int argc, char *argv[]) {
    // Convert arguments indexing to solely focus on arguments, not the program
    numFiles = argc - 1;
    
    // If program usage is incorrect, inform user and cease operations
    if (numFiles < 1 or numFiles > 3) {
        cerr << "Usage: SixDegrees dataFile [commandFile] [outputFile]\n";
        exit(EXIT_FAILURE);
    }
    
    // Initialize file names for future reading
    dataFile = argv[1];
    
    inputFile = "";
    if (numFiles > 1) {
        inputFile = argv[2];
    }
    
    outputFile = "";
    if (numFiles > 2) {
        outputFile = argv[3];
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: destructor
 * @purpose: deallocate all memory associated with this SixDegrees
 *           instance
 *
 * @preconditions: none
 * @postconditions: all heap-allocated memory associated with this
 *                  SixDegrees instace is freed
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
SixDegrees::~SixDegrees() {

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: run
 * @purpose: populate the CollabGraph with data from the provided data file,
 *           and feed input and output streams into the driver function
 *
 * @preconditions: provided data file is valid
 * @postconditions: the CollabGraph is populated, and SixDegrees will read
 *                  from user/file input and print to console/an output file
 *
 * @parameters: none
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SixDegrees::run() {
    // Populate the graph with data from the data file
    importData();
    
    // If no extra files are provided, receive user input and print to console
    if (numFiles == 1) {
        commandLoop(cin, cout);
    }
    // If an input file is provided, use its contents for input
    else {
        ifstream infile;
        infile.open(inputFile);
        // If the file doesn't open, cease operation
        if (infile.fail()) {
            cerr << inputFile << " cannot be opened.\n";
            exit(EXIT_FAILURE);
        }
        if (numFiles == 2) {
            commandLoop(infile, cout);
        }
        // If an output file is provided, direct output to the file
        else {
            ofstream outfile;
            outfile.open(outputFile);
            commandLoop(infile, outfile);
        }
        infile.close();
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: importData
 * @purpose: populates the CollabGraph
 *
 * @preconditions: none
 * @postconditions: the CollabGraph is populated
 *
 * @parameters: none
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SixDegrees::importData() {
    // Turn the provided data file into an input stream
    ifstream infile;
    infile.open(dataFile);
    
    // If the data file is invalid, cease operations
    if (infile.fail()) {
        cerr << dataFile << " cannot be opened.\n";
        exit(EXIT_FAILURE);
    }
    

    vector<Artist> artists = getArtists(infile);
        
    // If the file was empty, avoid calling populate_graph bad allocation
    if (artists.empty()) {
        return;
    }

    // Populate the graph with the data from the provided file
    CG.populate_graph(artists);
    
    infile.close();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: getArtitsts
 * @purpose: extracts Artists and their info from the data file and puts 
 *           them into a vector to be used for populating the graph
 *
 * @preconditions: a valid data file is provided
 * @postconditions: none
 *
 * @parameters: where to read input from
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
vector<Artist> SixDegrees::getArtists(istream &input) {
    string line;
    vector<Artist> artists;
    
    // Read file data, and create the artists until file data is exhausted
    while (getline(input, line)) {
        if (input.eof()) {
            return artists;
        }
        Artist newArtist(line);
        
        // Given a marker, start filling in artists' info
        while (line != "*") {
            newArtist.add_song(line);
            getline(input, line);
        }
        artists.push_back(newArtist);
    }
    
    return artists;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: commandLoop
 * @purpose: driver function which executes the program according to
 *           instructions from input
 *
 * @preconditions: program usage is correct
 * @postconditions: program performs desired tasks
 *
 * @parameters: where to read input from and where to print output
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SixDegrees::commandLoop(istream &input, ostream &output) {
    string command;
    
    // Accept input until program is quit or input file is completely read
    while (not quit and getline(input, command)) {
            if (command == "quit") {
                quit = true;
            }
            else if (command == "print") {
                CG.print_graph(output);
            }
            else if (command == "bfs" or 
                     command == "dfs" or 
                     command == "not") {
                traversalHelper(command, input, output);
            }
            else {
                output << command << " is not a command. Please try again.\n";
            }
        }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: traversalHelper
 * @purpose: gathers data for the traversal functions
 *
 * @preconditions: program usage is correct
 * @postconditions: desired traversal function is called with appropriate info
 *
 * @parameters: which command was called, where to receive input from and 
 *              where to print output to
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SixDegrees::traversalHelper(string command, istream &input, 
                                 ostream &output) {
    CG.clear_metadata();
    
    // Initialize variables to store input
    string a1, a2;
    getline(input, a1);
    getline(input, a2);
    
    // Call the appropriate function based on input
    if (command == "bfs") {
        bfsWrapper(a1, a2, output);
    }
    else if (command == "dfs") {
        dfsWrapper(a1, a2, output);
    }
    else {
        notWrapper(a1, a2, input, output);
    }
    // Print the result
    printPath(a1, a2, output);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: bfsWrapper
 * @purpose: checks if Artists to traverse between are valid, and if either of
 *           them were excluded from the not function/visited already
 *
 * @preconditions: none
 * @postconditions: error messages are printed or traversal begins
 *
 * @parameters: Artists provided by input and where to print error message to
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SixDegrees::bfsWrapper(const Artist& a1, const Artist& a2, 
                            ostream &output) {
    // Check if Artists are vertices in the graph
    if (not validArtists(a1, a2, output)) {
        return;
    }
    
    // If either of the Artists are excluded by the not function, stop 
    else if (CG.is_marked(a1) or CG.is_marked(a2)) {
        return;
    }
    
    // If conditions are good, call the traversal function
    bfs(a1, a2);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: bfs
 * @purpose: traverses by repeatedly visiting all neighbors of a node
 *
 * @preconditions: the Artists to traverse between are valid
 * @postconditions: predecessors for all visited nodes are set or error
 *                  message is printed
 *
 * @parameters: Artists provided by input
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SixDegrees::bfs(const Artist& a1, const Artist& a2) {
    queue<Artist> pQueue;
    
    CG.mark_vertex(a1);
    pQueue.push(a1);
    
    // Visit vertices until destination is reached or whole graph is traversed
    while (not pQueue.empty()) {
        Artist next = pQueue.front();
        CG.mark_vertex(next);
        
        // If we have reached the destination, nothing else is needed
        if (next == a2) {
            return;
        }
        
        vector<Artist> neighbors = CG.get_vertex_neighbors(next);
        
        // Update information for all unvisited neighbors
        for(size_t i = 0; i < neighbors.size(); i++) {
            if (not CG.is_marked(neighbors.at(i))) {
                CG.mark_vertex(neighbors.at(i));
                CG.set_predecessor(neighbors.at(i), next);
                pQueue.push(neighbors.at(i));
            }
        }
        pQueue.pop();
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: notWrapper
 * @purpose: marks excluded Artists as seen to avoid them in traversal then
 *           does a breadth first traversal of the graph
 *
 * @preconditions: none
 * @postconditions: predecessors for all visited nodes are set or error
 *                  message is printed
 *
 * @parameters: Artists provided by input, an input stream for Artists 
 *              to exclude, and output stream for printing error messages
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SixDegrees::notWrapper(const Artist& a1, const Artist& a2, 
                            istream &input, ostream &output) {
    vector<Artist> invalidExcludes;
    string excludedArtist;
    
    // Mark Artists to exclude as seen, to avoid them during traversal
    while (getline(input, excludedArtist) and excludedArtist != "*") {
        // Cannot access invalid vertices, so we add them to a list of invalids
        if (not CG.is_vertex(excludedArtist)) {
            invalidExcludes.push_back(excludedArtist);
        }
        else {
            CG.mark_vertex(excludedArtist);
        }
    }
    // Now that all not specific processes are done, call BFS
    bfsWrapper(a1, a2, output);
    
    // If there were invalid Artists print error messages, and stop function
    if (not invalidExcludes.empty()) {
        for (size_t i = 0; i < invalidExcludes.size(); i++) {
            errorMessage(invalidExcludes.at(i), output);
        }
        return;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: dfsWrapper
 * @purpose: checks if Artists to traverse between are valid
 *
 * @preconditions: none
 * @postconditions: error message is printed or traversal begins
 *
 * @parameters: Artists provided by input and where to print error message to
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SixDegrees::dfsWrapper(const Artist& a1, const Artist& a2, 
                            ostream &output) {
    // Check if Artists are vertices in the graph
    if (not validArtists(a1, a2, output)) {
        return;
    }
    
    // If conditions are good, call the traversal function
    dfs(a1, a2);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: dfs
 * @purpose: traverses by visiting nodes along the deepest path possible
 *
 * @preconditions: the Artists to traverse between are valid
 * @postconditions: predecessors for all visited nodes are set or error
 *                  message is printed
 *
 * @parameters: Artists provided by input
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SixDegrees::dfs(const Artist& a1, const Artist& a2) {
    stack<Artist> pStack;
    
    CG.mark_vertex(a1); 
    pStack.push(a1);
   
   // Visit vertices until destination is reached or whole graph is traversed
    while (not pStack.empty()) {
        Artist next = pStack.top();
        CG.mark_vertex(next);
        
        // If we have reached the destination, nothing else is needed
        if (next == a2) {
            return;
        }
        
        vector<Artist> neighbors = CG.get_vertex_neighbors(next);
        
        // Update information for all unvisited neighbors
        for (size_t i = 0; i < neighbors.size(); i++) {
            if (not CG.is_marked(neighbors.at(i))) {
               CG.mark_vertex(neighbors.at(i));
               CG.set_predecessor(neighbors.at(i), next);
               pStack.push(neighbors.at(i));
           }
        }
        pStack.pop();
    }  
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: validArtists
 * @purpose: ensures that the Artists we want to use for traversal are 
 *           valid vertices
 *
 * @preconditions: none
 * @postconditions: if provided Artists are not in the graph, an error message
 *                  will print and the traversal function will not run 
 *
 * @parameters: Artists provided by input and where to print error message to
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool SixDegrees::validArtists(const Artist& a1, const Artist& a2, 
                              ostream &output) {
    // Print error messages for any invalid vertices
    if (not CG.is_vertex(a1) and CG.is_vertex(a2)) {
        errorMessage(a1, output);
        return false;
    } 
    else if (not CG.is_vertex(a2) and CG.is_vertex(a1)) {
        errorMessage(a2, output);
        return false;
    } 
    else if (not CG.is_vertex(a1) and not CG.is_vertex(a2)) {
        errorMessage(a1, output);
        errorMessage(a2, output);
        return false;
    } 
    return true;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: errorMessage
 * @purpose: print the appropriate error message when an Artist doesn't exist
 *
 * @preconditions: none
 * @postconditions: none
 *
 * @parameters: an Artist provided by input and where to print error message to
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SixDegrees::errorMessage(const Artist& artist, ostream &output) {
    if (not CG.is_vertex(artist)) {
        output << "\"" << artist.get_name() 
             << "\" was not found in the dataset :(\n"; 
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: printPath
 * @purpose: prints the path between two Artists
 *
 * @preconditions: there is a valid path between the two artists
 * @postconditions: none
 *
 * @parameters: Artists provided by input and where to print output to
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SixDegrees::printPath(const Artist& a1, const Artist& a2, 
                           ostream &output) {
    // Only get the path from valid vertices, avoid accessing invalid space
    if (CG.is_vertex(a1) and CG.is_vertex(a2) == true) {
        stack<Artist> path = CG.report_path(a1, a2);
        
        if (path.empty() or a1 == a2) { // Cannot have a path to self
            output << "A path does not exist between \"" << a1 << "\" and \""
                   << a2 << "\".\n";
            return;
        }
        
        Artist curr = a1;
        Artist next;
        
        path.pop();
        
        // Print each collaboration along the path
        while (not path.empty()) {
            next = path.top();
            path.pop();
            
            output << "\"" << curr.get_name() << "\" collaborated with \"" 
                   << next.get_name() << "\" in \"" << CG.get_edge(curr, next)
                   << "\".\n";
            
            curr = next;
        }
        output << "***\n";
    }
}
