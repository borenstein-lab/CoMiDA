/* Author: Alex Eng
 * Date: 7/1/15
 *
 * This program takes a set of species, each associated with a set of reactions and finds a minimal set of species that provides reactions from 
 * specified set of substrates to a specified set of products
 *
 * Usage: 
 *     ./write_design_problem.o [-s] species_reactions substrates products
 */

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <list>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include "ilp_writing.hpp"

using namespace std;

// Declaring global constants
const string FROM_TO = "TO";
const string FLAG = "flag";
const string REAC_PREFIX = "rxn";
const string PROBLEM_NAME = "Community Design";
const string SPECIES_TAG = "s/";
const string SIMPLE_FLAG = "-s";
const string FORCE_FLAG = "-fs";
const char SPEC_REAC_DELIM = ')';
const char REAC_DELIM = '|';
const char REAC_SIDE_DELIM = ':';
const char METAB_DELIM = ',';
enum custom_exception {FILE_EX, EMPTY_SPECIES_EX, EMPTY_REACTION_EX, EMPTY_METABOLITE_EX};
enum vertexType {METABOLITE, ENZYME};

// The maximum coefficient value (set for what COIN-OR CBC can handle, depends on solver)
const int MAX_COEFF = 10005;

/* to_string(int)
 *
 * Converts the given integer to a string
 */
string to_string(int input){
  stringstream ss;
  ss << input;
  return ss.str();
}

// Structure to store reactions for the metabolic network
struct reaction{
  map<string, int> starts;
  map<string, int> ends;
  int reactionNumber;

  // Comparison operator
  bool operator <(const reaction& rhs) const {

    // If the start lists don't match, then decide using the start lists
    if (starts != rhs.starts){
      return starts < rhs.starts;
    } else {
      // Otherwise decide using the end lists
      return ends < rhs.ends;
    }
  }

  /* string getEnzymeVarName()
   *
   * Returns the variable name that should be associated with the vertex correpsonding to this reaction
   */
  string getEnzymeVarName() const {
    return REAC_PREFIX + to_string(reactionNumber);
  }

  /* string getIncomingEdgeVarName(string)
   *
   * Returns the variable name that should be associated with the edge from the given substrate metabolite to this reaction
   */
  string getIncomingEdgeVarName(string substrate) const {
    map<string, int>::const_iterator it = starts.find(substrate);
    return to_string(it->second) + FROM_TO + getEnzymeVarName();
  }

  /* string getOutgoingEdgeVarName(string)
   *
   * Returns the variable name that should be associated with the edge from this reaction to the given product metabolite
   */
  string getOutgoingEdgeVarName(string product) const {
    map<string, int>::const_iterator it = ends.find(product);
    return getEnzymeVarName() + FROM_TO + to_string(it->second);
  }
};

struct species{
  string name;
};

struct edge{
  string label;
  struct vertex* begin;
  vertex* end;
  edge(): label(""), begin(NULL), end(NULL){}
  edge(const string init_label): label(init_label), begin(NULL), end(NULL){}
};

struct vertex{
  string label;
  list<edge*> incomingEdges;
  list<edge*> outgoingEdges;
  vertexType type;
  vertex(): label(""), incomingEdges(list<edge*>()), outgoingEdges(list<edge*>()), type(METABOLITE){}
  vertex(const string init_label, vertexType init_type): label(init_label), incomingEdges(list<edge*>()), outgoingEdges(list<edge*>()), type(init_type){}
};

struct graph{
  vertex* vertices;
  edge* edges;
  size_t numVertices;
  size_t numEdges;
  bool is_simple;
  graph(size_t init_numVertices, size_t init_numEdges, bool init_is_simple): numVertices(init_numVertices), numEdges(init_numEdges), is_simple(init_is_simple){
    vertices = new vertex[numVertices];
    edges = new edge[numEdges];
  }
  ~graph(){
    delete[] vertices;
    delete[] edges;
  }
};

/* void readSpeciesReactionFile(list<species>*, map<reaction, set<string> >*, const char*, graphType, bool)
 *
 * A function that parses the information from a file of species and their associated reactions, adding that information to the list and map passed in
 */
