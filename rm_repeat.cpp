#include "SLISC/sha1sum.h"
#include "SLISC/arithmetic.h"
#include "SLISC/search.h"
#include "SLISC/input.h"
#include "SLISC/disp.h"
#include "SLISC/matb.h"

// check repeated files and ask if delete
using namespace slisc;
Int main(Int argc, Char *argv[])
{
	Long N;
	if (argc < 2)
		SLS_ERR("usage: rm_repeat <path1> <path2> ...");
	vecStr fnames, sha1s; // file names and sha1 sums
	VecLong sizes; // file size in bytes
	VecBool exist; // does file exist? (might be deleted)
	Bool auto_skip = false; // auto skip (intended to be used with auto delete)
	Str path_recyc = "./rm_repeat_recycle/"; // recycle folder
	Bool resume = file_exist(path_recyc + "data.matb"); // if rm_repeat_recycle/data.matb exist, then load data from it.
	
	if (resume) {
		cout << "resuming from previous data..." << endl;
		load_matb(sizes, "sizes", path_recyc + "data.matb");
		read_vec_str(fnames, path_recyc + "fnames.txt");
		read_vec_str(sha1s, path_recyc + "sha1s.txt");
		if (size(sizes) != size(fnames) || size(fnames) != size(sha1s))
			SLS_ERR("cannot load data files, file might be corrupted! delete data files for a new run.");
		N = fnames.size();
		exist.resize(N); copy(exist, true);
	}
	else {
		for (Long i = 1; i < argc; ++i)
		file_list_r(fnames, argv[i], true);

		N = fnames.size();
		sizes.resize(N); sha1s.resize(N); exist.resize(N);
		copy(exist, true);

		// file size
		Bool badfile = false;
		cout << "getting file size...\n=================================" << endl;
		for (Long i = 0; i < N; ++i) {
			if (!file_exist(fnames[i])) {
				badfile = true; sizes[i] = -1; continue;
			}
			sizes[i] = file_size(fnames[i]);
			cout << std::setw(2*log10(N)+4) << std::left << num2str(i + 1) + "/" + num2str(N)
				<< std::setw(13) << std::right << num2str(sizes[i]) + "  "
				<< std::setw(3) << std::left << fnames[i] << endl;
		}

		// checksum only for non-empty files with the the same sizes
		Long sha1count = 0;
		cout << "\n\nchecksum...\n==============================================================" << endl;
		for (Long i = 0; i < N; ++i) {
			if (sizes[i] <= 0)
				continue;
			for (Long j = i+1; j < N; ++j) {
				if (sizes[j] != sizes[i])
					continue;
				if (sha1s[i].empty()) {
					sha1s[i] = sha1sum_f(fnames[i]);
					++sha1count;
					cout << std::setw(3*log10(N)+9) << std::left << "(" + num2str(i+1) + ", " + num2str(j+1) + ")/" + num2str(N)
						<< std::setw(43) << sha1s[i]
						<< std::setw(3) << fnames[i] << endl;
				}
				if (sha1s[j].empty()) {
					sha1s[j] = sha1sum_f(fnames[j]);
					++sha1count;
					cout << std::setw(3*log10(N)+9) << std::left << "(" + num2str(i+1) + ", " + num2str(j+1) + ")/" + num2str(N)
						<< std::setw(43) << sha1s[j]
						<< std::setw(3) << fnames[j] << endl;
				}
			}
		}
		cout << "\nchecked: " << sha1count << "/" << N << endl;
		
		if (badfile) {
			cout << "\nthe following files cannot be opened (ignored)" << endl;
			cout << "===============================================" << endl;
			for (Long i = 0; i < N; ++i) {
				if (sizes[i] <= 0)
					cout << "ignored (cannot open): " << fnames[i] << endl;
			}
		}

		// save data
		mkdir(path_recyc);
		save_matb(sizes, "sizes", path_recyc + "data.matb");
		write_vec_str(fnames, path_recyc + "fnames.txt");
		write_vec_str(sha1s, path_recyc + "sha1s.txt");
	}

	// check repeated sha1 and choose (skip if sha1 is empty)
	Long Ndelete = 0;
	cout << "\nplease choose the files to move to \"./rm_repeat_recycle/\"..." << endl;
	cout << "===============================================" << endl;
	Str select, dest, buffer;
	vecStr ignor_sha1s, ignor_dirs, auto_del_dirs;
	for (Long i = 0; i < N; ++i) {
		if (sha1s[i].empty() || !exist[i] || search(sha1s[i], ignor_sha1s) >= 0
				|| search_head(ignor_dirs, path2dir(fnames[i])) >= 0)
			continue;
		for (Long j = i+1; j < N; ++j) {
			if (sha1s[j] != sha1s[i] || search_head(ignor_dirs, path2dir(fnames[i])) >= 0)
				continue;
			
			// auto delete (only if one of them is in auto_del_dirs[])
			Bool del1 = search_head(auto_del_dirs, path2dir(fnames[i])) >= 0;
			Bool del2 = search_head(auto_del_dirs, path2dir(fnames[j])) >= 0;
			if (del1 && !del2) {
				cout << "auto delete: " << fnames[i] << endl;
				dest = path_recyc + fnames[i];
				ensure_dir(dest);
				if (file_exist(fnames[i]))
					file_move(dest, fnames[i]);
				exist[i] = false; ++Ndelete;
				break;
			}
			else if (del2) {
				cout << "auto delete: " << fnames[j] << endl;
				dest = path_recyc + fnames[j];
				ensure_dir(dest);
				if (file_exist(fnames[j]))
					file_move(dest, fnames[j]);
				exist[j] = false; ++Ndelete;
				continue;
			}

			// user action
			cout << "\n" << i+1 << "/" << N << endl;
			cout << sha1s[i] + '\n' + fnames[i] + '\n' + fnames[j] << endl;
			if (auto_skip)
				break;
			cout << "[1/2/b(both)/i(ignore this sha1sum)/s(skip 1st)/as(auto skip)/<enter> (skip 2nd)/id=...(ignore dir)/ad=...(auto delete dir)]: "; cout.flush();
			getline(cin, select);			
			cout << "-----------------------------------------------" << endl;
			if (select == "1") {
				dest = path_recyc + fnames[i];
				ensure_dir(dest);
				if (file_exist(fnames[i]))
					file_move(dest, fnames[i]);
				exist[i] = false; ++Ndelete;
				break;
			}
			else if (select == "2") {
				dest = path_recyc + fnames[j];
				ensure_dir(dest);
				if (file_exist(fnames[j]))
					file_move(dest, fnames[j]);
				exist[j] = false; ++Ndelete;
			}
			else if (select == "b") {
				// 1
				dest = path_recyc + fnames[i];
				ensure_dir(dest);
				if (file_exist(fnames[i]))
					file_move(dest, fnames[i]);
				exist[i] = false; ++Ndelete;
				// 2
				dest = path_recyc + fnames[j];
				ensure_dir(dest);
				if (file_exist(fnames[j]))
					file_move(dest, fnames[j]);
				exist[j] = false; ++Ndelete;
				break;
			}
			else if (select == "s") {
				break;
			}
			else if (select == "as") {
				if (auto_del_dirs.empty()) {
					cout << "please set auto-delete before using auto skip" << endl;
					--j;
				}
				auto_skip = true;
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
				cout << "ignored directories:" << endl;
				disp(ignor_dirs);
				--j;
			}
			else if (select.substr(0,3) == "ad=") {
				if (select.back() != '/')
					select += '/';
				auto_del_dirs.push_back(select.substr(3));
				cout << "auto delete directories:" << endl;
				disp(auto_del_dirs);
				--j;
			}
			else if (select.empty())
				continue;
			else {
				cout << "unknown option, please select again." << endl;
				--j;
			}
		}
	}
	cout << "\nmoved: " << Ndelete << endl;
	cout << "done!" << endl;
}
