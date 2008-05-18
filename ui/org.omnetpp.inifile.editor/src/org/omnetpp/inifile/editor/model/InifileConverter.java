package org.omnetpp.inifile.editor.model;

import org.omnetpp.common.util.StringUtils;

/**
 * Converts ini files from 3.x format to 4.0.
 * @author Andras
 */
public class InifileConverter {
    public static String[] OBSOLETE_SECTIONS = {"Parameters", "Cmdenv", "Tkenv", "OutVectors", "Partitioning", "DisplayStrings"};

    public static String[] CMDENV_NAMES = {"autoflush", "event-banner-details", "event-banners", "express-mode",
        "message-trace", "module-messages", "output-file", "performance-display",
        "runs-to-execute", "status-frequency"};

    public static String[] TKENV_NAMES =  {"default-run", "image-path", "plugin-path"};

    public static String[] REMOVE_NAMES = {"preload-ned-files", 
        "anim-methodcalls", "animation-enabled", "animation-msgclassnames",
        "animation-msgcolors", "animation-msgnames", "animation-speed",
        "expressmode-autoupdate", "methodcalls-delay", "next-event-markers",
        "penguin-mode", "print-banners", "senddirect-arrows", "show-bubbles",
        "show-layouting", "slowexec-delay", "update-freq-express",
        "update-freq-fast", "use-mainwindow", "use-new-layouter"};

    // note: these should be the NEW option names!
    public static String[] COMMENT_OUT_NAMES = {"tkenv-default-run", "cmdenv-runs-to-execute"};

    public static String[] NUMBERS = {"", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine",
        "Ten", "Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen", 
        "Seventeen", "Eighteen", "Nineteen", "Twenty" };

	/**
	 * Tests whether the given inifile text is in old format (OMNeT++ 3.x) and 
	 * needs to be converted.
	 */
	public static boolean needsConversion(String text) {
	    String allObsoleteSections = StringUtils.join(OBSOLETE_SECTIONS, "|");
	    text = "\n"+text+"\n";
	    return text.matches("(?s).*\n\\s*\\[("+allObsoleteSections+"|Run ([0-9]+))\\][^\n]*\n.*");
	}
	