void readSpeciesReactionFile(list<species>* speciesList, map<reaction, set<string> >* reactionToSpecies, const char* fileName, bool is_simple){

  // Declaring variables for storing information as we parse the file
  string line;
  string speciesName;
  string reactionString;
  string starts;
  string ends;
  string start;
  string end;
  int reactionCount = 0;

  // Create the stream we will be reading the file from, and the string streams we will use to parse each line
  ifstream file(fileName);

  // Check for problems with the species/reaction file
  if (!file.good()){
    throw FILE_EX;
    return;
  }

  istringstream lineISS;
  istringstream reactionISS;
  istringstream metabolitesISS;
  
  // Read until we get to the end of the file
  while (getline(file, line)){
    
    // Clear the string stream for the previous line and then stream from the new line
    lineISS.clear();
    lineISS.str(line);
    
    // The speices name comes first, and is separated from the associated reactions by a ')'
    getline(lineISS, speciesName, SPEC_REAC_DELIM);
    
    // Check if the species name is empty
    if (speciesName.empty()){
      throw EMPTY_SPECIES_EX;
      return;
    }

    // Store the information about the species in a struct and add that to the list of species
    species newSpecies;
    newSpecies.name = SPECIES_TAG + speciesName;
    speciesList->push_back(newSpecies);
    
    // Next come the reactions, separated by '|'s
    while (getline(lineISS, reactionString, REAC_DELIM)){
      
      // Skip over completely empty reactions
      if (reactionString != ""){
        // Reset the string stream for the next reaction
        reactionISS.clear();
        reactionISS.str(reactionString);
        
        // Each reaction has 2 parts, the start metabolites and the end metabolites
        getline(reactionISS, starts, REAC_SIDE_DELIM);
        getline(reactionISS, ends, REAC_DELIM);
 
        // Check for non-empty reaction components
        if (starts.empty() || ends.empty()){
          throw EMPTY_REACTION_EX;
          return;
        }

        // Depending on the graph type, we change how we add in reactions.  If the graph type is hyper, then each reaction will be defined by multiple starts and ends.  If the graph type is simple, then we need to split each reaction into separate reactions for each start and end metabolite pair.  
        if (!is_simple){
          // Store the reaction information in a struct
          reaction newReaction;;

          // Clear the string stream for the set of start metabolites
          metabolitesISS.clear();
          metabolitesISS.str(starts);

          // Add the start metabolites
          int startsMetabolitesCount = 0;
          while (getline(metabolitesISS, start, METAB_DELIM)){
            newReaction.starts[start] = startsMetabolitesCount++;
          }

          // Clear the string stream for the set of end metabolites
          metabolitesISS.clear();
          metabolitesISS.str(ends);

          // Add the end metabolites
          int endsMetabolitesCount = 0;
          while (getline(metabolitesISS, end, METAB_DELIM)){
            newReaction.ends[end] = endsMetabolitesCount++;
          }

          // Check if the end metabolites of this reaction are a subset of the start metabolites, if so this reaction is a useless loop
          bool is_cyclic = true;
          for (map<string, int>::const_iterator endIt = newReaction.ends.begin(); endIt != newReaction.ends.end(); ++endIt){
            if (newReaction.starts.find(endIt->first) == newReaction.starts.end()){
              is_cyclic = false;
              break;
            }
          }

          // Check if the reaction is already present in the map of reactions to species
          if (!is_cyclic){
            map<reaction, set<string> >::iterator it = reactionToSpecies->find(newReaction);
            if (it == reactionToSpecies->end()){
              // If the reaction was not already in the map, add the reaction to the map linked to a list of species that contain it
              newReaction.reactionNumber = reactionCount++;
              reactionToSpecies->insert(pair<reaction, set<string> >(newReaction, set<string>()));
            } else {
              newReaction.reactionNumber = it->first.reactionNumber;
            }
         
            // Finally, add the species this reaction came from to the list of species providing this reaction
            (*reactionToSpecies)[newReaction].insert(newSpecies.name);
          }
        } else {
          // Grab the start and end metabolites first
          list<string> start_metabolites;
          list<string> end_metabolites;

          // Clear the string stream for the set of start metabolites
          metabolitesISS.clear();
          metabolitesISS.str(starts);

          // Add the start metabolites
          while (getline(metabolitesISS, start, METAB_DELIM)){
            start_metabolites.push_back(start);
          }         

          // Clear the string stream for the set of end metabolites
          metabolitesISS.clear();
          metabolitesISS.str(ends);

          // Add the end metabolites
          while (getline(metabolitesISS, end, METAB_DELIM)){
            end_metabolites.push_back(end);
          }

          // Now create reactions for each pair of start and end metabolites
          for (list<string>::const_iterator startIt = start_metabolites.begin(); startIt != start_metabolites.end(); ++startIt){
            for (list<string>::const_iterator endIt = end_metabolites.begin(); endIt != end_metabolites.end(); ++endIt){

              // Only create non-cyclic reactions
              if (*startIt != *endIt){
                reaction newReaction;
                newReaction.starts[*startIt] = 0;
                newReaction.ends[*endIt] = 0;

                map<reaction, set<string> >::const_iterator rxnIt = reactionToSpecies->find(newReaction);

                // Check if the reaction is already in the map of reactions to species and add a new entry if necessary
                if (rxnIt == reactionToSpecies->end()){
                  newReaction.reactionNumber = reactionCount++;
                  reactionToSpecies->insert(pair<reaction, set<string> >(newReaction, set<string>()));
                } else {
                  newReaction.reactionNumber = rxnIt->first.reactionNumber;
                }
                (*reactionToSpecies)[newReaction].insert(newSpecies.name);
              }
            }
          }
        }
      }
    }
  }
}

