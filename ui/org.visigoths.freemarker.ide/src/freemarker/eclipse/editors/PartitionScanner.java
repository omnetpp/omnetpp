/*
 * Copyright (c) 2003 The Visigoth Software Society. All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution, if
 *    any, must include the following acknowledgement:
 *       "This product includes software developed by the
 *        Visigoth Software Society (http://www.visigoths.org/)."
 *    Alternately, this acknowledgement may appear in the software 
 *    itself, if and wherever such third-party acknowledgements 
 *    normally appear.
 *
 * 4. Neither the name "FreeMarker", "Visigoth", nor any of the names 
 *    of the project contributors may be used to endorse or promote 
 *    products derived from this software without prior written 
 *    permission. For written permission, please contact 
 *    visigoths@visigoths.org.
 *
 * 5. Products derived from this software may not be called 
 *    "FreeMarker" or "Visigoth" nor may "FreeMarker" or "Visigoth" 
 *    appear in their names without prior written permission of the 
 *    Visigoth Software Society.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE VISIGOTH SOFTWARE SOCIETY OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Visigoth Software Society. For more
 * information on the Visigoth Software Society, please see
 * http://www.visigoths.org/
 */

package freemarker.eclipse.editors;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.rules.IPredicateRule;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.MultiLineRule;
import org.eclipse.jface.text.rules.RuleBasedPartitionScanner;
import org.eclipse.jface.text.rules.Token;

/**
 * The FreeMarker template partition scanner. This class separates
 * the text in the template into a number of non-overlapping
 * partitions. Each of the partitions can then apply their own
 * scanners and damage-repair policies within thier boundaries. This
 * two-stage scanning is done to simplify the scanning in each stage,
 * and also to minimize repetitive parsing. 
 * 
 * @version $Id: PartitionScanner.java,v 1.9 2003/12/01 00:57:31 cederberg Exp $
 * @author <a href="mailto:stephan@chaquotay.net">Stephan Mueller</a>
 */
public class PartitionScanner extends RuleBasedPartitionScanner {

	public final static String FTL_COMMENT = "__ftl_comment";
	public final static String FTL_DIRECTIVE = "__ftl_directive";
	public final static String FTL_INTERPOLATION = "__ftl_interpolation";
    public final static String XML_TAG = "__xml_tag";
    public final static String XML_COMMENT = "__xml_comment";

    /**
     * The array of partitions used.
     */
    public final static String[] PARTITIONS = {
        IDocument.DEFAULT_CONTENT_TYPE,
        FTL_COMMENT,
        FTL_DIRECTIVE,
        FTL_INTERPOLATION,
        XML_TAG,
        XML_COMMENT
    };

    /**
     * Creates a new partition scanner.
     */
	public PartitionScanner() {
		List rules = new ArrayList();

		IToken ftlComment = new Token(FTL_COMMENT);
		IToken ftlDirective = new Token(FTL_DIRECTIVE);
        IToken ftlInterpolation = new Token(FTL_INTERPOLATION);
        IToken xmlComment = new Token(XML_COMMENT);
        IToken xmlTag = new Token(XML_TAG);      

        rules.add(new MultiLineRule("<#--", "-->", ftlComment));
		rules.add(new DirectiveRule(ftlDirective));
        rules.add(new InterpolationRule(ftlInterpolation));
        rules.add(new XmlCommentRule(xmlComment));
        rules.add(new XmlRule(xmlTag));

		IPredicateRule[] result= new IPredicateRule[rules.size()];
		rules.toArray(result);
		setPredicateRules(result);
	}
}
