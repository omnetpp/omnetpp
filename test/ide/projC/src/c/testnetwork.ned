//
// This program is property of its copyright holder. All rights reserved.
//

package projc.c;

import proja.a.af1.Af1sm;
import proja.a.af2.Af2sm;
import projb.b.bf1.Bf1sm;
import projb.b.bf2.Bf2sm;
import projb.simulations.bf3.Bf3sm;


network Testnetwork
{
    submodules:
        af1sm: Af1sm {
            @display("p=46,35");
        }
        af2sm: Af2sm {
            @display("p=110,35");
        }
        bf1sm: Bf1sm {
            @display("p=194,35");
        }
        bf2sm: Bf2sm {
            @display("p=265,35");
        }
        c1sm: C1sm {
            @display("p=361,35");
        }
}
