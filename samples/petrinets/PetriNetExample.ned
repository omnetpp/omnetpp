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
// TODO auto-generated module
//
network PetriNetExample
{
    submodules:
        place: Place {
            @display("p=78,59");
        }
        transition: Transition {
            @display("p=198,98");
        }
        place1: Place {
            @display("p=78,162");
        }
        transition1: Transition {
            @display("p=394,193");
        }
        place2: Place {
            @display("p=320,116");
        }
        place3: Place {
            @display("p=91,267");
        }
        place4: Place {
            @display("p=477,193");
        }
    connections:
        place.out++ --> Arc --> transition.in++;
        place1.out++ --> Arc --> transition.in++;
        transition.out++ --> Arc --> place2.in++;
        place2.out++ --> Arc --> transition1.in++;
        place3.out++ --> Arc --> transition1.in++;
        transition1.out++ --> Arc --> place4.in++;
}
