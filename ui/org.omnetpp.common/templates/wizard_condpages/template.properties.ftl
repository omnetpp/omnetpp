# wizard properties
templateName = Newly Generated Wizard, with a Conditional Page
templateDescription = 
templateCategory = Generated Wizards
supportedWizardTypes = 

# variables
isAdvanced = false
someValue = 0
anotherValue = 0

# custom wizard pages
page.1.file = inputspage.xswt
page.1.title = Select Options

page.2.file = conditionalpage.xswt
page.2.title = Advanced Options
page.2.descriptions = This page only appears for certain settings on the previous page
page.2.condition = isAdvanced && someValue > 0
