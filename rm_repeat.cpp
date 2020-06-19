#include "SLISC/sha1sum.h"
#include "SLISC/search.h"
#include "SLISC/input.h"
#include "SLISC/disp.h"

// check repeated files and ask if delete
using namespace slisc;
Int main(Int argc, Char *argv[])
{
	Long N;
	if (argc != 2)
		SLS_ERR("usage: rm_repeat <path>");
	Str path(argv[1]);
	vecStr fnames, sha1s;
	
	file_list_r(fnames, path);
	N = fnames.size(); sha1s.resize(N);

	// recycle folder
	Str path_recyc = "./recycle/";

	cout << "checksum...\n" << endl;
	for (Long i = 0; i < N; ++i) {
		sha1s[i] = sha1sum_f(fnames[i]);
		cout << i + 1 << "/" << N << "  " << sha1s[i] << "   " << fnames[i] << endl;
	}
	cout << "\ndone!" << endl;

	Long ind1 = -1, ind2 = -1;
	Str select, dest, buffer;
	buffer.resize(20*1024*1024);
	while(true) {
		ind1 = find_repeat(ind2, sha1s, ind1 + 1);
		if (ind1 < 0)
			break;
		if (sha1s[ind1].empty())
			continue;
		cout << "\n------------------------------------" << endl;
		cout << sha1s[ind1] << "   " << fnames[ind1] << endl;
		cout << sha1s[ind2] << "   " << fnames[ind2] << endl;
		cout << "------------------------------------" << endl;
		cout << "which one to delete? [1/2] or enter to skip: ";
		getline(cin, select);
		if (select == "1") {
			dest = path_recyc + fnames[ind1];
			ensure_dir(dest);
			file_move(dest, fnames[ind1], buffer);
			sha1s[ind1].clear();
		}
		else if (select == "2") {
			dest = path_recyc + fnames[ind2];
			ensure_dir(dest);
			file_move(dest, fnames[ind2], buffer);
			sha1s[ind2].clear();
		}
	}
	cout << "\ndone!" << endl;
}
