//
// Test 10: channels II
//

channel na  endchannel

channel nb  delay 0  endchannel
channel nc  error 1e-5  endchannel
channel nd  datarate 9600  endchannel

channel ne  error 1e-5; datarate 9600; endchannel
channel nf  delay 0; datarate 9600; endchannel
channel ng  delay 0; error 1e-5; endchannel
channel nh  datarate 9600; error 1e-5; endchannel
channel ni  datarate 9600; delay 0; endchannel
channel nj  error 1e-5; delay 0; endchannel

channel nk  delay 0; error 1e-5; datarate 9600; endchannel
channel nl  delay 0; datarate 9600; error 1e-5; endchannel
channel nm  error 1e-5; delay 0; datarate 9600; endchannel
channel nn  error 1e-5; datarate 9600; delay 0; endchannel
channel no  datarate 9600; delay 0; error 1e-5; endchannel
channel np  datarate 9600; error 1e-5; delay 0; endchannel

//---------------------------------------------------------

simple SSSS
  gates: out: o;
endsimple

simple RRRR
  gates: in: i;
endsimple

module cccc
  submodules:
         s : SSSS[17];
         r : RRRR[17];
  connections:
         s[0].o --> nn --> r[0].i;
         s[0].o --> nn --> r[0].i;

         s[0].o --> r[0].i;
         s[0].o --> r[0].i;

         s[0].o -->  delay 0 --> r[0].i;
         s[0].o -->  error 1e-5 --> r[0].i;
         s[0].o -->  datarate 9600 --> r[0].i;

         s[0].o -->  error 1e-5 datarate 9600  --> r[0].i;
         s[0].o -->  delay 0 datarate 9600     --> r[0].i;
         s[0].o -->  delay 0 error 1e-5        --> r[0].i;
         s[0].o -->  datarate 9600 error 1e-5  --> r[0].i;
         s[0].o -->  datarate 9600 delay 0     --> r[0].i;
         s[0].o -->  error 1e-5 delay 0        --> r[0].i;

         s[0].o -->  delay 0 error 1e-5 datarate 9600  --> r[0].i;
         s[0].o -->  delay 0 datarate 9600 error 1e-5  --> r[0].i;
         s[0].o -->  error 1e-5 delay 0 datarate 9600  --> r[0].i;
         s[0].o -->  error 1e-5 datarate 9600 delay 0  --> r[0].i;
         s[0].o -->  datarate 9600 delay 0 error 1e-5  --> r[0].i;
         s[0].o -->  datarate 9600 error 1e-5 delay 0  --> r[0].i;

endmodule

