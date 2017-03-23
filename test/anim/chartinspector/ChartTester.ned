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

network ChartTester
{
    @display("bgb=816,513");
    submodules:
        c1e20: ChartInspectorTester {
            @display("p=113,94");
            deltaT = 1s;
            value = uniform(1e20, 1.01e20);
        }
        c0_100: ChartInspectorTester {
            @display("p=319,94");
            deltaT = 1s;
            value = uniform(0, 100);
        }
        c100_1001: ChartInspectorTester {
            @display("p=492,94");
            deltaT = 1s;
            value = uniform(100, 100.01);
        }
}
