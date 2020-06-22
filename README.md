## rm_repeat
remove repeated files
* usage: `rm_repeat <dir>`
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
* `ad=<dir>` will auto delete files in this directory is there is an identical one outsize this directory (this is useful for cleaning two folders with different versions)
