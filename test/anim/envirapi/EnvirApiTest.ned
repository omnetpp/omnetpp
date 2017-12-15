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

package envirapi;

network EnvirApiTest
{
    parameters:
        @display("bgb=500,400");
        @figure[explanation](type=text; pos=25,25; zIndex=2; text="Testing the following cEnvir functions:\nappendToImagePath, loadImage, getSubmodulePosition.\nYou should see five images next to each other:\nLena, the Cornell box, a baboon, the Utah teapot, and the Stanford bunny.\nThe tester module is here to ask the environment to load the images\nat runtime from C++ code, see the initialize() function of the Tester class.\nIt also adds a yellow rectangle which should always be tightly around\nthe submodule icon.\nThe zoom level displayed in the corresponding text figure should always\nmatch (in at least one inspector) the inspector's real zoom level.\nThe dummy module and the connection to it serves no other purpose\nthan to make the positions change when the network is relayouted.");
        @figure[lena](type=image; image="lena"; pos=50,300);
        @figure[box](type=image; image="box"; pos=150,300);
        @figure[baboon](type=image; image="baboon"; pos=250,300);
        @figure[teapot](type=image; image="cgi/teapot"; pos=350,300);
        @figure[bunny](type=image; image="3d/hare"; pos=450,300);
        @figure[submodules](zIndex=1);
    submodules:
        tester: Tester;
        dummy: Dummy;
    connections:
        tester.dummy <--> dummy.dummy;
}
