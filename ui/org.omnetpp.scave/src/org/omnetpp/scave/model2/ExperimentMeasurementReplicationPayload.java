package org.omnetpp.scave.model2;

public class ExperimentMeasurementReplicationPayload implements Comparable<ExperimentMeasurementReplicationPayload> {
    public String experiment;

    public String measurement;

    public String replication;

    public ExperimentMeasurementReplicationPayload(String experiment, String measurement, String replication) {
        this.experiment = experiment;
        this.measurement = measurement;
        this.replication = replication;
    }

    public int compareTo(ExperimentMeasurementReplicationPayload other) {
        int c = experiment.compareTo(other.experiment);

        if (c != 0)
            return c;
        else {
            c = measurement.compareTo(other.measurement);

            if (c != 0)
                return c;
            else
                return replication.compareTo(other.replication);
        }
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((experiment == null) ? 0 : experiment.hashCode());
        result = prime * result + ((measurement == null) ? 0 : measurement.hashCode());
        result = prime * result + ((replication == null) ? 0 : replication.hashCode());
        return result;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        ExperimentMeasurementReplicationPayload other = (ExperimentMeasurementReplicationPayload) obj;
        if (experiment == null) {
            if (other.experiment != null)
                return false;
        }
        else if (!experiment.equals(other.experiment))
            return false;
        if (measurement == null) {
            if (other.measurement != null)
                return false;
        }
        else if (!measurement.equals(other.measurement))
            return false;
        if (replication == null) {
            if (other.replication != null)
                return false;
        }
        else if (!replication.equals(other.replication))
            return false;
        return true;
    }
}
