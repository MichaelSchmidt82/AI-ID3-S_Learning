#ifndef GLOBALS_H
#define GLOBALS_H

/* Includes */
#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

/* Using statements */
using std::cout;
using std::endl;
using std::fstream;
using std::numeric_limits;
using std::string;
using std::stringstream;
using std::vector;

/* Global constants */
const int N_CLASSIFIERS = 3;

/* Forward declarations */
struct Node;

/* Typedefs */
typedef vector<double> Dataset;
typedef vector<Dataset> DataMatrix;
typedef vector<int> Indices;
typedef vector<Indices> IndicesMatrix;

#endif  // !GLOBALS_H
