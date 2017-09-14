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

simple Animator
{
    parameters:
        @display("i=block/cogwheel");

        // figure prototypes
        @figure[line](type=line;points=0,0,5,0;lineWidth=5;lineColor=red);
        @figure[rect](type=rectangle;size=5,5;lineColor=; fillColor=red);
        @figure[bigrect](type=rectangle;size=20,20;lineColor=; fillColor=red);
        @figure[img](type=image;image=misc/node; interpolation=none);
        @figure[img_vs](type=image;image=misc/node_vs; interpolation=none);
        @figure[icon](type=icon;image=misc/node);
        @figure[icon_vs](type=icon;image=misc/node_vs);

        int population;
        string figurePrototype = default("img"); // "line", "rect", "bigrect", "img", etc.
}
