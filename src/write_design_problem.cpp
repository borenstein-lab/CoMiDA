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
const string VERTEX_LABEL = "vtx";
const string PROBLEM_NAME = "Community Design";
const string SPECIES_TAG = "s/";
const string SIMPLE_FLAG = "-s";
const string SPECIES_REACTION_DELIMITER_FLAG = "-srd";
const string REACTION_DELIMITER_FLAG = "-rd";
const string SUBSTRATE_PRODUCT_DELIMITER_FLAG = "-spd";
const string METABOLITE_DELIMITER_FLAG = "-md";
const char DEFAULT_SPECIES_REACTION_DELIMITER = ')';
const char DEFAULT_REACTION_DELIMITER = '|';
const char DEFAULT_SUBSTRATE_PRODUCT_DELIMITER = ':';
const char DEFAULT_METABOLITE_DELIMITER = ',';
const string SUBSTRATE_HEADER = "SUBSTRATES";
const string FORCED_HEADER = "FORCED_SUBSTRATES";
const string PRODUCT_HEADER = "PRODUCTS";
const string SPECIES_COST_HEADER = "SPECIES_COSTS";
const string NETWORK_HEADER = "SPECIES_NETWORKS";
string section_array[] = { SUBSTRATE_HEADER, FORCED_HEADER, PRODUCT_HEADER, SPECIES_COST_HEADER, NETWORK_HEADER};
const set<string> sections(section_array, section_array + 5);
enum custom_exception {FILE_EX, EMPTY_SPECIES_EX, EMPTY_REACTION_EX, EMPTY_SUBSTRATE_EX, EMPTY_FORCED_EX, EMPTY_PRODUCT_EX, UNKNOWN_SECTION_EX};
enum vertexType {METABOLITE, REACTION};

