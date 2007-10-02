package com.simulcraft.test.gui.access;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.graphics.Point;

import com.simulcraft.test.gui.core.InUIThread;

public class StyledTextAccess extends CompositeAccess
{
	public StyledTextAccess(StyledText styledText) {
		super(styledText);
	}

	public StyledText getStyledText() {
		return (StyledText)widget;
	}

	@InUIThread
	public String getTextContent() {
		return getStyledText().getText();
	}

	@InUIThread
	public void moveCursorAfter(String patternString) {
	    int targetOffset = getOffsetAfter(patternString);
	    reveal(targetOffset);
		Point loc = getStyledText().toDisplay(getStyledText().getLocationAtOffset(targetOffset));
		clickAbsolute(LEFT_MOUSE_BUTTON, loc);
	}
	
	@InUIThread
	public void reveal(int offset) {
	    // FIXME it moves the offset at the top. it would be enough to show only on the screen
	    // need to scroll horizontally as well
	    int lineAtOffset = getStyledText().getLineAtOffset(offset);
        getStyledText().setTopIndex(lineAtOffset);
	}

    @InUIThread
    public int getOffsetAfter(String patternString) {
        String text = getTextContent();
		Pattern pattern = Pattern.compile(".*(" + patternString + ").*", Pattern.DOTALL);
		Matcher matcher = pattern.matcher(text);
		boolean matches = matcher.matches();
		Assert.isTrue(matches);
		int targetOffset = matcher.end(1);
		matcher.region(matcher.end(), text.length());
		Assert.isTrue(!matcher.matches());
        return targetOffset;
    }
}
