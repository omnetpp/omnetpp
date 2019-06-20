//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//


//
// Reads a PNML (Petri Net Markup Language) file, and sets up the Petri net
// under the compound module that contains this module.
//
simple PetriNetBuilder
{
    parameters:
        @display("i=block/cogwheel_s");
        xml pnmlFile;  // PNML file to read the network from
        string id = default(""); // id of the network in the file; if empty, the first network will be chosen
        string placeNedType = default("Place");
        string transitionNedType = default("Transition");
        string arcNedType = default("Arc");
        string transitionSchedulerModule = default("scheduler");
}
