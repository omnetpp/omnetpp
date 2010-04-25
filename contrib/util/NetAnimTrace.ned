//
// Records a NetAnim trace
//
simple NetAnimTrace
{
    parameters:
        string filename = default(expand("${resultdir}/${configname}-${runnumber}.tr"));
}
