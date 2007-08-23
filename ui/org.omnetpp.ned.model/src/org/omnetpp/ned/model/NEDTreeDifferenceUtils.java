package org.omnetpp.ned.model;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.Random;

import org.eclipse.core.runtime.Assert;
import org.eclipse.ui.internal.texteditor.quickdiff.compare.rangedifferencer.IRangeComparator;
import org.eclipse.ui.internal.texteditor.quickdiff.compare.rangedifferencer.RangeDifference;
import org.eclipse.ui.internal.texteditor.quickdiff.compare.rangedifferencer.RangeDifferencer;
import org.omnetpp.common.editor.text.TextDifferenceUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.ex.NEDElementFactoryEx;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.pojo.CommentElement;
import org.omnetpp.ned.model.pojo.ConnectionElement;
import org.omnetpp.ned.model.pojo.NEDElementFactory;
import org.omnetpp.ned.model.pojo.NEDElementTags;


public class NEDTreeDifferenceUtils {
	/**
	 * Applies changes identified by the tree diff algorithm to the tree. 
	 */
	public interface IApplier {
		public void replaceSourceLocation(INEDElement element, String sourceLocation, NEDSourceRegion sourceRegion);

		public void replaceElements(INEDElement parent, int start, int end, int offset, INEDElement[] replacement);

		public void replaceAttribute(INEDElement element, String name, String value);
	}

	@SuppressWarnings("unchecked")
	public static void applyTreeDifferences(INEDElement original, INEDElement target, IApplier applier) {
		applyAttributeDifferences(original, target, applier);
		applier.replaceSourceLocation(original, target.getSourceLocation(), target.getSourceRegion());

		NEDElementChildrenComparator comparatorOriginal = new NEDElementChildrenComparator(original);
		NEDElementChildrenComparator comparatorTarget = new NEDElementChildrenComparator(target);
		List<RangeDifference> differences = RangeDifferencer.findRanges(comparatorOriginal, comparatorTarget);

		Collections.sort(differences, new Comparator<RangeDifference>() {
			public int compare(RangeDifference o1, RangeDifference o2) {
				return o1.leftStart() - o2.leftStart();
			}}
		);

		int offset = 0;
		for (RangeDifference difference : differences) {
//			System.out.println(difference);

			int leftStart = difference.leftStart();
			int leftEnd = difference.leftEnd();
			int rightStart = difference.rightStart();
			int rightEnd = difference.rightEnd();

			if (difference.kind() == RangeDifference.NOCHANGE) {
				for (int i = leftStart; i < leftEnd; i++)
					applyTreeDifferences(original.getChild(i) , target.getChild(rightStart + i - leftStart), applier);
			}
			else if (difference.kind() == RangeDifference.CHANGE) {
				INEDElement[] replacement = comparatorTarget.getElementRange(rightStart, rightEnd);
				applier.replaceElements(original, leftStart, leftEnd, offset, replacement);
				offset += difference.rightLength() - difference.leftLength();
			}
			else
				throw new RuntimeException("Unkown difference kind");
		}
	}

	private static void applyAttributeDifferences(INEDElement original, INEDElement target, IApplier applier) {
		int num = original.getNumAttributes();
		for (int j = 0; j < num; j++) {
			String originalAttribute = original.getAttribute(j);
			String targetAttribute = target.getAttribute(j);

			if (!StringUtils.equals(originalAttribute, targetAttribute))
				applier.replaceAttribute(original, target.getAttributeName(j), targetAttribute);
		}
	}

	/**
	 * The default implementation of NEDTreeDifferenceUtils.IApplier.
	 */
	public static class Applier implements IApplier {
		final ArrayList<Runnable> runnables = new ArrayList<Runnable>();
		
		// statistics, for debugging
		private int numSourceLocationChanges = 0;
		private int numAttributeChanges = 0;
		private int numElementReplacements = 0;

		public void replaceSourceLocation(final INEDElement element, final String sourceLocation, final NEDSourceRegion sourceRegion) {
			numSourceLocationChanges++;
			runnables.add(new Runnable() {
				public void run() {
					element.setSourceLocation(sourceLocation);
					element.setSourceRegion(sourceRegion);
				}
			});
		}
		