/* set<string>* readVertices(const char*)
 *
 * A function to parse a comma-separated list of vertex names
 */
set<string>* readVertices(const char* verticesList){
  set<string>* vertices = new set<string>;
  stringstream s;
  s << verticesList;
  string v;
  while (getline(s, v, ',')){
    if (v.empty()){
      throw EMPTY_METABOLITE_EX;
      delete vertices;
      return NULL;
    }
    vertices->insert(v);
  }
  return vertices;
}

/* graph* createGraph(map<reaction, set<string> >*, set<string>*, set<string>*, set<string*>, bool)
 *
 * A function to create an internal representation of a graph from the reactions associated with species and the metabolites of interest
 */
graph* createGraph(map<reaction, set<string> >* reactionToSpecies, set<string>* starts, set<string>* ends, set<string>* forced, bool is_simple){
  // Get all metabolite and reaction names since these will be the vertices.  At the same time, count the number of edges (from metabolite to reaction and reaction to metabolite)
  set<string> metabolites;
  size_t num_edges = 0;
  for (map<reaction, set<string> >::iterator mapIt = reactionToSpecies->begin(); mapIt != reactionToSpecies->end(); ++mapIt){
    for (map<string, int>::const_iterator startIt = mapIt->first.starts.begin(); startIt != mapIt->first.starts.end(); ++startIt){
      metabolites.insert(startIt->first);
      num_edges += 1;
    }
    for (map<string, int>::const_iterator endIt = mapIt->first.ends.begin(); endIt != mapIt->first.ends.end(); ++endIt){
      metabolites.insert(endIt->first);
      num_edges += 1;
    }
  }
  for (set<string>::iterator it = starts->begin(); it != starts->end(); ++it){
    metabolites.insert(*it);
  }
  for (set<string>::iterator it = ends->begin(); it != ends->end(); ++it){
    metabolites.insert(*it);
  }
  for (set<string>::iterator it = forced->begin(); it != forced->end(); ++it){
    metabolites.insert(*it);
  }

  // Initialize the graph with a number of vertices equal to the number of metabolites and reactions if a hyper graph, otherwise just the number of metabolites, with the number of edges as the connections from metabolites to reactions and reactions to metabolites if a hyper graph, otherwise just the number of reactions
  graph* output_graph = NULL;
  if (!is_simple){
    output_graph = new graph(metabolites.size() + reactionToSpecies->size(), num_edges, is_simple);
  } else {
    output_graph = new graph(metabolites.size(), reactionToSpecies->size(), is_simple);
  }
  
  // Now add all vertices and keep track of pointers to them for adding edges
  vertex* currVert = output_graph->vertices;
  map<string, vertex*> labelToVertex;
  for (set<string>::iterator it = metabolites.begin(); it != metabolites.end(); ++it){
    *currVert = vertex(*it, METABOLITE);
    labelToVertex[*it] = currVert;
    ++currVert;
  }

  // If this is a hyper graph, then there are vertices for each reaction
  if (!is_simple){
    for (map<reaction, set<string> >::const_iterator it = reactionToSpecies->begin(); it != reactionToSpecies->end(); ++it){
      *currVert = vertex(it->first.getEnzymeVarName(), ENZYME);
      labelToVertex[it->first.getEnzymeVarName()] = currVert;
      ++currVert;
    }
  }

  // Now add all edges
  edge* currEdge = output_graph->edges;
  for (map<reaction, set<string> >::iterator mapIt = reactionToSpecies->begin(); mapIt != reactionToSpecies->end(); ++mapIt){

    // If this is a hyper graph, then there are edges between metabolites and reactions
    if (!is_simple){
      // There's an edge from each metabolite to each reaction that has that metabolite in its start metabolites
      for (map<string, int>::const_iterator startIt = mapIt->first.starts.begin(); startIt != mapIt->first.starts.end(); ++startIt){
        *currEdge = edge(mapIt->first.getIncomingEdgeVarName(startIt->first));
        currEdge->begin = labelToVertex[startIt->first];
        currEdge->end = labelToVertex[mapIt->first.getEnzymeVarName()];
        currEdge->begin->outgoingEdges.push_back(currEdge);
        currEdge->end->incomingEdges.push_back(currEdge);
        ++currEdge;
      }

      // There's also an edge from each reaction to each metabolite that is in the reaction's end metabolites
      for (map<string, int>::const_iterator endIt = mapIt->first.ends.begin(); endIt != mapIt->first.ends.end(); ++endIt){
        *currEdge = edge(mapIt->first.getOutgoingEdgeVarName(endIt->first));
        currEdge->begin = labelToVertex[mapIt->first.getEnzymeVarName()];
        currEdge->end = labelToVertex[endIt->first];
        currEdge->begin->outgoingEdges.push_back(currEdge);
        currEdge->end->incomingEdges.push_back(currEdge);
        ++currEdge;
      }
    } else {
      // Otherwise, there are edges between metabolites
      *currEdge = edge(mapIt->first.getEnzymeVarName());
      currEdge->begin = labelToVertex[mapIt->first.starts.begin()->first];
      currEdge->end = labelToVertex[mapIt->first.ends.begin()->first];
      currEdge->begin->outgoingEdges.push_back(currEdge);
      currEdge->end->incomingEdges.push_back(currEdge);
      ++currEdge;
    }
  }
  return output_graph;
}

