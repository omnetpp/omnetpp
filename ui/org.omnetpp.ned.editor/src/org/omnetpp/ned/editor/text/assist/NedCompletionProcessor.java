package org.omnetpp.ned.editor.text.assist;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.TextPresentation;
import org.eclipse.jface.text.contentassist.ICompletionProposal;
import org.eclipse.jface.text.contentassist.IContextInformation;
import org.eclipse.jface.text.contentassist.IContextInformationPresenter;
import org.eclipse.jface.text.contentassist.IContextInformationValidator;
import org.eclipse.jface.text.templates.Template;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.texteditor.ITextEditor;

import org.omnetpp.common.editor.text.NedCompletionHelper;
import org.omnetpp.common.editor.text.NedSyntaxHighlightHelper;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.NEDResources;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.engine.PackageNode;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver.IPredicate;
import org.omnetpp.ned.model.pojo.SubmoduleElement;

// TODO completion within inner types
// TODO a better structure is needed for storing the completion proposals
// TODO context help can be supported, to show the documentation of the proposed keyword
// TODO F4 "Open Declaration"
// TODO completion for imports
/**
 * NED completion processor.
 *
 * @author andras
 */
public class NedCompletionProcessor extends NedTemplateCompletionProcessor {

	/**
	 * Simple content assist tip closer. The tip is valid in a range
	 * of 5 characters around its popup location.
     * TODO implement correctly the context information
	 */
	protected static class Validator implements IContextInformationValidator, IContextInformationPresenter {
		protected int fInstallOffset;

		/*
		 * @see IContextInformationValidator#isContextInformationValid(int)
		 */
		public boolean isContextInformationValid(int offset) {
			return Math.abs(fInstallOffset - offset) < 5;
		}

		/*
		 * @see IContextInformationValidator#install(IContextInformation, ITextViewer, int)
		 */
		public void install(IContextInformation info, ITextViewer viewer, int offset) {
			fInstallOffset= offset;
		}

		/*
		 * @see org.eclipse.jface.text.contentassist.IContextInformationPresenter#updatePresentation(int, TextPresentation)
		 */
		public boolean updatePresentation(int documentPosition, TextPresentation presentation) {
			return false;
		}
	}

	// for CompletionInfo.sectionType
	protected static final int SECT_GLOBAL = 0;
	protected static final int SECT_PARAMETERS = 1;
	protected static final int SECT_GATES = 2;
	protected static final int SECT_TYPES = 3;
	protected static final int SECT_CONNECTIONS = 4;
	protected static final int SECT_SUBMODULES = 5;
	protected static final int SECT_SUBMODULE_PARAMETERS = 6;
	protected static final int SECT_SUBMODULE_GATES = 7;

	protected static class CompletionInfo {
		public String linePrefix; // relevant line (lines) just before the insertion point
		public String linePrefixTrimmed; // like linePrefix, but last identifier (which the user is currently typing) chopped
		public String componentName;
		public int sectionType; // SECT_xxx
		public String submoduleTypeName;
	}

	protected IContextInformationValidator fValidator = new Validator();
	protected ITextEditor editor;

    public NedCompletionProcessor(ITextEditor editor) {
    	this.editor = editor;
	}

