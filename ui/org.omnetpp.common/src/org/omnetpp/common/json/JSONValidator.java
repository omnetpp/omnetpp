package org.omnetpp.common.json;

import java.text.CharacterIterator;
import java.text.StringCharacterIterator;

/**
 * StringTree JSON, see http://www.stringtree.org/stringtree-json.html
 */
public class JSONValidator {

    private JSONErrorListener listener;
    private CharacterIterator it;
    private char c;
    private int col;
    
    public JSONValidator(JSONErrorListener listener) {
        this.listener = listener;
    }
    
    public boolean validate(String input) {
        input = input.trim();
        listener.start(input);
        boolean ret = valid(input);
        listener.end();
        return ret;
    }

    private boolean valid(String input) {
        if ("".equals(input)) return true;
        
        boolean ret = true;
        it = new StringCharacterIterator(input);
        c = it.first();
        col = 1;
        if (!value()) {
            ret = error("value", 1);
        } else {
            skipWhiteSpace();
            if (c != CharacterIterator.DONE) {
                ret = error("end", col);
            }
        }
        
        return ret;
    }

    private boolean value() {
        return 
            literal("true") ||
            literal("false") ||
            literal("null") ||
            string() ||
            number() ||
            object() ||
            array();
    }

    private boolean literal(String text) {
        CharacterIterator ci = new StringCharacterIterator(text);
        char t = ci.first();
        if (c != t) return false;
        
        int start = col;
        boolean ret = true;
        for (t = ci.next(); t != CharacterIterator.DONE; t = ci.next()) {
            if (t != nextCharacter()) {
                ret = false;
                break;
            }
        }
        nextCharacter();

        if (!ret) error("literal " + text, start);
        return ret;
    }

    private boolean array() {
        return aggregate('[', ']', false);
    }

    private boolean object() {
        return aggregate('{', '}', true);
    }

    private boolean aggregate(char entryCharacter, char exitCharacter, boolean prefix) {
        if (c != entryCharacter) return false;
        nextCharacter();
        skipWhiteSpace();
        if (c == exitCharacter) {
            nextCharacter();
            return true;
        }
        
        for(;;) {
            if (prefix) {
                int start = col;
                if (!string()) return error("string", start);
                skipWhiteSpace();
                if (c != ':') return error("colon", col);
                nextCharacter();
                skipWhiteSpace();
            }
            if (value()) {
                skipWhiteSpace();
                if (c == ',') {
                    nextCharacter();
                } else if (c == exitCharacter) {
                    break;
                } else { 
                    return error("comma or " + exitCharacter, col);
                }
            } else {
                return error("value", col);
            }
            skipWhiteSpace();
        }
        
        nextCharacter();
        return true;
    }

    private boolean number() {
        if (!Character.isDigit(c) && c != '-') return false;
        int start = col;

        if (c == '-') nextCharacter();

        if (c == '0') {
            nextCharacter();
        } else if (Character.isDigit(c)) {
            while(Character.isDigit(c)) nextCharacter();
        } else {
            return error("number", start);
        }

        if (c == '.') {
            nextCharacter();
            if (Character.isDigit(c)) {
                while(Character.isDigit(c)) nextCharacter();
            } else {
                return error("number", start);
            }        
        }

        if (c == 'e' || c == 'E') {
            nextCharacter();
            if (c == '+' || c=='-') {
                nextCharacter();
            }
            if (Character.isDigit(c)) {
                while(Character.isDigit(c)) nextCharacter();
            } else {
                return error("number", start);
            }        
        }

        return true;
    }

    private boolean string() {
        if (c != '"') return false;
        
        int start = col;
        boolean escaped = false;

        for (nextCharacter(); c != CharacterIterator.DONE; nextCharacter()) {
            if (!escaped && c == '\\') {
                escaped = true;
            } else if (escaped) {
                if (!escape()) {
                    return false;
                }
                escaped = false;
            } else if (c == '"') {
                nextCharacter();
                return true;
            }
        }

        return error("quoted string", start);
    }

    private boolean escape() {
        int start = col-1;
        if ("\\\"/bfnrtu".indexOf(c) < 0) {
            return error("escape sequence \\\",\\\\,\\/,\\b,\\f,\\n,\\r,\\t or \\uxxxx", start);
        } 
        if (c == 'u') {
            if (!ishex(nextCharacter()) || !ishex(nextCharacter()) || 
                    !ishex(nextCharacter()) || !ishex(nextCharacter())) {
                return error("unicode escape sequence \\uxxxx", start);
            }
        }
        return true;
    }

    private boolean ishex(char d) {
        return "0123456789abcdefABCDEF".indexOf(c) >= 0;
    }

    private char nextCharacter() {
        c = it.next();
        ++col;
        return c;
    }

    private void skipWhiteSpace() {
        while (Character.isWhitespace(c)) {
            nextCharacter();
        }
    }

    private boolean error(String type, int col) {
        if (listener != null) listener.error(type, col);
        return false;
    }
}
