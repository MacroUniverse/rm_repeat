#include "SLISC/sha1sum.h"
#include "SLISC/search.h"
#include "SLISC/input.h"
#include "SLISC/disp.h"

// check repeated files and ask if delete
using namespace slisc;
Int main(Int argc, Char *argv[])
{
	Long N;
	if (argc <= 2)
		SLS_ERR("usage: rm_repeat <path1> <path2> ...");
	vecStr fnames, sha1s; // file names and sha1 sums
	VecLong sizes; // file size in bytes
	VecBool exist; // does file exist? (might be deleted)
	
	for (Long i = 1; i < argc; ++i)
		file_list_r(fnames, argv[i], true);

	N = fnames.size();
	sizes.resize(N); sha1s.resize(N); exist.resize(N);
	copy(exist, true);

	// recycle folder
	Str path_recyc = "./rm_repeat_recycle/";

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
				cout << "(" << i+1 << ", " << j+1 << ")/" << N << "  " << sha1s[i] << "   " << fnames[i] << endl;
			}
			if (sha1s[j].empty()) {
				sha1s[j] = sha1sum_f(fnames[j]);
				++sha1count;
				cout << "(" << i+1 << ", " << j+1 << ")/" << N << "  " << sha1s[j] << "   " << fnames[j] << endl;
			}
		}
	}
	cout << "\nchecked: " << sha1count << "/" << N << endl;
	
	// check repeated sha1 and choose (skip if sha1 is empty)
	Long Ndelete = 0;
	cout << "\nplease choose the files to move to \"./rm_repeat_recycle/\"..." << endl;
	cout << "===============================================" << endl;
	Str select, dest, buffer;
	vecStr ignor_sha1s, ignor_dirs;
	for (Long i = 0; i < N; ++i) {
		if (sha1s[i].empty() || !exist[i] || search(sha1s[i], ignor_sha1s) >= 0
				|| search_head(ignor_dirs, path2dir(fnames[i])) >= 0)
			continue;
		for (Long j = i+1; j < N; ++j) {
			if (sha1s[j] != sha1s[i] || search_head(ignor_dirs, path2dir(fnames[i])) >= 0)
				continue;
			cout << "\n" << i+1 << "/" << N << endl;
			cout << sha1s[i] + '\n' + fnames[i] + '\n' + fnames[j] << endl;
			cout << "[1/2/b(both)/i(ignore this sha1sum)/id=...(ignore dir)] or enter to skip: "; cout.flush();
			getline(cin, select);
			cout << "-----------------------------------------------" << endl;
			if (select == "1") {
				dest = path_recyc + fnames[i];
				ensure_dir(dest);
				file_move(dest, fnames[i]);
				exist[i] = false; ++Ndelete;
				break;
			}
			else if (select == "2") {
				dest = path_recyc + fnames[j];
				ensure_dir(dest);
				file_move(dest, fnames[j]);
				exist[j] = false; ++Ndelete;
			}
			else if (select == "b") {
				// 1
				dest = path_recyc + fnames[i];
				ensure_dir(dest);
				file_move(dest, fnames[i]);
				exist[i] = false; ++Ndelete;
				// 2
				dest = path_recyc + fnames[j];
				ensure_dir(dest);
				file_move(dest, fnames[j]);
				exist[j] = false; ++Ndelete;
				break;
			}
			else if (select == "s") {
				break;
			}
			else if (select == "i") {
				ignor_sha1s.push_back(sha1s[i]);
				break;
			}
			else if (select.substr(0,3) == "id=") {
				if (select.back() != '/')
					select += '/';
				ignor_dirs.push_back(select.substr(3));
			}
		}
	}
	cout << "\nmoved: " << Ndelete << endl;
	cout << "done!" << endl;
}
