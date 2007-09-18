package org.omnetpp.test.unit.common;

import junit.framework.TestCase;

import org.junit.Test;

import org.omnetpp.common.util.StringUtils;

public class StringUtils_ToInstanceName extends TestCase {

    /**
     * Converts the provided CamelCase type-name to instance name (starting with lower case)
     * RULES: TCP -> tcp; _TCP -> _tcp; TCPModule -> tcpModule; TCP_module -> tcp_module
     * TCP1 -> tcp1; helloWorld -> helloWorld; CamelCase -> camelCase; TCP_SACK -> tcp_sack
     * TCP_Reno -> tcp_Reno
     */
    @Test
    public void testToInstanceName() {
        check("TCP", "tcp");
        check("_TCP", "_tcp");
        check("TCPModule", "tcpModule");
        check("TCP_module", "tcp_module");
        check("TCP1", "tcp1");
        check("helloWorld", "helloWorld");
        check("CamelCase", "camelCase");
        check("TCP_SACK", "tcp_sack");
        check("TCP_Reno", "tcp_Reno");
    }
    
    private static void check(String input, String expectedOutput) {
        String actualOutput = StringUtils.toInstanceName(input);
        assertEquals("StringUtils.tInstanceName: wrong output for "+input, expectedOutput, actualOutput);
    }

}
