%description:
Gatesize that depends on parameters

%file: {}.ned

module {Submod}
{
  parameters:
    int n = default(2);
  gates:
    output out[n];
    input in[n];
  connections allowunconnected:
}

network {network}
{
  submodules:
    dump: Dump;
    s: {Submod};
  connections allowunconnected:
}

%inifile: {}.ini
[General]
network={network}

[Parameters]
**.n = 5

%exitcode: 0

%contains: stdout
module NED_gatesize_1_network
module NED_gatesize_1_network.s
  P n = 5
  G  0 out[0]: not connected
  G  1 out[1]: not connected
  G  2 out[2]: not connected
  G  3 out[3]: not connected
  G  4 out[4]: not connected
  G  5 in[0]: not connected
  G  6 in[1]: not connected
  G  7 in[2]: not connected
  G  8 in[3]: not connected
  G  9 in[4]: not connected
done
