#include "algo.h"
using namespace std;

int main(int argc, char* argv[]) {
	string fn= R"(C:\Users\tadej\PycharmProjects\INA\circles.net)";
	if (argc > 1) {
		fn = argv[1];
	}
	else {
		cout << "First argument should be path to pajek file containing a network!" << endl;
		exit(0);
	}

	/*if (argc > 2) {
		//TODO
		int n_threads;
	}*/

	auto g = readPajek(fn);
	cout << "Network loaded." << endl;
	g = reduceToLCC(g);
	cout << "Nodes of largest connected component: " << g.size() << endl;
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	auto dists = distances(g);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	cout << "Calculating distances took: " << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000. / 1000. << " s" << endl;
	begin = std::chrono::steady_clock::now();
	auto res = convexGrowth(g, dists);
	end = std::chrono::steady_clock::now();
	cout << "Algorithm took: " << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000. / 1000. << " s" << endl;
	return 0;
}