package org.omnetpp.test.unit.cdt;

import java.util.List;

import junit.framework.Assert;
import junit.framework.TestCase;

import org.junit.Test;
import org.omnetpp.cdt.makefile.MakefileTools;
import org.omnetpp.cdt.makefile.MakefileTools.Include;
import org.omnetpp.common.util.StringUtils;

public class CppToolsTest extends TestCase {

    @Test
    public void testParseIncludes() {
        String src = 
            "#include <one.h>\n" +
            "#include fake1.h\n" +
            "#include <fake2.h\n" +
            "// #include <fake3.h\n" +
            " \t # \t  include <two.h> \t \n" +
            " \t # \t  include <two.h> \t // something\n" +
            " \t # \t  include <three.h>//something\n" +
            "#include <sys/four.h>//something\n" +
            "#include <../sys/five.h>//something\n" +
            "#include \"six.h\"\n" +
            " \t # \t  include \"../seven.h\"//something\n" +
            "#include \"last.h\"  ";  // note unterminated last line (no \n) 
        List<Include> includes = MakefileTools.parseIncludes(src);
        String concat = StringUtils.join(includes, ",");
        Assert.assertEquals(concat, "<one.h>,<two.h>,<two.h>,<three.h>,<sys/four.h>,<../sys/five.h>,\"six.h\",\"../seven.h\",\"last.h\"");

    }
    
}
