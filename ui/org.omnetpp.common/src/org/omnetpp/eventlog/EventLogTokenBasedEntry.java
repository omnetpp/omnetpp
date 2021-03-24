package org.omnetpp.eventlog;

import org.omnetpp.common.engine.BigDecimal;

/**
 * Base class for entries represented by key value tokens.
 */
public abstract class EventLogTokenBasedEntry extends EventLogEntry
{
    public static String qoute(String text) {
        if (text.indexOf('"') != -1)
            text = text.replaceAll("\"", "\\\"");
        if (text.indexOf(' ') == -1)
            return text;
        else
            return "\"" + text + "\"";
    }

    public static String getToken(String[] tokens, int numTokens, String sign, boolean mandatory) {
        for (int i = 1; i < numTokens; i += 2)
            if (sign.equals(tokens[i]))
                return tokens[i + 1];
        if (mandatory)
            throw new RuntimeException("Missing mandatory token " + sign + " in line: " + currentLine.substring(0, currentLineLength));
        return null;
    }

    public static boolean getBoolToken(String[] tokens, int numTokens, String sign, boolean mandatory, boolean defaultValue) {
        int value = getIntToken(tokens, numTokens, sign, mandatory, defaultValue ? 1 : 0);
        if (value == 0)
            return false;
        else if (value == 1)
            return true;
        else
            throw new RuntimeException("Invalid boolean " + value + " in line " + currentLine.substring(0, currentLineLength));
    }

    public static int getIntToken(String[] tokens, int numTokens, String sign, boolean mandatory, int defaultValue) {
        String token = getToken(tokens, numTokens, sign, mandatory);
        if (token != null)
            return Integer.valueOf(token);
        else
            return defaultValue;
    }

    public static short getShortToken(String[] tokens, int numTokens, String sign, boolean mandatory, short defaultValue) {
        String token = getToken(tokens, numTokens, sign, mandatory);
        if (token != null)
            return Short.valueOf(token);
        else
            return defaultValue;
    }

    public static long getLongToken(String[] tokens, int numTokens, String sign, boolean mandatory, long defaultValue) {
        String token = getToken(tokens, numTokens, sign, mandatory);

        if (token != null)
            return Long.valueOf(token);
        else
            return defaultValue;
    }

    public static long getInt64Token(String[] tokens, int numTokens, String sign, boolean mandatory, long defaultValue) {
        String token = getToken(tokens, numTokens, sign, mandatory);
        if (token != null)
            return Long.valueOf(token);
        else
            return defaultValue;
    }

    public static String getStringToken(String[] tokens, int numTokens, String sign, boolean mandatory, String defaultValue) {
        String token = getToken(tokens, numTokens, sign, mandatory);
        return token != null ? token : defaultValue;
    }

    public static long getEventNumberToken(String[] tokens, int numTokens, String sign, boolean mandatory, long defaultValue) {
        String token = getToken(tokens, numTokens, sign, mandatory);
        if (token != null)
            return Long.valueOf(token);
        else
            return defaultValue;
    }

    public static BigDecimal getSimtimeToken(String[] tokens, int numTokens, String sign, boolean mandatory, BigDecimal defaultValue) {
        String token = getToken(tokens, numTokens, sign, mandatory);
        if (token != null)
            return BigDecimal.parse(token);
        else
            return defaultValue;
    }

    public abstract void parse(String[] tokens, int numTokens);
}
