case $- in
  *i*) ;;
  *) return ;;
esac

if [ "${OPP_NO_MSYS:-0}" != "1" ]; then
  if opp-win-shell --find-root >/dev/null; then
    exec opp-win-shell
  fi
  echo "windows-box: no OMNeT++ Windows tree found (cd into one or set OMNETPP_WIN_ROOT), then run 'opp-win-shell'."
fi
