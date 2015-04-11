/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.unimod.generator.cpp.util;

import org.apache.commons.lang.StringUtils;

public abstract class BaseTool {
    /* Identifier manipulation methods */
    public String getClassName(String prefix, String name, String suffix) {
        return createIdentifier(prefix, name, suffix, true, false);
    }

    /**
     * Turns given name to valid C++ identifier.
     * <br>
     * States:
     * 0 - first word letter
     * 1 - first element letter
     * 2 - common letter
     * Skip separators and symbols other then [a-zA-Z0-9]
     * Identify words starting with uppercase letter
     * Identify words after digit sequence
     * Prepend 'a' when name starts with digit and there is no prefix
     * @param allUpper TODO
     */
    public String createIdentifier(String prefix, String name, String suffix, boolean isFirstUpper, boolean allUpper) {
        int state = 0;
        StringBuffer identifier = new StringBuffer(prefix);
        for (int i = 0; i < name.length(); i++) {
            char c = name.charAt(i);
            switch (state) {
                case 0: // first word letter
                    if (isIdentSeparator(c)) {
                        // skip
                    } else if (Character.isUpperCase(c)) {
                        identifier.append(isFirstUpper || allUpper ? c : Character.toLowerCase(c));
                        state = 2;
                    } else if (Character.isLowerCase(c)) {
                        identifier.append(isFirstUpper || allUpper ? Character.toUpperCase(c) : c);
                        state = 2;
                    } else if (isDigit(c)) {
                        if (StringUtils.isEmpty(suffix)) {
                            identifier.append(isFirstUpper || allUpper ? 'A' : 'a');
                        }
                        identifier.append(c);
                        state = 1;
                    }
                    break;
                case 1: // first element letter
                    if (isIdentSeparator(c)) {
                        // skip
                    } else if (Character.isUpperCase(c)) {
                        identifier.append(c);
                        state = 2;
                    } else if (Character.isLowerCase(c)) {
                        identifier.append(Character.toUpperCase(c));
                        state = 2;
                    } else if (isDigit(c)) {
                        identifier.append(c);
                    }
                    break;
                case 2: // common letter
                    if (isIdentSeparator(c)) {
                        // skip
                        state = 1;
                        if (allUpper) {
                            identifier.append('_');
                        }
                    } else if (Character.isUpperCase(c)) {
                        if (allUpper) {
                            identifier.append('_');
                        }
                        identifier.append(c);
                    } else if (Character.isLowerCase(c)) {
                        identifier.append(allUpper ? Character.toUpperCase(c) : c);
                    } else if (isDigit(c)) {
                        identifier.append(c);
                        state = 1;
                    }
                    break;
            }
        }
        identifier.append(suffix);
        return identifier.toString();
    }

    public boolean isIdentSeparator(char c) {
        return StringUtils.contains(" \n\t\r.,_-", c);
    }

    public boolean isDigit(char c) {
        return '0' <= c && c <= '9';
    }

    public String offset(int times) {
        return StringUtils.repeat(" ", times);
    }
}
