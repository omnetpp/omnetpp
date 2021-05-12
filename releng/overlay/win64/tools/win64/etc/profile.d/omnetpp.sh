# add extra path elements passed from the calling process
if [ -n "$__EXTRA_PATH" ]; then
  COMPILER_EXTRA_PATH=$(cygpath -u -p "$__EXTRA_PATH")
  PATH="$COMPILER_EXTRA_PATH:$PATH"
  unset COMPILER_EXTRA_PATH
fi