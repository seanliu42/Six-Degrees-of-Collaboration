/**
 ** CollabGraph.cpp
 **
 **
 ** Project Two: Six Degrees of Collaboration
 **
 ** Purpose:
 **   Represent a collaboration graph of artists, where each artist is
 **   a vertex and an edge between two artists is a song in which those
 **   artists have collaborated. Accessor and mutator functions are provided,
 **   allowing for convenient traversal of the graph.
 **
 ** Notes:
 **   1) The graph is undirected
 **   2) Non-existent edges are represented by the empty string
 **   3) Artists with the empty string as their name are forbidden
 **   4) Loops (edges from a vertex to itself) are forbidden
 **
 ** ChangeLog:
 **   17 Nov 2020: zgolds01
 **       CollabGraph class created
 **   April 2021: CR Calabrese
 **       Edited the CollabGraph to use an Edge struct & remove vector of
 **       pointers to all vertices.
 **       Edited function contracts to improve consistency with course style
 **       guide and other starter code.
 **   11 Nov 2021: mrussell
 **       refactored map from string->Artist to string->Vertex*
 **   10 December 2021: Sean Liu
 **       Finished populate_graph, report_path, and get_vertex_neighbors for
 **       submission in phase 1 of project 2
 **/

#include <algorithm>
#include <exception>
#include <functional>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <vector>


#include "Artist.h"
#include "CollabGraph.h"

using namespace std;

