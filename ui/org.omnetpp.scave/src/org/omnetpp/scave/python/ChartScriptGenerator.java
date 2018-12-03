package org.omnetpp.scave.python;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItemField;
import org.omnetpp.scave.engine.RunAttribute;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model2.ScaveModelUtil;

public class ChartScriptGenerator {

    public static String makeNativeChartScript(final ResultFileManager manager, final ResultType type, final IDList idList) {
        return ResultFileManager.callWithReadLock(manager, () -> {
            String[] filterFields = new String[] { RunAttribute.EXPERIMENT, RunAttribute.MEASUREMENT, RunAttribute.REPLICATION,
                    ResultItemField.MODULE, ResultItemField.NAME };

            String typeName = type.getName();
            typeName += "s";


            StringBuilder inputBuilder = new StringBuilder();

            inputBuilder.append(
                "from omnetpp.scave import results, chart\n" +
                "\n" +
                "from omnetpp.scave import vectorops as ops\n" +
                "\n" +
                "# This expression selects the results (you might be able to logically simplify it)\n"
            );

            inputBuilder.append("filter_expression = \"\"\"\n" + ScaveModelUtil.getIDListAsChartInput(idList, filterFields, manager) + "\"\"\"\n\n");

            inputBuilder.append(
                "# The data is returned as a Pandas DataFrame\n" +
                "df = results.get_" + typeName + "(filter_expression)\n" +
                "\n" +
                "# Which we turn into a more usable format\n" +
                "df = results.transform_results(df)\n" +
                "\n" +
                "# <|> vectorops marker <|>\n" +
                "# You can perform any transformations on the data here\n" +
                "\n" +
                "# Finally, the results are plotted\n" +
                "chart.plot_" + typeName + "(df)\n"
            );

            return inputBuilder.toString();
        });
    }

    public static String makeMatplotlibChartScript(String title, IDList ids, String[] runidFields, ResultType type, ResultFileManager manager) {
        return ResultFileManager.callWithReadLock(manager, () -> {
            Assert.isNotNull(runidFields);
            String[] filterFields = ScaveModelUtil.getFilterFieldsFor(runidFields);

            String typeName = type.getName();
            typeName += "s";

            StringBuilder sb = new StringBuilder("import numpy as np\n" +
                    "import pandas as pd\n" +
                    "import matplotlib.pyplot as plt\n" +
                    "from omnetpp.scave import results, chart\n" +
                    "from omnetpp.scave import vectorops as ops\n\n" +
                    "df = results.get_" + typeName + "(\"\"\"\n");

            sb.append(ScaveModelUtil.getIDListAsChartInput(ids, filterFields, manager));

            sb.append("\n\"\"\")\n");

            sb.append("df = results.transform_results(df)\n");

            sb.append("\n# You can perform any transformations on the data here\n\n");

            if (type.getName().equals("vector")) {
                sb.append("plt.xlabel('Simulation time (s)')\n");
                sb.append("plt.ylabel('Vector value')\n");

                sb.append("for t in df[[('result', 'vectime'), ('result', 'vecvalue'), 'module', 'name']].itertuples(index=False):\n");
                sb.append("    plt.plot(t[0], t[1], label=(t[2] + ':' + t[3])[:-7], drawstyle='steps-post')\n");

            } else if (type.getName().equals("scalar")) {
                sb.append("df.plot(kind='bar')\n");
            } else if (type.getName().equals("histogram")) {
                sb.append("for t in df[['binedges', 'binvalues', 'module', 'name']].itertuples(index=False):\n");
                sb.append("    plt.hist(bins=t[0][1:], x=t[0][1:-1], weights=t[1][1:-1], label=t[2] + ':' + t[3], histtype='step')\n");
            }

            sb.append("plt.legend()\n");
            sb.append("plt.title(\"\"\"" + title + "\"\"\")\n");
            sb.append("plt.tight_layout()\n");

            //sb.append("cursor = mpl.widgets.Cursor(plt.gca(), useblit=True, color='red', linewidth=2)\n");

            return sb.toString();
        });
    }
}
