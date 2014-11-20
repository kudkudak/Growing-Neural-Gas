#include "Utils.h"

void __init_rnd() {
	srand(time(NULL));
}

int __rnd(int min, int max) {
	return (rand() % (max - min + 1) + min);
}

int __int_rnd(int min, int max) {
	return (rand() % (max - min + 1) + min);
}

double __double_rnd(double min, double max) {
	return min + (max - min) * ((double) rand()) / RAND_MAX;
}
void _write_bin(ostream & out, double v) {
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

void _write_bin_vect(ostream & out, vector<double> & v) {
	_write_bin(out, (double) v.size());
	for (int i = 0; i < v.size(); ++i) {
		_write_bin(out, v[i]);
	}
}
double _load_bin(istream & in) {
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

vector<double> _load_bin_vector(istream & in) {
	int N = (int) _load_bin(in);
	vector<double> x;
	x.reserve(N);
	cout << "Size " << N << endl;
	for (int i = 0; i < N; ++i) {
		x.push_back(_load_bin(in));
	}
	return x;
}