/*********************************************************************
 ******************** public function definitions ********************
 *********************************************************************/


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: Nullary constructor
 * @purpose: initialize a CollabGraph instance
 *
 * @preconditions: none
 * @postconditions: none
 *
 * @parameters: none
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CollabGraph::CollabGraph() {}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: destructor
 * @purpose: deallocate all memory associated with this CollabGraph
 *           instance
 *
 * @preconditions: none
 * @postconditions: all heap-allocated memory associated with this
 *                  CollabGraph instace is freed
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CollabGraph::~CollabGraph() {
    self_destruct();
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: copy constructor
 * @purpose: deeply copy another CollabGraph instance
 *
 * @preconditions: none
 * @postconditions: this instance is a deep copy of the provided
 *                  CollabGraph reference
 *
 * @parameters: a const CollabGraph reference, to be deeply copied
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CollabGraph::CollabGraph(const CollabGraph& source) {
    *this = source;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: assignment operator overload
 * @purpose: deeply copy another CollabGraph instance
 *
 * @parameters: a const CollabGraph reference, to be deeply copied
 * @returns: a CollabGraph reference
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CollabGraph& CollabGraph::operator=(const CollabGraph& rhs) {
    if (this == &rhs) return *this;

    self_destruct();

    /* ITERATING OVER AN UNORDERED_MAP:
     * An unordered map is made up of (key, value) pairs, of type std::pair.
     * We use a std::iterator to iterate over the entire map, which starts
     * out as the result of the unordered_map begin() function.
     *
     * Declaring iterator types in C++ is pretty complicated, so this is
     * the one case in which our course style guide allows for the use of
     * the "auto" keyword.
     */

    /* first, insert the new vertices, one per artist
       this is necessary so the neighbors / predecessors will exist in 'this'
       graph
    */
    for (auto itr = rhs.graph.begin(); itr != graph.end(); itr++) {
        this->insert_vertex(itr->second->artist);
    }

    /* now, add the adjacency lists and prev pointers */
    for (auto itr = rhs.graph.begin(); itr != graph.end(); itr++) {

        Artist curr_artist = graph[itr->first]->artist;

        for (const Edge& edge : itr->second->neighbors) {
            this->insert_edge(curr_artist, edge.neighbor->artist, edge.song);
        }

        this->set_predecessor(curr_artist, itr->second->predecessor->artist);
    }

    return *this;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: populate_graph
 * @purpose: based on a vector of artists, build the graph
 *
 * @parameters: a const vector<Artist>& from which to build the graph
 * @preconditions: the graph is empty
 *
 * @postconditions: the graph is populated based on the provided artists
 * @returns: none
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CollabGraph::populate_graph(const vector<Artist>& artists) {
    // Create a vertex in the CollabGraph for each Artist
    for(size_t i = 0; i < artists.size(); i++) {
        insert_vertex(artists.at(i));
    }
    
    // Check for collabs, create edges in the CollabGraph if there are any
    for (size_t i = 0; i < artists.size(); i++) {
        for (size_t j = i + 1; j < artists.size(); j++) {
            string song = artists.at(i).get_collaboration(artists.at(j));
            if (song != "") { 
                insert_edge(artists.at(i), artists.at(j), song);
            }
        }
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: insert_vertex
 * @purpose: insert a vertex in the collaboration graph
 *
 * @parameters: a const Artist reference to use as a vertex in the graph
 * @returns: none
 *
 * @notes: 1) if the artist is already in the graph, then nothing happens
 *         2) throws a runtime_error iff the provided Artist has the empty
 *            string as its name, since an Artist instance with the empty
 *            string as its name is improperly initialized
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CollabGraph::insert_vertex(const Artist& artist) {
    if (artist.get_name() == "") {
        string message = "cannot insert an improperly initialized "
                         "Artist instance (name must be non-empty)";
        throw runtime_error(message.c_str());
    }

    /* Do not insert a vertex into the graph if that
     * vertex already exists in the graph. The unordered_map insert
     * function won't insert if the key already exists anyways, but since
     * we're storing everything as vertex pointers, this makes memory
     * management simpler.
     *
     * Storing vertex pointers in the map rather than vertices makes keeping
     * track of predecessors simpler.
     */
    if (not is_vertex(artist)) {
        Vertex* vertex = new Vertex(artist);
        /* these curly braces make an initializer list for the pair struct
         */
        graph.insert({artist.get_name(), vertex});
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: insert_edge
 * @purpose: insert an edge between two vertices in the collaboration graph
 *
 * @preconditions: both 'a1' and 'a2' are vertices in the graph
 *
 * @postconditions: an edge with name 'edgeName' is inserted between 'a1' and
 *                  'a2' iff there was not already an edge connecting those
 *                  vertices
 *
 * @parameters: 1) a const Artist reference, which should be a vertex in
 *                the graph
 *              2) a const Artist reference, which should be a different
 *                 vertex in the graph
 *              3) a const string reference, the name of the edge
 *                 that will connect the two provided vertices
 * @returns:   none
 *
 * @notes: throws a runtime_error, if:
 *            1) an empty string is provided as the 'edgeName', since this
 *               would violate the representation invariant that empty
 *               strings represent non-existent edges OR,
 *            2) 'a1' and 'a2' are the same vertex, because creating an edge
 *               between a vertex and itself would
 *               product a loop (which would very likely result in an
 *               infinite loop during traversal)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CollabGraph::insert_edge(const Artist& a1, const Artist& a2,
                              const string& edgeName) {
    enforce_valid_vertex(a1);
    enforce_valid_vertex(a2);

    if (edgeName == "") {
        string message = "the empty string is not a valid edge name";
        throw runtime_error(message.c_str());
    }

    if (a1 == a2) {
        string message = "cannot insert an edge between a "
                         "vertex and itself";
        throw runtime_error(message.c_str());
    }

    /* Do not insert an edge between a1 and a2 if there
     * is already an edge that connects them.
     */
    if (get_edge(a1, a2) != "") return;


    vector<Edge>& a1_neighbors = graph[a1.get_name()]->neighbors;
    vector<Edge>& a2_neighbors = graph[a2.get_name()]->neighbors;

    a1_neighbors.push_back(Edge(graph[a2.get_name()], edgeName));
    a2_neighbors.push_back(Edge(graph[a1.get_name()], edgeName));
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: mark_vertex
 * @purpose: mark the given vertex as visited
 *
 * @parameters: an Artist instance, which should correspond to a vertex
 *              in the graph
 *
 * @returns:    none
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CollabGraph::mark_vertex(const Artist& artist) {
    enforce_valid_vertex(artist);
    graph[artist.get_name()]->visited = true;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: unmark_vertex
 * @purpose: mark the given vertex as not visited
 *
 * @parameters: an Artist instance, which should correspond to a vertex
 *             in the graph
 * @returns:    none
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CollabGraph::unmark_vertex(const Artist& artist) {
    enforce_valid_vertex(artist);
    graph[artist.get_name()]->visited = false;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: set_predecessor
 * @purpose: update the predecessor field of the 'to' vertex for path
 *           generation
 *
 * parameters: 1) a const Artist reference, the vertex those predecessor
 *                will be set. should be a vertex in the graph
 *             2) a const Artist reference, the predecessor. should be a
 *                vertex in the graph.
 *
 * @returns:    none
 * @note:       does nothing if 'to' already has a predecessor
 *              when the path is generated, the path will move from the
 *             'from' (predecessor) vertex to the 'to' vertex
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CollabGraph::set_predecessor(const Artist& to, const Artist& from) {
    enforce_valid_vertex(to);
    enforce_valid_vertex(from);

    Vertex* vertex      = graph[to.get_name()];
    Vertex* pred_vertex = graph[from.get_name()];

    /* It is a mistake to set the predecessor of a vertex if it
     * already has one.
     */
    if (vertex->predecessor == nullptr) { vertex->predecessor = pred_vertex; }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: clear_metadata
 * @purpose: clears the metadata in the graph, preparing the graph for
 *           another traversal
 *
 * @postconditions: 1) all vertices in the graph are marked as unvisited
 *                  2) the predecessor of each vertex is set to 'nullptr'
 *
 * @parameters: none
 * @returns: none
 *
 * @warning: this function MUST be called before each traversal, and failure
 *           to heed this warning will result in undefined behavior
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CollabGraph::clear_metadata() {
    /* Again, we use the std::iterator to iterate over an unordered_map.
     * Whereas the graph.at() function returns just the "value", when we
     * iterate we're getting the (key, value) pairs, so we have to
     * traverse to the second element in the pair to get the vertex itself.
     */
    for (auto itr = graph.begin(); itr != graph.end(); itr++) {
        itr->second->visited     = false;
        itr->second->predecessor = nullptr;
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: is_vertex
 * @purpose: determine whether an artist maps to a vertex in the
 *          collaboration graph
 *
 * @parameters: a const Artist referece
 * @returns: a bool, true iff the provided artist is in the graph
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CollabGraph::is_vertex(const Artist& artist) const {
    return graph.find(artist.get_name()) != graph.end();
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: is_marked
 * @purpose: determine whether the provided vertex has been visited
 *
 * @parameters: a const Artist reference, which should be in the graph
 * @returns: a bool, true iff the provided vertex has been visited
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CollabGraph::is_marked(const Artist& artist) const {
    enforce_valid_vertex(artist);
    return graph.at(artist.get_name())->visited;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: get_predecessor
 * @purpose: retrieve predecessor of a vertex in the collaboration graph
 *
 * @parameters: a const Artist reference, which should be a vertex in the
 *              graph
 * @returns: an Artist, which is the predecessor of the provided
 *           'artist' in the collaboration graph; or an artist with the
 *           empty string as the name if the given artist has no predecessor
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
Artist CollabGraph::get_predecessor(const Artist& artist) const {
    enforce_valid_vertex(artist);

    /* An artist with the empty string as its name represents a
     * non-existent artist */
    Artist pred_artist;


    Vertex* pred_vertex = graph.at(artist.get_name())->predecessor;

    if (pred_vertex) pred_artist = pred_vertex->artist;

    return pred_artist;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: get_edge
 * @purpose: retrieve the edge between two vertices in the collaboration
 *           graph
 *
 * @parameters: 1) a const Artist reference, which should be in the graph
 *              2) a const Artist reference, which should be in the graph
 *
 * @returns:    a string, which is the name of the edge connecting 'a1' and
 *             'a2', or the empty string if there is no edge connecting them
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
string CollabGraph::get_edge(const Artist& a1, const Artist& a2) const {
    enforce_valid_vertex(a1);
    enforce_valid_vertex(a2);

    vector<Edge> edges = graph.at(a1.get_name())->neighbors;

    for (const Edge& edge : edges) {
        if (edge.neighbor->artist == a2) return edge.song;
    }

    return "";
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: get_vertex_neighbors
 * @purpose: retrieve the neighbors of a vertex in the collaboration graph
 *
 * @parameters: a const Artist reference, which should be in the graph
 * @returns:    a vector of Artist instances, which contains all neighbors
 *              of the provided vertex
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
vector<Artist> CollabGraph::get_vertex_neighbors(const Artist& artist) const {
    vector<Artist> neighbors;

    enforce_valid_vertex(artist);
    
    // Create a vector for all of the vertex's edges 
    vector<Edge> edges = graph.at(artist.get_name())->neighbors;
    for (size_t i = 0; i < edges.size(); i++) {
        // Add each Artist connected to the edges to the neighbor vector
        neighbors.push_back(edges.at(i).neighbor->artist);
    }

    return neighbors;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: report_path
 * @purpose: accumualate the path from the provided 'source' vertex to the
 *           provided 'dest' vertex
 *
 * @preconditions: 1) the graph has been traversed, and the predecessor field
 *                   of all vertices between 'source' and 'dest' has been
 *                   properly set
 *
 * @parameters: 1) a const Artist reference, the source vertex.
 *                should be in the graph
 *              2) a const Artist reference, the destination vertex.
 *                should be in the graph
 * @returns: a stack of Artist instances representing the path from the
 *             source to the destination, where the top-most element is the
 *             source vertex, and the bottom-most element is the
 *             destination vertex
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
stack<Artist> CollabGraph::report_path(const Artist& source,
                                       const Artist& dest) const {
    stack<Artist> path;
    stack<Artist> empty;

    enforce_valid_vertex(source);
    enforce_valid_vertex(dest);
    
    // There are not at least 2 elements for the path to oneself, so invalid
    if (source == dest) {
        return empty;
    }
    
    // Start from the destination
    Artist curr = dest;
    path.push(curr);
    
    // Iterate by predecessor until we reach the source of the path
    while (curr != source) {
        path.push(get_predecessor(curr));
        curr = get_predecessor(curr);
        // If there's no predecessor there is no path so return an empty stack
        if (curr.get_name() == "") {
            return empty;
        }
    }
    
    return path;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * function: print_graph
 * purpose: print a representation of the information stored in the
 *          collaboration graph IN SORTED ORDER
 *
 * parameters: a ostream reference, where output is sent
 * returns: none
 * NOTES:   Prints the graph in sorted order!
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CollabGraph::print_graph(ostream& out) {
    vector<string> output;

    string quote = "\"";

    for (auto itr = graph.begin(); itr != graph.end(); itr++) {

        /* It is always the case that the edges and neighbors
         * vectors have the same number of elements
         */
        vector<Edge> neighbors = itr->second->neighbors;
        for (size_t i = 0; i < neighbors.size(); i++) {
            string s = quote + itr->second->artist.get_name() + quote + " " +
                       "collaborated with " + quote +
                       neighbors.at(i).neighbor->artist.get_name() + quote +
                       " in " + quote + neighbors.at(i).song + quote + ".\n";
            if (i == neighbors.size() - 1) s += '\n';
            output.push_back(s);
        }
    }

    // if empty graph, don't print
    if (output.empty()) { return; }

    // sort output to add clarity and autograding ease
    sort(output.begin(), output.end());

    for (string& s : output) {
        if (s == output[output.size() - 1]) { s = s.substr(0, s.size() - 1); }
        out << s;
    }
}


/**********************************************************************
 ******************** private function definitions ********************
 **********************************************************************/


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: self_destruct
 * @purpose: free all memory associated with this CollabGraph instance
 *
 * @parameters: none
 * @returns: none
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CollabGraph::self_destruct() {
    for (auto itr = graph.begin(); itr != graph.end(); itr++) {
        delete itr->second;
    }

    graph.clear();
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @function: enforce_valid_vertex
 * @purpose: ensure that the given artist is in the graph; throw an error if
 *           it is not. if an exception is not thrown, then the provided
 *           Artist instance maps to a vertex in the collaboration graph
 *
 * @parameters: a const Artist reference, which should map to a vertex in
 *              the collaboration graph
 * @returns: none
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CollabGraph::enforce_valid_vertex(const Artist& artist) const {
    if (not is_vertex(artist)) {
        string message = "artist \"" + artist.get_name() +
                         "\" does not exist in the collaboration graph";
        throw runtime_error(message.c_str());
    }
}
