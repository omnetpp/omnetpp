/*
 * Copyright (c) 2003 The Visigoth Software Society. All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution, if
 *    any, must include the following acknowledgement:
 *       "This product includes software developed by the
 *        Visigoth Software Society (http://www.visigoths.org/)."
 *    Alternately, this acknowledgement may appear in the software 
 *    itself, if and wherever such third-party acknowledgements 
 *    normally appear.
 *
 * 4. Neither the name "FreeMarker", "Visigoth", nor any of the names 
 *    of the project contributors may be used to endorse or promote 
 *    products derived from this software without prior written 
 *    permission. For written permission, please contact 
 *    visigoths@visigoths.org.
 *
 * 5. Products derived from this software may not be called 
 *    "FreeMarker" or "Visigoth" nor may "FreeMarker" or "Visigoth" 
 *    appear in their names without prior written permission of the 
 *    Visigoth Software Society.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE VISIGOTH SOFTWARE SOCIETY OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Visigoth Software Society. For more
 * information on the Visigoth Software Society, please see
 * http://www.visigoths.org/
 */

package freemarker.eclipse.editors;

import org.eclipse.jface.text.rules.ICharacterScanner;

/**
 * A class containing utility methods. In particular, this class 
 * contains metods for performing the lexical analysis needed for 
 * correct syntax highlighting.  
 *
 * @author  <a href="mailto:stephan@chaquotay.net">Stephan Mueller</a>
 * @author  <a href="mailto:per&#64;percederberg.net">Per Cederberg</a>
 * @version $Id: FreemarkerTools.java,v 1.3 2004/03/03 21:55:43 stephanmueller Exp $
 */
public class FreemarkerTools {

    /**
     * The FreeMarker comment start character sequence.
     */
    private static final char[] COMMENT_START = "<#--".toCharArray(); 

    /**
     * The FreeMarker comment end character sequence.
     */
    private static final char[] COMMENT_END = "-->".toCharArray(); 

    /**
     * The XML comment start character sequence.
     */
    private static final char[] XML_COMMENT_START = "<!--".toCharArray(); 

    /**
     * The XML comment end character sequence.
     */
    private static final char[] XML_COMMENT_END = "-->".toCharArray(); 

    /**
     * Checks for a FreeMarker comment start. If a matching character
     * sequence was detected, it will be read. If no match was found
     * starting at the first character returned by the scanner, false
     * will be returned and any characters read will be unread 
     * (leaving the scanner in the original state).
     * 
     * @param scanner        the character scanner
     * 
     * @return true if a comment start was read, or
     *         false otherwise
     */
    public static boolean readCommentStart(ICharacterScanner scanner) {
        return readSequence(scanner, COMMENT_START);
    }
    
    /**
     * Reads characters until a FreeMarker comment end is found.
     * 
     * @param scanner        the character scanner
     */
    public static void readCommentEnd(ICharacterScanner scanner) {
        int  c;

        do {
            c = scanner.read();
            if (c == '-') {
                scanner.unread();
                if (readSequence(scanner, COMMENT_END)) {
                    break;
                } else {
                    c = scanner.read();
                }
            }
        } while (c != ICharacterScanner.EOF);
    }

    /**
     * Checks for a FreeMarker directive start. If a matching 
     * character sequence was detected, it will be read. If no match 
     * was found starting at the first character returned by the 
     * scanner, false will be returned and any characters read will 
     * be unread (leaving the scanner in the original state).
     * 
     * @param scanner        the character scanner
     * 
     * @return true if a directive start was read, or
     *         false otherwise
     */
    public static boolean readDirectiveStart(ICharacterScanner scanner) {
        int  c;

        c = scanner.read();
        if (c != '<') {
            scanner.unread();
            return false;
        }
        c = scanner.read();
        if (c != '#' && c != '@' && c != '/') {
            scanner.unread();
            scanner.unread();
            return false;
        }
        if (c == '/') {
            c = scanner.read();
            if (c != '#' && c != '@') {
                scanner.unread();
                scanner.unread();
                scanner.unread();
                return false;
            }
        }
        return true;
    }