// A practical upper bound on the maximum flow along a single edge in the network, while theorectically the maximum flow needed could be higher for some networks, in most cases this upper bound will suffice and also avoid overflow issues in ILP solvers
const size_t MAX_COEFF = 100;

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

  /* string getREACTIONVarName()
   *
   * Returns the variable name that should be associated with the vertex correpsonding to this reaction
   */
  string getREACTIONVarName() const {
    return REAC_PREFIX + to_string(reactionNumber);
  }

  /* string getIncomingEdgeVarName(string, string)
   *
   * Returns the variable name that should be associated with the edge from the given substrate metabolite to the given reaction product
   */
  string getIncomingEdgeVarName(string substrate, string product) const {
    map<string, int>::const_iterator it = starts.find(substrate);
    return to_string(it->second) + FROM_TO + getProductVertexVarName(product);
  }

  /* string getOutgoingEdgeVarName(string)
   *
   * Returns the variable name that should be associated with the edge from this reaction to the given product metabolite
   */
  string getOutgoingEdgeVarName(string product) const {
    map<string, int>::const_iterator it = ends.find(product);
    return getREACTIONVarName() + FROM_TO + to_string(it->second);
  }

  /* string getProductVertexVarName(string)
   *
   * Returns the variable name that should be associated with the vertex linking this reaction's inputs to a specific product metabolite
   */
  string getProductVertexVarName(string product) const {
    map<string, int>::const_iterator it = ends.find(product);
    return getREACTIONVarName() + FROM_TO + to_string(it->second) + VERTEX_LABEL;
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

/* void parseSpeciesNetwork(istringstream&, list<species>*, map<reaction, set<string> >*, bool, char, char, char, char, int&)
 * 
 * Parses a line representing a species' metabolic network and updates the appropriate data structures
 */
void parseSpeciesNetwork(istringstream& lineISS, list<species>* species_list, map<reaction, set<string> >* reaction_to_species_map, bool is_simple, char species_reaction_delimiter, char reaction_delimiter, char substrate_product_delimiter, char metabolite_delimiter, int& reaction_count){

  // Declaring variables for storing the different parts of the line as we parse it
  string species_name;
  string reaction_string;
  string substrates;
  string products;
  string substrate;
  string product;
  istringstream reactionISS;
  istringstream metabolitesISS;

  // The species name comes first and is separated from the associated reactions by the given delimiter
  getline(lineISS, species_name, species_reaction_delimiter);
  // Check if the species name is empty
  if (species_name.empty()){
    throw EMPTY_SPECIES_EX;
    return;
  }

  // Store the information about the species in a struct and add that to the list of species
  species new_species;
  new_species.name = SPECIES_TAG + species_name;
  species_list->push_back(new_species);

  // Next are the reactions, separated by the given delimiter
  while (getline(lineISS, reaction_string, reaction_delimiter)){

    // Skip over completely empty reactions
    if (reaction_string != ""){
      // Reset the reaction string stream for the next reaction
      reactionISS.clear();
      reactionISS.str(reaction_string);

      // Each reaction has two parts, the substrates and the products
      getline(reactionISS, substrates, substrate_product_delimiter);
      getline(reactionISS, products);

      // Check for non-empty reaction components
      if (substrates.empty() || products.empty()){
        throw EMPTY_REACTION_EX;
        return;
      }

      // Depending on the graph type, we change how we add reactions. If the graph type is hyper, then each reaction will be defined by multiple substrates and products. If the graph type is simple, then we need to split each reaction into separate reactions for each substrate and product metabolite pair.
      if (!is_simple){
        // Store the reaction information in a struct
        reaction new_reaction;

        // Clear the metabolite string stream for the set of substrate metabolites
        metabolitesISS.clear();
        metabolitesISS.str(substrates);

        // Add the substrate metabolties
        int substrate_metabolite_count = 0;
        while (getline(metabolitesISS, substrate, metabolite_delimiter)){
          new_reaction.starts[substrate] = substrate_metabolite_count++;
        }

        // Clear the metabolite string stream for the set of product metabolites
        metabolitesISS.clear();
        metabolitesISS.str(products);

        // Add the product metabolites
        int product_metabolite_count = 0;
        while (getline(metabolitesISS, product, metabolite_delimiter)){
          new_reaction.ends[product] = product_metabolite_count++;
        }

        // Check if the product metabolties of this reaction are a subset of the substrate metabolites, if so this reaction is a useless loop
        bool is_cyclic = true;
        for (map<string, int>::const_iterator product_it = new_reaction.ends.begin(); product_it != new_reaction.ends.end(); ++product_it){
          if (new_reaction.starts.find(product_it->first) == new_reaction.starts.end()){
            is_cyclic = false;
            break;
          }
        }

        if (!is_cyclic){

          // Check if the reaction is already present in the map of reactions to species
          map<reaction, set<string> >::iterator it = reaction_to_species_map->find(new_reaction);
          if (it == reaction_to_species_map->end()){
            // If the reaction was not already in the map, add the reaction to the map linked to a set of the species that contain it
            new_reaction.reactionNumber = reaction_count++;
            (*reaction_to_species_map)[new_reaction] = set<string>();
          } else {
            // If the reaction is already in the map, then grab its reaction number
            new_reaction.reactionNumber = it->first.reactionNumber;
          }

          // Finally, add the species this reaction came from to the set of species that can catalyze this reaction
          (*reaction_to_species_map)[new_reaction].insert(new_species.name);
        }
      } else {
        // Otherwise we're using a simple representation of the metabolic network where there is a separate metabolic reaction for each substrate of an original reaction to each of that reaction's products

        // Grab the substrate and product metabolites first
        list<string> substrate_metabolites;
        list<string> product_metabolites;

        // Clear the metaboltie string stream for the set of substrate metabolites
        metabolitesISS.clear();
        metabolitesISS.str(substrates);

        // Add the start metabolites
        while (getline(metabolitesISS, substrate, metabolite_delimiter)){
          substrate_metabolites.push_back(substrate);
        }

        // Clear the metabolite string stream for the set of product metabolites
        metabolitesISS.clear();
        metabolitesISS.str(products);

        // Add the product metabolites
        while (getline(metabolitesISS, product, metabolite_delimiter)){
          product_metabolites.push_back(product);
        }

        // Now create the reactions for each pair of substrate and product metabolites
        for (list<string>::const_iterator substrate_it = substrate_metabolites.begin(); substrate_it != substrate_metabolites.end(); ++substrate_it){
          for (list<string>::const_iterator product_it = product_metabolites.begin(); product_it != product_metabolites.end(); ++product_it){

            // Only create non-cyclic reactions
            if (*substrate_it != *product_it){
              reaction new_reaction;
              new_reaction.starts[*substrate_it] = 0;
              new_reaction.ends[*product_it] = 0;

              // Check if the reaction is already in the map of reactions to species and add a new entry if necessary
              map<reaction, set<string> >::const_iterator reaction_it = reaction_to_species_map->find(new_reaction);
              if (reaction_it == reaction_to_species_map->end()){
                new_reaction.reactionNumber = reaction_count++;
                (*reaction_to_species_map)[new_reaction] = set<string>();
              } else {
                // if the reaction is already in the map, then grab the correct reaction number
                new_reaction.reactionNumber = reaction_it->first.reactionNumber;
              }
              (*reaction_to_species_map)[new_reaction].insert(new_species.name);
            }
          }
        }
      }
    }
  }
}

/* void readProblemDefinitionFile(set<string>*, set<string>*, set<string>*, map<string, double>*, list<species>*, map<reaction, set<string> >*, const char*, bool, char, char, char, char)
 *
 * A function that parses the problem definition file containing the available substrates, forced substrates, target products, species costs, and species metabolic networks and updates the input data structures accordingly.
 */
void readProblemDefinitionFile(set<string>* substrates, set<string>* forced_substrates, set<string>* products, map<string, double>* species_costs, list<species>* species_list, map<reaction, set<string> >* reaction_to_species_map, const char* fileName, bool is_simple, char species_reaction_delimiter, char reaction_delimiter, char substrate_product_delimiter, char metabolite_delimiter){

  // Set up tracker for which section of the input file we're in
  string section = "";

  // Setting up the reaction counter so we can store unique reaction names
  int reaction_count = 0;

  // Declaring variables for storing intermediate iformation as we parse the file
  string line;

  // Create the stream we will be reading the file from
  ifstream file(fileName);

  // Check for problems with the input file
  if (!file.good()){
    throw FILE_EX;
    return;
  }

  // Create string streams used for parsing lines
  istringstream lineISS;

  // Read until we get to the end of the file
  while (getline(file, line)){

    // Check if the line matches a section header
    set<string>::iterator find_header_it = sections.find(line);
    if (find_header_it != sections.end()){
      // If so, then we update the current section
      section = line;
    } else {
      // Otherwise, start parsing based on whatever section we're in
      if (section != SPECIES_COST_HEADER && section != NETWORK_HEADER){

        // Check which metabolite set we should add this metaoblite to after checking if the metabolite is a non-empty string
        if (section == SUBSTRATE_HEADER){
          if (line.empty()){
            throw EMPTY_SUBSTRATE_EX;
            return;
          }
          substrates->insert(line);
        } else if (section == FORCED_HEADER){
          if (line.empty()){
            throw EMPTY_FORCED_EX;
            return;
          }
          forced_substrates->insert(line);
        } else if (section == PRODUCT_HEADER){
          if (line.empty()){
            throw EMPTY_PRODUCT_EX;
            return;
          }
          products->insert(line);
        }
      } else {
        // Otherwise, we'll need to do some line parsing for the other sections
        lineISS.clear();
        lineISS.str(line);

        if (section == SPECIES_COST_HEADER){
          // Each line will have two parts, the species name and the cost
          string species_name;
          double cost = 1;
          lineISS >> species_name;

          // Check to see if we actually got a species name
          if (species_name.empty()){
            throw EMPTY_SPECIES_EX;
            return;
          }

          lineISS >> cost;
          (*species_costs)[SPECIES_TAG + species_name] = cost;
        } else if (section == NETWORK_HEADER){
          // We'll pass off the species network line parsing to a separate function
          try{
            parseSpeciesNetwork(lineISS, species_list, reaction_to_species_map, is_simple, species_reaction_delimiter, reaction_delimiter, substrate_product_delimiter, metabolite_delimiter, reaction_count);
          } catch (custom_exception ex){
            throw ex;
            return;
          }
        } else {
          // If we didn't find any of the expected sections, throw an error
          throw UNKNOWN_SECTION_EX;
        }
      }
    }
  }
}

/* graph* createGraph(map<reaction, set<string> >*, set<string>*, set<string>*, set<string*>, bool)
 *
 * A function to create an internal representation of a graph from the reactions associated with species and the metabolites of interest
 */
graph* createGraph(map<reaction, set<string> >* reactionToSpecies, set<string>* starts, set<string>* ends, set<string>* forced, bool is_simple){
  // Get all metabolite names.  At the same time, count the number of edges and the number of reaction products.
  set<string> metabolites;
  size_t num_edges = 0;
  size_t num_products = 0;
  for (map<reaction, set<string> >::iterator mapIt = reactionToSpecies->begin(); mapIt != reactionToSpecies->end(); ++mapIt){
    for (map<string, int>::const_iterator startIt = mapIt->first.starts.begin(); startIt != mapIt->first.starts.end(); ++startIt){
      metabolites.insert(startIt->first);
      num_edges += mapIt->first.ends.size();
    }
    for (map<string, int>::const_iterator endIt = mapIt->first.ends.begin(); endIt != mapIt->first.ends.end(); ++endIt){
      metabolites.insert(endIt->first);
      num_edges += 1;
      num_products += 1;
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

  // Initialize the graph with a number of vertices equal to the number of metabolites and reaction products if a hyper graph, otherwise just the number of metabolites, with the number of edges as the connections from metabolites to reactions and reactions to metabolites if a hyper graph, otherwise just the number of reactions
  graph* output_graph = NULL;
  if (!is_simple){
    output_graph = new graph(metabolites.size() + num_products, num_edges, is_simple);
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

  // If this is a hyper graph, then there are vertices for each reaction product
  if (!is_simple){
    for (map<reaction, set<string> >::const_iterator mapIt = reactionToSpecies->begin(); mapIt != reactionToSpecies->end(); ++mapIt){
      for (map<string, int>::const_iterator product_it = mapIt->first.ends.begin(); product_it != mapIt->first.ends.end(); ++product_it){
        *currVert = vertex(mapIt->first.getProductVertexVarName(product_it->first), REACTION);
        labelToVertex[mapIt->first.getProductVertexVarName(product_it->first)] = currVert;
        ++currVert;
      }
    }
  }

  // Now add all edges
  edge* currEdge = output_graph->edges;
  for (map<reaction, set<string> >::iterator mapIt = reactionToSpecies->begin(); mapIt != reactionToSpecies->end(); ++mapIt){

    // If this is a hyper graph, then there are edges between metabolites and reactions
    if (!is_simple){
      // There's an edge from each metabolite to each reaction product that has that metabolite in its start metabolites
      for (map<string, int>::const_iterator startIt = mapIt->first.starts.begin(); startIt != mapIt->first.starts.end(); ++startIt){
        for (map<string, int>::const_iterator end_it = mapIt->first.ends.begin(); end_it != mapIt->first.ends.end(); ++end_it){
          *currEdge = edge(mapIt->first.getIncomingEdgeVarName(startIt->first, end_it->first));
          currEdge->begin = labelToVertex[startIt->first];
          currEdge->end = labelToVertex[mapIt->first.getProductVertexVarName(end_it->first)];
          currEdge->begin->outgoingEdges.push_back(currEdge);
          currEdge->end->incomingEdges.push_back(currEdge);
          ++currEdge;
        }
      }

      // There's also an edge from each reaction product vertex to the associated product metabolite
      for (map<string, int>::const_iterator endIt = mapIt->first.ends.begin(); endIt != mapIt->first.ends.end(); ++endIt){
        *currEdge = edge(mapIt->first.getOutgoingEdgeVarName(endIt->first));
        currEdge->begin = labelToVertex[mapIt->first.getProductVertexVarName(endIt->first)];
        currEdge->end = labelToVertex[endIt->first];
        currEdge->begin->outgoingEdges.push_back(currEdge);
        currEdge->end->incomingEdges.push_back(currEdge);
        ++currEdge;
      }
    } else {
      // Otherwise, there are edges between metabolites
      *currEdge = edge(mapIt->first.getREACTIONVarName());
      currEdge->begin = labelToVertex[mapIt->first.starts.begin()->first];
      currEdge->end = labelToVertex[mapIt->first.ends.begin()->first];
      currEdge->begin->outgoingEdges.push_back(currEdge);
      currEdge->end->incomingEdges.push_back(currEdge);
      ++currEdge;
    }
  }
  return output_graph;
}

/* void addSpeciesVars(map<string, var>*, list<species>*, map<string, double>*)
 *
 * A function that adds variables to the given map of names to variable info for each species
 */
void addSpeciesVars(map<string, var>* vars, list<species>* speciesList, map<string, double>* species_costs){
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

    // Check if the species has been assigned a cost in the objective function
    map<string, double>::const_iterator cost_it = species_costs->find(it->name);
    if (cost_it != species_costs->end()){
      new_var.coef = (*species_costs)[it->name];
    }

    vars->insert(pair<string, var>(it->name, new_var));
  }
}

/* void addFlowVars(map<string, var>*, graph*, int)
 *
 * A function that adds variables to the given map of names to variable info for each edge in the graph
 */
void addFlowVars(map<string, var>* vars, graph* input_graph, int numSinks){
  for (int i = 0; i < input_graph->numEdges; ++i){
    

    // Each flow variable is an integer with values between 0 or the maximum coefficient set with weight of 0 in the optimization function
    var new_var = var();
    new_var.col = vars->size() + 1;
    new_var.name = input_graph->edges[i].label;
    new_var.colBound = DOUBLE;
    new_var.lb = 0;
    new_var.ub = MAX_COEFF;
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
    new_var.name = it->first.getREACTIONVarName();
    new_var.colBound = DOUBLE;
    new_var.lb = 0;
    new_var.ub = 1;
    new_var.coef = 0;
    new_var.vType = BINARY;

    // The name for the reaction variable needs to have an addition in the simple graph case, otherwise it's the same name as the flow variable
    if (is_simple){
      new_var.name = it->first.getREACTIONVarName() + FLAG;
    }
    vars->insert(pair<string, var>(new_var.name, new_var));
  }
}

/* void addCoverConstraints(list<constraint>*, map<reaction, list<string> >*, map<string, var>, set<string>*, size_t, bool)
 *
 * A function that adds constraints defining that each reaction in the path between the substrates and products must be provided by at least one species
 */
void addCoverConstraints(list<constraint>* constraints, map<reaction, set<string> >* reactionToSpecies, map<string, var>* vars, set<string>* ends, size_t numEdges, bool is_simple){
  
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
      varInfo = &((*vars)[mapIt->first.getREACTIONVarName() + FLAG]);
    } else {
      varInfo = &((*vars)[mapIt->first.getREACTIONVarName()]);
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
      varInfo = &((*vars)[mapIt->first.getREACTIONVarName() + FLAG]);
      varInfo->rows.push_back(constraints->back().row);
      varInfo->values.push_back(ends->size());

      // Subtract the flow through the reaction
      varInfo = &((*vars)[mapIt->first.getREACTIONVarName()]);
      varInfo->rows.push_back(constraints->back().row);
      varInfo->values.push_back(-1);
    } else {
      // Otherwise, we use the flow through each of the reaction product edges associated with the reaction
      for (map<string, int>::const_iterator endIt = mapIt->first.ends.begin(); endIt != mapIt->first.ends.end(); ++endIt){
        constraint new_constraint = constraint();
        new_constraint.row = constraints->size() + 1;
        new_constraint.rowBound = LOWER;
        new_constraint.lb = 0;
        new_constraint.ub = 0;
        constraints->push_back(new_constraint);
        string edgeName = mapIt->first.getOutgoingEdgeVarName(endIt->first);

        // Add the reaction times the maximum possible flow
        varInfo = &((*vars)[mapIt->first.getREACTIONVarName()]);
        varInfo->rows.push_back(constraints->back().row);

        // Practical upper bound
        varInfo->values.push_back(MAX_COEFF);

        // Theoretical upper bound
        //varInfo->values.push_back(numEdges);

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
      // The flow into a metabolite must be equal to the flow out of that metabolite unless it's a start or end metabolite
      if (currVertex->type == METABOLITE){
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
      }
      // If this is a reaction product vertex, we require that the flow across each edge incoming to the vertex must be equal to the flow exiting the reaction product vertex
      else if (currVertex->type == REACTION){
        for (list<edge*>::iterator incomingIt = currVertex->incomingEdges.begin(); incomingIt != currVertex->incomingEdges.end(); ++incomingIt){
          
          constraint new_constraint = constraint();
          new_constraint.row = constraints->size() + 1;
          new_constraint.rowBound = FIXED;
          new_constraint.lb = 0;
          new_constraint.ub = 0;
          constraints->push_back(new_constraint);

          // Add the flow for this incoming edge
          string incomingEdgeName = (*incomingIt)->label;
          varInfo = &((*vars)[incomingEdgeName]);
          varInfo->rows.push_back(constraints->back().row);
          varInfo->values.push_back(1);

          // Subtract the flow for the outgoing edge
          string outgoingEdgeName = currVertex->outgoingEdges.front()->label;
          varInfo = &((*vars)[outgoingEdgeName]);
          varInfo->rows.push_back(constraints->back().row);
          varInfo->values.push_back(-1);
        }
      }
    }
  }
}

int main(int argc, const char* argv[])
{
  // Check for flags
  bool is_simple = false;
  char species_reaction_delimiter = DEFAULT_SPECIES_REACTION_DELIMITER;
  char reaction_delimiter = DEFAULT_REACTION_DELIMITER;
  char substrate_product_delimiter = DEFAULT_SUBSTRATE_PRODUCT_DELIMITER;
  char metabolite_delimiter = DEFAULT_METABOLITE_DELIMITER;
  const char* problem_definition_file = NULL;
  int positionArgumentCount = 0;

  // Parse the command line arguments
  for (int i = 1; i < argc; ++i){
    if (argv[i][0] == '-'){

      // First check for flags with no arguments
      if (argv[i] == SIMPLE_FLAG){
        is_simple = true;

      } else {
        // Next check for flags with one argument
        if (i + 1 >= argc){
          cerr<<"Command line option "<<argv[i]<<" requires an argument."<<endl;
          return 1;
        } else if (argv[i] == SPECIES_REACTION_DELIMITER_FLAG){
          species_reaction_delimiter = argv[i + 1][0];
          ++i;
        } else if (argv[i] == REACTION_DELIMITER_FLAG){
          reaction_delimiter = argv[i + 1][0];
          ++i;
        } else if (argv[i] == SUBSTRATE_PRODUCT_DELIMITER_FLAG){
          substrate_product_delimiter = argv[i + 1][0];
          ++i;
        } else if (argv[i] == METABOLITE_DELIMITER_FLAG){
          metabolite_delimiter = argv[i + 1][0];
          ++i;
        }
      }

    } else {
      // If not a flag, move on to positional required arguments
      switch (positionArgumentCount){
        case 0:
          problem_definition_file = argv[i];
          ++positionArgumentCount;
          break;
      }
    }
  }

  // Check for empty positional arguments
  if (problem_definition_file == NULL){
    cerr<<"No problem definition filename provided."<<endl;
    return 1;
  }

  // Read the problem definition file
  set<string>* substrates = new set<string>;
  set<string>* forced_substrates = new set<string>;
  set<string>* products = new set<string>;
  map<string, double>* species_costs = new map<string, double>;
  list<species>* speciesList = new list<species>;
  map<reaction, set<string> >* reactionToSpecies = new map<reaction, set<string> >;
  try {
    readProblemDefinitionFile(substrates, forced_substrates, products, species_costs, speciesList, reactionToSpecies, problem_definition_file, is_simple, species_reaction_delimiter, reaction_delimiter, substrate_product_delimiter, metabolite_delimiter);
  } catch (custom_exception ex){
    if (ex == FILE_EX){
      cerr<<"ERROR: There is a problem with the species/reaction file."<<endl;
    } else if (ex == EMPTY_SUBSTRATE_EX){
      cerr<<"ERROR: Substrate metabolites must be non-empty strings."<<endl;
    } else if (ex == EMPTY_FORCED_EX){
      cerr<<"ERROR: Forced substrate metabolites must be non-empty strings."<<endl;
    } else if (ex == EMPTY_PRODUCT_EX){
      cerr<<"ERROR: Product metabolites must be non-empty strings."<<endl;
    } else if (ex == EMPTY_SPECIES_EX){
      cerr<<"ERROR: Species names must be non-empty strings."<<endl;
    } else if (ex == EMPTY_REACTION_EX){
      cerr<<"ERROR: Reactions must contain 2 non-empty components, 1) substrate metabolites, 2) product metabolites."<<endl;
    } else if (ex == UNKNOWN_SECTION_EX){
      cerr<<"ERROR: Unknown section type in problem definition file."<<endl;
    }
    delete substrates;
    delete forced_substrates;
    delete products;
    delete species_costs;
    delete speciesList;
    delete reactionToSpecies;
    return 1;
  }

  // Create the graph representation of the metabolic network
  graph* graph = createGraph(reactionToSpecies, substrates, products, forced_substrates, is_simple);

  list<constraint>* constraints = new list<constraint>;
  map<string, var>* vars = new map<string, var>;
  addSpeciesVars(vars, speciesList, species_costs);
  addFlowVars(vars, graph, products->size());
  addReactionVars(vars, reactionToSpecies, is_simple);
  addCoverConstraints(constraints, reactionToSpecies, vars, products, graph->numEdges, is_simple);
  addFlowConstraints(constraints, vars, substrates, products, forced_substrates, graph);

  writeMPS(vars, constraints, PROBLEM_NAME);
  //writeCPLEX(vars, constraints, PROBLEM_NAME, MINIMIZE);

  delete substrates;
  delete products;
  delete graph;
  delete species_costs;
  delete speciesList;
  delete reactionToSpecies;
  delete constraints;
  delete vars;
  return 0;
}
