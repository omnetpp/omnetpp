/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model;

import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Vector;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.Debug;
import org.omnetpp.common.displaymodel.DimensionF;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.displaymodel.PointF;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.ex.NedElementUtilEx;
import org.omnetpp.ned.model.interfaces.IHasDisplayString;


/**
 * The default implementation of IDisplayString. It is strongly tied to the NedElement
 * model tree, and keeps its contents in sync with the LiteralElement in a display property.
 *
 * IMPORTANT: DisplayString doesn't support a listener list, and please do NOT add one!
 * If you want to get notified about display string changes, listen on the IHasDisplayString
 * NED element. This is the only way to get notified about changes that were made
 * directly on the tree, bypassing the DisplayString object.
 *
 * @author rhornig, andras (cleanup)
 */
public class DisplayString implements IDisplayString {

    // contains the default fallback values for the different tags if a variable is used in that position
    public static final DisplayString VARIABLE_DEFAULTS = new DisplayString(VARIABLE_DEFAULTS_STR);

    // contains the default fallback values for the different tags if it is empty
    public static final DisplayString EMPTY_DEFAULTS = new DisplayString(EMPTY_DEFAULTS_STR);

    // special value used to remove (neutralize) an inherited value
    public static final String ANTIVALUE = "-";

    // display string used for fallback (i.e. display string of the ancestor type)
    protected DisplayString fallbackDisplayString = null;

    // the owner of the display string
    protected IHasDisplayString owner = null;

    // cached return value of toString(); null means not calculated.
    // Every public method that modifies the object's contents must call invalidate()
    // to null out this field.
    protected String cachedDisplayString = null;

    // map that stores the currently available tag instances
    private Map<String, TagInstance> tagMap = new LinkedHashMap<String, TagInstance>();

    public static class TagInstance {
        private String name = "";
        private Vector<String> args = new Vector<String>(1);

        /**
         * Creates a tag with the given string as name
         */
        public TagInstance(String name) {
            this.name = name;
        }

        /**
         * Returns the tag name
         */
        public String getName() {
            return name;
        }

        public Tag getTag() {
            return Tag.valueOf(name);
        }

        public int getArgSize() {
            return args.size();
        }

        /**
         * Returns a tag argument at the given position, or an empty string if it does not exist
         */
        public String getArg(int pos) {
            if (pos < 0 || pos >= args.size() || args.get(pos) == null)
                return "";
            return args.get(pos);
        }

        /**
         * Sets the value at a given position.
         */
        public void setArg(int pos, String newValue) {
            // extend the vector if necessary
            args.setSize(Math.max(args.size(), pos+1));
            args.set(pos, newValue);
        }

        /**
         * Check if the tag's value is default. Returns true if all tag values are
         * missing or empty.
         */
        public boolean isEmpty() {
            for (String value : args)
                if (value != null && !value.equals(""))
                    return false;
            return true;
        }

        /**
         * Returns the values part of the tag
         */
        public String getArgString() {
            StringBuffer sb = new StringBuffer(20);

            // check for the last non-default value
            int endPos;
            for (endPos = args.size() - 1; endPos>0; endPos--)
                if (args.get(endPos) != null && !"".equals(args.get(endPos)))
                    break;

            // if there are unnecessary default values at the end, throw them away
            if (endPos < args.size() - 1)
                args.setSize(endPos + 1);

            boolean firstArg = true;
            for (String val : args) {
                if (firstArg)
                    firstArg = false;
                else
                    sb.append(',');

                if (val != null)
                    sb.append(val);
            }
            return sb.toString();
        }

        /**
         * Returns the string representation of the tag, or the empty string
         * if all the args are empty
         */
        @Override
        public String toString() {
            // return an empty string if all the values are default
            if (isEmpty())
                return "";
            return getName() + "=" + getArgString();
        }
    }


    /**
     * Create a display string tokenizer class only derived classes allowed to be created
     * @param owner owner of the display string object; its "@display" property will be kept up-to-date
     * @param value The string to be parsed
     */
    public DisplayString(IHasDisplayString owner, String value) {
        this(value);
        this.owner = owner;
    }

    public DisplayString(String value) {
        if (value != null) {
            set(value);
            cachedDisplayString = value;
        }
    }

    /**
     * Owner of this this display string, or null if it does not have one.
     */
    public IHasDisplayString getOwner() {
        return owner;
    }

    /**
     * Returns the currently set fallback display string
     */
    public DisplayString getFallbackDisplayString() {
        return fallbackDisplayString;
    }

    /**
     * Sets the fallback display string used if the current object has empty or null value for a
     * requested property (points to the type or ancestor display string).
     */
    public void setFallbackDisplayString(DisplayString defaults) {
        this.fallbackDisplayString = defaults;
    }