/* void addSpeciesVars(map<string, var>*, list<species>*)
 *
 * A function that adds variables to the given map of names to variable info for each species
 */
void addSpeciesVars(map<string, var>* vars, list<species>* speciesList){
  for (list<species>::iterator it = speciesList->begin(); it != speciesList->end(); ++it){

    // Each species variable has the same name as the species and is set as a binary variable with weight 1 in the optimization function
    var new_var = var();
    new_var.col = vars->size() + 1;
    new_var.name = it->name;
    new_var.colBound = FREE;
    new_var.lb = 0;
    new_var.ub = 1;
    new_var.coef = 1;
    new_var.vType = BINARY;
    vars->insert(pair<string, var>(it->name, new_var));
  }
}

/* void addFlowVars(map<string, var>*, graph*, int)
 *
 * A function that adds variables to the given map of names to variable info for each edge in the graph
 */
void addFlowVars(map<string, var>* vars, graph* input_graph, int numSinks){
  for (int i = 0; i < input_graph->numEdges; ++i){
    

    // Each flow variable is bounded to integer values 0 or greater with weight of 0 in the optimization function
    var new_var = var();
    new_var.col = vars->size() + 1;
    new_var.name = input_graph->edges[i].label;
    new_var.colBound = LOWER;
    new_var.lb = 0;
    new_var.ub = 0;
    new_var.coef = 0;
    new_var.vType = INTEGER;
    // The flow variables have an upper bound of the number of sinks in simple graphs
    if (input_graph->is_simple){
      new_var.colBound = DOUBLE;
      new_var.ub = numSinks;
    } 
    vars->insert(pair<string, var>(new_var.name, new_var));
  }
}

/* void addReactionVars(map<string, var>*, map<reaction, set<string> >*, bool)
 *
 * A function that adds variables to the given map of names to variable info for each reaction in the metabolic network
 */
