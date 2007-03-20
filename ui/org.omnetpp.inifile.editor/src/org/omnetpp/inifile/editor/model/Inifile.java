package org.omnetpp.inifile.editor.model;

import java.util.ArrayList;
import java.util.List;

/**
 * Represents an ini file.
 * 
 * @author Andras
 */
public class Inifile {
	public enum LineType {
		COMMENT, // blank or comment line (optional whitespace + optional comment) 
		HEADING, // section heading (plus optional comment) 
		KEYVALUE,   // key = value (plus optional comment)
		DIRECTIVE, // like "include foo.ini" (plus optional comment)  
		CONTINUATION  // continued from previous line (which ended in backslash)
	};

	class Line {
		private LineType type;
		private String rawText;
		private String comment; // without "#" plus leading whitespace
	}

	class CommentLine extends Line {
	}

	class HeadingLine extends Line {
		private String heading;
	}

	class KeyValueLine extends Line {
		private String key;
		private String value;
	}

	class DirectiveLine extends Line {
		private String directive;
		private String argument;
	}

	class ContinuationLine extends Line {
	}


	/**
	 * Stores the lines of the file. The array is indexed by line number,
	 * starting at zero. 
	 */
	private List<Line> lines = new ArrayList<Line>();

	public Inifile() {
	}

}