    /**
     * Returns the value of the given tag on the given position
     * @param tag Tag to be checked
     * @param pos Position (0-based)
     * @return TagInstance arg's value, the empty string if empty,  or
     * <code>null</code> if the tag does not exist at all
     */
    protected String getTagArg(Tag tag, int pos) {
        TagInstance tagInst = getTag(tag);

        // if tag doesn't exist, return null
        if (tagInst == null)
            return null;

        // check for the value itself
        String value = tagInst.getArg(pos);
        return value == null ? "" :  value;
    }

    /**
     * Returns TagInstance arg's value or <code>""</code> if empty and no default is defined.
     * If a default display string was provided with <code>setFallbackDisplayString</code>,
     * it tries to look up the property from there.
     */
    protected String getTagArgUsingDefaults(Tag tagName, int pos, boolean useEmptyDefaults, boolean useVariableDefaults) {
        TagInstance tag = getTag(tagName);
        String value = (tag == null) ? "" : tag.getArg(pos);
        Assert.isNotNull(value);

        if (value.startsWith("$") && useVariableDefaults)
            return VARIABLE_DEFAULTS.getTagArg(tagName, pos);
        else if (value.equals(ANTIVALUE))  // antivalue disables inherited values, so we return the default
            return useEmptyDefaults ? EMPTY_DEFAULTS.getTagArg(tagName, pos) : "";
        else if (value.length() > 0)
            return value;
        else if (getFallbackDisplayString() != null)
            return getFallbackDisplayString().getTagArgUsingDefaults(tagName, pos, useEmptyDefaults, useVariableDefaults);
        else
            return useEmptyDefaults ? EMPTY_DEFAULTS.getTagArg(tagName, pos) : "";
    }

    public boolean containsProperty(Prop prop) {
        TagInstance tag = getTag(prop.getTag());
        String value = (tag == null) ? "" : tag.getArg(prop.getPos());
        Assert.isNotNull(value);

        if (!value.equals(""))
            return true;
        else if (getFallbackDisplayString() != null)
            return getFallbackDisplayString().containsProperty(prop);
        else
            return false;
    }

    public boolean containsTag(Tag tagName) {
        TagInstance tag = getTag(tagName);
        if (tag != null)
            return true;
        else if (getFallbackDisplayString() != null)
            return getFallbackDisplayString().containsTag(tagName);
        else
            return false;
    }

    /**
     * Sets the value of a tag at a given position. Extends the tag vector if necessary
     * @param tag Name of the tag
     * @param pos Position to be updated (0 based)
     * @param newValue New value to be stored
     */
    protected void setTagArg(Tag tag, int pos, String newValue) {
        TagInstance tagInstance = getTag(tag);

        // if the value is empty or null and the tag does not exist, do nothing
        if (tagInstance == null && (newValue == null || "".equals(newValue)))
            return;

        // if the tag does not exist add it to the map
        if (tagInstance == null) {
            tagInstance = new TagInstance(tag.name());
            tagMap.put(tag.name(), tagInstance);
        }
        tagInstance.setArg(pos, newValue);

        // remove the tag if every value is empty
        if (tagInstance.isEmpty())
            tagMap.remove(tag.name());
    }

    /**
     * Returns the tag with a given name
     * @param tag requested tag name
     * @return The requested tag or <code>NULL</code> if does not exist
     */
    protected TagInstance getTag(Tag tag) {
        return tagMap.get(tag.name());
    }

    /**
     * Parse the given string and store its contents.
     */
    public void set(String newValue) {
        tagMap = parseTags(newValue);
        cachedDisplayString = newValue;
        updateNedElement();
    }