    /**
     * Reads characters until a FreeMarker directive end is found. 
     * This method attempts to detect and handle '>' characters used 
     * inside parenthesized expressions, as well as quoted strings 
     * and FreeMarker comments.
     * 
     * @param scanner        the character scanner
     */
    public static void readDirectiveEnd(ICharacterScanner scanner) {
        int  parens = 0;
        int  c;

        do {
            c = scanner.read();
            if (c == '<') {
                scanner.unread();
                if (readCommentStart(scanner)) {
                    readCommentEnd(scanner);
                } else {
                    c = scanner.read();
                }
            } else if (c == '"') {
                readQuoteEnd(scanner);
            } else if (c == '(') {
                parens++;
            } else if (c == ')') {
                parens--;
            }
        } while (c != ICharacterScanner.EOF 
              && (c != '>' || parens > 0));
    }

    /**
     * Checks for a FreeMarker interpolation start. If a matching 
     * character sequence was detected, it will be read. If no match 
     * was found starting at the first character returned by the 
     * scanner, false will be returned and any characters read will 
     * be unread (leaving the scanner in the original state).
     * 
     * @param scanner        the character scanner
     * 
     * @return true if an interpolation start was read, or
     *         false otherwise
     */
    public static boolean readInterpolationStart(ICharacterScanner scanner) {
        int  c;

        c = scanner.read();
        if (c != '$' && c != '#') {
            scanner.unread();
            return false;
        }
        c = scanner.read();
        if (c != '{') {
            scanner.unread();
            scanner.unread();
            return false;
        }
        return true;
    }

    /**
     * Reads characters until a FreeMarker comment end is found. This 
     * method attempts to detect and handle quoted strings and hash
     * literals inside the interpolation. 
     * 
     * @param scanner        the character scanner
     */
    public static void readInterpolationEnd(ICharacterScanner scanner) {
        int  c;

        do {
        	c = scanner.read();
            if (c == '"') {
                readQuoteEnd(scanner);
            } else if (c == '{') {
            	readHashEnd(scanner);
            }
        } while (c != ICharacterScanner.EOF && c != '}');
    }

    /**
     * Checks for a literal hash start. If a matching character
     * sequence was detected, it will be read. If no match was found
     * starting at the first character returned by the scanner, false
     * will be returned and any characters read will be unread 
     * (leaving the scanner in the original state).
     * 
     * @param scanner        the character scanner
     * 
     * @return true if a literal hash start was read, or
     *         false otherwise
     */
    public static boolean readHashStart(ICharacterScanner scanner) {
    	if (scanner.read() != '{') {
    		scanner.unread();
    		return false;
    	} else {
    		return true;
    	}
    }
    
    /**
	 * Reads characters until a literal hash end is found.
     * 
     * @param scanner        the character scanner
	 */
	public static void readHashEnd(ICharacterScanner scanner) {
		int  c;

		do {
			c = scanner.read();
			if (c == '"') {
				readQuoteEnd(scanner);
			} else if (c == '{') {
				readHashEnd(scanner);
			}
			
		} while (c != ICharacterScanner.EOF && c != '}');
	}

	/**
     * Checks for a quoted string start. If a matching character
     * sequence was detected, it will be read. If no match was found
     * starting at the first character returned by the scanner, false
     * will be returned and any characters read will be unread 
     * (leaving the scanner in the original state).
     * 
     * @param scanner        the character scanner
     * 
     * @return true if a quoted string start was read, or
     *         false otherwise
     */
    public static boolean readQuoteStart(ICharacterScanner scanner) {
        if (scanner.read() != '"') {
            scanner.unread();
            return false;
        } else {
            return true;
        }
    }

