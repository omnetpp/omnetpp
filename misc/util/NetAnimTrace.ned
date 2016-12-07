//
// Records a NetAnim trace, using network-level modules and their connections.
// See http://www.nsnam.org/wiki/index.php/NetAnim
// 
// Note: NetAnim does not have a built-in layouter, and requires explicit 
// node coordinates to be written into the file. This module takes node
// coordinates from module display strings; if there are no coordinates
// there, random coordinates will be generated. Thus, for good results 
// it is recommended that you assign explicit coordinates to all network 
// nodes.
//
// @author Andras
//
simple NetAnimTrace
{
    parameters:
        bool enabled = default(true);
        string filename = default(expand("${resultdir}/${configname}-${runnumber}.tr"));
        @display("i=block/control_s");
}
