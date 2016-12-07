//
// Exports the network topology in XML.
//
simple TopologyExporter
{
    parameters:
        double t @unit(s) = default(1s);  // simulation time to export at
        string filename = default("out.xml");
}
