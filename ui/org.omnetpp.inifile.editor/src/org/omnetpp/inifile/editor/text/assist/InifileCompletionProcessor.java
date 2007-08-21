package org.omnetpp.inifile.editor.text.assist;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.CONFIG_;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.TextPresentation;
import org.eclipse.jface.text.contentassist.CompletionProposal;
import org.eclipse.jface.text.contentassist.ICompletionProposal;
import org.eclipse.jface.text.contentassist.IContextInformation;
import org.eclipse.jface.text.contentassist.IContextInformationPresenter;
import org.eclipse.jface.text.contentassist.IContextInformationValidator;
import org.eclipse.jface.text.templates.Template;
import org.eclipse.jface.text.templates.TemplateContextType;
import org.omnetpp.common.editor.text.IncrementalCompletionProcessor;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.contentassist.InifileConfigKeyContentProposalProvider;
import org.omnetpp.inifile.editor.contentassist.InifileParamKeyContentProposalProvider;
import org.omnetpp.inifile.editor.contentassist.InifileValueContentProposalProvider;
import org.omnetpp.inifile.editor.contentassist.PerObjectConfigKeyContentProposalProvider;
import org.omnetpp.inifile.editor.editors.InifileEditorData;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.text.InifileTextEditorHelper;

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
	@Override
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
		InifileAnalyzer analyzer = editorData.getInifileAnalyzer();
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
			if (linePrefix.matches("include\\b.*")) {
				// include directive: offer directories and ini files (relative to the edited file)
				try {
					IContainer currentDir = doc.getDocumentFile().getParent();
					String arg = linePrefix.replaceFirst("include\\b\\s*(.*)", "$1"); 
					String argDir = arg.replaceFirst("(.*/)", "$1"); // stuff up to the last "/"
					IResource dir = argDir.equals("") ? currentDir : currentDir.findMember(new Path(argDir));
					if (dir != null && dir instanceof IFolder) {
						for (IResource resource : ((IFolder)dir).members()) {
							if (resource instanceof IFile && "ini".equals(resource.getFileExtension()))
								proposals.add(argDir + resource.getName());
							if (resource instanceof IFolder)
								proposals.add(argDir + resource.getName()+"/");
						}
						proposals.add(argDir + "../");
					}
				} catch (CoreException e) {
					InifileEditorPlugin.logError(e);
				}
			}
			if (linePrefix.length()==0 || linePrefix.matches("\\[[a-zA-Z]*")) {
				// section heading
				if (!doc.containsSection(GENERAL))
					proposals.add("["+GENERAL+"]");
				proposals.add("["+CONFIG_);
			}

			if (!linePrefix.startsWith("[") && !linePrefix.matches("include\\s.*")) {
				// key-value line
				if (!linePrefix.contains("=")) {
					// offer parameter keys
					if (section != null) {
						IContentProposalProvider paramProposalProvider = new InifileParamKeyContentProposalProvider(section, true, doc, analyzer);
						IContentProposal[] paramProposals = paramProposalProvider.getProposals(linePrefix, linePrefix.length());
						addProposals(result, paramProposals, documentOffset);

						IContentProposalProvider perObjectConfigProposalProvider = new PerObjectConfigKeyContentProposalProvider(section, true, doc, analyzer);
						IContentProposal[] keyProposals = perObjectConfigProposalProvider.getProposals(linePrefix, linePrefix.length());
						addProposals(result, keyProposals, documentOffset);
					}

					// offer configuration keys
					IContentProposalProvider configProposalProvider = new InifileConfigKeyContentProposalProvider(section, true, doc, analyzer);
					IContentProposal[] keyProposals = configProposalProvider.getProposals(linePrefix, linePrefix.length());
					addProposals(result, keyProposals, documentOffset);
				}
				else {
					// offer value completions
					String key = linePrefix.replaceFirst("=.*", "").trim();
					String value = linePrefix.replaceFirst(".*?=", "").trim();
					IContentProposalProvider proposalProvider = new InifileValueContentProposalProvider(section, key, doc, analyzer, true);
					IContentProposal[] valueProposals = proposalProvider.getProposals(value, value.length());
					
					// re-wrap IContentProposals as ICompletionProposal
					addProposals(result, valueProposals, documentOffset);
				}
			}
		}

		addProposals(viewer, documentOffset, result, proposals, null);
		return result.toArray(new ICompletionProposal[result.size()]);
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

	protected static void addProposals(List<ICompletionProposal> result, IContentProposal[] paramProposals, int documentOffset) {
		if (paramProposals.length==1 && paramProposals[0].getContent().equals(""))
			return;  // workaround: don't add "(no proposal)" proposal -- see ContentProposalProvider class
		for (IContentProposal p : paramProposals)
			result.add(convertToCompletionProposal(p, documentOffset));
	}

	protected static ICompletionProposal convertToCompletionProposal(IContentProposal p, int documentOffset) {
		return new CompletionProposal(p.getContent(), documentOffset, 0, p.getCursorPosition(), null, p.getLabel(), null, p.getDescription());
	}
	
	protected void addProposals(ITextViewer viewer, int documentOffset, List<ICompletionProposal> result, Set<String> proposals, String description) {
		result.addAll(createProposals(viewer, documentOffset, InifileTextEditorHelper.spaceSeparatedWordDetector, "", proposals.toArray(new String[0]), "", description));
	}

	@Override
	public IContextInformation[] computeContextInformation(ITextViewer viewer, int documentOffset) {
		return null;
	}

	@Override
	public char[] getCompletionProposalAutoActivationCharacters() {
		return new char[] { '.', '=' };
	}

	@Override
	public char[] getContextInformationAutoActivationCharacters() {
		return null;  //XXX new char[] { '(' };
	}

	@Override
	public IContextInformationValidator getContextInformationValidator() {
		return fValidator;
	}

	@Override
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
