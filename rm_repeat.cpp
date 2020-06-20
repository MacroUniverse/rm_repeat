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
	vecStr fnames, sha1s; // file names and sha1 sums
	VecLong sizes; // file size in bytes
	VecBool exist; // does file exist? (might be deleted)
	
	file_list_r(fnames, path);
	N = fnames.size();
	sizes.resize(N); sha1s.resize(N); exist.resize(N);
	copy(exist, true);

	// recycle folder
	Str path_recyc = "./recycle/";

	// file size
	cout << "getting file size...\n=================================" << endl;
	for (Long i = 0; i < N; ++i) {
		sizes[i] = file_size(fnames[i]);
		cout << i + 1 << "/" << N << "  "
			<< (Float)sizes[i] << " bytes   " << fnames[i] << endl;
	}

	// checksum only for non-empty files with the the same sizes
	Long sha1count = 0;
	cout << "\n\nchecksum...\n==============================================================" << endl;
	for (Long i = 0; i < N; ++i) {
		if (sizes[i] == 0)
			continue;
		for (Long j = i+1; j < N; ++j) {
			if (sizes[j] != sizes[i])
				continue;
			if (sha1s[i].empty()) {
				sha1s[i] = sha1sum_f(fnames[i]);
				++sha1count;
				cout << sha1count << "/" << N << "  " << sha1s[i] << "   " << fnames[i] << endl;
			}
			if (sha1s[j].empty()) {
				sha1s[j] = sha1sum_f(fnames[j]);
				++sha1count;
				cout << sha1count << "/" << N << "  " << sha1s[i] << "   " << fnames[i] << endl;
			}
		}
	}
	
	// check repeated sha1 and choose (skip if sha1 is empty)
	cout << "\nplease choose...\n===============================================" << endl;
	Str select, dest, buffer;
	for (Long i = 0; i < N; ++i) {
		if (sha1s[i].empty() || !exist[i])
			continue;
		for (Long j = i+1; j < N; ++j) {
			if (sha1s[j] != sha1s[i])
				continue;
			cout << "\n" << i+1 << "/" << N << endl;
			cout << sha1s[i] + '\n' + fnames[i] + '\n' + fnames[j] << endl;
			cout << "which one to delete? [1/2/12] or enter to skip: "; cout.flush();
			getline(cin, select);
			cout << "-----------------------------------------------" << endl;
			if (select == "1" || select == "12") {
				dest = path_recyc + fnames[i];
				ensure_dir(dest);
				file_move(dest, fnames[i]);
				exist[i] = false;
			}
			if (select == "2" || select == "12") {
				dest = path_recyc + fnames[j];
				ensure_dir(dest);
				file_move(dest, fnames[j]);
				exist[j] = false;
			}
			if (select == "1" || select == "12")
				break;
		}
	}
	cout << "\ndone!" << endl;
}
