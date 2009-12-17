/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.text.assist;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.contentassist.ICompletionProposal;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.texteditor.ITextEditor;
import org.omnetpp.common.editor.text.NedCompletionHelper;
import org.omnetpp.ned.core.NEDResources;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;

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
public class NedCompletionProcessor extends AbstractNedCompletionProcessor {
    public NedCompletionProcessor(ITextEditor editor) {
        super(editor);
	}

	@Override
    public ICompletionProposal[] computeCompletionProposals(ITextViewer viewer, int documentOffset) {
		// long startMillis = System.currentTimeMillis(); // measure time

		List<ICompletionProposal> result = new ArrayList<ICompletionProposal>();

		// find out where we are: in which module, submodule, which section etc.
		CompletionInfo info = computeCompletionInfo(viewer, documentOffset);
		// if the position is invalid return no proposals
		if (info == null || info.linePrefix == null || info.linePrefixTrimmed == null)
		    return new ICompletionProposal[0];

		NEDResources res = NEDResourcesPlugin.getNEDResources();
		IFile file = ((IFileEditorInput)editor.getEditorInput()).getFile();
		NedFileElementEx nedFileElement = res.getNedFileElement(file);
		IProject project = file.getProject();

		String line = info.linePrefixTrimmed;

		// calculate the lookup context used in nedresource calls
		INedTypeLookupContext context = nedFileElement;
		INEDTypeInfo nedTypeInfo = null;
		if (info.nedTypeName!=null) {
		    nedTypeInfo = res.lookupNedType(info.nedTypeName, context);
		    if (nedTypeInfo != null && nedTypeInfo.getNEDElement() instanceof CompoundModuleElementEx)
		        context = (CompoundModuleElementEx)nedTypeInfo.getNEDElement();
		}

		INEDTypeInfo nedEnclosingTypeInfo = null;
		if (info.enclosingNedTypeName != null) { // we are inside an inner type
		    nedEnclosingTypeInfo = res.lookupNedType(info.enclosingNedTypeName, nedFileElement);
            if (nedEnclosingTypeInfo != null && nedEnclosingTypeInfo.getNEDElement() instanceof CompoundModuleElementEx)
                context = (CompoundModuleElementEx)nedEnclosingTypeInfo.getNEDElement();
		}

		INEDTypeInfo submoduleType = null;
		if (info.submoduleTypeName!=null) {
			submoduleType = res.lookupNedType(info.submoduleTypeName, context);
		}

		if (info.sectionType==SECT_GLOBAL || info.sectionType==SECT_TYPES)
		{
			// Debug.println("testing proposals for GLOBAL and TYPES scope");

			// match various "extends" and "like" clauses and offer component types
			if (line.matches(".*\\bsimple .* extends"))
			    addNedTypeProposals(viewer, documentOffset, result, project, nedTypeInfo, NEDResources.SIMPLE_MODULE_FILTER);
			else if (line.matches(".*\\b(module|network) .* extends"))
                addNedTypeProposals(viewer, documentOffset, result, project, nedTypeInfo, NEDResources.COMPOUND_MODULE_FILTER);
			else if (line.matches(".*\\bchannel .* extends"))
                addNedTypeProposals(viewer, documentOffset, result, project, nedTypeInfo, NEDResources.CHANNEL_FILTER);
			else if (line.matches(".*\\bmoduleinterface .* extends"))
                addNedTypeProposals(viewer, documentOffset, result, project, nedTypeInfo, NEDResources.MODULEINTERFACE_FILTER);
			else if (line.matches(".*\\bchannelinterface .* extends"))
                addNedTypeProposals(viewer, documentOffset, result, project, nedTypeInfo, NEDResources.CHANNELINTERFACE_FILTER);

			// match "like" clauses
			if (line.matches(".*\\bsimple .* like") || line.matches(".*\\bsimple .* like .*,"))
                addNedTypeProposals(viewer, documentOffset, result, project, nedTypeInfo, NEDResources.MODULEINTERFACE_FILTER);
			else if (line.matches(".*\\b(module|network) .* like") || line.matches(".*\\b(module|network) .* like .*,"))
                addNedTypeProposals(viewer, documentOffset, result, project, nedTypeInfo, NEDResources.MODULEINTERFACE_FILTER);
			else if (line.matches(".*\\bchannel .* like") || line.matches(".*\\bchannel .* like .*,"))
                addNedTypeProposals(viewer, documentOffset, result, project, nedTypeInfo, NEDResources.CHANNELINTERFACE_FILTER);

			if (!line.equals("") && !line.matches(".*\\b(like|extends)\\b.*") && line.matches(".*\\b(simple|module|network|channel|interface|channelinterface)\\b [_A-Za-z0-9]+"))
				addProposals(viewer, documentOffset, result, new String[]{"extends "}, "keyword");

            if (!line.equals("") && line.matches(".*\\b(simple|module|network|channel)\\b [_A-Za-z0-9]+( extends [_A-Za-z0-9]+)?"))
                addProposals(viewer, documentOffset, result, new String[]{"like "}, "keyword");
		}

		// propose line start: param names, gate names, keywords
		if (line.equals("")) {
			// offer param and gate names
			if (info.sectionType == SECT_PARAMETERS && nedTypeInfo!=null)
				addProposals(viewer, documentOffset, result, nedTypeInfo.getParamDeclarations().keySet(), "parameter");
			if (info.sectionType == SECT_GATES && nedTypeInfo!=null)
				addProposals(viewer, documentOffset, result, nedTypeInfo.getGateDeclarations().keySet(), "gate");
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
			// Debug.println("proposals for expressions");

			// offer parameter names, gate names, types,...
			if (line.matches(".*\\bthis *\\.")) {
				if (submoduleType!=null)
					addProposals(viewer, documentOffset, result, submoduleType.getParamDeclarations().keySet(), "parameter");
			}
			else if (line.matches(".*\\bsizeof *\\(")) {
				if (nedTypeInfo!=null) {
                    // FIXME add only vector submodules and vectors
					addProposals(viewer, documentOffset, result, nedTypeInfo.getGateDeclarations().keySet(), "gate");
					addProposals(viewer, documentOffset, result, nedTypeInfo.getSubmodules().keySet(), "submodule");
				}
			}
	    	else if (line.endsWith(".")) {
	    		// after dot: offer params (and after sizeof(), gates too) of given submodule
	    		if (nedTypeInfo!=null) {
					String submodTypeName = extractSubmoduleTypeName(line, nedTypeInfo);
					if (submodTypeName != null) {
					    // Debug.println(" offering params of type "+submodTypeName);
					    INEDTypeInfo submodType = res.lookupNedType(submodTypeName, context);
					    if (submodType!=null) {
					        if (line.matches(".*\\bsizeof *\\(.*"))
					            addProposals(viewer, documentOffset, result, submodType.getGateDeclarations().keySet(), "gate");
					        addProposals(viewer, documentOffset, result, submodType.getParamDeclarations().keySet(), "parameter");
					    }
					}
	    		}
	    	}
			else {
				if (nedTypeInfo!=null) {
					addProposals(viewer, documentOffset, result, nedTypeInfo.getParamDeclarations().keySet(), "parameter");
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
            if (info.sectionType == SECT_GLOBAL ) {
                addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedFilePropertyTempl);
            }
            if (info.sectionType == SECT_PARAMETERS && nedTypeInfo!=null) {
                addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedComponentPropertyTempl);
                addProposals(viewer, documentOffset, result, "@", nedTypeInfo.getProperties().keySet(), "", "property");
            }
            if (info.sectionType == SECT_SUBMODULE_PARAMETERS && submoduleType!=null) {
                addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedSubmodulePropertyTempl);
                addProposals(viewer, documentOffset, result, "@", submoduleType.getProperties().keySet(), "", "property");
            }
        }
        else if ((line.contains("=") && !line.endsWith("=")) || !line.contains("=")) {
            if (info.sectionType == SECT_PARAMETERS || info.sectionType == SECT_SUBMODULE_PARAMETERS)
                addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedParamPropertyTempl);
            if (info.sectionType == SECT_GATES || info.sectionType == SECT_SUBMODULE_GATES)
                addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedGatePropertyTempl);
        }

		// complete submodule type name
		if (info.sectionType == SECT_SUBMODULES) {
			// Debug.println("testing proposals for SUBMODULES scope");
			if (line.matches(".*:")) {
			    if (nedEnclosingTypeInfo != null)    // we are inside an inner type (use the enclosing module' inner types)
	                addNedTypeProposals(viewer, documentOffset, result, project, nedEnclosingTypeInfo, NEDResources.MODULE_FILTER);
			    else  // top level type
			        addNedTypeProposals(viewer, documentOffset, result, project, nedTypeInfo, NEDResources.MODULE_FILTER);
			}
			else if (line.matches(".*: *<")) {  // "like" syntax
				if (nedTypeInfo!=null)
					addProposals(viewer, documentOffset, result, nedTypeInfo.getParamDeclarations().keySet(), "parameter");
			}
			else if (line.matches(".*: *<.*>")) {   // "like" syntax, cont'd
					addProposals(viewer, documentOffset, result, new String[]{" like "}, "keyword");
			}
			else if (line.matches(".*\\blike")) {
                if (nedEnclosingTypeInfo != null)    // we are inside an inner type (use the enclosing module' inner types)
                    addNedTypeProposals(viewer, documentOffset, result, project, nedEnclosingTypeInfo, NEDResources.MODULEINTERFACE_FILTER);
                else  // top level type
                    addNedTypeProposals(viewer, documentOffset, result, project, nedTypeInfo, NEDResources.MODULEINTERFACE_FILTER);
			}
		}

		if (info.sectionType == SECT_CONNECTIONS) {
			// Debug.println("testing proposals for CONNECTIONS scope");
			if (line.matches(".*\\bconnections")) {
				// user forgot "allowunconnected" keyword
				addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedConnsKeywords, "keyword");
			}

			if (line.equals("") || line.endsWith("-->") || line.endsWith("<-->") || line.endsWith("<--")) {
	    		// right at line start or after arrow: offer submodule names and parent module's gates
	    		if (nedTypeInfo!=null) {
	    			addProposals(viewer, documentOffset, result, nedTypeInfo.getSubmodules().keySet(), "submodule");
	    			addProposals(viewer, documentOffset, result, nedTypeInfo.getGateDeclarations().keySet(), "gate");
	    			// only a single arrow can be present in the line to give channel assistance to
                    if (line.matches(".*--.*") && !line.matches(".*--.*--.*"))
                        addNedTypeProposals(viewer, documentOffset, result, project, nedTypeInfo, NEDResources.CHANNEL_FILTER);
	    		}
	    	}
	    	else if (line.endsWith(".")) {
	    		// after dot: offer gates of given submodule
	    		if (nedTypeInfo != null) {
	    		    String submodTypeName = extractSubmoduleTypeName(line, nedTypeInfo);
	    		    if (submodTypeName != null) {
	    		        // Debug.println(" offering gates of type "+submodTypeName);
	    		        INEDTypeInfo submodType = res.lookupNedType(submodTypeName, context);
	    		        if (submodType != null)
	    		            addProposals(viewer, documentOffset, result, submodType.getGateDeclarations().keySet(), "gate");
	    		    }
	    		}
	    	}

			// offer templates for connection, loop connection, connection with channel, etc
            if (line.equals(""))
                addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedConnectionTempl);
		}

		// long millis = System.currentTimeMillis()-startMillis;
		// Debug.println("Proposal creation: "+millis+"ms");

	    return (ICompletionProposal[]) result.toArray(new ICompletionProposal[result.size()]);
	}
}
