# preprocessing for OMNeT++ build
export WHAT="OMNeT++"
export EDITION="Academic Public License -- NOT FOR COMMERCIAL USE"

# rewite the launcher files
$REGEXPREPLACE 's/OMNEST/OMNeT\+\+/g' src/utils/omnest.cmd src/utils/omnest
$REGEXPREPLACE 's/LAUNCHER=omnest/LAUNCHER=omnetpp/g' src/utils/omnest.cmd src/utils/omnest
