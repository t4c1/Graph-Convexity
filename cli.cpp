#include "algo.h"
using namespace std;

int main(int argc, char* argv[]) {
	string fn;
	//defaults
	int repeats = 100;
	int maxSteps = -1;
	double cGlob = 1.0;
	double cLoc = 1.0;
	string raw;
	// parse args
	if (argc > 1) {
		fn = argv[1];
		if (argc > 2) {
			sscanf(argv[2], "%d", &repeats);
			cout << "repeats set to " << repeats << endl;
			if (argc > 3) {
				sscanf(argv[3], "%d", &maxSteps);
				cout << "maxSteps set to " << maxSteps << endl;
				if (argc > 4) {
					sscanf(argv[4], "%lf", &cGlob);
					cout << "global c set to " << cGlob << endl;
					if (argc > 5) {
						sscanf(argv[5], "%lf", &cLoc);
						cout << "local c set to " << cLoc << endl;
						if (argc > 6) {
							raw = argv[6];
							cout << "print_steps set to " << raw.c_str() << endl;
						}
					}
				}
			}
		}
	}
	else {
		cout << "First argument should be path to pajek file containing a network!" << endl;
		exit(0);
	}


	auto net = readPajek(fn);
	cout << endl << "Network loaded." << endl;
	net = reduceToLCC(net);
	cout << "Nodes in largest connected component: " << net.size() << endl;
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	auto dists = distances(net);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	cout << "Calculating distances took: " << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000. / 1000. << " s" << endl;

	begin = std::chrono::steady_clock::now();
	vector<vector<int>> growths(repeats);
	double glob=0, loc=0;
	#pragma omp parallel for
	for (int i = 0; i < repeats; i++) {
		growths[i] = convexGrowth(net, dists, maxSteps);
		#pragma omp critical
		{
		glob += cConvexity_Xc(growths[i], net.size(), cGlob);
		loc += maxConvexSubsetSize_Lc(growths[i], cLoc);
		}
	}
	end = std::chrono::steady_clock::now();
	cout << repeats <<" runs of algorithm  took: " << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000. / 1000. << " s" << endl << endl;

	cout << "Global convexity (c=" << cGlob << ") = " << glob / repeats << endl;
	cout << "Maximum size of convex subsets (c=" << cLoc << ") = " << loc / repeats << endl;

	cout.flush();
	if (raw[0]!='\0') {
		std::ofstream output(raw, std::ofstream::out);
		if (!output.is_open()) {
			perror(fn.c_str());
			exit(0);
		}
		for (auto inst : growths) {
			for (int g : inst) {
				output << g << ", ";
			}
			output << endl;
		}
		output.close();
	}
	return 0;
}