void addReactionVars(map<string, var>* vars, map<reaction, set<string> >* reactionToSpecies, bool is_simple){
  for (map<reaction, set<string> >::const_iterator it = reactionToSpecies->begin(); it != reactionToSpecies->end(); ++it){


    // For each reaction, there is an associated binary value flag to indicate whether flow is non-zero through the reaction with a weight of 0 in the optimization function, 
    var new_var = var();
    new_var.col = vars->size() + 1;
    new_var.name = it->first.getEnzymeVarName();
    new_var.colBound = DOUBLE;
    new_var.lb = 0;
    new_var.ub = 1;
    new_var.coef = 0;
    new_var.vType = BINARY;

    // The name for the reaction variable needs to have an addition in the simple graph case, otherwise it's the same name as the flow variable
    if (is_simple){
      new_var.name = it->first.getEnzymeVarName() + FLAG;
    }
    vars->insert(pair<string, var>(new_var.name, new_var));
  }
}

/* void addCoverConstraints(list<constraint>*, map<reaction, list<string> >*, map<string, var>, set<string>*, bool)
 *
 * A function that adds constraints defining that each reaction in the path between the substrates and products must be provided by at least one species
 */
void addCoverConstraints(list<constraint>* constraints, map<reaction, set<string> >* reactionToSpecies, map<string, var>* vars, set<string>* ends, bool is_simple){
  
  // If a reaction has flow across it, then it must be covered by at least one species
  for (map<reaction, set<string> >::const_iterator mapIt = reactionToSpecies->begin(); mapIt != reactionToSpecies->end(); ++mapIt){
    
    // This constraint is broken into two constraints
    
    // First constraint, the sum of the species variables that contain the reaction must be greater than or equal to the variable of whether the reaction has flow, so their difference must be greater than 0
    constraint new_constraint = constraint();
    new_constraint.row = constraints->size() + 1;
    new_constraint.rowBound = LOWER;
    new_constraint.lb = 0;
    new_constraint.ub = 0;
    constraints->push_back(new_constraint);

    // Add the sum of the binary variables for the species
    var* varInfo;
    const set<string>* speciesList = &(mapIt->second);
    for (set<string>::const_iterator speciesIt = speciesList->begin(); speciesIt != speciesList->end(); ++speciesIt){
      varInfo = &((*vars)[(*speciesIt)]);
      varInfo->rows.push_back(constraints->back().row);
      varInfo->values.push_back(1);
    }
    
    // Subtract the variable for flow through the reaction, the name has an additional flag in a simple graph
    if (is_simple){
      varInfo = &((*vars)[mapIt->first.getEnzymeVarName() + FLAG]);
    } else {
      varInfo = &((*vars)[mapIt->first.getEnzymeVarName()]);
    }
    varInfo->rows.push_back(constraints->back().row);
    varInfo->values.push_back(-1);

    // Second constraint, the flow through a reaction must be less than or equal to the flow flag for that reaction (i.e. if there is flow, then the flag must be non-zero and thus 1 since the flag is binary)

    // If the graph is simple, then we use the flow through the actual reaction
    if (is_simple){
      constraint new_constraint = constraint();
      new_constraint.row = constraints->size() + 1;
      new_constraint.rowBound = LOWER;
      new_constraint.lb = 0;
      new_constraint.ub = 0;
      constraints->push_back(new_constraint);

      // Add the flag for flow times the number of sinks so the flow is at most the number of sinks
      varInfo = &((*vars)[mapIt->first.getEnzymeVarName() + FLAG]);
      varInfo->rows.push_back(constraints->back().row);
      varInfo->values.push_back(ends->size());

      // Subtract the flow through the reaction
      varInfo = &((*vars)[mapIt->first.getEnzymeVarName()]);
      varInfo->rows.push_back(constraints->back().row);
      varInfo->values.push_back(-1);
    } else {
      // Otherwise, we use the flow through each of the edges exiting the reaction vertex
      for (map<string, int>::const_iterator endIt = mapIt->first.ends.begin(); endIt != mapIt->first.ends.end(); ++endIt){
        constraint new_constraint = constraint();
        new_constraint.row = constraints->size() + 1;
        new_constraint.rowBound = LOWER;
        new_constraint.lb = 0;
        new_constraint.ub = 0;
        constraints->push_back(new_constraint);
        string edgeName = mapIt->first.getOutgoingEdgeVarName(endIt->first);

        // Add the enzyme times the largest possible value you can store in memory, since that is upper bound for the flow across a single edge
        varInfo = &((*vars)[mapIt->first.getEnzymeVarName()]);
        varInfo->rows.push_back(constraints->back().row);
        varInfo->values.push_back(MAX_COEFF);

        // Subtract the flow through the reaction
        varInfo = &((*vars)[edgeName]);
        varInfo->rows.push_back(constraints->back().row);
        varInfo->values.push_back(-1);
      }
    }
  }
}

