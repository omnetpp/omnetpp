package org.omnetpp.inifile.editor.text.assist;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.TextPresentation;
import org.eclipse.jface.text.contentassist.ICompletionProposal;
import org.eclipse.jface.text.contentassist.IContextInformation;
import org.eclipse.jface.text.contentassist.IContextInformationPresenter;
import org.eclipse.jface.text.contentassist.IContextInformationValidator;
import org.eclipse.jface.text.templates.Template;
import org.eclipse.jface.text.templates.TemplateContextType;
import org.omnetpp.common.editor.text.IncrementalCompletionProcessor;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.editors.InifileEditorData;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.ConfigurationRegistry;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.ParamResolution;
import org.omnetpp.inifile.editor.model.InifileAnalyzer.KeyType;
import org.omnetpp.inifile.editor.text.NedHelper;

/**
 * Computes completion proposals for ini files.
 */
public class InifileCompletionProcessor extends IncrementalCompletionProcessor {
	/**
	 * Simple content assist tip closer. The tip is valid in a range
	 * of 5 characters around its popup location.
	 */
	protected static class Validator implements IContextInformationValidator, IContextInformationPresenter {
		protected int fInstallOffset;

		public boolean isContextInformationValid(int offset) {
			return Math.abs(fInstallOffset - offset) < 5;
		}

		public void install(IContextInformation info, ITextViewer viewer, int offset) {
			fInstallOffset = offset;
		}

		public boolean updatePresentation(int documentPosition, TextPresentation presentation) {
			return false;
		}
	}

	private IContextInformationValidator fValidator = new Validator();
	private InifileEditorData editorData;

	/**
	 * Constructor.
	 */
	public InifileCompletionProcessor(InifileEditorData editorData) {
		this.editorData = editorData;
	}

	/**
	 * This is the entry point for creating proposals. 
	 */
	public ICompletionProposal[] computeCompletionProposals(ITextViewer viewer, int documentOffset) {
		List<ICompletionProposal> result = new ArrayList<ICompletionProposal>();

		// determine prefix (content of the given line before the cursor)
		String linePrefix = "";
		int lineNumber = -1;
		try {
			IDocument doc = viewer.getDocument();
			lineNumber = doc.getLineOfOffset(documentOffset);
			int lineStartOffset = doc.getLineOffset(lineNumber);
			linePrefix = doc.get(lineStartOffset, documentOffset - lineStartOffset);
			//XXX if previous line ends in "\", it has to be included in the prefix
		} catch (BadLocationException e) {
			InifileEditorPlugin.logError(e);
		}

		// cut off leading spaces
		linePrefix = linePrefix.replaceFirst("^\\s+", "");

		// try to determine which section we are in
		IInifileDocument doc = editorData.getInifileDocument(); 
		String section = doc.getSectionForLine(lineNumber);
		
		Set<String> proposals = new HashSet<String>();

		ContextType type = lineEndContext(linePrefix);
		if (type==ContextType.COMMENT) {
			// we are inside a comment: no completion can be offered
		}
		else if (type==ContextType.STRINGLITERAL) {
			// inside string literal: no completion
			proposals.add("\"");
		}
		else {
			// normal context: not inside string literal or comment 
			// identify where we are: include, section name, key, value, or comment.
			if ("include".startsWith(linePrefix)) {
				proposals.add("include ");
			}
			if (linePrefix.matches("include\\s.*")) {
				// include directive: offer filenames
				proposals.add("foo.ini");
				proposals.add("bar.ini");
				proposals.add("somethingelse.ini");
			}
			if (linePrefix.length()==0 || linePrefix.startsWith("[")) {
				// section heading
				proposals.add("[General]\n");
				proposals.add("[Parameters]\n");
				proposals.add("[Cmdenv]\n");
				proposals.add("[Tkenv]\n");
				proposals.add("[Run ");
			}

			if (!linePrefix.startsWith("[") && !linePrefix.matches("include\\s.*")) {
				// key-value line
				if (!linePrefix.contains("=")) {
					// offer key completion proposals
					for (ConfigurationEntry e : ConfigurationRegistry.getEntries())
						proposals.add(e.getName()+" = "); //XXX check if section is ok, config not in there yet, etc

					// offer unassigned parameters
					if (section != null && InifileAnalyzer.isParamSection(section)) {
						InifileAnalyzer ana = editorData.getInifileAnalyzer();
						ParamResolution[] resList = ana.getUnassignedParams(section);
						
						//XXX generate, sort and add each group independently?
						proposals.add("**.use-default = ");
						for (ParamResolution res : resList) {
							// offer three versions for each 
							//XXX alternative: offer some continuations after each dot?
							proposals.add("**." + res.paramNode.getName() + " = ");
							proposals.add(res.moduleFullPath.replaceFirst("^[^\\.]+", "**") + "." + res.paramNode.getName() + " = ");
							proposals.add(res.moduleFullPath + "." +res.paramNode.getName() + " = ");

							// propose .use-default= lines; XXX only if at least some of these parameters have default value
							proposals.add("**." + res.paramNode.getName() + ".use-default = ");
							proposals.add(res.moduleFullPath.replaceFirst("^[^\\.]+", "**") + "." + res.paramNode.getName() + ".use-default = ");
							proposals.add(res.moduleFullPath + "." +res.paramNode.getName() + ".use-default = ");
						}
					}

					//XXX offer per-object configuration completion? (use-default, etc)
				}
				else {
					// offer value completions
					String key = linePrefix.replaceFirst("=.*", "").trim();
					if (InifileAnalyzer.getKeyType(key) == KeyType.CONFIG) {
						// configuration key: offer value completion based on the type
						ConfigurationEntry e = ConfigurationRegistry.getEntry(key);
						if (e != null) {
							switch (e.getType()) {
							case CFG_BOOL: 
								proposals.add("true\n");
								proposals.add("false\n");
								break;
							case CFG_STRING: 
								proposals.add("\"\"");
								break;
							case CFG_FILENAME:
							case CFG_FILENAMES:
								//TODO offer file name completion
								break;
							}
						}
					}
					if (InifileAnalyzer.getKeyType(key) == KeyType.PARAM) {
						// parameter value proposals -- possibly depending on the type of the parameter matched
					}
					if (InifileAnalyzer.getKeyType(key) == KeyType.PER_OBJECT_CONFIG) {
						if (key.endsWith(".use-default") || key.endsWith(".ev-output")) {
							proposals.add("true\n");
							proposals.add("false\n");
						}
						if (key.endsWith(".record-interval")) {  //XXX name not yet in use!
							proposals.add("$1..");
							proposals.add("$1..$2");
							proposals.add("..$2"); //XXX use templated proposals here!
						}
						//XXX what else...?
					}
					
				}
			}
		}

		addProposals(viewer, documentOffset, result, proposals, null);
		return (ICompletionProposal[]) result.toArray(new ICompletionProposal[result.size()]);
	}

