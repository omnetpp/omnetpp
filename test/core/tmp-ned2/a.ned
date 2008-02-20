// gatesize that depends on parameters
module Sub 
{
  parameters:
    int n = default(5);
  gates:
    output out[n];
    input in[n];
  connections allowunconnected:
}

network AAA
{
  submodules:
    s: Sub;
  connections allowunconnected:
}
 