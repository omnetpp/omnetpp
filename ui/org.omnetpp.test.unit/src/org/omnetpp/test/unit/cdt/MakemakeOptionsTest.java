package org.omnetpp.test.unit.cdt;

import junit.framework.Assert;
import junit.framework.TestCase;

import org.junit.Test;
import org.omnetpp.cdt.makefile.MakemakeOptions;

public class MakemakeOptionsTest extends TestCase {
    private void checkArgs(String args) {
        checkArgs(args, args);
    }
    
    private void checkArgs(String args, String expectedArgs) {
        System.out.println("checking: " + args);
        MakemakeOptions obj = new MakemakeOptions(args);
        String resultArgs = obj.toString();
        Assert.assertEquals(expectedArgs, resultArgs);
    }
    
    @Test
    public void testOptions() {
        checkArgs("-f");
        checkArgs("--force", "-f");
        checkArgs("-e cc");
        checkArgs("--ext cc", "-e cc");
        checkArgs("-o myprog");
        checkArgs("--outputfile myprog", "-o myprog");
        checkArgs("-X somedir -X otherdir");
        checkArgs("--except somedir --except otherdir", "-X somedir -X otherdir");
        checkArgs("-Xsomedir -Xotherdir", "-X somedir -X otherdir");
        checkArgs("-b c:/basedir");
        checkArgs("--basedir c:/basedir", "-b c:/basedir");
        checkArgs("-c c:/config/configuser.vc71");
        checkArgs("--configfile c:/config/configuser.vc71", "-c c:/config/configuser.vc71");
        checkArgs("-n");
        checkArgs("--nolink", "-n");
        checkArgs("-d foo -d bar");
        checkArgs("-dfoo -dbar", "-d foo -d bar");
        checkArgs("--subdir foo --subdir bar", "-d foo -d bar");
        checkArgs("-s");
        checkArgs("--make-so", "-s");
        checkArgs("-t somelib.lib -t otherlib.lib");
        checkArgs("--importlib somelib.lib --importlib otherlib.lib", "-t somelib.lib -t otherlib.lib");
        checkArgs("-S");
        checkArgs("--fordll", "-S");
        checkArgs("-w");
        checkArgs("--withobjects", "-w");
        checkArgs("-x");
        checkArgs("--notstamp", "-x");
        checkArgs("-u CMDENV");
        checkArgs("-u Cmdenv", "-u CMDENV");
        checkArgs("-i makefrag2 -i makefrag3");
        checkArgs("--includefragment makefrag2 --includefragment makefrag3", "-i makefrag2 -i makefrag3");
        checkArgs("-Ic:/include -Ic:/include/foo");
        checkArgs("-I c:/include -I c:/include/foo", "-Ic:/include -Ic:/include/foo");
        checkArgs("-Lc:/lib -Lc:/lib/foo");
        checkArgs("-L c:/lib -L c:/lib/foo", "-Lc:/lib -Lc:/lib/foo");
        checkArgs("-lzip -lfoo");
        checkArgs("-PFOO");
        //TODO dirs and object files

    }
    
}
