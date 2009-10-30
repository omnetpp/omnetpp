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

import org.eclipse.jface.text.DocumentEvent;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITypedRegion;
import org.eclipse.jface.text.presentation.IPresentationReconciler;
import org.eclipse.jface.text.presentation.PresentationReconciler;
import org.eclipse.jface.text.reconciler.IReconciler;
import org.eclipse.jface.text.reconciler.MonoReconciler;
import org.eclipse.jface.text.rules.DefaultDamagerRepairer;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.ITokenScanner;
import org.eclipse.jface.text.rules.RuleBasedScanner;
import org.eclipse.jface.text.source.IAnnotationHover;
import org.eclipse.jface.text.source.ISourceViewer;
import org.eclipse.jface.text.source.SourceViewerConfiguration;

import freemarker.eclipse.FreemarkerPlugin;
import freemarker.eclipse.preferences.IPreferenceConstants;

/**
 * The FreeMarker editor configuration. This class holds instances of
 * various objects related to a FreeMarker template editor. The most
 * important of these objects is the presentation reconciler, that is
 * the object that handles all the coloring (and color repairs) of
 * the editor text.
 * 
 * @version $Id: Configuration.java,v 1.10 2004/02/05 00:16:23 stephanmueller Exp $
 * @author <a href="mailto:stephan@chaquotay.net">Stephan Mueller</a>
 * @author <a href="mailto:per&#64;percederberg.net">Per Cederberg</a>
 */