	/**
	 * Migrates 3.x ini files to 4.x format. This consists of the following:
	 * 
	 *  - rename the sections [Parameters], [Cmdenv], [Tkenv], [OutVectors], [Partitioning] to [General]
	 *  - merge the resulting multiple occurrences of [General] into one
	 *  - prefix [Cmdenv]/[Tkenv] config names with "cmdenv-" and "tkenv-" (unless already begins with that)
	 *  - remove obsolete [Tkenv] keys
	 *  - rename sections [Run 1], [Run 2] etc to [Config config1], etc.
	 *  - rename per-object config options: "**.enabled", "**.interval", "**.use-default", etc
	 *  
	 * Note: this method was created from the Perl script omnetpp/migrate/migrate-inifile. 
	 */
	public static String convert(String text) {
		String MULTILINE = "(?m)";  // turns on regex MULTILINE mode (Perl: s///m) 
		
        // rename Cmdenv keys
	    String allCmdenvNames = StringUtils.join(CMDENV_NAMES, "|");
	    text = text.replaceAll(MULTILINE+"^(\\s*[;#]?\\s*)("+allCmdenvNames+")\\b", "$1cmdenv-$2");

	    // rename Tkenv keys
	    String allTkenvNames = StringUtils.join(TKENV_NAMES, "|");
	    text = text.replaceAll(MULTILINE+"^(\\s*[;#]?\\s*)("+allTkenvNames+")\\b", "$1tkenv-$2");

	    // extra-stack-kb: we don't know whether it refers to Cmdenv or Tkenv, so we duplicate it
	    text = text.replaceAll(MULTILINE+"^(\\s*[;#]?\\s*)(extra-stack-kb\\s*=.*)$", "$1cmdenv-$2\n$1tkenv-$2");

	    // remove obsolete keys
        String allRemoveNames = StringUtils.join(REMOVE_NAMES, "|");
        text = text.replaceAll("\n[ \t]*[;#]?[ \t]*("+allRemoveNames+")[ \t]*=[^\n]*", "");

	    // comment out some keys
	    String allCommentOutNames = StringUtils.join(COMMENT_OUT_NAMES, "|");
	    text = text.replaceAll(MULTILINE+"^\\s*(("+allCommentOutNames+")\\s*=.*)", "# $1");
	    
	    // rename per-object keys
    	text = text.replaceAll(MULTILINE+"\\.use-default\\s*=", ".apply-default =");
    	text = text.replaceAll(MULTILINE+"\\.ev-output\\s*=", ".cmdenv-ev-output =");
    	text = text.replaceAll(MULTILINE+"\\.enabled\\s*=", ".vector-recording =");
    	text = text.replaceAll(MULTILINE+"\\.interval\\s*=", ".vector-recording-interval =");
    	text = text.replaceAll(MULTILINE+"\\.pingApp\\.vector-recording =", ".pingApp.enabled =");  // change back false hit
    	text = text.replaceAll(MULTILINE+"\\.pingApp\\.vector-recording-interval =", ".pingApp.interval =");  // change back false hit
    	text = text.replaceAll(MULTILINE+"\\.akaroa\\s*=", ".with-akaroa =");

        // convert total-stack-kb, cmdenv-extra-stack-kb, tkenv-extra-stack-kb:
        // remove -kb from key, add KB to value
        String allKbNames = "total-stack|cmdenv-extra-stack|tkenv-extra-stack";
        text = text.replaceAll(MULTILINE+"^(\\s*[;#]?\\s*)("+allKbNames+")-kb\\s*=\\s*([0-9]+)", "$1$2 = $3KB");
    	
        // convert yes/no and on/off to true/false
    	text = text.replaceAll(MULTILINE+"=\\s*yes\\b", "= true");
    	text = text.replaceAll(MULTILINE+"=\\s*no\\b",  "= false");
        text = text.replaceAll(MULTILINE+"=\\s*on\\b",  "= true");
        text = text.replaceAll(MULTILINE+"=\\s*off\\b", "= false");

        // rename sections to [General]
        String allObsoleteSections = StringUtils.join(OBSOLETE_SECTIONS, "|");
        text = text.replaceAll(MULTILINE+"^(\\s*[;#]?\\s*)\\[("+allObsoleteSections+")\\]", "$1\\[General\\]");

        // rename [Run X] sections to [Config configX]
        for (int i=1; i<NUMBERS.length; i++)
            text = text.replaceAll(MULTILINE+"^(\\s*[;#]?\\s*)\\[Run "+i+"\\]", "$1\\[Config "+NUMBERS[i]+"\\]");
        text = text.replaceAll(MULTILINE+"^(\\s*[;#]?\\s*)\\[Run ([0-9]+)\\]", "$1\\[Config config$2\\]");

	    // make exactly one space on both sides of the "=" sign (optional, just cosmetics)
	    text = text.replaceAll(MULTILINE+"^([^//;=]*?) *= *", "$1 = ");

	    // cut off superfluous trailing ";" (optional, just cosmetics)
	    text = text.replaceAll(MULTILINE+"^([^;#]*?);$", "$1");

	    // replace ";" with "#" as comment mark, ignoring it inside string constants
        text = text.replaceAll(MULTILINE+"^([^#;\"]*?);", "$1#");
        text = text.replaceAll(MULTILINE+"^([^#]*);([^\"]*)$", "$1#$2");
	    
	    // merge several [General] sections into one
	    String currentSection = "";
	    StringBuilder tmp = new StringBuilder();
	    for (String line : text.split("\n")) {
	        line = line + "\n";
	        if (line.matches("(?s)^\\s*\\[(.*?)\\].*")) {
	            String thisSection = line.replaceFirst("(?s)^\\s*\\[(.*?)\\].*", "$1");
	            if (thisSection.equals(currentSection))
	            	line = "";
	            currentSection = thisSection;
	        }
	        if (line.matches("(?s)^\\s*#\\s*\\[(.*?)\\].*"))
	        	line = "" ;
	        tmp.append(line);
	    }
	    text = tmp.toString();

	    return text;
	}
}
