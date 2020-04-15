package com.swtworkbench.community.xswt.dataparser.parsers;

import java.util.StringTokenizer;

import com.swtworkbench.community.xswt.dataparser.NonDisposableDataParser;

public class StringArrayDataParser extends NonDisposableDataParser {
    public Object parse(String source) {
        StringTokenizer stringTokenizer = new StringTokenizer(source, " \t\r\n");
        int tokens = stringTokenizer.countTokens();
        String[] tokenArray = new String[tokens];

        for (int i = 0; i < tokens; ++i) {
            tokenArray[i] = stringTokenizer.nextToken();
        }
        return tokenArray;
    }
}