    public static LinkedHashMap<String, TagInstance> parseTags(String text) {
        LinkedHashMap<String, TagInstance> tagMap = new LinkedHashMap<String, TagInstance>();
        TagInstance currentTag = null;

        StringBuilder sb = new StringBuilder(); // collects the current tag name or arg value
        boolean insideTagName = true;
        int len = text.length();

        for (int pos = 0; pos < len; pos++) {
            char c = text.charAt(pos);
            if (c == '\\' && (pos+1) < len) {
                // allow escaping display string special chars (=,;) with backslash.
                // No need to deal with "\t", "\n" etc here, since they already got
                // interpreted by opp_parsequotedstr().
                sb.append(text.charAt(++pos));
            }
            else if (c == ';') {
                if (insideTagName) {
                    String name = sb.toString();
                    sb = new StringBuilder();
                    currentTag = new TagInstance(name);
                    tagMap.put(name, currentTag);
                    insideTagName = false;
                }
                else
                    currentTag.args.add(sb.toString());
                sb = new StringBuilder();
                insideTagName = true;
            }
            else if (c == '$' && (pos+1) < len && text.charAt(pos+1) == '$' && !insideTagName) {
                sb.append(text.charAt(pos+1));
            }
            else if (c == '$' && (pos+1) < len && text.charAt(pos+1) == '{' && !insideTagName) {
                // skip expression
                int end = StringUtils.findCloseParen(text, pos+1);
                sb.append(text.substring(pos, end+1));
                pos = end;
            }
            else if (c == '=' && insideTagName) {
                String name = sb.toString();
                sb = new StringBuilder();
                currentTag = new TagInstance(name);
                tagMap.put(name, currentTag);
                insideTagName = false;
            }
            else if (c == ',' && !insideTagName) {
                // new argument of current tag begins
                currentTag.args.add(sb.toString());
                sb = new StringBuilder();
            }
            else
                sb.append(c);
        }

        if (sb.length() > 0) {
            if (insideTagName) {
                String name = sb.toString();
                currentTag = new TagInstance(name);
                tagMap.put(name, currentTag);
            }
            else
                currentTag.args.add(sb.toString());
        }
        return tagMap;
    }

    protected void updateNedElement() {
        if (owner != null) {
            // Change the underlying NedElement tree.
            // This could be optimized somewhat by remembering the LiteralElement, and quickly
            // checking here if that's still where we have to change the display string
            NedElementUtilEx.setDisplayStringLiteral(owner, toString());
        }
    }

    /**
     * Returns the local display string, i.e. without inherited tags.
     */
    @Override
    public String toString() {
        if (cachedDisplayString == null) {
            StringBuffer sb = new StringBuffer(50);
            boolean firstTag = true;
            for (TagInstance tag : tagMap.values()) {
                String tagVal = tag.toString();
                if (!tagVal.equals("")) {
                    if (firstTag) firstTag = false;
                    else sb.append(';');

                    sb.append(tagVal); //FIXME quoting??? string may contain ";" or ","  --Andras
                }
            }
            cachedDisplayString = sb.toString();
        }
        return cachedDisplayString;
    }

    public int cumulativeHashCode() {
        int hash = 0;

        for (DisplayString ds = this; ds != null; ds = ds.getFallbackDisplayString())
            hash += ds.toString().hashCode();

        return hash;
    }

    /**
     * The property value. NULL if tag does not exist, "" if the value
     * is empty.
     */
    public String getAsStringLocal(Prop property) {
        return getTagArg(property.getTag(), property.getPos());
    }

    public String getAsString(Prop property, boolean useEmptyDefaults, boolean useVariableDefaults) {
        return getTagArgUsingDefaults(property.getTag(), property.getPos(), useEmptyDefaults, useVariableDefaults);
    }

    public String getAsString(Prop property) {
        return getTagArgUsingDefaults(property.getTag(), property.getPos(), true, true);
    }

    public int getAsInt(Prop propName, int defaultValue) {
        Assert.isTrue(propName.getType() == PropType.INTEGER);
        try {
            String value = getAsString(propName);
            if (!StringUtils.isBlank(value))
                return Integer.valueOf(value);
        } catch (NumberFormatException e) { }
        return defaultValue;
    }

    public float getAsDistance(Prop propName) {
        Assert.isTrue(propName.getType() == PropType.DISTANCE);
        try {
            String value = getAsString(propName);
            if (!StringUtils.isBlank(value)) {
                float floatValue = Float.valueOf(value);
                if (floatValue > 0)
                    return floatValue;
            }
        } catch (NumberFormatException e) { }
        return Float.NaN;
    }

    public float getAsCoordinate(Prop propName) {
        Assert.isTrue(propName.getType() == PropType.COORDINATE);
        try {
            String value = getAsString(propName);
            if (!StringUtils.isBlank(value))
                return Float.valueOf(value);
        } catch (NumberFormatException e) { }
        return Float.NaN;
    }

    /**
     * Sets the specified property to the given value in the display string
     */
    public void set(Prop property, String newValue) {
        setTagArg(property.getTag(), property.getPos(), newValue);
        cachedDisplayString = null;
        updateNedElement();
    }

    public float getRange() {
        float value = getAsDistance(DisplayString.Prop.RANGE);
        return value <= 0 ? Float.NaN : value;
    }

    public PointF getLocation() {
        // return NaN to signal that the property is missing
        Float x = getAsCoordinate(Prop.X);
        Float y = getAsCoordinate(Prop.Y);

        // if it's unspecified in any direction, we should return a NULL constraint
        if (Float.isNaN(x) || Float.isNaN(y))
            return null;

        return new PointF(x, y);
    }

