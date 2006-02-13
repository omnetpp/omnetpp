package org.omnetpp.ned.editor.graph.properties;

import java.util.*;

public class DisplayStringParser {

    // map that stores the currently available tags 
    private LinkedHashMap<String, Tag> tagMap = new LinkedHashMap<String, Tag>();

    public class Tag {
        public final static String DEFAULT_VALUE = "";
        private String name = DEFAULT_VALUE;
        private Vector<String> args = new Vector<String>(1);

        /**
         * Creates a tag by parsing the given string
         * @param tagString
         */
        public Tag(String tagString) {
            if (tagString != null)
                parseTag(tagString);
        }
        
        /**
         * @return The tagname
         */
        public String getName() {
            return name;   
        }
        
        /**
         * Returns a tag argument at the given position
         * @param pos
         * @return The value os that position or <code>DEFAULT_VALUE</code> if does not exist 
         */
        public String getArg(int pos) {
            if (pos < 0 || pos >= args.size() || args.get(pos) == null) 
                return DEFAULT_VALUE;
            return args.get(pos);
        }

        /**
         * Sets the value at a given position.
         * @param pos
         * @param newValue
         */
        public void setArg(int pos, String newValue) {
            // extend the vector if necessary
            args.setSize(Math.max(args.size(), pos+1));
            args.set(pos, newValue);
        }
        
        /**
         * Check if the tag's value is default
         * @return <code>true</code> if ALL tag values are missing or have the default value
         */
        public boolean isDefault() {
            for(String val : args)
                if(val != null && !DEFAULT_VALUE.equals(val)) return false; 
            return true;
        }
        
        /**
         * @return the values part of the tag
         */
        public String getArgString() {
            StringBuffer sb = new StringBuffer(20);
            // check for the last non default value
            int endPos;
            for(endPos = args.size() - 1; endPos>0; endPos--)
                if(args.get(endPos) != null && !DEFAULT_VALUE.equals(args.get(endPos))) break;
            // if there are unnecessary default values at the end, throw them away
            if(endPos < args.size() - 1) args.setSize(endPos + 1);
            
            boolean firstArg = true;
            for (String val : args) {
                if(firstArg) firstArg = false;
                    else sb.append(','); 
                
                if (val != null) sb.append(val);
            }
            return sb.toString();
        }
        
        /**
         * @return the string representation of the tag or <code>DEFAULT_VALUE</code>
         * if all the args are default
         */
        public String toString() {
            // return an empty string if all the values are default
            if (isDefault()) return DEFAULT_VALUE;
            return getName()+"="+getArgString();
        }
        
        // parse a single tag and its values into a string vector
        private void parseTag(String tagStr) {
            args = new Vector<String>(2);
            Scanner scr = new Scanner(tagStr);
            // TODO string literal and escaping must be correctly handled
            // StreamParser can handle comments too, maybe it would be a better choice
            scr.useDelimiter("=|,");
            // parse for the tagname
            if (scr.hasNext()) name = scr.next().trim();
            // parse for the tag values with a new tokenizer
            while (scr.hasNext())
                args.add(scr.next().trim());
        }
    }

    /**
     * Create a display string tokenizer class
     * @param value The string to be parsed
     */
    public DisplayStringParser(String value) {
        if (value != null)
            parseString(value);
    }

    /**
     * Returns the value of the given tag on the given position
     * @param tagName Tag to be checked
     * @param pos Position (0 based)
     * @return Tag arg's value or <code>DEFAULT_VALUE</code> if empty or <code>null</code> if tag does not exist at all 
     */
    public String getTagArg(String tagName, int pos) {
        Tag tag = getTag(tagName);
        // if tag doesn't exsist return default value
        if (tag == null) return null;
        // check for the value itself
        String val = tag.getArg(pos);
        if(val == null) val = Tag.DEFAULT_VALUE;
        return val;
    }
    
    /**
     * Sets the value of a tag at a given position. Extends the tag vector if necessary
     * @param tagName Name of the tag
     * @param pos Position to be updated (0 based)
     * @param newValue New value to be stored
     */
    public void setTagArg(String tagName, int pos, String newValue) {
        Tag tag = getTag(tagName);
        // if the tag dos not exist add it to the map
        if (tag == null) {
            tag = new Tag(tagName);
            tagMap.put(tagName, tag);
        }
        tag.setArg(pos, newValue);
    }
    
    /**
     * Returns the tag with a given name
     * @param tagName equested tagname
     * @return The requested tag or <code>NULL</code> if does not exist
     */
    public Tag getTag(String tagName) {
        return tagMap.get(tagName);
    }
    
    /**
     * @return the full display string
     */
    public String toString() {
        StringBuffer sb = new StringBuffer(50);
        boolean firstTag = true;
        for(Tag tag : tagMap.values()) {
            String tagVal = tag.toString();
            if(!tagVal.equals("")) {
                if(firstTag) firstTag = false;
                    else sb.append(';');

                sb.append(tagVal);
            }
        }
        
        return sb.toString();
    }
    
    // parse the display string into tags along ;
    private void parseString(String str) {
        // tokenize along ;
        Scanner scr = new Scanner(str);
        scr.useDelimiter(";");
        while (scr.hasNext()) {
            Tag parsedTag = new Tag(scr.next().trim());
            tagMap.put(parsedTag.getName(), parsedTag);
        }

    }
}
