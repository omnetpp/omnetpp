/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.contentassist;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.contentassist.ContentProposalEx;
import org.omnetpp.common.contentassist.ContentProposalProvider;
import org.omnetpp.common.engine.PatternMatcher;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.IReadonlyInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.ParamResolution;
import org.omnetpp.inifile.editor.model.ParamResolutionDisabledException;
import org.omnetpp.inifile.editor.model.ParamResolutionTimeoutException;
import org.omnetpp.inifile.editor.model.Timeout;

/**
 * Generate proposals for inifile module paths.
 *
 * Used for the text editor and field editors.
 *
 * @author Andras
 */
public class InifileModulePathContentProposalProvider extends ContentProposalProvider {
    private String section;
    private boolean addEqualSign = false;
    //private IReadonlyInifileDocument doc;
    private InifileAnalyzer analyzer;

    public InifileModulePathContentProposalProvider(String section, boolean addEqualSign, IReadonlyInifileDocument doc, InifileAnalyzer analyzer) {
        super(true);
        this.section = section;
        this.addEqualSign = addEqualSign;
        //this.doc = doc;
        this.analyzer = analyzer;
    }

    public void configure(String section, boolean addEqualSign) {
        this.section = section;
        this.addEqualSign = addEqualSign;
    }

    /**
     * Generate a list of proposal candidates. They will be sorted and filtered by prefix
     * before presenting them to the user.
     */
    @Override
    protected List<IContentProposal> getProposalCandidates(String prefix) {
        if (section != null) {
            Set<String> fullPaths = new HashSet<String>();

            try {
                // collect unique param resolution full paths
                //ParamResolution[] paramResolutions = analyzer.getUnassignedParams(section);
                ParamResolution[] paramResolutions = analyzer.getParamResolutions(section, new Timeout(InifileEditor.CONTENTASSIST_TIMEOUT));
                for (ParamResolution res : paramResolutions)
                    if (res.type != ParamResolution.ParamResolutionType.NED)
                        fullPaths.add(res.fullPath);
            } catch (ParamResolutionDisabledException e) {
                // do not offer proposals depend on analysis
            } catch (ParamResolutionTimeoutException e) {
                // do not offer proposals depend on analysis
                fullPaths.clear();
            }

            Set<String> moduleProposals = new HashSet<String>();
            Set<String> otherProposals = new HashSet<String>();

            // find last "**" or "." ==> (part1,rest)
            int lastDotPos = prefix.lastIndexOf('.');
            int lastXXPos = prefix.lastIndexOf("**");
            int restPos = Math.max(lastDotPos==-1 ? 0 : lastDotPos+1, lastXXPos==-1 ? 0 : lastXXPos+2);
            String prefixPart1 = prefix.substring(0, restPos);  // if no dot or **, prefixPart1 will be ""
            String prefixRest = prefix.substring(restPos);
            //Debug.println("prefix: "+prefixPart1+" + "+prefixRest);

            // after "*" or "**" we'll want to add an extra dot
            String optDot = prefix.endsWith("*") ? "." : "";

            // check every fullPath:
            PatternMatcher prefixPart1Matcher = new PatternMatcher(prefixPart1, true, true, true);
            for (String fullPath : fullPaths) {
                // split it into two at each dot ==> (part1,rest).
                // like: "" + "net.host.param", "net." + "host.param", "net.host." + "param"
                for (int dotPos = -2; dotPos != -1; dotPos = fullPath.indexOf('.', dotPos+1)) {
                    if (dotPos == -2) dotPos = -1; // get started
                    String fullPathPart1 = fullPath.substring(0, dotPos+1);
                    String fullPathRest = fullPath.substring(dotPos+1);
                    //Debug.println("fullPath: "+fullPathPart1+" + "+fullPathRest);

                    // prefix part1 should match fullPath part1, and fullPath rest must begin with prefix rest
                    if (prefixPart1Matcher.matches(fullPathPart1) && fullPathRest.startsWith(prefixRest)) {
                        //Debug.println("        suggesting: "+prefixPart1+fullPathRest+" =");
                        if (fullPathRest.contains(".")) {
                            // offer only the next submodule (i.e. rest up to the next dot)
                            int restDotPos = fullPathRest.indexOf('.');
                            String nextSubmodule = fullPathRest.substring(0, restDotPos+1);
                            moduleProposals.add(prefixPart1+optDot+nextSubmodule);
                        }
                        else {
                            // if rest contains no dot, then it is the last segment of fullpath
                            moduleProposals.add(prefixPart1+optDot+fullPathRest);
                        }
                    }
                }
            }

            // add ** and * as proposals, if it looks appropriate
            if (!prefixPart1.endsWith("*") && !prefixPart1.endsWith("*") && !moduleProposals.isEmpty()) {
                String prefixPart1dot = (prefixPart1.length()==0 || prefixPart1.endsWith(".")) ? prefixPart1 : (prefixPart1+".");
                otherProposals.add(prefixPart1dot+"*.");
                otherProposals.add(prefixPart1dot+"**.");
            }

            // convert strings to actual proposals, and return them. Each group will be sorted separately
            List<IContentProposal> proposals = new ArrayList<IContentProposal>();
            addProposals(proposals, otherProposals, "", null);
            addProposals(proposals, moduleProposals, "", InifileUtils.ICON_PROPOSAL_MODULE);

            return proposals;
        }

        return null;
    }

    private void addProposals(List<IContentProposal> proposals, Set<String> texts, String label, Image image) {
        if (!label.equals(""))
            label = " -- " + label;
        String[] array = texts.toArray(new String[]{});
        Arrays.sort(array);
        for (String text : array) {
            if (!text.endsWith(".") && addEqualSign)
                text += " = ";
            proposals.add(new ContentProposalEx(text, text+label, null, image));
        }
    }
}