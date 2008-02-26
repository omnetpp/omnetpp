# http://www.germane-software.com/software/rexml/docs/tutorial.html
# http://www.rubycentral.com/book/tut_stdtypes.html

require "rexml/document"

puts "Parsing started"
# open and parse the new document
doc = REXML::Document.new(File.new("inetframework.xml"))
puts "Documet parsed"

#write out the whole xml to be able to compare wuth the results
doc.write(File.open("step0.xml", "w"))

defaultParamsFile = File.new("defaultparams.txt")

#helper function
def nvl(par1, par2)
    if par1.nil?
        par2
    else
        par1
    end
end

# gets an element or creates one if does not exists (returns the requested element)
def createOrGetElement(parent, whereXPath, tagName, attributes)
    parent.elements.each(whereXPath) { |node|
        return node
    }
    return parent.add_element(tagName, attributes)
end

# set the unit value (add a property to the parameter)
def setUnitProperty(node, unit)
    property = createOrGetElement(node, "property[@name='unit']", "property", {"name"=>"unit"})
    propkey = createOrGetElement(property, "property-key", "property-key", {})
    literal = createOrGetElement(propkey, "literal", "literal", {"type"=>"string", "value"=>unit})
end

# create module hash table for later use
moduleHash = {}
doc.elements.each("*/*/compound-module | */*/simple-module") { |mod|
    moduleHash[mod.attributes["name"]] = mod
}

#change the module parameters to have default value
moduleName = ""
defaultParamsFile.each_line { |line|
    a = line.chomp.split(/ *\t */)
    next if a.length == 0
    moduleName = a[0] if a.length == 1
    if a.length > 1
        paramName = a[1]
        paramType = a[2]
        paramValue = nvl(a[3], "")
        paramUnit = nvl(a[4], "")
        puts "Processing: #{moduleName}"
        if !moduleHash[moduleName].nil?
            moduleHash[moduleName].elements.each("parameters/param[@name='#{paramName}']") { |paramNode|
                paramNode.attributes['value'] = paramValue unless (paramValue == "-" || paramValue == "TBD")
                paramNode.attributes['type'] = paramType
                paramNode.attributes['is-default'] = "true"
                setUnitProperty(paramNode, paramUnit) unless paramUnit == ""
            }
        end
    end
}

doc.write(File.open("step1.xml", "w"))

