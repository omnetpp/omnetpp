from omnetpp.scave.analysis import *
from omnetpp.scave.charttemplate import *

print("Loading chart templates from: ", get_chart_template_locations())

templates = load_chart_templates()
print("Available chart templates (and their properties):")
for id in sorted(templates.keys()):
    template = templates[id]
    print('    {}: "{}" ({})'.format(id, template.name, template.type))
    for key,value in template.properties.items():
        print('        {} = {}'.format(key, repr(value)))

print("Creating anf file with one example chart for each template")

charts = [t.create_chart(name="Example "+t.name, props={'filter': '*'} if 'filter' in t.properties else {}) for t in templates.values()]
analysis = analysis.Analysis(inputs=['fifo'], items=charts)
analysis.to_anf_file("out.anf")

