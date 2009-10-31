package com.swtworkbench.community.xswt.editors;

import org.eclipse.jface.text.ITextDoubleClickStrategy;
import org.eclipse.jface.text.TextAttribute;
import org.eclipse.jface.text.presentation.IPresentationReconciler;
import org.eclipse.jface.text.presentation.PresentationReconciler;
import org.eclipse.jface.text.rules.DefaultDamagerRepairer;
import org.eclipse.jface.text.rules.Token;
import org.eclipse.jface.text.source.ISourceViewer;
import org.eclipse.jface.text.source.SourceViewerConfiguration;

public class XMLConfiguration extends SourceViewerConfiguration {
    private XMLDoubleClickStrategy doubleClickStrategy;
    private XMLTagScanner tagScanner;
    private XMLScanner scanner;
    private ColorManager colorManager;

    public XMLConfiguration(ColorManager colorManager) {
        this.colorManager = colorManager;
    }

    public String[] getConfiguredContentTypes(ISourceViewer sourceViewer) {
        return new String[] { "__dftl_partition_content_type", "__xml_comment", "__xml_tag" };
    }

    public ITextDoubleClickStrategy getDoubleClickStrategy(ISourceViewer sourceViewer, String contentType) {
        if (this.doubleClickStrategy == null)
            this.doubleClickStrategy = new XMLDoubleClickStrategy();
        return this.doubleClickStrategy;
    }

    protected XMLScanner getXMLScanner() {
        if (this.scanner == null) {
            this.scanner = new XMLScanner(this.colorManager);
            this.scanner.setDefaultReturnToken(new Token(
                    new TextAttribute(this.colorManager.getColor(IXMLColorConstants.DEFAULT))));
        }
        return this.scanner;
    }

    protected XMLTagScanner getXMLTagScanner() {
        if (this.tagScanner == null) {
            this.tagScanner = new XMLTagScanner(this.colorManager);
            this.tagScanner
                    .setDefaultReturnToken(new Token(new TextAttribute(this.colorManager.getColor(IXMLColorConstants.TAG))));
        }
        return this.tagScanner;
    }

    public IPresentationReconciler getPresentationReconciler(ISourceViewer sourceViewer) {
        PresentationReconciler reconciler = new PresentationReconciler();

        DefaultDamagerRepairer dr = new DefaultDamagerRepairer(getXMLTagScanner());
        reconciler.setDamager(dr, "__xml_tag");
        reconciler.setRepairer(dr, "__xml_tag");

        dr = new DefaultDamagerRepairer(getXMLScanner());
        reconciler.setDamager(dr, "__dftl_partition_content_type");
        reconciler.setRepairer(dr, "__dftl_partition_content_type");

        NonRuleBasedDamagerRepairer ndr = new NonRuleBasedDamagerRepairer(new TextAttribute(this.colorManager
                .getColor(IXMLColorConstants.XML_COMMENT)));
        reconciler.setDamager(ndr, "__xml_comment");
        reconciler.setRepairer(ndr, "__xml_comment");

        return reconciler;
    }
}