		public void replaceAttribute(final INEDElement element, final String name, final String value) {
			numAttributeChanges++;
			runnables.add(new Runnable() {
				public void run() {
					element.setAttribute(name, value);
				}
			});
		}

		public void replaceElements(final INEDElement parent, final int start, final int end, final int offset, final INEDElement[] replacement) {
			numElementReplacements++;
			runnables.add(new Runnable() {
				public void run() {
					for (int i = start; i < end; i++)
						parent.getChild(offset + start).removeFromParent();

					INEDElement startChild = parent.getChild(offset + start);

					for (INEDElement element : replacement)
						parent.insertChildBefore(startChild, element);
				}
			});
		}
		
		public void apply() {
			for (Runnable runnable : runnables)
				runnable.run();
		}

		public boolean hasDifferences() {
			return !runnables.isEmpty();
		}
		
		@Override
		public String toString() {
			return "element replacements: " + numElementReplacements +
				   ", attribute changes: " + numAttributeChanges +
			       ", source location changes: " + numSourceLocationChanges; 
		}
	}

	public static void main(String[] args) {
		new NEDTreeDifferenceTest().run();
	}
}

class NEDElementChildrenComparator implements IRangeComparator {
	private INEDElement parent;

	public NEDElementChildrenComparator(INEDElement parent) {
		this.parent = parent;
	}

	public int getRangeCount() {
		return parent.getNumChildren();
	}

	public boolean rangesEqual(int thisIndex, IRangeComparator other, int otherIndex) {
		// NOTE: In this tree diff algorithm, we return true iff two the nodes are 
		// the same type AND we want to synchronize them by changing differing attributes 
		// and recursively synchronizing child elements. I.e. this is the case when the
		// two nodes are submodules of the same *name* but potentially different
		// vector size etc and contents: we return true.  If we just want one to be thrown
		// out and simply replaced by the other, we return false.
		//
		INEDElement thisElement = getElementAt(thisIndex);
		INEDElement otherElement = ((NEDElementChildrenComparator)other).getElementAt(otherIndex);

		if (thisElement == null || otherElement == null || thisElement.getTagCode() != otherElement.getTagCode())
			return false;
		
		if (thisElement.getTagCode() == NEDElementTags.NED_CONNECTION) {
			ConnectionElement thisConnection = (ConnectionElement)thisElement;
			ConnectionElement otherConnection = (ConnectionElement)otherElement;
			
			String thisId = getConnectionId(thisConnection);
			String otherId = getConnectionId(otherConnection);

			return thisId.equals(otherId);
		}
		else if (thisElement instanceof IHasName) {
			// if they have the same name: synchronize (true), otherwise do remove+add (false)
			String thisName = ((IHasName)thisElement).getName();
			String otherName = ((IHasName)otherElement).getName();
			
			return StringUtils.equals(thisName, otherName);
		}
		else {
			// we assume they correspond to each other
			return true;
		}
	}

	private String getConnectionId(ConnectionElement connection) {
		return connection.getSrcModule() + connection.getSrcGate() + connection.getSrcGateIndex() +
			connection.getDestModule() + connection.getDestGate() + connection.getDestGateIndex();
	}

	public INEDElement getElementAt(int index) {
		return parent.getChild(index);
	}

	public INEDElement[] getElementRange(int start, int end) {
		INEDElement[] range = new INEDElement[end - start];
		for (int i = 0; i < range.length; i++)
			range[i] = getElementAt(start + i);
		return range;
	}

	public boolean skipRangeComparison(int length, int maxLength, IRangeComparator other) {
		return false;
	}
}

class NEDTreeDifferenceTest {
	private static Random random = new Random(1);
	
