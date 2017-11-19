/*
Author:		Michael Schmidt

Assumptions:
- There are 3 classification and this value is hardcoded.
See the constant N_CLASSIFIERS to adjust.
*/


/* Includes */
#include "globals.h"
#include "node.h"

/* Function Prototypes */
inline double prob(double count, int total);
DataMatrix parse(DataMatrix & testing_out, char * argv[]);
IndicesMatrix sort_attributes(DataMatrix & data);
void clear_nodes(NodePtr & node);

IndicesMatrix split(const DataMatrix & data,
	IndicesMatrix & mod_to_grtr,
	int prop,
	double threshold);

double shannon_entropy(const DataMatrix & data,
	const IndicesMatrix & indices,
	const size_t CLASS_ROW);

double maximize_gain(const DataMatrix & data,
	const IndicesMatrix & indices,
	int & prop,
	double & threshold,
	const size_t CLASS_ROW);

double expected_given(const DataMatrix & data,
	const IndicesMatrix & indices,
	const int PROP,
	double threshold,
	const size_t CLASS_ROW);

void build_tree(const DataMatrix & data,
	IndicesMatrix & indices,
	NodePtr curr,
	const size_t CLASS_ROW);

bool is_terminal(const DataMatrix & data,
	const IndicesMatrix & indices,
	int prop,
	const size_t CLASS_ROW);

int classify(const DataMatrix & data,
	const IndicesMatrix & indices,
	const size_t CLASS_ROW);

void validate(const DataMatrix & d,
	NodePtr root,
	const size_t CLASS_ROW);

bool check(NodePtr node,
	Dataset data,
	const size_t CLASS_ROW);

/* int main() */
int main(int argc, char* argv[]) {
	assert(argc == 4 && "3 CLAs must be provided");

	/* Constants */
	const size_t CLASS_ROW = atoi(argv[1]);

	/* Variables */
	DataMatrix testing;
	DataMatrix training = parse(testing, argv);
	IndicesMatrix indices = sort_attributes(training);
	NodePtr root = nullptr;

	/* Procedure */
	try {
		root = new Node();
		build_tree(training, indices, root, CLASS_ROW);
	} catch (const std::bad_alloc & ba) {
		std::cerr << "Whoops: " << ba.what() << endl;
	}

	validate(testing, root, CLASS_ROW);

	/* Tidy up */
	clear_nodes(root);
	return 0;
}

/* Functions */
void validate(const DataMatrix & data, NodePtr root, const size_t CLASS_ROW) {
	Dataset d;
	int count = 0;

	for (int c = 0; c < data[CLASS_ROW].size(); c++) {
		for (int r = 0; r <= CLASS_ROW; r++)
			d.push_back(data[r][c]);

		if (check(root, d, CLASS_ROW))
			count++;

		d.clear();
	}
	cout << count << endl;
}

bool check(NodePtr node, Dataset data, const size_t CLASS_ROW) {
	// base case
	if (node->terminal)
		return (node->classification == data[CLASS_ROW]);
	else {
		int prop = node->property;
		double threshold = node->threshold;
		if (data[prop] < threshold)
			return check(node->left, data, CLASS_ROW);
		else
			return check(node->right, data, CLASS_ROW);
	}
}

void build_tree(const DataMatrix & data,
	IndicesMatrix & indices,
	NodePtr curr,
	const size_t CLASS_ROW) {

	/* Variables */
	int & prop = curr->property;
	double & threshold = curr->threshold;
	double entropy = maximize_gain(data, indices, prop, threshold, CLASS_ROW);
	bool terminal = is_terminal(data, indices, prop, CLASS_ROW);

	NodePtr left, right;
	IndicesMatrix lesser;

	curr->terminal = terminal;

	/* Base case */
	if (curr->terminal) {
		curr->classification = classify(data, indices, CLASS_ROW);
		return;
	}

	/* Recurvise case */
	lesser = split(data, indices, prop, threshold);

	curr->left = new Node();
	curr->right = new Node();

	build_tree(data, lesser, curr->left, CLASS_ROW);
	build_tree(data, indices, curr->right, CLASS_ROW);
}

bool is_terminal(const DataMatrix & data, const IndicesMatrix & indices, int prop, const size_t CLASS_ROW) {
	bool terminal = true;
	int classification = data[CLASS_ROW][indices[CLASS_ROW][0]];

	/* Zero entropy */
	if (prop != -1)
		terminal = false;

	/* Check to see if this node should be a terminal node */
	if (!terminal && indices[0].size() > 1)
		terminal = false;

	for (int c = 1; !terminal && (c < indices[0].size()); c++)
		if (data[CLASS_ROW][indices[CLASS_ROW][c]] != classification) {
			terminal = false;
			break;
		}

	/* If all the data per attribute is not the same, not terminal */
	double col_data = data[0][indices[0][0]];
	for (int r = 0; !terminal && r < CLASS_ROW; r++) {
		for (int c = 1; c < indices[r].size(); c++)
			if (data[r][indices[r][c]] != col_data) {
				terminal = false;
				break;
			}
		col_data = data[r][indices[r][0]];
	}

	return terminal;
}

int classify(const DataMatrix & data, const IndicesMatrix & indices, const size_t CLASS_ROW) {
	int max_val = -1;
	int max_idx = 0;
	if (indices[CLASS_ROW].size() == 1)
		return data[CLASS_ROW][indices[CLASS_ROW][0]];

	vector<int> class_cnt(N_CLASSIFIERS, 0);

	for (int c = 0; c < indices[CLASS_ROW].size(); c++)
		class_cnt[data[CLASS_ROW][indices[CLASS_ROW][c]]]++;

	for (int i = 0; i < class_cnt.size(); i++)
		if (class_cnt[i] > max_val) {
			max_val = class_cnt[i];
			max_idx = i;
		}

	return max_idx;
}

