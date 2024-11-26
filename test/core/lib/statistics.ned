package testlib;

simple Statistics extends SignalSource
{
    @isNetwork(true);

    @signal[foo](type="double");
    @signal[bar](type="double");
    @statistic[foo](record=last,mean,stats,vector);
    @statistic[bar](record=last,mean,stats,vector);

    script = xml("<root>\
       <e at='1s' name='foo' type='double' value='.41'/> \
       <e at='2s' name='foo' type='double' value='.46'/> \
       <e at='3s' name='foo' type='double' value='.38'/> \
       <e at='4s' name='foo' type='double' value='.42'/> \
       <e at='1.5s' name='bar' type='double' value='3.1'/> \
       <e at='2.5s' name='bar' type='double' value='2.6'/> \
       <e at='3.5s' name='bar' type='double' value='1.8'/> \
       <e at='4.5s' name='bar' type='double' value='5.8'/> \
    </root>");
}
