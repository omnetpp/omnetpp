/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.text.highlight;


import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.rules.*;
import org.omnetpp.inifile.editor.text.InifileTextEditorHelper;

/**
 * A rule based scanner for inifile comments.
 */
public class InifileCommentColorizerScanner extends RuleBasedScanner {
    /**
     * Constructor.
     */
     public InifileCommentColorizerScanner() {
        // this is the default token for a comment
        setDefaultReturnToken(InifileTextEditorHelper.commentToken);

        List<IRule> list = new ArrayList<IRule>();
        // add rules if needed...
        setRules(list.toArray(new IRule[]{}));
    }
}
