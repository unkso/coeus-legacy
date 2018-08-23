#include "stdafx.h"
#include <cpr\cpr.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <functional>
#include <vector>
#include <exception>
#include <fstream>

using std::cout;
using std::cin;
using std::string;
using std::vector;

int main() {
	std::ifstream fin("file.txt");
	double aux;
	fin.ignore(500, '\n');
	fin.ignore(500, '\n');
	fin >> aux;
	cout << "Valor: " << aux << '\n';
	fin.close();
	return 0;
}