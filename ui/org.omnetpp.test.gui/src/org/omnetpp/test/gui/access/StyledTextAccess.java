package org.omnetpp.test.gui.access;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.StyledText;
import org.omnetpp.test.gui.core.InUIThread;

public class StyledTextAccess extends CompositeAccess
{
	public StyledTextAccess(StyledText styledText) {
		super(styledText);
	}

	public StyledText getStyledText() {
		return (StyledText)widget;
	}

	@InUIThread
	public String getText() {
		return getStyledText().getText();
	}

	@InUIThread
	public void moveCursorAfter(String patternString) {
		String text = getText();
		Pattern pattern = Pattern.compile(".*(" + patternString + ").*", Pattern.DOTALL);
		Matcher matcher = pattern.matcher(text);
		boolean matches = matcher.matches();
		Assert.isTrue(matches);
		int targetOffset = matcher.end(1);
		matcher.region(matcher.end(), text.length());
		Assert.isTrue(!matcher.matches());
		
		int currentOffset = getStyledText().getCaretOffset();
		
		for (int i = 0; i < targetOffset - currentOffset; i++)
			pressKey(SWT.ARROW_RIGHT);
	}
}