    /**
     * Sets the location properties (P tag). If NULL is given both location property is cleared
     * to indicate that the module is unpinned and can be freely moved by the layouter.
     * It fires a single property change notification for Prop.X
     */
    public void setLocation(PointF location) {
        // if location is not specified, remove the constraint from the display string
        if (location == null) {
            set(Prop.X, null);
            set(Prop.Y, null);
        }
        else {
            set(Prop.X, validFloatToString(location.x));
            set(Prop.Y, validFloatToString(location.y));
        }
    }

    public DimensionF getSize() {
        // the following getters use EMPTY_DEFAULTS
        float width = getAsDistance(Prop.SHAPE_WIDTH);
        float height = getAsDistance(Prop.SHAPE_HEIGHT);

        // if one of the dimensions is missing, use the other dimension instead
        boolean widthExist = containsProperty(IDisplayString.Prop.SHAPE_WIDTH);
        boolean heightExist = containsProperty(IDisplayString.Prop.SHAPE_HEIGHT);
        if (!widthExist && heightExist)
            width = height;
        else if (widthExist && !heightExist)
            height = width;

        return new DimensionF(width, height);
    }

    /**
     * Sets the size of the submodule shape ("b" tag). If Nan or a negative number
     * is given as width or height, the corresponding property is deleted.
     * It fires property change notification for Prop.SHAPE_WIDTH
     */
    public void setSize(DimensionF size) {
        // if the size is unspecified, remove the size constraint from the model
        if (size == null || Float.isNaN(size.width))
            set(Prop.SHAPE_WIDTH, null);
        else
            set(Prop.SHAPE_WIDTH, positiveFloatToString(size.width));

        // if the size is unspecified, remove the size constraint from the model
        if (size == null || Float.isNaN(size.height))
            set(Prop.SHAPE_HEIGHT, null);
        else
            set(Prop.SHAPE_HEIGHT, positiveFloatToString(size.height));
    }

    public DimensionF getCompoundSize() {
        float width = getAsDistance(Prop.MODULE_WIDTH);
        float height = getAsDistance(Prop.MODULE_HEIGHT);
        return new DimensionF(width, height);
    }

    /**
     * Sets the size of the compound module background ("bgb" tag). If NaN
     * is given as width or height, the corresponding property is deleted.
     * It fires property change notification for Prop.MODULE_WIDTH
     */
    public void setCompoundSize(DimensionF size) {
        // if the size is unspecified, remove the size constraint from the model
        if (size == null || Float.isNaN(size.width))
            set(Prop.MODULE_WIDTH, null);
        else
            set(Prop.MODULE_WIDTH, positiveFloatToString(size.width));

        // if the size is unspecified, remove the size constraint from the model
        if (size == null || Float.isNaN(size.height))
            set(Prop.MODULE_HEIGHT, null);
        else
            set(Prop.MODULE_HEIGHT, positiveFloatToString(size.height));
    }

    private static String positiveFloatToString(float value) {
        Assert.isTrue(value > 0);
        return validFloatToString(value);
    }

    private static String validFloatToString(float value) {
        Assert.isTrue(!Float.isNaN(value));
        return StringUtils.removeEnd(String.valueOf(value), ".0");
    }

    /**
     * Dump all supported tags
     */
    public static void dumpSupportedTags() {
        for (Prop p : Prop.values()) {
            Debug.println(p.getTag()+"\t "+p.getPos()+"\t "+p.getGroup()+"\t "+p.getName()
                    +"\t "+p.getDefaultDesc()+"\t "+p.getEnumDesc()+"\t "+p.getDesc());
        }
    }

    /**
     * Dump all supported tags in laTex
     */
    public static void dumpSupportedTagsInTex() {
        Debug.println("\\begin{longtable}{|p{6cm}|p{8cm}|}");
        Debug.println("\\hline");
        for (Prop prop : Prop.values()) {
            if (prop.getTag() == null)
                continue;
            Debug.println("\\tbf{"+prop.getTag()+"}["+prop.getPos()+"] - "+prop.getName());
            Debug.println("&");

            String enumDesc = prop.getEnumDesc();
            String defaultValue = prop.getDefaultDesc();
            String desc = prop.getShortDesc().replace("#","\\#");
            desc += (StringUtils.isNotEmpty(enumDesc) ?  ". Values: " + enumDesc : "");
            desc += (StringUtils.isNotEmpty(defaultValue) ? ". Default: "+defaultValue : "");
            Debug.println(desc);
            Debug.println("\\\\ \n \\hline");
        }
        Debug.println("\\end{longtable}");
    }

    void dump() {
        for (TagInstance tag : tagMap.values()) {
            Debug.println("'" + tag.name + "':");
            for (String arg : tag.args) {
                Debug.println("    '" + arg + "'");
            }
        }
    }

//    static {
//      dumpSupportedTagsInTex();
//    };
}


