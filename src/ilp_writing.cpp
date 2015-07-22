/* Author: Alex Eng
 * Date: 7/1/15
 *
 * This library contains functions for writing integer linear programming problems.
 *
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
#include <vector>
#include "ilp_writing.hpp"

using namespace std;

constraint::constraint(){
  	row = 0;
  	rowBound = FREE;
  	lb = 0;
  	ub = 0;
}

var::var(){
  	col = 0;
  	name = "";
  	colBound = FREE;
  	lb = 0;
  	ub = 0;
  	coef = 0;
  	vType = CONTINUOUS;
  	rows = list<int>();
  	values = list<double>();
}

void writeMPS(map<string, var>* vars, list<constraint>* constraints, string problem_name){

  // Write the problem name
  cout<<"NAME          "<<problem_name<<endl;

  // Write the labels of all of the rows preceded by their bound type
  cout<<"ROWS"<<endl;
  cout<<" N  COST"<<endl;
  int numRows = constraints->size();
  int numCols = vars->size();
  
  for (list<constraint>::iterator it = constraints->begin(); it != constraints->end(); ++it){
    cout<<" ";
    bound rowBound = it->rowBound;
    if (rowBound == FREE) {
      cout<<"N";
    } else if (rowBound == LOWER) {
      cout<<"G";
    } else if (rowBound == UPPER) {
      cout<<"L";
    } else if (rowBound == FIXED) {
      cout<<"E";
    }
    cout<<" ROW"<<it->row<<endl;
  }

  // Write the coefficient for each variable in the optimization function, along with the coefficient of the variable in each row
  cout<<"COLUMNS"<<endl;
  for (map<string, var>::iterator it = vars->begin(); it != vars->end(); ++it) {
    var* varInfo = &(it->second);
    cout<<" "<<varInfo->name.c_str();
    cout<<" COST "<<varInfo->coef<<endl;
    list<int>::iterator rowIt = varInfo->rows.begin();
    list<double>::iterator valIt = varInfo->values.begin();
    for (; rowIt != varInfo->rows.end() && valIt != varInfo->values.end(); ++rowIt, ++valIt){
      cout<<" "<<varInfo->name.c_str()<<" ROW"<<*rowIt<<" "<<*valIt<<endl;
    }
    //cout<<endl;
  }

  // Write the right hand side of each row
  cout<<"RHS"<<endl;
  for (list<constraint>::iterator it = constraints->begin(); it != constraints->end(); ++it){
    cout<<" RHS1 ROW"<<it->row<<" ";
    bound rowBound = it->rowBound;
    if (rowBound == LOWER || rowBound == FIXED) {
      cout<<it->lb;
    } else {
      cout<<it->ub;
    }
    cout<<endl;
  }

  // Write the bounds of each variable
  cout<<"BOUNDS"<<endl;
  for (map<string, var>::iterator it = vars->begin(); it != vars->end(); ++it){
    var* varInfo = &(it->second);
    if (varInfo->vType == BINARY){
      cout<<" BV BOUND "<<varInfo->name<<" "<<varInfo->ub<<endl;
    } else if (varInfo->vType == INTEGER){
      if (varInfo->colBound == UPPER){
        cout<<" UI BOUND "<<varInfo->name<<" "<<varInfo->ub<<endl;
      } else if (varInfo->colBound == LOWER){
        cout<<" LI BOUND "<<varInfo->name<<" "<<varInfo->lb<<endl;
      } else if (varInfo->colBound == DOUBLE){
        cout<<" LI BOUND "<<varInfo->name<<" "<<varInfo->lb<<endl;
        cout<<" UI BOUND "<<varInfo->name<<" "<<varInfo->ub<<endl;
      } 
    } else if (varInfo->vType == CONTINUOUS){
      if (varInfo->colBound == UPPER){
        cout<<" UP BOUND "<<varInfo->name<<" "<<varInfo->ub<<endl;
      } else if (varInfo->colBound == LOWER){
        cout<<" LO BOUND "<<varInfo->name<<" "<<varInfo->lb<<endl;
      } else if (varInfo->colBound == DOUBLE){
        cout<<" LI BOUND "<<varInfo->name<<" "<<varInfo->lb<<endl;
        cout<<" UI BOUND "<<varInfo->name<<" "<<varInfo->ub<<endl;
      } else if (varInfo->colBound == FIXED){
        cout<<" LX BOUND "<<varInfo->name<<" "<<varInfo->lb<<endl;
      } else if (varInfo->colBound == MINUS_INF){
        cout<<" MI BOUND "<<varInfo->name<<" "<<varInfo->ub<<endl;
      } else if (varInfo->colBound == PLUS_INF){
        cout<<" PL BOUND "<<varInfo->name<<" "<<varInfo->lb<<endl;
      }
    }
  }

  // Write the mandatory end tag
  cout<<"ENDATA"<<endl;
}

void writeCPLEX(map<string, var>* vars, list<constraint>* constraints, string problem_name, direction dir){

  // Write the direction
  if (dir == MAXIMIZE){
    cout<<"Maximize"<<endl;
  } else if (dir == MINIMIZE){
    cout<<"Minimize"<<endl;
  }

  // Make the structure to store rows so we can save time generating the objective function and rows at the same time
  vector<string> rows = vector<string>();
  vector<bool> is_starts = vector<bool>();
  for (list<constraint>::iterator it = constraints->begin(); it != constraints->end(); ++it){
    rows.push_back("\tc");
    is_starts.push_back(true);
    ostringstream oss;
    oss << it->row;
    rows[rows.size() - 1] += oss.str();
    rows[rows.size() - 1] += ": ";
  }

  // Write the objective function
  cout<<"\tobj: ";
  bool is_start = true;
  for (map<string, var>::iterator it = vars->begin(); it != vars->end(); ++it){
    var* varInfo = &(it->second);
    double coef = varInfo->coef;
    if (coef != 0){
      if (!is_start && coef > 0){
        cout<<" + ";
      }        
      cout<<coef<<" v"<<varInfo->name.c_str();
      is_start = false;
    }

    // Adding information to the rows
    list<int>::iterator rowIt = varInfo->rows.begin();
    list<double>::iterator valIt = varInfo->values.begin();
    for (; rowIt != varInfo->rows.end() && valIt != varInfo->values.end(); ++rowIt, ++valIt){
      ostringstream oss;
      oss << *valIt;
      if (!is_starts[*rowIt - 1] && *valIt > 0){
        rows[*rowIt - 1] += "+ ";
      }
      rows[*rowIt - 1] += oss.str();
      rows[*rowIt - 1] += " v";
      rows[*rowIt - 1] += varInfo->name.c_str();
      rows[*rowIt - 1] += " ";
      is_starts[*rowIt - 1] = false;
    }
  }
  cout<<endl;

  // Write the constraints
  cout<<"Subject To"<<endl;
  list<constraint>::iterator constraintIt = constraints->begin();
  vector<string>::iterator rowIt = rows.begin();
  for (; constraintIt != constraints->end() && rowIt != rows.end(); ++constraintIt, ++rowIt){
    cout<<*rowIt;
    bound rowBound = constraintIt->rowBound;
    if (rowBound == LOWER) {
      cout<<">= "<<constraintIt->lb<<endl;
    } else if (rowBound == UPPER){
      cout<<"<= "<<constraintIt->ub<<endl;
    } else if (rowBound == FIXED){
      cout<<"= "<<constraintIt->lb<<endl;
    }
  }

  // Write the variable bounds
  cout<<"Bounds"<<endl;
  for (map<string, var>::iterator it = vars->begin(); it != vars->end(); ++it){
    var* varInfo = &(it->second);
    if (varInfo->vType != BINARY){
      cout<<"\t";
      bound colBound = varInfo->colBound;
      string name = varInfo->name;
      if (colBound == UPPER){
        cout<<"-inf <= v"<<name.c_str()<<" <= "<<varInfo->ub<<endl;
      } else if (colBound == LOWER){
        cout<<varInfo->lb<<" <= v"<<name.c_str()<<" <= +inf"<<endl;
      } else if (colBound == DOUBLE){
        cout<<varInfo->lb<<" <= v"<<name.c_str()<<" <= "<<varInfo->ub<<endl;
      }
    }
  }

  // Write the variable types
  cout<<"Integer"<<endl;
  for (map<string, var>::iterator it = vars->begin(); it != vars->end(); ++it){
    var* varInfo = &(it->second);
    if (varInfo->vType == INTEGER){
      cout<<"\tv"<<varInfo->name.c_str()<<endl;
    }
  }
  cout<<"Binary"<<endl;
  for (map<string, var>::iterator it = vars->begin(); it != vars->end(); ++it){
    var* varInfo = &(it->second);
    if (varInfo->vType == BINARY){
      cout<<"\tv"<<varInfo->name.c_str()<<endl;
    }
  }
  cout<<"End"<<endl;
}