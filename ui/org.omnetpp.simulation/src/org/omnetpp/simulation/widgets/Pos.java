package org.omnetpp.simulation.widgets;

/**
 * Primarily for TextViewer
 * @author Andras
 */
public class Pos {
    public int line;
    public int column;

    public Pos(int line, int column) {
        this.line = line;
        this.column = column;
    }

    public boolean lessThan(Pos other) {
        if (line != other.line)
            return line < other.line;
        return column < other.column;
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        return prime*line + column;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null || getClass() != obj.getClass())
            return false;
        Pos other = (Pos) obj;
        return column == other.column && line == other.line;
    }
}
