#include "SLISC/sha1sum.h"
#include "SLISC/search.h"

// check repeated files and ask if delete
using namespace slisc;
Int main(Int argc, Char *argv[])
{
	Long N = argc - 1;
	vecStr fnames; fnames.resize(N);
	vecStr sha1s; sha1s.resize(N);
	// ofstream fout("sha1sum.txt");
	// if (!fout.good())
	// 	SLS_ERR("cannot create sha1sum.txt");
	cout << "checksum..." << endl;
	for (Int i = 0; i < N; ++i) {
		fnames[i] = argv[i + 1];
		sha1s[i] = sha1sum_f(fnames[i]);
		cout << i + 1 << "/" << N << "  " << sha1s[i] << "   " << fnames[i] << endl;
		cout << "done!" << endl;
	}

	while(true) {
		Long ind1, ind2;
		ind1 = find_repeat(ind2, sha1s, ind1 + 1);
		if (ind1 < 0)
			break;
		if (fnames[ind1].empty())
			continue;
		cout << "identical files:" << endl;
		cout << sha1s[ind1] << "   " << fnames[ind1] << endl;
		cout << sha1s[ind2] << "   " << fnames[ind2] << endl;
		cout << "which one to delete? [1/2] or enter to skip:";
		Int select; cin >> select;
		if (select == 1) {
			file_remove(fnames[ind1]); fnames[ind1].clear();
		}
		else if (select == 2) {
			file_remove(fnames[ind2]); fnames[ind1].clear();
		}
	}
}
