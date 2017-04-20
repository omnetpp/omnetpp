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

network FigureApiTest
{
    @display("bgb=800,500");
    submodules:

        timer: Timer {
            @display("p=50,50");
        }
        visible: Visible {
            @display("p=100,100");
        }
        addRemove: AddRemove {
            @display("p=200,100");
        }
        zIndex: ZIndex {
            @display("p=300,100");
        }
        tags: Tags {
            @display("p=400,100");
        }
        transform: Transform {
            @display("p=500,100");
        }
        misc: Misc {
            @display("p=600,100");
        }
        lineStyle: LineStyle {
            @display("p=100,300");
        }
        shapeStyle: ShapeStyle {
            @display("p=200,300");
        }
        text: Text {
            @display("p=100,200");
        }
        image: Image {
            @display("p=200,200");
        }
        imageContents: ImageContents {
            @display("p=300,200");
        }
        lineGeometry: LineGeometry {
            @display("p=100,400");
        }
        shapeGeometry: ShapeGeometry {
            @display("p=200,400");
        }
        path: Path {
            @display("p=400,200");
        }
}
