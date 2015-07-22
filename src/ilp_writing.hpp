/* Author: Alex Eng
 * Date: 7/1/15
 *
 * Declaring the structs and functions used for writing integer linear programming problems.
 *
 */

#ifndef ILP_WRITING_HPP_INCLUDED
#define ILP_WRITING_HPP_INCLUDED 1

#include <string>
#include <list>
#include <map>

using namespace std;

enum bound {FREE, LOWER, UPPER, FIXED, DOUBLE, MINUS_INF, PLUS_INF};
enum variableType {BINARY, INTEGER, CONTINUOUS};
enum direction {MAXIMIZE, MINIMIZE};

// Structure for storing information about ILP constraints
struct constraint{
	int row;
  bound rowBound;
  double lb;
  double ub;

  // Default constructor declaration
  constraint();
};

// Structure for storing information about ILP variables
struct var{
	int col;
  string name;
  bound colBound;
  double lb;
  double ub;
  double coef;
  variableType vType;
  list<int> rows;
  list<double> values;

  // Default constructor declaration
  var();
};

/* void writeMPS(map<string, var>*, list<constraint>*, string)
 *
 * A function to write the text file containing the ILP problem formulation in MPS format
 */
void writeMPS(map<string, var>* vars, list<constraint>* constraints, string problem_name);

/* void writeCPLEX(map<string, var>*, list<constraint>*, string, direction)
 *
 * A function to write the test file containing the ILP problem formulation in CPLEX format
 */
void writeCPLEX(map<string, var>* vars, list<constraint>* constraints, string problem_name, direction dir);

#endif // ILP_WRITING_HPP_INCLUDED