/* void addFlowConstraints(list<constraint>*, map<string, var>*, set<string>*, set<string>*, set<string>*, graph*)
 *
 * A function to add contraints defining that there must be a path from substrates to products using network flow
 */
void addFlowConstraints(list<constraint>* constraints, map<string, var>* vars, set<string>* starts, set<string>* ends, set<string>* forced, graph* graph){
  // Now adding constraints for vertices
  vertex* currVertex = graph->vertices;
  for (int i = 0; i < graph->numVertices; ++i, ++currVertex){

    var* varInfo;

    // FLow rules depend on whether the graph is simple or not
    if (graph->is_simple){

      // The flow into a vertex must be equal to the flow out of that vertex unless that vertex is a start or end vertex
      constraint new_constraint = constraint();
      new_constraint.row = constraints->size() + 1;
      new_constraint.rowBound = FIXED;
      new_constraint.lb = 0;
      new_constraint.ub = 0;
  
      // The net flow out of a start vertex must be greater than or equal to 0      
      if (starts->find(currVertex->label) != starts->end()){
        new_constraint.rowBound = UPPER;

        // If this is a forced start vertex, then the flow out of it needs to be greater than or equal to 1
        if (forced->find(currVertex->label) != forced->end()){
          new_constraint.ub = -1;
        }
      } else if (ends->find(currVertex->label) != ends->end()){
        // The net flow into an end vertex must sum to 1
        new_constraint.lb = 1;
        new_constraint.ub = 1;
      }
      constraints->push_back(new_constraint);

      // Sum the incoming edges
      for (list<edge*>::iterator it = currVertex->incomingEdges.begin(); it != currVertex->incomingEdges.end(); ++it){
        varInfo = &((*vars)[(*it)->label]);
        varInfo->rows.push_back(constraints->back().row);
        varInfo->values.push_back(1);
      }

      // Subtract the sum of the outgoing edges
      for (list<edge*>::iterator it = currVertex->outgoingEdges.begin(); it != currVertex->outgoingEdges.end(); ++it){
        varInfo = &((*vars)[(*it)->label]);
        varInfo->rows.push_back(constraints->back().row);
        varInfo->values.push_back(-1);
      }
    } else {
      // The flow into a metabolite or enzyme must be equal to the flow out of that metabolite or enzyme unless it's a start or end metabolite
      constraint new_constraint = constraint();
      new_constraint.row = constraints->size() + 1;
      new_constraint.rowBound = FIXED;
      new_constraint.lb = 0;
      new_constraint.ub = 0;
      
      // The net flow out of a start vertex must be greater than or equal to 0
      if (starts->find(currVertex->label) != starts->end()){
        new_constraint.rowBound = UPPER;

        // If this is a forced start vertex, then the flow out of it needs to be greater than or equal to 1
        if (forced->find(currVertex->label) != forced->end()){
          new_constraint.ub = -1;
        }
      } else if (ends->find(currVertex->label) != ends->end()){
        // The net flow into an end vertex must be greater than or equal to 1
        new_constraint.rowBound = LOWER;
        new_constraint.lb = 1;
      }
      constraints->push_back(new_constraint);

      // Sum the incoming edges
      for (list<edge*>::iterator it = currVertex->incomingEdges.begin(); it != currVertex->incomingEdges.end(); ++it){
        string edgeName = (*it)->label;
        varInfo = &((*vars)[edgeName]);
        varInfo->rows.push_back(constraints->back().row);
        varInfo->values.push_back(1);
      }
   
      // Subtract the sum of the outgoing edges
      for (list<edge*>::iterator it = currVertex->outgoingEdges.begin(); it != currVertex->outgoingEdges.end(); ++it){
        string edgeName = (*it)->label;
        varInfo = &((*vars)[edgeName]);
        varInfo->rows.push_back(constraints->back().row);
        varInfo->values.push_back(-1);
      }

      // Additionally, if this is an enzyme, we require that the flow across each edge incoming to the vertex must be greater than or equal to the flag indicating the enzyme is used
      if (currVertex->type == ENZYME){

        for (list<edge*>::iterator incomingIt = currVertex->incomingEdges.begin(); incomingIt != currVertex->incomingEdges.end(); ++incomingIt){
          
          constraint new_constraint = constraint();
          new_constraint.row = constraints->size() + 1;
          new_constraint.rowBound = LOWER;
          new_constraint.lb = 0;
          new_constraint.ub = 0;
          constraints->push_back(new_constraint);

          // Add the flow for this incoming edge
          string edgeName = (*incomingIt)->label;
          varInfo = &((*vars)[edgeName]);
          varInfo->rows.push_back(constraints->back().row);
          varInfo->values.push_back(1);

          // Subtract the flag for this enzyme
          string enzymeName = currVertex->label;
          varInfo = &((*vars)[enzymeName]);
          varInfo->rows.push_back(constraints->back().row);
          varInfo->values.push_back(-1);
        }
      }
    }
  }
}