	public void run() {
		NEDElementFactoryEx.setInstance(new NEDElementFactoryEx());

		for (int i = 0; i < 100; i++) {
			INEDElement original = NEDTreeUtil.loadNedSource("C:\\Workspace\\Repository\\trunk\\omnetpp\\samples\\queuenet\\CallCenter.ned", new INEDErrorStore() {
				public void addError(INEDElement context, String message) {}
				public void addError(INEDElement context, int line, String message) {}
				public void addWarning(INEDElement context, String message) {}
				public void addWarning(INEDElement context, int line, String message) {}
				public void add(int severity, INEDElement context, int line, String message) {}
			});
			test(original);
		}
	}

	private void test(INEDElement original) {
		INEDElement target = original.deepDup();
		doRandomTreeChanges(target);
		
		String targetNED = NEDTreeUtil.generateNedSource(target, false);
		String targetXML = NEDTreeUtil.generateXmlFromPojoElementTree(target, "", false);

		NEDTreeDifferenceUtils.Applier applier = new NEDTreeDifferenceUtils.Applier();
		NEDTreeDifferenceUtils.applyTreeDifferences(original, target, applier);
		applier.apply();
		
		final String originalNED = NEDTreeUtil.generateNedSource(original, false);
		final String originalXML = NEDTreeUtil.generateXmlFromPojoElementTree(original, "", false);

		System.out.println("Original NED: " + originalNED);
		System.out.println("Target NED: " + targetNED);

		TextDifferenceUtils.applyTextDifferences(originalNED, targetNED, new TextDifferenceUtils.ITextDifferenceApplier() {
			public void replace(int start, int end, String replacement) {
				System.out.println("Original first line: >>>" + StringUtils.getLines(originalNED, start, end) + "<<<");
				System.out.println("Target start: " + start + " end: " + end + " difference: >>>" + replacement + "<<<");
			}
		});

		System.out.println("Original XML: " + originalXML);
		System.out.println("Target XML: " + targetXML);

		TextDifferenceUtils.applyTextDifferences(originalXML, targetXML, new TextDifferenceUtils.ITextDifferenceApplier() {
			public void replace(int start, int end, String replacement) {
				System.out.println("Original: >>>" + StringUtils.getLines(originalXML, start, end) + "<<<");
				System.out.println("Target start: " + start + " end: " + end + " difference: >>>" + replacement + "<<<");
			}
		});

		Assert.isTrue(originalNED.equals(targetNED));
		Assert.isTrue(originalXML.equals(targetXML));
	}
	
	private void doRandomTreeChanges(INEDElement element) {
		dupRandomNodes(element);
		deleteRandomChildren(element);
		insertRandomCommentNodes(element);
		changeRandomAttributes(element);

		for (INEDElement child : element)
			doRandomTreeChanges(child);
	}
	
	private void deleteRandomChildren(INEDElement element) {
		for (int i = 0; i < element.getNumChildren(); i++)
			if (random.nextDouble() < 0.1)
				element.getChild(i).removeFromParent();
	}

	private void insertRandomCommentNodes(INEDElement element) {
		if (!(element instanceof CommentElement)) {
			for (int i = 0; i < random.nextInt(3); i++) {
				CommentElement comment = (CommentElement)NEDElementFactory.getInstance().createElement("comment");
				comment.setLocid("banner");
				comment.setContent("// Comment: " + String.valueOf(random.nextInt(10000)));
				element.insertChildBefore(element.getChild(random.nextInt(element.getNumChildren() + 1)), comment);
			}
		}
	}
	
	private void dupRandomNodes(INEDElement element) {
		if (element.getNumChildren() > 0)
			while (random.nextDouble() < 0.1) {
				INEDElement child = element.getChild(random.nextInt(element.getNumChildren()));
				element.insertChildBefore(element.getChild(random.nextInt(element.getNumChildren() + 1)), child.deepDup());
			}
	}

	private static void changeRandomAttributes(INEDElement element) {
		for (int i = 0; i < element.getNumAttributes(); i++)
			if (random.nextDouble() < 0.3) {
				try {
					element.setAttribute(i, String.valueOf(random.nextInt(100)));
				}
				catch (RuntimeException e) {
					// ignore
				}
			}	
	}
}
