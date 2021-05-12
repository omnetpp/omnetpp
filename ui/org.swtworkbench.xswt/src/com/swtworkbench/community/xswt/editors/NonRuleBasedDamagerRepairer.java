package com.swtworkbench.community.xswt.editors;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.DocumentEvent;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITypedRegion;
import org.eclipse.jface.text.Region;
import org.eclipse.jface.text.TextAttribute;
import org.eclipse.jface.text.TextPresentation;
import org.eclipse.jface.text.presentation.IPresentationDamager;
import org.eclipse.jface.text.presentation.IPresentationRepairer;
import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.custom.StyleRange;

public class NonRuleBasedDamagerRepairer implements IPresentationDamager, IPresentationRepairer {
    protected IDocument fDocument;
    protected TextAttribute fDefaultTextAttribute;

    public NonRuleBasedDamagerRepairer(TextAttribute defaultTextAttribute) {
        Assert.isNotNull(defaultTextAttribute);

        this.fDefaultTextAttribute = defaultTextAttribute;
    }

    public void setDocument(IDocument document) {
        this.fDocument = document;
    }

    protected int endOfLineOf(int offset) throws BadLocationException {
        IRegion info = this.fDocument.getLineInformationOfOffset(offset);
        if (offset <= info.getOffset() + info.getLength()) {
            return (info.getOffset() + info.getLength());
        }
        int line = this.fDocument.getLineOfOffset(offset);
        try {
            info = this.fDocument.getLineInformation(line + 1);
            return (info.getOffset() + info.getLength());
        }
        catch (BadLocationException localBadLocationException) {
        }
        return this.fDocument.getLength();
    }

    public IRegion getDamageRegion(ITypedRegion partition, DocumentEvent event, boolean documentPartitioningChanged) {
        if (!(documentPartitioningChanged)) {
            try {
                IRegion info = this.fDocument.getLineInformationOfOffset(event.getOffset());
                int start = Math.max(partition.getOffset(), info.getOffset());

                int end = event.getOffset() + ((event.getText() == null) ? event.getLength() : event.getText().length());

                if ((info.getOffset() <= end) && (end <= info.getOffset() + info.getLength())) {
                    end = info.getOffset() + info.getLength();
                }
                else
                    end = endOfLineOf(end);

                end = Math.min(partition.getOffset() + partition.getLength(), end);
                return new Region(start, end - start);
            }
            catch (BadLocationException localBadLocationException) {
            }
        }
        return partition;
    }

    public void createPresentation(TextPresentation presentation, ITypedRegion region) {
        addRange(presentation, region.getOffset(), region.getLength(), this.fDefaultTextAttribute);
    }

    protected void addRange(TextPresentation presentation, int offset, int length, TextAttribute attr) {
        if (attr != null)
            presentation
                    .addStyleRange(new StyleRange(offset, length, attr.getForeground(), attr.getBackground(), attr.getStyle()));
    }
}