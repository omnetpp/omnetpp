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

	public static String[] TKENV_NAMES = {"anim-methodcalls", "animation-enabled", "animation-msgclassnames",
				               "animation-msgcolors", "animation-msgnames", "animation-speed",
				               "default-run", "expressmode-autoupdate", "image-path", "methodcalls-delay",
				               "next-event-markers", "penguin-mode", "plugin-path", "print-banners",
				               "senddirect-arrows", "show-bubbles", "show-layouting", "slowexec-delay",
				               "update-freq-express", "update-freq-fast", "use-mainwindow",
				               "use-new-layouter"};
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
	 *  - rename sections [Run 1], [Run 2] etc to [Config config1], etc.
	 *  - rename output vector configuration sections "**.interval=" to "**.record-interval="
	 *  - rename "**.use-default" to "**.apply-default"
	 */
	public static String convert(String text) {
		// Note: this method was created from the Perl script _scripts\migrate\migrateinifile.pl 
		String MULTILINE = "(?m)";  // turns on regex MULTILINE mode (Perl: s///m) 
		
	    String allCmdenvNames = StringUtils.join(CMDENV_NAMES, "|");
	    text = text.replaceAll(MULTILINE+"^(\\s*[;#]?\\s*)("+allCmdenvNames+")\\b", "$1cmdenv-$2");

	    // rename Tkenv keys
	    String allTkenvNames = StringUtils.join(TKENV_NAMES, "|");
	    text = text.replaceAll(MULTILINE+"^(\\s*[;#]?\\s*)("+allTkenvNames+")\\b", "$1tkenv-$2");

	    // rename sections to [General]
	    String allObsoleteSections = StringUtils.join(OBSOLETE_SECTIONS, "|");
	    text = text.replaceAll(MULTILINE+"^(\\s*[;#]?\\s*)\\[("+allObsoleteSections+")\\]", "$1\\[General\\]");

	    // rename [Run X] sections to [Config configX]
	    for (int i=1; i<NUMBERS.length; i++)
	    	text = text.replaceAll(MULTILINE+"^(\\s*[;#]?\\s*)\\[Run "+i+"\\]", "$1\\[Config "+NUMBERS[i]+"\\]");
    	text = text.replaceAll(MULTILINE+"^(\\s*[;#]?\\s*)\\[Run ([0-9]+)\\]", "$1\\[Config config$2\\]");

    	// rename .use-default= to .apply-default=, .interval= to .record-interval etc. 
    	text = text.replaceAll(MULTILINE+"^([^;#\"]*)\\.use-default\\b", "$1.apply-default");
    	text = text.replaceAll(MULTILINE+"^([^;#\"]*)\\.interval\\b", "$1.record-interval");
    	text = text.replaceAll(MULTILINE+"^([^;#\"]*)\\.enabled\\b", "$1.record-enabled"); //XXX or what became the new name??

    	// replace yes/no with true/false
    	text = text.replaceAll(MULTILINE+"(=\\s*)yes(\\s*([;#].*)?)$", "$1true$2");
    	text = text.replaceAll(MULTILINE+"(=\\s*)no(\\s*([;#].*)?)$", "$1false$2");

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