public class Configuration extends SourceViewerConfiguration
    implements IPreferenceConstants {

    private DirectiveScanner directiveScanner;
    private RuleBasedScanner commentScanner;
    private RuleBasedScanner interpolationScanner;
    private RuleBasedScanner defaultScanner;
    private RuleBasedScanner xmlCommentScanner;
    private XmlScanner xmlTagScanner;
    private ITokenManager tokenManager;
    private FreemarkerEditor fEditor;

    /**
     * Creates a new editor configuration. A new configuration
     * instance is created for each and every editor, i.e. for each
     * FreeMarker template opened for editing.
     *
     * @param anEditor       the editor object
     * @param colorManager   the color manager
     */
    public Configuration(FreemarkerEditor anEditor, ITokenManager tokenManager) {
        this.fEditor = anEditor;
        this.tokenManager = tokenManager;
    }

    /*
     * (non-Javadoc)
     * @see org.eclipse.jface.text.source.SourceViewerConfiguration#getConfiguredContentTypes(org.eclipse.jface.text.source.ISourceViewer)
     */
    public String[] getConfiguredContentTypes(ISourceViewer sourceViewer) {
        return PartitionScanner.PARTITIONS;
    }

    /*
     * (non-Javadoc)
     * @see org.eclipse.jface.text.source.SourceViewerConfiguration#getAnnotationHover(org.eclipse.jface.text.source.ISourceViewer)
     */
    public IAnnotationHover getAnnotationHover(ISourceViewer aSourceViewer) {
        return new AnnotationHover();
    }

    /*
     * (non-Javadoc)
     * @see org.eclipse.jface.text.source.SourceViewerConfiguration#getReconciler(org.eclipse.jface.text.source.ISourceViewer)
     */
    public IReconciler getReconciler(ISourceViewer aSourceViewer) {
        return new MonoReconciler(fEditor.getReconcilingStrategy(), false);
    }

    /**
     * Returns the normal text scanner. This scanner is used to color
     * normal text in templates (i.e. everything that is neither XML
     * nor FreeMarker syntax).
     *
     * @return the normal text scanner
     */
    protected ITokenScanner getDefaultScanner() {
        IToken  token;

        if (defaultScanner == null) {
            defaultScanner = new RuleBasedScanner();
            token = tokenManager.getTextToken();
            defaultScanner.setDefaultReturnToken(token);
        }
        return defaultScanner;
    }

    /**
     * Returns the directive scanner. This scanner is used to color
     * FreeMarker directives and macros.
     *
     * @return the directive scanner
     */
    protected DirectiveScanner getDirectiveScanner() {
        IToken  token;

        if (directiveScanner == null) {
            directiveScanner = new DirectiveScanner(tokenManager);
            token = tokenManager.getDirectiveToken();
            directiveScanner.setDefaultReturnToken(token);
        }
        return directiveScanner;
    }

    /**
     * Returns the comment scanner. This scanner is used to color
     * FreeMarker comments.
     *
     * @return the comment scanner
     */
    protected ITokenScanner getCommentScanner() {
        IToken  token;

        if (commentScanner == null) {
            commentScanner = new RuleBasedScanner();
            token = tokenManager.getCommentToken();
            commentScanner.setDefaultReturnToken(token);
        }
        return commentScanner;
    }

    /**
     * Returns the interpolation scanner. This scanner is used to
     * color FreeMarker interpolations.
     *
     * @return the interpolation scanner
     */
    protected ITokenScanner getInterpolationScanner() {
        IToken  token;

        if (interpolationScanner == null) {
            interpolationScanner = new RuleBasedScanner();
            token = tokenManager.getInterpolationToken();
            interpolationScanner.setDefaultReturnToken(token);
        }
        return interpolationScanner;
    }

    /**
     * Returns the XML comment scanner. This scanner is used to color
     * XML comments.
     *
     * @return the XML comment scanner
     */
    protected ITokenScanner getXmlCommentScanner() {
        IToken  token;

        if (xmlCommentScanner == null) {
            xmlCommentScanner = new XmlCommentScanner(tokenManager);
            token = tokenManager.getXmlCommentToken();
            xmlCommentScanner.setDefaultReturnToken(token);
        }
        return xmlCommentScanner;
    }

    /**
     * Returns the XML tag scanner. This scanner is used to color
     * XML tags.
     *
     * @return the XML tag scanner
     */
    protected XmlScanner getXmlTagScanner() {
        IToken  token;

        if (xmlTagScanner == null) {
            xmlTagScanner = new XmlScanner(tokenManager);
            token = tokenManager.getXmlToken();
            xmlTagScanner.setDefaultReturnToken(token);
        }
        return xmlTagScanner;
    }

    /*
     * (non-Javadoc)
     * @see org.eclipse.jface.text.source.SourceViewerConfiguration#getPresentationReconciler(org.eclipse.jface.text.source.ISourceViewer)
     */
    public IPresentationReconciler getPresentationReconciler(ISourceViewer sourceViewer) {
        PresentationReconciler reconciler = new PresentationReconciler();
        DefaultDamagerRepairer dr;
        
        boolean xmlHighlighting = true;
        try {
        	xmlHighlighting = FreemarkerPlugin.getInstance().getPreferenceStore().getBoolean(XML_HIGHLIGHTING);
        } catch (NullPointerException npe) {
        	
        }

        dr = new DefaultDamagerRepairer(getDefaultScanner());
        reconciler.setDamager(dr, IDocument.DEFAULT_CONTENT_TYPE);
        reconciler.setRepairer(dr, IDocument.DEFAULT_CONTENT_TYPE);

        dr = new DefaultDamagerRepairer(getDirectiveScanner());
        reconciler.setDamager(dr, PartitionScanner.FTL_DIRECTIVE);
        reconciler.setRepairer(dr, PartitionScanner.FTL_DIRECTIVE);

        dr = new DefaultDamagerRepairer(getCommentScanner());
        reconciler.setDamager(dr, PartitionScanner.FTL_COMMENT);
        reconciler.setRepairer(dr, PartitionScanner.FTL_COMMENT);

        dr = new DefaultDamagerRepairer(getInterpolationScanner());
        reconciler.setDamager(dr, PartitionScanner.FTL_INTERPOLATION);
        reconciler.setRepairer(dr, PartitionScanner.FTL_INTERPOLATION);

        if(xmlHighlighting) {
            dr = new DefaultDamagerRepairer(getXmlCommentScanner());
            reconciler.setDamager(dr, PartitionScanner.XML_COMMENT);
            reconciler.setRepairer(dr, PartitionScanner.XML_COMMENT);
        
            dr = new SimpleDamagerRepairer(getXmlTagScanner());
            reconciler.setDamager(dr, PartitionScanner.XML_TAG);
            reconciler.setRepairer(dr, PartitionScanner.XML_TAG);
        }
        
        return reconciler;
    }


    /**
     * A simple presentation damager and repairer. This only differs
     * from the default damager and repairer by marking the whole 
     * partition as damaged upon every change. This is not efficient 
     * for large partitions, but allows the usage of stateful 
     * scanners within a partition.
     *
     * @version $Id: Configuration.java,v 1.10 2004/02/05 00:16:23 stephanmueller Exp $
     * @author <a href="mailto:per&#64;percederberg.net">Per Cederberg</a>
     */
    private class SimpleDamagerRepairer extends DefaultDamagerRepairer {

        /**
         * Creates a new simple damager and repairer.
         * 
         * @param scanner        the lexical scanner to use
         */
        public SimpleDamagerRepairer(ITokenScanner scanner) {
            super(scanner);
        }
        
        /**
         * Returns the region damaged in the document presentation.
         * The damage region returned by this method is always the 
         * whole partition.
         *
         * @param partition      the document partition
         * @param e              the document change event
         * @param documentPartitioningChanged a flag set if the 
         *                       partitioning was changed
         */
        public IRegion getDamageRegion(ITypedRegion partition,
                                       DocumentEvent e,
                                       boolean documentPartitioningChanged) {

            return partition;
        }
    }
}