	@Override
    public ICompletionProposal[] computeCompletionProposals(ITextViewer viewer, int documentOffset) {
		// long startMillis = System.currentTimeMillis(); // measure time

		List<ICompletionProposal> result = new ArrayList<ICompletionProposal>();

		// find out where we are: in which module, submodule, which section etc.
		CompletionInfo info = computeCompletionInfo(viewer, documentOffset);
		NEDResources res = NEDResourcesPlugin.getNEDResources();
		IFile file = ((IFileEditorInput)editor.getEditorInput()).getFile();
		NedFileElementEx nedFileElement = res.getNedFileElement(file);
		IProject project = file.getProject();

		String line = info.linePrefixTrimmed;
		INEDTypeInfo parentComponent = null;
		INedTypeLookupContext context = nedFileElement;
		if (info.componentName!=null) {
			parentComponent = res.lookupNedType(info.componentName, nedFileElement);
			if (parentComponent instanceof CompoundModuleElementEx)
				context = (CompoundModuleElementEx)parentComponent;
		}
		INEDTypeInfo submoduleType = null;
		if (info.submoduleTypeName!=null) {
			submoduleType = res.lookupNedType(info.submoduleTypeName, context);
		}

		if (info.sectionType==SECT_GLOBAL || info.sectionType==SECT_TYPES)
		{
			// System.out.println("testing proposals for GLOBAL and TYPES scope");

			// match various "extends" and "like" clauses and offer component types
			if (line.matches(".*\\bsimple .* extends"))
			    addNedTypeProposals(viewer, documentOffset, result, project, parentComponent, NEDResources.SIMPLE_MODULE_FILTER);
			else if (line.matches(".*\\b(module|network) .* extends"))
                addNedTypeProposals(viewer, documentOffset, result, project, parentComponent, NEDResources.COMPOUND_MODULE_FILTER);
			else if (line.matches(".*\\bchannel .* extends"))
                addNedTypeProposals(viewer, documentOffset, result, project, parentComponent, NEDResources.CHANNEL_FILTER);
			else if (line.matches(".*\\bmoduleinterface .* extends"))
                addNedTypeProposals(viewer, documentOffset, result, project, parentComponent, NEDResources.MODULEINTERFACE_FILTER);
			else if (line.matches(".*\\bchannelinterface .* extends"))
                addNedTypeProposals(viewer, documentOffset, result, project, parentComponent, NEDResources.CHANNELINTERFACE_FILTER);

			// match "like" clauses
			if (line.matches(".*\\bsimple .* like") || line.matches(".*\\bsimple .* like .*,"))
                addNedTypeProposals(viewer, documentOffset, result, project, parentComponent, NEDResources.MODULEINTERFACE_FILTER);
			else if (line.matches(".*\\b(module|network) .* like") || line.matches(".*\\b(module|network) .* like .*,"))
                addNedTypeProposals(viewer, documentOffset, result, project, parentComponent, NEDResources.MODULEINTERFACE_FILTER);
			else if (line.matches(".*\\bchannel .* like") || line.matches(".*\\bchannel .* like .*,"))
                addNedTypeProposals(viewer, documentOffset, result, project, parentComponent, NEDResources.CHANNELINTERFACE_FILTER);

			if (!line.equals("") && !line.matches(".*\\b(like|extends)\\b.*") && line.matches(".*\\b(simple|module|network|channel|interface|channelinterface)\\b.*"))
				addProposals(viewer, documentOffset, result, new String[]{"extends "}, "keyword");

            if (!line.equals("") && !line.matches(".*\\blike\\b.*") && line.matches(".*\\b(simple|module|network|channel)\\b.*"))
                addProposals(viewer, documentOffset, result, new String[]{"like "}, "keyword");
		}

		// propose line start: param names, gate names, keywords
		if (line.equals("")) {
			// offer param and gate names
			if (info.sectionType == SECT_PARAMETERS && parentComponent!=null)
				addProposals(viewer, documentOffset, result, parentComponent.getParamDeclarations().keySet(), "parameter");
			if (info.sectionType == SECT_GATES && parentComponent!=null)
				addProposals(viewer, documentOffset, result, parentComponent.getGateDeclarations().keySet(), "gate");
			if (info.sectionType == SECT_SUBMODULE_PARAMETERS && submoduleType!=null)
				addProposals(viewer, documentOffset, result, submoduleType.getParamDeclarations().keySet(), "parameter");
			if (info.sectionType == SECT_SUBMODULE_GATES && submoduleType!=null)
				addProposals(viewer, documentOffset, result, submoduleType.getGateDeclarations().keySet(), "gate");

			// offer param and gate type name keywords
			if (info.sectionType == SECT_PARAMETERS)
				addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedParamTypes, "parameter type");
			else if (info.sectionType == SECT_GATES)
				addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedGateTypes, "gate type");

