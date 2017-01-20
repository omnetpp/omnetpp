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

package animation2;

import ned.IdealChannel;


network Network2
{
    @display("bgb=500,700");
    @figure[title](type=text; pos=300,75; anchor=c; text="");
    @figure[operation](type=text; pos=25,150; anchor=start; text="");

    submodules:
        baker: SimpleSource {
            @display("p=100,100");
        }
        oven: Passthrough {
            @display("p=400,200");
        }
        warehouse: Passthrough {
            @display("p=100,300");
        }
        kitten: Sink {
            @display("p=400,600");
        }
        shop: Passthrough {
            @display("p=400,400");
        }
        fridge: Passthrough {
            @display("p=100,500");
        }
    connections:
        baker.out --> Ideal --> oven.in;
        oven.out --> DelayAndRate --> warehouse.in;
        warehouse.out --> Delay --> shop.in;
        shop.out --> Ideal --> fridge.in;
        fridge.out --> Delay --> kitten.in;
}
