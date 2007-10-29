package com.simulcraft.test.gui.access;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import junit.framework.Assert;

import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.graphics.Point;
import org.omnetpp.common.util.StringUtils;

import com.simulcraft.test.gui.core.UIStep;

public class StyledTextAccess extends CompositeAccess
{
	public StyledTextAccess(StyledText styledText) {
		super(styledText);
	}

    @Override
	public StyledText getControl() {
		return (StyledText)widget;
	}

	@UIStep
	public String getTextContent() {
		return getControl().getText();
	}

	@UIStep
	public void moveCursorAfter(String patternString) {
	    int targetOffset = getOffsetAfter(patternString);
	    reveal(targetOffset);
		Point loc = getControl().toDisplay(getControl().getLocationAtOffset(targetOffset));
		clickAbsolute(LEFT_MOUSE_BUTTON, loc);
	}
	
	@UIStep
	public void reveal(int offset) {
	    // FIXME it moves the offset at the top. it would be enough to show only on the screen
	    // need to scroll horizontally as well
	    int lineAtOffset = getControl().getLineAtOffset(offset);
        getControl().setTopIndex(lineAtOffset);
	}

    @UIStep
    public int getOffsetAfter(String patternString) {
        String text = getTextContent();
		Pattern pattern = Pattern.compile(".*(" + patternString + ").*", Pattern.DOTALL);
		Matcher matcher = pattern.matcher(text);
		boolean matches = matcher.matches();
		Assert.assertTrue("cannot find "+patternString+" in styled text",matches);
		int targetOffset = matcher.end(1);
		matcher.region(matcher.end(), text.length());
		Assert.assertFalse("more than one match for "+patternString, matcher.matches());
        return targetOffset;
    }
    
    /**
     * Checks StyledText contents with String.equals(). Note: the StyledText
     * control does NOT contain the content of collapsed folding regions.
     */
    @UIStep
    public void assertContent(String expectedContent) {
        String actualContent = getTextContent();
        Assert.assertTrue("StyledText content does not match", actualContent.equals(expectedContent));
    }

    /**
     * Checks StyledText contents after normalizing all whitespace. Note: 
     * the StyledText control does NOT contain the content of collapsed 
     * folding regions.
     */
    @UIStep
    public void assertContentIgnoringWhiteSpace(String expectedContent) {
        String actualContent = getTextContent();
        Assert.assertTrue("StyledText content does not match", StringUtils.areEqualIgnoringWhiteSpace(actualContent, expectedContent));
    }
    
    /**
     * Checks StyledText contents with regex match. Note: the StyledText
     * control does NOT contain the content of collapsed folding regions.
     */
    @UIStep
    public void assertContentMatches(String regex) {
        String actualContent = getTextContent();
        Assert.assertTrue("StyledText content does not match", actualContent.matches(regex));
    }

}
