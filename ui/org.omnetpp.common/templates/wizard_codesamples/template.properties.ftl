# wizard properties
templateName = Newly Generated Wizard
templateDescription = Does not generate anything useful, but contains template code examples
templateCategory = Generated Wizards
supportedWizardTypes =

<#if wantBasics>
# variables
boolVar = true
numericVar = 42
textVar = "lorem ipsum dolor"
</#if>

<#if wantJSON>
# list, hash
listVar = [ "one", "two", "three" ]
hashVar = { "first": "Joe", "last": "Brown", "title": "Mr" }
compound = { "nonprime": 1, "primes": [2, 3, 5, 7, 11, "and many more"] }
</#if>