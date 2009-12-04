#ifndef DECHUNK_H
#define DECHUNK_H

#include <sstream>
#include <vector>

using namespace std;


bool dechunk (const vector<char>& vec, stringstream& ss);

void gunzip (stringstream& ss);

#endif /* DECHUNK_H */