IndicesMatrix split(const DataMatrix & data, IndicesMatrix & indices, int prop, double threshold) {
	IndicesMatrix lesser, greater;
	for (int i = 0; i < indices.size(); i++) {
		lesser.push_back(Indices());
		greater.push_back(Indices());
	}

	/* Copy indices to greater/lesser indices */
	for (size_t c = 0; c < indices[prop].size(); c++)
		if (data[prop][indices[prop][c]] < threshold)
			for (int prop = 0; prop < indices.size(); prop++)
				lesser[prop].push_back(indices[prop][c]);
		else
			for (int prop = 0; prop < indices.size(); prop++)
				greater[prop].push_back(indices[prop][c]);

	indices.clear();
	indices = greater;	// greater by reference
	return lesser;		// lesser is returned
}

double maximize_gain(const DataMatrix & data,
	const IndicesMatrix & indices,
	int & _prop,
	double & _threshold,
	const size_t CLASS_ROW) {

	size_t total = indices[0].size();
	double entropy = shannon_entropy(data, indices, CLASS_ROW);
	double best = entropy;

	for (int pr = 0; pr < CLASS_ROW; pr++)
		for (size_t c = 1; c < total; c++)
			if (data[pr][indices[pr][c - 1]] != data[pr][indices[pr][c]]) {
				double curr = expected_given(data, indices, pr, c, CLASS_ROW);
				if (curr < best) {
					best = curr;
					_prop = pr;
					_threshold =
						data[pr][indices[pr][c - 1]] +
						data[pr][indices[pr][c]];
					_threshold /= 2;
				}
			}

	return entropy - best;
}

double expected_given(const DataMatrix & data,
	const IndicesMatrix & indices,
	const int PROP,
	double threshold,
	const size_t CLASS_ROW) {

	/* Variables */
	Indices c_probs(N_CLASSIFIERS, 0);
	Indices nc_probs(N_CLASSIFIERS, 0);

	double c_coef;
	double nc_coef;
	double term_c = 0;
	double term_nc = 0;

	int total = indices[PROP].size();

	/* Tally which classifiers fall below or at/above some given thershold */
	for (int c = 0; c < total; c++)
		if (c < threshold)
			c_probs[data[CLASS_ROW][indices[PROP][c]]]++;
		else
			nc_probs[data[CLASS_ROW][indices[PROP][c]]]++;

	/* Create the coefficients for E(x) */
	c_coef = double(threshold) / total;
	nc_coef = double(total - threshold) / total;

	/* Calculate the sums */
	for (int c : c_probs)
		term_c += prob(c, threshold);

	for (int nc : nc_probs)
		term_nc += prob(nc, total - threshold);

	/* Finish and return E(x) */
	return (c_coef * term_c) + (nc_coef * term_nc);
}

double shannon_entropy(const DataMatrix & data, const IndicesMatrix & indices, const size_t CLASS_ROW) {
	Indices c_probs(N_CLASSIFIERS, 0);
	size_t total = indices[0].size();
	double ret = 0;

	/* Tally classifier counts */
	for (size_t c = 0; c < total; c++)
		c_probs[data[CLASS_ROW][indices[CLASS_ROW][c]]]++;

	/* Compute terms, sum, and return */
	for (int c : c_probs)
		ret += prob(c, total);

	return ret;
}

inline double prob(double count, int total) {
	double d = (count / total * -1) * log2(count / total);
	if (std::isnan(d) || abs(d) == numeric_limits<double>::infinity())
		return 0;
	return d;
}

IndicesMatrix sort_attributes(DataMatrix & data) {
	IndicesMatrix indices;

	indices.resize(data.size());
	for (size_t r = 0; r < indices.size(); r++) {
		indices[r].resize(data[r].size());
		iota(indices[r].begin(), indices[r].end(), 0);
		Dataset &d = (data[r]);
		sort(indices[r].begin(), indices[r].end(),
			[&](size_t a, size_t b) { return d[a] < d[b]; });
	}
	return indices;
}

DataMatrix parse(DataMatrix & testing_out, char * argv[]) {
	fstream training, testing;
	training.open(argv[2]);
	testing.open(argv[3]);

	string line;
	double value;
	DataMatrix data;

	assert(training.is_open() && testing.is_open() && "data files failed to open.");

	getline(training, line);
	stringstream digest(line);

	while (!digest.eof()) {
		digest >> value;
		data.push_back(Dataset());
		testing_out.push_back(Dataset());
	}

	while (!training.eof()) {
		digest.str(line);
		for (size_t i = 0; i < data.size(); i++) {
			digest >> value;
			data[i].push_back(value);
		}
		getline(training, line);
		digest.clear();
	}

	getline(testing, line);
	digest.str(line);

	while (!testing.eof()) {
		digest.str(line);
		for (size_t i = 0; i < data.size(); i++) {
			digest >> value;
			testing_out[i].push_back(value);
		}
		getline(testing, line);
		digest.clear();
	}

	testing.close();
	training.close();
	return data;
}

void clear_nodes(NodePtr & node) {
	/* Recursive case */
	if (!node->terminal) {
		clear_nodes(node->left);
		clear_nodes(node->right);
	}
	/* Base case */
	delete node;
	node = nullptr;
}
