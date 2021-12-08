from omnetpp.scave.analysis import *
from omnetpp.scave.charttemplate import *

print("loading chart templates from: ", get_chart_template_locations())

templates = load_chart_templates()
print("available chart templates:")
for id in sorted(templates.keys()):
    print('    {}: "{}" ({})'.format(id, templates[id].name, templates[id].type))

print("creating anf file with one example chart for each template")

charts = [t.create_chart(name="Example "+t.name, custom_props={ 'filter': '*'}) for t in templates.values()]
analysis = analysis.Analysis(inputs=['fifo'], items=charts)
analysis.to_anf_file("out.anf")