    /**
     * Reads characters until a quote end is found. This method 
     * attempts to detect and handle character escapes inside the
     * quoted string. 
     * 
     * @param scanner        the character scanner
     */
    public static void readQuoteEnd(ICharacterScanner scanner) {
        int  c;

        do {
            c = scanner.read();
            if (c == '\\') {
                scanner.read();
            }
        } while (c != ICharacterScanner.EOF && c != '"');
    }

    /**
     * Checks for an XML comment start. If a matching character
     * sequence was detected, it will be read. If no match was found
     * starting at the first character returned by the scanner, false
     * will be returned and any characters read will be unread
     * (leaving the scanner in the original state).
     *
     * @param scanner        the character scanner
     *
     * @return true if an XML comment start was read, or
     *         false otherwise
     */
    public static boolean readXmlCommentStart(ICharacterScanner scanner) {
        return readSequence(scanner, XML_COMMENT_START);
    }
    
    /**
     * Reads characters until an XML comment end is found. This
     * method attempts to detect and handle FreeMarker comments,
     * directives and interpolations inside the XML comment. 
     *
     * @param scanner        the character scanner
     */
    public static void readXmlCommentEnd(ICharacterScanner scanner) {
        int  c;

        do {
            c = scanner.read();
            if (c == '$' || c == '#') {
                scanner.unread();
                if (readInterpolationStart(scanner)) {
                    readInterpolationEnd(scanner);
                } else {
                    c = scanner.read();
                }
            } else if (c == '<') {
                scanner.unread();
                if (readCommentStart(scanner)) {
                    readCommentEnd(scanner);
                } else if (readDirectiveStart(scanner)) {
                    readDirectiveEnd(scanner);
                } else {
                    c = scanner.read();
                }
            } else if (c == '-') {
                scanner.unread();
                if (readSequence(scanner, XML_COMMENT_END)) {
                    break;
                } else {
                    c = scanner.read();
                }
            }
        } while (c != ICharacterScanner.EOF);
    }

    /**
     * Checks for an XML tag start. If a matching character sequence 
     * was detected, it will be read. If no match was found starting 
     * at the first character returned by the scanner, false will be 
     * returned and any characters read will be unread (leaving the 
     * scanner in the original state).
     * 
     * @param scanner        the character scanner
     * 
     * @return true if an XML tag start was read, or
     *         false otherwise
     */
    public static boolean readXmlTagStart(ICharacterScanner scanner) {
        if (scanner.read() != '<') {
            scanner.unread();
            return false;
        } else {
            return true;
        }
    }

    /**
     * Reads characters until an XML tag end is found. This method 
     * attempts to detect and handle FreeMarker comments, directives
     * and interpolations inside the tag. 
     * 
     * @param scanner        the character scanner
     */
    public static void readXmlTagEnd(ICharacterScanner scanner) {
        int  c;
        
        do {
            c = scanner.read();
            if (c == '$' || c == '#') {
                scanner.unread();
                if (readInterpolationStart(scanner)) {
                    readInterpolationEnd(scanner);
                } else {
                    c = scanner.read();
                }
            } else if (c == '<') {
                scanner.unread();
                if (readCommentStart(scanner)) {
                    readCommentEnd(scanner);
                } else if (readDirectiveStart(scanner)) {
                    readDirectiveEnd(scanner);
                } else {
                    c = scanner.read();
                }
            }
        } while (c != ICharacterScanner.EOF && c != '>');
    }
    
    /**
     * Checks for a character sequence. If a matching sequence was 
     * detected, it will be read. If no match was found starting at 
     * the first character returned by the scanner, false will be 
     * returned and any characters read will be unread (leaving the 
     * scanner in the original state).
     * 
     * @param scanner        the character scanner
     * 
     * @return true if a character sequence was read, or
     *         false otherwise
     */
    private static boolean readSequence(ICharacterScanner scanner, 
                                        char[] chars) {

        int  pos = 0;
        int  c;
        
        do {
            c = scanner.read();
            if (chars[pos] != c) {
                while (pos-- >= 0) {
                    scanner.unread();
                }
                return false;
            }
            pos++;
        } while (pos < chars.length);

        return true;
    }
}