			// provide global start keywords and section names
            if (info.sectionType==SECT_GLOBAL) {
                addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedTopLevelKeywords, "keyword (top level)");
                addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedTypeDefinerKeywords, "keyword");
            }
            else if (info.sectionType==SECT_PARAMETERS) {
                addProposals(viewer, documentOffset, result, new String[]{"connections:", "connections allowunconnected:", "gates:", "parameters:", "submodules:", "types:"}, "section");
            }
            else if (info.sectionType==SECT_GATES) {
                addProposals(viewer, documentOffset, result, new String[]{"connections:", "connections allowunconnected:", "submodules:", "types:"}, "section");
            }
            else if (info.sectionType==SECT_TYPES) {
				addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedTypeDefinerKeywords, "keyword");
                addProposals(viewer, documentOffset, result, new String[]{"connections:", "connections allowunconnected:", "submodules:"}, "section");
	    	}
	    	else if (info.sectionType==SECT_SUBMODULES) {
                addProposals(viewer, documentOffset, result, new String[]{"connections:", "connections allowunconnected:"}, "section");
            }
            else if (info.sectionType==SECT_SUBMODULE_PARAMETERS) {
	    		addProposals(viewer, documentOffset, result, new String[]{"gates:"}, "section");
	    	}

	    	// offer templates
	    	if (info.sectionType==SECT_GLOBAL || info.sectionType==SECT_TYPES) {
			    addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedGlobalTempl);
	    	}
	    	else if (info.sectionType==SECT_SUBMODULES) {
	    		addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedSubmoduleTempl);
	    	}
		}

		// offer double/int/string/xml after "volatile"
		if (line.equals("volatile")) {
			addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedBaseParamTypes, "parameter type");
		}

		// expressions: after "=", opening "[", "if" or "for"
		if (line.contains("=") || line.matches(".*\\b(if|for)\\b.*") || containsOpenBracket(line)) {
			// System.out.println("proposals for expressions");

			// offer parameter names, gate names, types,...
			if (line.matches(".*\\bthis *\\.")) {
				if (submoduleType!=null)
					addProposals(viewer, documentOffset, result, submoduleType.getParamDeclarations().keySet(), "parameter");
			}
			else if (line.matches(".*\\bsizeof *\\(")) {
				if (parentComponent!=null) {
                    // FIXME add only vector submodules and vectors
					addProposals(viewer, documentOffset, result, parentComponent.getGateDeclarations().keySet(), "gate");
					addProposals(viewer, documentOffset, result, parentComponent.getSubmodules().keySet(), "submodule");
				}
			}
	    	else if (line.endsWith(".")) {
	    		// after dot: offer params (and after sizeof(), gates too) of given submodule
	    		if (parentComponent!=null) {
					String submodTypeName = extractSubmoduleTypeName(line, parentComponent);
					// System.out.println(" offering params of type "+submodTypeName);
					INEDTypeInfo submodType = res.lookupNedType(submodTypeName, context);
					if (submodType!=null) {
						if (line.matches(".*\\bsizeof *\\(.*"))
							addProposals(viewer, documentOffset, result, submodType.getGateDeclarations().keySet(), "gate");
						addProposals(viewer, documentOffset, result, submodType.getParamDeclarations().keySet(), "parameter");
					}
	    		}
	    	}
			else {
				if (parentComponent!=null) {
					addProposals(viewer, documentOffset, result, parentComponent.getParamDeclarations().keySet(), "parameter");
				}
			}
			addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedConstants, "");

			addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedOtherExpressionKeywords, "keyword");
		    addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedOperatorsTempl);
		    addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedContinuousDistributionsTempl);
		    addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedDiscreteDistributionsTempl);
		    addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedContinuousDistributionsTemplExt);
		    addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedDiscreteDistributionsTemplExt);
		    addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedFunctionsTempl);
		}

        // offer existing and standard property names after "@"
        if (line.equals("")) {
            if (info.sectionType == SECT_PARAMETERS && parentComponent!=null) {
                addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedComponentPropertyTempl);
                addProposals(viewer, documentOffset, result, parentComponent.getProperties().keySet(), "property");
            }
            if (info.sectionType == SECT_SUBMODULE_PARAMETERS && submoduleType!=null)
                addProposals(viewer, documentOffset, result, submoduleType.getProperties().keySet(), "property");
        }
        else if ((line.contains("=") && !line.endsWith("=")) || !line.contains("=")) {
            if (info.sectionType == SECT_PARAMETERS || info.sectionType == SECT_SUBMODULE_PARAMETERS)
                addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedParamPropertyTempl);
            if (info.sectionType == SECT_GATES || info.sectionType == SECT_SUBMODULE_GATES)
                addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedGatePropertyTempl);
        }

		// complete submodule type name
		if (info.sectionType == SECT_SUBMODULES) {
			// System.out.println("testing proposals for SUBMODULES scope");
			if (line.matches(".*:")) {
			    addNedTypeProposals(viewer, documentOffset, result, project, parentComponent, NEDResources.MODULE_FILTER);
			}
			else if (line.matches(".*: *<")) {  // "like" syntax
				if (parentComponent!=null)
					addProposals(viewer, documentOffset, result, parentComponent.getParamDeclarations().keySet(), "parameter");
			}
			else if (line.matches(".*: *<.*>")) {   // "like" syntax, cont'd
					addProposals(viewer, documentOffset, result, new String[]{" like "}, "keyword");
			}
			else if (line.matches(".*\\blike")) {
                addNedTypeProposals(viewer, documentOffset, result, project, parentComponent, NEDResources.MODULEINTERFACE_FILTER);
			}
		}

		if (info.sectionType == SECT_CONNECTIONS) {
			// System.out.println("testing proposals for CONNECTIONS scope");
			if (line.matches(".*\\bconnections")) {
				// user forgot "allowunconnected" keyword
				addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedConnsKeywords, "keyword");
			}

			if (line.equals("") || line.endsWith("-->") || line.endsWith("<-->") || line.endsWith("<--")) {
	    		// right at line start or after arrow: offer submodule names and parent module's gates
	    		if (parentComponent!=null) {
	    			addProposals(viewer, documentOffset, result, parentComponent.getSubmodules().keySet(), "submodule");
	    			addProposals(viewer, documentOffset, result, parentComponent.getGateDeclarations().keySet(), "gate");
	    			// only a single arrow can be present in the line to give channel assistance to
                    if (line.matches(".*--.*") && !line.matches(".*--.*--.*"))
                        addNedTypeProposals(viewer, documentOffset, result, project, parentComponent, NEDResources.CHANNEL_FILTER);
	    		}
	    	}
	    	else if (line.endsWith(".")) {
	    		// after dot: offer gates of given submodule
	    		if (parentComponent!=null) {
					String submodTypeName = extractSubmoduleTypeName(line, parentComponent);
					// System.out.println(" offering gates of type "+submodTypeName);
					INEDTypeInfo submodType = res.lookupNedType(submodTypeName, context);
					if (submodType!=null)
						addProposals(viewer, documentOffset, result, submodType.getGateDeclarations().keySet(), "gate");
	    		}
	    	}

			// offer templates for connection, loop connection, connection with channel, etc
            if (line.equals(""))
                addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedConnectionTempl);
		}

		// long millis = System.currentTimeMillis()-startMillis;
		// System.out.println("Proposal creation: "+millis+"ms");

	    return (ICompletionProposal[]) result.toArray(new ICompletionProposal[result.size()]);
	}

    private void addNedTypeProposals(ITextViewer viewer, int documentOffset, List<ICompletionProposal> result,
            IProject project, INEDTypeInfo parentComponent, IPredicate predicate) {
        NEDResources res = NEDResourcesPlugin.getNEDResources();
        // add inner types
        if (parentComponent != null) {
            Set<String> innerTypeNames = new HashSet<String>();
            for (INedTypeElement innerTypeElement : parentComponent.getInnerTypes().values()) {
                if (predicate.matches(innerTypeElement.getNEDTypeInfo()))
                    innerTypeNames.add(innerTypeElement.getName());
            }
            addProposals(viewer, documentOffset, result, innerTypeNames, "inner type");
        }
        
        // add top level types
        // XXX offer "like" template too
        Set<String> qnames = res.getNedTypeQNames(predicate, project);
        String names[] = new String[qnames.size()];
        String descriptions[] = new String[qnames.size()];
        int i = 0;
        for (String qname : qnames) {
            INEDTypeInfo nedTypeInfo = res.getToplevelNedType(qname, project);
            names[i] = nedTypeInfo.getName();
            String packageName = StringUtils.chomp(nedTypeInfo.getNamePrefix(), ".");
            packageName = StringUtils.isBlank(packageName) ? "" : packageName+" - ";
            descriptions[i] =  packageName + nedTypeInfo.getNEDElement().getReadableTagName()+" type";
            i++;
        }
        addProposals(viewer, documentOffset, result, names, descriptions);
    }

	private boolean containsOpenBracket(String line) {
		while (line.matches(".*\\[[^\\[\\]]*\\].*"))
			line = line.replaceAll("\\[[^\\[\\]]*\\]", "###");
        return line.contains("[");
	}

	private String extractSubmoduleTypeName(String line, INEDTypeInfo parentComponent) {
		// first, get rid of everything before any arrow(s), because it causes a problem for the next regexp
		line = line.replaceFirst("^.*(-->|<--|<-->)", "");
		// identifier followed by ".", potentially a submodule index ("[something]") in between
		Matcher matcher = Pattern.compile("([A-Za-z_][A-Za-z0-9_]*) *(\\[[^\\[\\]]*\\])? *\\.$").matcher(line);
		if (matcher.find()) { // use find() because line may start with garbage
			String submoduleName = matcher.group(1);
			INEDElement submodNode = parentComponent.getMembers().get(submoduleName);
			if (submodNode instanceof SubmoduleElement) {
				SubmoduleElement submod = (SubmoduleElement) submodNode;
				String submodTypeName = submod.getType();
				if (submodTypeName==null || submodTypeName.equals(""))
					submodTypeName = submod.getLikeType();
				return submodTypeName;
			}
		}
		return null; // bad luck
	}

    private void addProposals(ITextViewer viewer, int documentOffset, List<ICompletionProposal> result, String[] proposals, String[] descriptions) {
        result.addAll(createProposals(viewer, documentOffset, new NedSyntaxHighlightHelper.NedWordDetector(), "", proposals, "", descriptions));
    }

	private void addProposals(ITextViewer viewer, int documentOffset, List<ICompletionProposal> result, String[] proposals, String description) {
		result.addAll(createProposals(viewer, documentOffset, new NedSyntaxHighlightHelper.NedWordDetector(), "", proposals, "", description));
	}

	private void addProposals(ITextViewer viewer, int documentOffset, List<ICompletionProposal> result, Set<String> proposals, String description) {
		result.addAll(createProposals(viewer, documentOffset, new NedSyntaxHighlightHelper.NedWordDetector(), "", proposals.toArray(new String[] {}), "", description));
	}

	private void addProposals(ITextViewer viewer, int documentOffset, List<ICompletionProposal> result, Template[] templates) {
	    result.addAll(Arrays.asList(createTemplateProposals(viewer, documentOffset, new NedSyntaxHighlightHelper.NedWordDetector(), templates)));
	}

	private CompletionInfo computeCompletionInfo(ITextViewer viewer, int documentOffset) {
		IDocument docu = viewer.getDocument();
        int offset = documentOffset;
        try {
    		String source = docu.get(0,offset);
    		// kill string literals
			source = source.replaceAll("\".*\"", "\"###\"");  //FIXME but ignore embedded backslash+quote \" !!!
    		// kill comments
    		source = source.replaceAll("(?m)//.*", "");

    		// completion prefix (linePrefix): stuff after last semicolon,
    		// curly brace, "parameters:", "gates:", "connections:" etc.
    		String prefix = source;
    		prefix = prefix.replaceAll("(?s)\\s+", " "); // normalize whitespace
    		prefix = prefix.replaceFirst(".*[;\\{\\}]", "");
    		prefix = prefix.replaceFirst(".*\\b(parameters|gates|types|submodules|connections|connections +[a-z]+) *:", "");
    		String prefix2 = prefix.replaceFirst("[a-zA-Z_@][a-zA-Z0-9_]*$", "").trim(); // chop off last word

    		// kill {...} regions (including bodies of inner types, etc)
    		while (source.matches("(?s).*\\{[^\\{\\}]*\\}.*"))
    			source = source.replaceAll("(?s)\\{[^\\{\\}]*\\}", "###");

			// detect what section we are in
			int sectionType;
			if (source.matches("(?s).*\\bconnections\\b.*"))
				sectionType = SECT_CONNECTIONS;
			else if (source.matches("(?s).*\\bsubmodules\\b.*\\bgates\\b.*"))
				sectionType = SECT_SUBMODULE_GATES;
			else if (source.matches("(?s).*\\bsubmodules\\b.*\\{.*"))
				sectionType = SECT_SUBMODULE_PARAMETERS;
			else if (source.matches("(?s).*\\bsubmodules\\b.*"))
				sectionType = SECT_SUBMODULES;
			else if (source.matches("(?s).*\\btypes\\b.*"))
			    sectionType = SECT_TYPES;
			else if (source.matches("(?s).*\\bgates\\b.*"))
				sectionType = SECT_GATES;
			else if (source.matches("(?s).*\\{.*"))
				sectionType = SECT_PARAMETERS;
			else
				sectionType = SECT_GLOBAL;

            // eliminate the types: section
            source = source.replaceAll("(?s)\\btypes\\b[^{]*?\\b(submodules|connections)\\b", "$1");

			// detect module name: identifier after last "simple", "module", etc.
			String pat = "(?s).*(simple|module|network|channel|interface|channelinterface)\\s+(withcppclass\\s+)?([A-Za-z_][A-Za-z0-9_]+)";
			Matcher matcher = Pattern.compile(pat).matcher(source);
			String componentName = null;
			if (matcher.lookingAt())
				componentName = matcher.group(3);

			// detect submodule type: last occurrence of "identifier {"
			String submoduleTypeName = null;
			if (sectionType == SECT_SUBMODULE_GATES || sectionType == SECT_SUBMODULE_PARAMETERS) {
				String pat2 = "(?s).*[:\\s]([A-Za-z_][A-Za-z0-9_]+)\\s*\\{";
				Matcher matcher2 = Pattern.compile(pat2).matcher(source);
				if (matcher2.lookingAt())
					submoduleTypeName = matcher2.group(1);
			}

			//System.out.println(">>>"+source+"<<<");
//			System.out.println("SECTIONTYPE:"+sectionType+"  COMPONENT:"+componentName+"  SUBMODTYPENAME:"+submoduleTypeName);
//			System.out.println("PREFIX: >>"+prefix+"<<");
//			System.out.println("PREFIX2: >>"+prefix2+"<<");

			CompletionInfo ret = new CompletionInfo();
			ret.linePrefix = prefix;
			ret.linePrefixTrimmed = prefix2;
			ret.componentName = componentName;
			ret.sectionType = sectionType;
			ret.submoduleTypeName = submoduleTypeName;
			return ret;

        } catch (BadLocationException e) {
        	return null;
        }
	}

	public IContextInformation[] computeContextInformation(ITextViewer viewer, int documentOffset) {
		//XXX what the heck is this?
		//		IContextInformation[] result= new IContextInformation[5];
		//		for (int i= 0; i < result.length; i++)
		//			result[i]= new ContextInformation(
		//				MessageFormat.format(NedEditorMessages.getString("CompletionProcessor.ContextInfo.display.pattern"), new Object[] { new Integer(i), new Integer(documentOffset) }),
		//				MessageFormat.format(NedEditorMessages.getString("CompletionProcessor.ContextInfo.value.pattern"), new Object[] { new Integer(i), new Integer(documentOffset - 5), new Integer(documentOffset + 5)}));
		//		return result;
		return null;
	}

	public char[] getCompletionProposalAutoActivationCharacters() {
		return new char[] { '.', '@' };
	}

	public char[] getContextInformationAutoActivationCharacters() {
		return new char[] { '(' };
	}

	public IContextInformationValidator getContextInformationValidator() {
		return fValidator;
	}

	public String getErrorMessage() {
		return null;
	}
}
