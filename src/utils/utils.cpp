#include "utils.h"

void initRnd() {
	srand(time(NULL));
}

int rnd(int min, int max) {
	return (rand() % (max - min + 1) + min);
}

int intRnd(int min, int max) {
	return (rand() % (max - min + 1) + min);
}

double doubleRnd(double min, double max) {
	return min + (max - min) * ((double) rand()) / RAND_MAX;
}
void writeBin(ostream & out, double v) {
	if (isCpuLittleEndian ^ isFileLittleEndian) {
		// Switch between the two
		char data[8], *pDouble = (char*) (double*) (&v);
		for (int i = 0; i < 8; ++i) {
			data[i] = pDouble[7 - i];
		}
		out.write(data, 8);
	} else
		out.write((char*) (&v), 8);
}

void writeBinVect(ostream & out, vector<double> & v) {
	writeBin(out, (double) v.size());
	for (int i = 0; i < v.size(); ++i) {
		writeBin(out, v[i]);
	}
}
double loadBin(istream & in) {
	char data[8];
	double res;
	in.read(data, 8);
	if (isCpuLittleEndian ^ isFileLittleEndian) {
		char data_load[8];
		// Switch between the two
		for (int i = 0; i < 8; ++i) {
			data_load[i] = data[7 - i];
		}
		memcpy((char*) &res, &data_load[0], 8);
	} else
		memcpy((char*) &res, &data[0], 8);

	return res;
}

vector<double> loadBinVector(istream & in) {
	int N = (int) loadBin(in);
	vector<double> x;
	x.reserve(N);
	REPORT(N);
	for (int i = 0; i < N; ++i) {
		x.push_back(loadBin(in));
	}
	return x;
}
