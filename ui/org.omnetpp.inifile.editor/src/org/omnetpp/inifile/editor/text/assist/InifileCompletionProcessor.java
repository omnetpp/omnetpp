package org.omnetpp.inifile.editor.text.assist;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Set;

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
	
    /**
     * This is the main entry point. 
     */
	public ICompletionProposal[] computeCompletionProposals(ITextViewer viewer, int documentOffset) {
		List<ICompletionProposal> result = new ArrayList<ICompletionProposal>();
		// TODO
		String[] proposals = new String[] {"ecc", "pecc", "kimehecc"};
		addProposals(viewer, documentOffset, result, proposals, "some description or whatever"); //XXX
	    return (ICompletionProposal[]) result.toArray(new ICompletionProposal[result.size()]);
	}

	private void addProposals(ITextViewer viewer, int documentOffset, List<ICompletionProposal> result, String[] proposals, String description) {
		result.addAll(createProposals(viewer, documentOffset, NedHelper.inifileWordDetector, "", proposals, "", description));
	}

	private void addProposals(ITextViewer viewer, int documentOffset, List<ICompletionProposal> result, Set<String> proposals, String description) {
		result.addAll(createProposals(viewer, documentOffset, NedHelper.inifileWordDetector, "", proposals.toArray(new String[0]), "", description));
	}

	private void addProposals(ITextViewer viewer, int documentOffset, List<ICompletionProposal> result, Template[] templates) {
	    result.addAll(Arrays.asList(createTemplateProposals(viewer, documentOffset, templates)));
	}

	public IContextInformation[] computeContextInformation(ITextViewer viewer, int documentOffset) {
		return null;
	}

	public char[] getCompletionProposalAutoActivationCharacters() {
		return null;
		//XXX return new char[] { '.' };
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
