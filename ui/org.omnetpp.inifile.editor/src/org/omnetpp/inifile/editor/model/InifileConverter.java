package org.omnetpp.inifile.editor.model;

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

//	public static boolean  
	public static String convert(String text) {
		final String MULTILINE = "(?m)";  // turns on MULTILINE mode (Perl: s///m) 
		
	    String allCmdenvNames = join("|", CMDENV_NAMES);
	    text = text.replaceAll(MULTILINE+"^(\\s*[;#]?\\s*)("+allCmdenvNames+")\\b", "$1cmdenv-$2");

	    // rename Tkenv keys
	    String allTkenvNames = join("|", TKENV_NAMES);
	    text = text.replaceAll(MULTILINE+"^(\\s*[;#]?\\s*)("+allTkenvNames+")\\b", "$1tkenv-$2");

	    // rename sections to [General]
	    String allObsoleteSections = join("|", OBSOLETE_SECTIONS);
	    text = text.replaceAll(MULTILINE+"^(\\s*[;#]?\\s*)\\[("+allObsoleteSections+")\\]", "$1\\[General\\]");

	    // rename [Run X] sections to [Config configX]
	    for (int i=1; i<NUMBERS.length; i++)
	    	text = text.replaceAll(MULTILINE+"^(\\s*[;#]?\\s*)\\[Run "+i+"\\]", "$1\\[Config "+NUMBERS[i]+"\\]");
    	text = text.replaceAll(MULTILINE+"^(\\s*[;#]?\\s*)\\[Run ([0-9]+)\\]", "$1\\[Config config$2\\]");

	    // make exactly one space on both sides of the "=" sign (optional, just cosmetics)
	    text = text.replaceAll(MULTILINE+"^([^//;=]*?) *= *", "$1 = ");

	    // cut off superfluous trailing ";" (optional, just cosmetics)
	    text = text.replaceAll(MULTILINE+"^([^;#]*?);$", "$1");

	    // merge several [General] sections into one
//	    $currentSection = "";
//	    $tmp = "";
//	    foreach $line (split("\n", $txt)) {
//	        $line .= "\n";
//	        if ($line =~ /^\\s*\\[(.*?)\\]/) {
//	            $thisSection = $1;
//	            $line = "" if ($thisSection eq $currentSection);
//	            $currentSection = $thisSection;
//	        }
//	        $line = "" if ($line =~ /^\\s*[//;]\\s*\\[(.*?)\\]/);
//	        $tmp .= $line;
//	    }
//	    $txt = $tmp;

	    return text;
	}

	private static String join(String separator, String[] parts) {
		StringBuffer result = new StringBuffer();
		for (String part : parts) {
			if (result.length()!=0)
				result.append(separator);
			result.append(part);
		}
		return result.toString();
	}

}