int main(int argc, const char* argv[])
{
  // Check for simple graph flag
  bool is_simple = false;
  const char* speciesFile = NULL;
  const char* startsMetabolites = NULL;
  const char* endsMetabolites = NULL;
  const char* forcedStarts = NULL;
  int positionArgumentCount = 0;
  for (int i = 1; i < argc; ++i){
    if (argv[i][0] == '-'){
      if (argv[i] == SIMPLE_FLAG){
        is_simple = true;
      } else if (argv[i] == FORCE_FLAG){
        forcedStarts = argv[i + 1];
        ++i;
      }
    } else {
      switch (positionArgumentCount){
        case 0:
          speciesFile = argv[i];
          ++positionArgumentCount;
          break;
        case 1:
          startsMetabolites = argv[i];
          ++positionArgumentCount;
          break;
        case 2:
          endsMetabolites = argv[i];
          ++positionArgumentCount;
          break;
      }
    }
  }

  // Read in the species to reactions file
  list<species>* speciesList = new list<species>;
  map<reaction, set<string> >* reactionToSpecies = new map<reaction, set<string> >;
  try {
    readSpeciesReactionFile(speciesList, reactionToSpecies, speciesFile, is_simple);
  } catch (custom_exception ex){
    if (ex == FILE_EX){
      cerr<<"ERROR: There is a problem with the species/reaction file."<<endl;
    } else if (ex == EMPTY_SPECIES_EX){
      cerr<<"ERROR: Species names must be non-empty strings."<<endl;
    } else if (ex == EMPTY_REACTION_EX){
      cerr<<"ERROR: Reactions must contain 3 space-delimited non-empty components, 1) substrate metabolite, 2) substrate/product separator, 3) product metabolite."<<endl;
    }
    delete speciesList;
    delete reactionToSpecies;
    return 1;
  }

  // Read in sets of starting and ending vertices
  set<string>* starts;
  set<string>* ends;
  set<string>* forced;
  try{
    starts = readVertices(startsMetabolites);
  } catch (custom_exception ex){
    if (ex == EMPTY_METABOLITE_EX){
      cerr<<"ERROR: Substrate metabolites must be non-empty strings."<<endl;
    }
    delete speciesList;
    delete reactionToSpecies;
    return 1;
  }
  try{
    ends = readVertices(endsMetabolites);
  } catch (custom_exception ex){
    if (ex == EMPTY_METABOLITE_EX){
      cerr<<"ERROR: Product metabolites must be non-empty strings."<<endl;
    }
    delete speciesList;
    delete reactionToSpecies;
    return 1;
  }
  try{
    forced = readVertices(forcedStarts);
  } catch (custom_exception ex){
    if (ex == EMPTY_METABOLITE_EX){
      cerr<<"ERROR: Forced start metabolites must be non-empty strings."<<endl;
    }
    delete speciesList;
    delete reactionToSpecies;
    return 1;
  }

  // Create the graph representation of the metabolic network
  graph* graph = createGraph(reactionToSpecies, starts, ends, forced, is_simple);

  list<constraint>* constraints = new list<constraint>;
  map<string, var>* vars = new map<string, var>;
  addSpeciesVars(vars, speciesList);
  addFlowVars(vars, graph, ends->size());
  addReactionVars(vars, reactionToSpecies, is_simple);
  addCoverConstraints(constraints, reactionToSpecies, vars, ends, is_simple);
  addFlowConstraints(constraints, vars, starts, ends, forced, graph);

  writeMPS(vars, constraints, PROBLEM_NAME);
  //writeCPLEX(vars, constraints, PROBLEM_NAME, MINIMIZE);

  delete starts;
  delete ends;
  delete graph;
  delete speciesList;
  delete reactionToSpecies;
  delete constraints;
  delete vars;
  return 0;
}
