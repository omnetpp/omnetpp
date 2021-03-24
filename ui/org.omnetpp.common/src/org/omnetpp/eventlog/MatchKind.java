package org.omnetpp.eventlog;

public enum MatchKind
{
    EXACT, // returns not null if exactly one event was found
    FIRST_OR_PREVIOUS, // if multiple matches were found it returns the first one or the previous if there are no matches at all
    FIRST_OR_NEXT, // if multiple matches were found it returns the first one or the next if there are no matches at all
    LAST_OR_NEXT, // as per FIRST_OR_PREVIOUS but the opposite
    LAST_OR_PREVIOUS; // as per FIRST_OR_NEXT but the opposite
}
