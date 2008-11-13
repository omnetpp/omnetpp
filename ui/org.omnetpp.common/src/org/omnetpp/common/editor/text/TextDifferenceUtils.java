package org.omnetpp.common.editor.text;

import java.util.Arrays;
import java.util.Comparator;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.ui.internal.texteditor.quickdiff.compare.rangedifferencer.IRangeComparator;
import org.eclipse.ui.internal.texteditor.quickdiff.compare.rangedifferencer.RangeDifference;
import org.eclipse.ui.internal.texteditor.quickdiff.compare.rangedifferencer.RangeDifferencer;
import org.omnetpp.common.util.StringUtils;

/**
 * @author levy
 */
public class TextDifferenceUtils {
	public interface ITextDifferenceApplier {
		void replace(int start, int end, String replacement);
	}

	public static void modifyTextEditorContentByApplyingDifferences(final IDocument document, String newText) {
		applyTextDifferences(document.get(), newText, new ITextDifferenceApplier() {
			public void replace(int start, int end, String replacement) {
				try {
					int numberOfLines = document.getNumberOfLines();
					int startOffset = document.getLineOffset(start);
					int endOffset = end == numberOfLines ? document.getLength() : document.getLineOffset(end);
					
					document.replace(startOffset, endOffset - startOffset, replacement);
				}
				catch (BadLocationException e) {
					throw new RuntimeException(e);
				}
			}
		});

		Assert.isTrue(document.get().equals(newText));
	}

	public static void applyTextDifferences(String original, String target, ITextDifferenceApplier applier) {
		LineRangeComparator comparatorOriginal = new LineRangeComparator(original);
		LineRangeComparator comparatorTarget = new LineRangeComparator(target);
		RangeDifference[] differences = RangeDifferencer.findDifferences(comparatorOriginal, comparatorTarget);

		Arrays.sort(differences, 0, differences.length, new Comparator<RangeDifference>() {
			public int compare(RangeDifference o1, RangeDifference o2) {
				return o1.leftStart() - o2.leftStart();
			}}
		);

		//TODO see IDocumentExtension4 which introduces "rewrite sessions"! --Andras
		int offset = 0;
		for (int i = 0 ; i < differences.length; i++) {
			RangeDifference difference = differences[i];
			//Debug.println(difference);

			int leftStart = difference.leftStart();
			int leftEnd = difference.leftEnd();
			int rightStart = difference.rightStart();
			int rightEnd = difference.rightEnd();

			String replacement = comparatorTarget.getLineRange(rightStart, rightEnd);
			applier.replace(offset + leftStart, offset + leftEnd, replacement);
			offset += difference.rightLength() - difference.leftLength();
		}
	}
}

class LineRangeComparator implements IRangeComparator {
	private String text;

	private String[] lines;
	
	public LineRangeComparator(String text) {
		this.text = text;
		lines = StringUtils.splitToLines(text);
	}
	
	public String getText() {
		return text;
	}

	public int getRangeCount() {
		return lines.length;
	}

	public boolean rangesEqual(int thisIndex, IRangeComparator other, int otherIndex) {
		return getLineAt(thisIndex).equals(((LineRangeComparator)other).getLineAt(otherIndex));
	}

	public String getLineAt(int index) {
		return lines[index];
	}

	public String getLineRange(int start, int end) {
		return StringUtils.join(ArrayUtils.subarray(lines, start, end));
	}

	public boolean skipRangeComparison(int length, int maxLength, IRangeComparator other) {
		return false;
	}	
}

class StringDifferenceApplier implements TextDifferenceUtils.ITextDifferenceApplier {
	private String text;
	
	public StringDifferenceApplier(String text) {
		this.text = text;
	}

	public String getText() {
		return text;
	}

	public void replace(int start, int end, String replacement) {
		String[] lines = StringUtils.splitToLines(text);
		text =
			StringUtils.join(ArrayUtils.subarray(lines, 0, start)) +
			replacement +
			StringUtils.join(ArrayUtils.subarray(lines, end, lines.length));
	}
}
