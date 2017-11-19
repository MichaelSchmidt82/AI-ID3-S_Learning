#ifndef NODE_H
#define NODE_H

//#include "globals.h"

// Typedefs
typedef Node* NodePtr;

struct Node {
	Node() : property(-1), classification(-1), terminal(false),
	 	threshold(-1), left(nullptr), right(nullptr) {};

	int property;
	int classification;
	bool terminal;
	double threshold;

	NodePtr left;
	NodePtr right;
};
#endif // !NODE_H
