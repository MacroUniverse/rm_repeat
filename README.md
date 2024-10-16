## rm_repeat
remove repeated files in folders recursively

supports Linux, MacOS, MSYS2, MinGW-64

alternatives:
* `fdupes`
* `fslint`
* `rdfind`
* `duff`
* `jdupes`, 

* usage: `rm_repeat <dir1> <dir2> ...`
* the 1st loop checks file size for every file
* the 2nd loop checks sha1 hash for all files with non-unique sizes.
* the thrid lood will check identical sha1 hash, and ask interactively what to do

## Actions
* `1` will delete (move to folder `rm_repeat_recycle`) the first file
* `2` will delete the second file
* `b` will delete both files
* press enter to keep both files
* `s` to skip the first file
* `i` will ignore all files with the current sha1 hash
* `id=<dir>` will ignore `<dir>` directory (including it's subdirectories), just copy and past part of the directory from the console output. No space allowed after `id=`, trailing `/` is optional.
* `ad=<dir>` will auto delete repeated files in this directory, and its subdirectories.
