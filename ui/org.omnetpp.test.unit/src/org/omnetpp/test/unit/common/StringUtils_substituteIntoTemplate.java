package org.omnetpp.test.unit.common;

import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

import junit.framework.TestCase;

import org.junit.Test;
import org.omnetpp.common.util.StringUtils;

public class StringUtils_substituteIntoTemplate extends TestCase {
    private final String TEMPLATE1 = 
        "{false:}line 1 - {foo}\n" +
        "{true:}line 2 - {foo}\n" +
        "{~true:}line 3 - {bar}\n" +
        "{~false:}line 4 - {bar}\n" +
        "line 5 {foo}{bar}.\n" +
        "line 6 {true?some-text}{~true?other-text}.\n" +
        "line 7 {false?some-text}{~false?other-text}.\n" +
        "Numbers: {@numbers} '{numbers}', and {/numbers}...\n" +
        "Colors:{@colors}\n" +
        "  '{colors}' {@numbers} /{numbers}/{/numbers}{/colors}\n" +
        "done\n";
    private final String EXPECTED1 = 
        "line 2 - The-foo-value\n" +
        "line 4 - The-bar-value\n" +
        "line 5 The-foo-valueThe-bar-value.\n" +
        "line 6 some-text.\n" +
        "line 7 other-text.\n" +
        "Numbers:  'one', and  'two', and  'three', and  'four', and ...\n" +
        "Colors:\n" + 
        "  'red'  /one/ /two/ /three/ /four/\n" +
        "  'green'  /one/ /two/ /three/ /four/\n" +
        "  'blue'  /one/ /two/ /three/ /four/\n" + 
        "done\n";

    @Test
    public void testSubstituteIntoTemplate() {
        Map<String, Object> m = new HashMap<String, Object>(); 
        m.put("true", true);
        m.put("false", false);
        m.put("foo", "The-foo-value");
        m.put("bar", "The-bar-value");
        m.put("numbers", Arrays.asList("one two three four".split(" ")));
        m.put("colors", Arrays.asList("red green blue".split(" ")));
        
        check(TEMPLATE1, m, EXPECTED1);
        
    }
    
    private static void check(String template, Map<String, Object> map, String expectedOutput) {
        String actualOutput = StringUtils.substituteIntoTemplate(template, map);
        assertEquals("StringUtils.substituteIntoTemplate: wrong output for "+template, expectedOutput, actualOutput);
    }

}
