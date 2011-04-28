<@setoutput path=srcFolder+"/Txc.ned"/>
${bannerComment}

<#if srcPackage!="">package ${srcPackage};</#if>

//
// Immediately sends out any message it receives. It can optionally generate
// a message at the beginning of the simulation, to bootstrap the process.
//
simple Txc
{
    parameters:
        bool sendInitialMessage = default(false);
    gates:
        input in;
        output out;
}


