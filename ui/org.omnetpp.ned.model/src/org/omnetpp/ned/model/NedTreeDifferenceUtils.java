/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.Random;

import org.apache.commons.lang3.ObjectUtils;
import org.eclipse.compare.rangedifferencer.IRangeComparator;
import org.eclipse.compare.rangedifferencer.RangeDifference;
import org.eclipse.compare.rangedifferencer.RangeDifferencer;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.Debug;
import org.omnetpp.common.editor.text.TextDifferenceUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.ex.NedElementFactoryEx;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.pojo.CommentElement;
import org.omnetpp.ned.model.pojo.ConnectionElement;
import org.omnetpp.ned.model.pojo.NedElementTags;


public class NedTreeDifferenceUtils {
    /**
     * Applies changes identified by the tree diff algorithm to the tree.
     */
    public interface IApplier {
        public void replaceNonAttributeData(INedElement element, String sourceLocation, NedSourceRegion sourceRegion,
                int syntaxProblemMaxLocalSeverity, int consistencyProblemMaxLocalSeverity);

        public void replaceElements(INedElement parent, int start, int end, int offset, INedElement[] replacement);

        public void replaceAttribute(INedElement element, String name, String value);
    }

    public static void applyTreeDifferences(INedElement original, INedElement target, IApplier applier) {
        applier.replaceNonAttributeData(original, target.getSourceLocation(), target.getSourceRegion(),
                target.getSyntaxProblemMaxLocalSeverity(), target.getConsistencyProblemMaxLocalSeverity());

        applyAttributeDifferences(original, target, applier);

        NedElementChildrenComparator comparatorOriginal = new NedElementChildrenComparator(original);
        NedElementChildrenComparator comparatorTarget = new NedElementChildrenComparator(target);
        List<RangeDifference> differences = Arrays.asList(RangeDifferencer.findRanges(comparatorOriginal, comparatorTarget));

        Collections.sort(differences, new Comparator<RangeDifference>() {
            public int compare(RangeDifference o1, RangeDifference o2) {
                return o1.leftStart() - o2.leftStart();
            }}
        );

        int offset = 0;
        for (RangeDifference difference : differences) {
            int leftStart = difference.leftStart();
            int leftEnd = difference.leftEnd();
            int rightStart = difference.rightStart();
            int rightEnd = difference.rightEnd();

            if (difference.kind() == RangeDifference.NOCHANGE || (difference.leftLength() == 0 && difference.rightLength() == 0 && difference.ancestorLength() == 0)) {
                for (int i = leftStart; i < leftEnd; i++)
                    applyTreeDifferences(original.getChild(i) , target.getChild(rightStart + i - leftStart), applier);
            }
            else if (difference.kind() == RangeDifference.CHANGE) {
                INedElement[] replacement = comparatorTarget.getElementRange(rightStart, rightEnd);
                applier.replaceElements(original, leftStart, leftEnd, offset, replacement);
                offset += difference.rightLength() - difference.leftLength();
            }
            else
                throw new RuntimeException("Unknown difference kind");
        }
    }

    private static void applyAttributeDifferences(INedElement original, INedElement target, IApplier applier) {
        int num = original.getNumAttributes();
        for (int j = 0; j < num; j++) {
            String originalAttribute = original.getAttribute(j);
            String targetAttribute = target.getAttribute(j);

            if (!StringUtils.equals(originalAttribute, targetAttribute))
                applier.replaceAttribute(original, target.getAttributeName(j), targetAttribute);
        }
    }

    /**
     * The default implementation of NedTreeDifferenceUtils.IApplier.
     */
    public static class Applier implements IApplier {
        final ArrayList<Runnable> runnables = new ArrayList<Runnable>();

        // statistics, for debugging
        private int numNonAttributeDataChanges = 0;
        private int numAttributeChanges = 0;
        private int numElementReplacements = 0;

        public void replaceNonAttributeData(final INedElement element, final String sourceLocation, final NedSourceRegion sourceRegion,
                final int syntaxProblemMaxLocalSeverity, final int consistencyProblemMaxLocalSeverity) {
            if (!ObjectUtils.equals(element.getSourceLocation(), sourceLocation) || !ObjectUtils.equals(element.getSourceRegion(), sourceRegion) ||
                element.getSyntaxProblemMaxLocalSeverity() != syntaxProblemMaxLocalSeverity || element.getConsistencyProblemMaxLocalSeverity() != consistencyProblemMaxLocalSeverity)
            {
                numNonAttributeDataChanges++;
                runnables.add(new Runnable() {
                    public void run() {
                        element.setSourceLocation(sourceLocation);
                        element.setSourceRegion(sourceRegion);
                        element.setSyntaxProblemMaxLocalSeverity(syntaxProblemMaxLocalSeverity);
                        element.setConsistencyProblemMaxLocalSeverity(consistencyProblemMaxLocalSeverity);
                    }
                });
            }
        }

        public void replaceAttribute(final INedElement element, final String name, final String value) {
            numAttributeChanges++;
            runnables.add(new Runnable() {
                public void run() {
                    element.setAttribute(name, value);
                }
            });
        }

