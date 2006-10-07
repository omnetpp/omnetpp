
# absolute dir
./abspath /some/dir

# relative dir
./abspath .
./abspath dir
./abspath dir/dir2

# path (arg2) absolute, basedir (arg1) ignored
./abspath /bla /some/dir
./abspath bla /some/dir

# path relative, basedir absolute
./abspath /base/foo dir
./abspath /base/foo some/dir

# path relative, basedir relative
./abspath base dir
./abspath base/foo some/dir

# path has no drive but absolute dir
./abspath base /dir
./abspath /base /dir

# path has no drive and relative dir
./abspath base dir
./abspath /base dir

