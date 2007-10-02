package org.omnetpp.test.unit.common;

import junit.framework.TestCase;

import org.junit.Test;

import org.omnetpp.common.util.StringUtils;

public class StringUtils_normalizeWhiteSpace extends TestCase {

    @Test
    public void testNormalizeWhiteSpace() {
        check("foo   bar     baz   ", "foo bar baz");
        check("\n  foo \n\n  bar     baz\n\n\n \n", "foo\nbar baz");
        check("\t  foo \n\t\n  bar \t    baz\n\n", "foo\nbar baz");
    }
    
    private static void check(String input, String expectedOutput) {
        String actualOutput = StringUtils.normalizeWhiteSpace(input);
        assertEquals("StringUtils.normalizeWhiteSpace: wrong output for "+input, expectedOutput, actualOutput);
    }

}