        public void replaceElements(final INedElement parent, final int start, final int end, final int offset, final INedElement[] replacement) {
            numElementReplacements++;
            runnables.add(new Runnable() {
                public void run() {
                    for (int i = start; i < end; i++)
                        parent.getChild(offset + start).removeFromParent();

                    int k = offset + start;
                    INedElement startChild = (k >= parent.getNumChildren()) ? null : parent.getChild(k);

                    for (INedElement element : replacement)
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
                   ", srcloc/markers: " + numNonAttributeDataChanges;
        }
    }

    public static void main(String[] args) {
        new NedTreeDifferenceTest().run();
    }
}

class NedElementChildrenComparator implements IRangeComparator {
    private INedElement parent;

    public NedElementChildrenComparator(INedElement parent) {
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
        INedElement thisElement = getElementAt(thisIndex);
        INedElement otherElement = ((NedElementChildrenComparator)other).getElementAt(otherIndex);

        if (thisElement == null || otherElement == null || thisElement.getTagCode() != otherElement.getTagCode())
            return false;

        if (thisElement.getTagCode() == NedElementTags.NED_CONNECTION) {
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

    public INedElement getElementAt(int index) {
        return parent.getChild(index);
    }

    public INedElement[] getElementRange(int start, int end) {
        INedElement[] range = new INedElement[end - start];
        for (int i = 0; i < range.length; i++)
            range[i] = getElementAt(start + i);
        return range;
    }

    public boolean skipRangeComparison(int length, int maxLength, IRangeComparator other) {
        return false;
    }
}

class NedTreeDifferenceTest {
    private static Random random = new Random(1);

    public void run() {
        NedElementFactoryEx.setInstance(new NedElementFactoryEx());

        for (int i = 0; i < 100; i++) {
            String nedFile = "C:\\Workspace\\Repository\\trunk\\omnetpp\\samples\\queuenet\\CallCenter.ned";
            INedElement original = NedTreeUtil.parseNedFile(nedFile, new SysoutNedErrorStore(), "/queuenet/callcenter.ned", NedElement.getDefaultNedTypeResolver());
            test(original);
        }
    }

    private void test(INedElement original) {
        INedElement target = original.deepDup();
        doRandomTreeChanges(target);

        String targetNED = NedTreeUtil.generateNedSource(target, false);
        String targetXML = NedTreeUtil.generateXmlFromPojoElementTree(target, "", false);

        NedTreeDifferenceUtils.Applier applier = new NedTreeDifferenceUtils.Applier();
        NedTreeDifferenceUtils.applyTreeDifferences(original, target, applier);
        applier.apply();

        final String originalNED = NedTreeUtil.generateNedSource(original, false);
        final String originalXML = NedTreeUtil.generateXmlFromPojoElementTree(original, "", false);

        Debug.println("Original NED: " + originalNED);
        Debug.println("Target NED: " + targetNED);

        TextDifferenceUtils.applyTextDifferences(originalNED, targetNED, new TextDifferenceUtils.ITextDifferenceApplier() {
            public void replace(int start, int end, String replacement) {
                Debug.println("Original first line: >>>" + StringUtils.getLines(originalNED, start, end) + "<<<");
                Debug.println("Target start: " + start + " end: " + end + " difference: >>>" + replacement + "<<<");
            }
        });

        Debug.println("Original XML: " + originalXML);
        Debug.println("Target XML: " + targetXML);

        TextDifferenceUtils.applyTextDifferences(originalXML, targetXML, new TextDifferenceUtils.ITextDifferenceApplier() {
            public void replace(int start, int end, String replacement) {
                Debug.println("Original: >>>" + StringUtils.getLines(originalXML, start, end) + "<<<");
                Debug.println("Target start: " + start + " end: " + end + " difference: >>>" + replacement + "<<<");
            }
        });

        Assert.isTrue(originalNED.equals(targetNED));
        Assert.isTrue(originalXML.equals(targetXML));
    }

    private void doRandomTreeChanges(INedElement element) {
        dupRandomNodes(element);
        deleteRandomChildren(element);
        insertRandomCommentNodes(element);
        changeRandomAttributes(element);

        for (INedElement child : element)
            doRandomTreeChanges(child);
    }

    private void deleteRandomChildren(INedElement element) {
        for (int i = 0; i < element.getNumChildren(); i++)
            if (random.nextDouble() < 0.1)
                element.getChild(i).removeFromParent();
    }

    private void insertRandomCommentNodes(INedElement element) {
        if (!(element instanceof CommentElement)) {
            for (int i = 0; i < random.nextInt(3); i++) {
                CommentElement comment = (CommentElement)NedElementFactoryEx.getInstance().createElement("comment");
                comment.setLocid("banner");
                comment.setContent("// Comment: " + String.valueOf(random.nextInt(10000)));
                element.insertChildBefore(element.getChild(random.nextInt(element.getNumChildren() + 1)), comment);
            }
        }
    }

    private void dupRandomNodes(INedElement element) {
        if (element.getNumChildren() > 0)
            while (random.nextDouble() < 0.1) {
                INedElement child = element.getChild(random.nextInt(element.getNumChildren()));
                element.insertChildBefore(element.getChild(random.nextInt(element.getNumChildren() + 1)), child.deepDup());
            }
    }

    private static void changeRandomAttributes(INedElement element) {
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