	enum ContextType {NORMAL, COMMENT, STRINGLITERAL};

	private static ContextType lineEndContext(String linePrefix) {
		int k = 0;
		while (k < linePrefix.length()) {
			switch (linePrefix.charAt(k)) {
			case '"':
				// string literal: skip it
				k++;
				while (k < linePrefix.length() && linePrefix.charAt(k) != '"') {
					if (linePrefix.charAt(k) == '\\')  // skip \", \\, etc.
						k++; 
					k++;  
				}
				if (k >= linePrefix.length())
					return ContextType.STRINGLITERAL;
				k++;
				break;
			case '#': case ';':
				// comment
				return ContextType.COMMENT;
			default:
				k++;
			}
		}
		return ContextType.NORMAL;
	}

	private void addProposals(ITextViewer viewer, int documentOffset, List<ICompletionProposal> result, Set<String> proposals, String description) {
		result.addAll(createProposals(viewer, documentOffset, NedHelper.spaceSeparatedWordDetector, "", proposals.toArray(new String[0]), "", description));
	}

	private void addValueProposals(ITextViewer viewer, int documentOffset, List<ICompletionProposal> result, Set<String> proposals, String description) {
		result.addAll(createProposals(viewer, documentOffset, NedHelper.inifileWordDetector, "", proposals.toArray(new String[0]), "", description));
	}

	public IContextInformation[] computeContextInformation(ITextViewer viewer, int documentOffset) {
		return null;
	}

	public char[] getCompletionProposalAutoActivationCharacters() {
		//return null;
		return new char[] { '.', '=' };
	}

	public char[] getContextInformationAutoActivationCharacters() {
		return null;
		//XXX return new char[] { '(' };
	}

	public IContextInformationValidator getContextInformationValidator() {
		return fValidator;
	}

	public String getErrorMessage() {
		return null;
	}

	@Override
	protected TemplateContextType getContextType(ITextViewer viewer, IRegion region) {
		return null;
	}

	@Override
	protected Template[] getTemplates(String contextTypeId) {
		return new Template[0]; // no template suggestions
	}
}
