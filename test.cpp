#include "algo.h"
using namespace std;

int main() {
	//string fn = R"(C:\Users\tadej\PycharmProjects\INA\circles.net)";
	string fn = R"(F:\Users\Tadej\Documents\fax_dn\INA\circles.net)";
	auto g = readPajek(fn);
	g = reduceToLCC(g);
	cout << g.size() << endl;
	int m1 = 0;
	for (auto i : g) {
		m1 += i.size();
	}
	m1 /= 2;
	cout << "m " << m1 << endl;
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	auto dists = distances(g);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	cout << "dists " << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000. / 1000. << " s" << endl;
	begin = std::chrono::steady_clock::now();
	auto res = convexGrowth(g, dists);
	end = std::chrono::steady_clock::now();
	cout << "algo " << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000. / 1000. << " s" << endl;
	(cout << (std::accumulate(res.begin(), res.end(), 0)) << " " << g.size()) << endl;
	assert(std::accumulate(res.begin(), res.end(), 0) == g.size()); //sanity check - holds only for connected graphs
	return 0;
}