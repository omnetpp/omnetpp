package org.omnetpp.scave.editors.ui;

import org.eclipse.jface.fieldassist.ControlDecoration;
import org.eclipse.jface.fieldassist.FieldDecoration;
import org.eclipse.jface.fieldassist.FieldDecorationRegistry;
import org.eclipse.jface.fieldassist.SimpleContentProposalProvider;
import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.omnetpp.scave.engine.ResultItemFields;

/**
 * Adds control decoration and content proposals to a text control.
 *
 * @author tomi
 */
public class ResultItemNamePatternField {
	
	public ResultItemNamePatternField(Text text) {
		this(text, ResultItemFields.getFieldNames().toArray());
	}

	public ResultItemNamePatternField(Text text, String[] fieldNames) {
		FieldDecoration contentAssistDecoration = FieldDecorationRegistry.getDefault().getFieldDecoration(FieldDecorationRegistry.DEC_CONTENT_PROPOSAL);
		ControlDecoration decorator = new ControlDecoration(text, SWT.TOP | SWT.LEFT, text.getParent());
		decorator.setImage(contentAssistDecoration.getImage());
		decorator.setDescriptionText(contentAssistDecoration.getDescription());

		String[] proposals = new String[fieldNames.length];
		for (int i = 0; i < proposals.length; ++i)
			proposals[i] = "{" + fieldNames[i] + "}";
		SimpleContentProposalProvider proposalProvider = new SimpleContentProposalProvider(proposals);
		
		new ContentAssistCommandAdapter(text,
				new TextContentAdapter(),
				proposalProvider,
				ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS,
				" ".toCharArray() /*auto-activation*/);
	}
}
