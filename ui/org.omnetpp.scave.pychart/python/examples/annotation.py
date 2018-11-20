

dialog = """
<?xml version='1.0' encoding='UTF-8'?>
<xswt xmlns:x='http://sweet_swt.sf.net/xswt'>
  <import xmlns='http://sweet_swt.sf.net/xswt'>
    <package name='java.lang'/>
    <package name='org.eclipse.swt.widgets' />
    <package name='org.eclipse.swt.layout' />
  </import>
  <layout x:class='GridLayout' />
  <x:children>
    <group text='Options'>
      <layoutData x:class='GridData' horizontalAlignment='FILL' verticalAlignment='FILL' grabExcessHorizontalSpace='true'/>
      <layout x:class='GridLayout' numColumns='2'/>
      <x:children>
         <button x:id='columnNames' text='Add header row' x:style='CHECK' selection='true'>
           <layoutData x:class='GridData' horizontalSpan='2'/>
         </button>
         <label text='Title:'/>
         <text x:id='chartTitle' >
           <layoutData x:class='GridData' widthHint='100'/>
         </text>
         <label text='Data precision:'/>
         <spinner x:id='precision' x:style='BORDER' minimum='1' maximum='16' textLimit='2' selection='14'>
           <layoutData x:class='GridData' widthHint='100'/>
         </spinner>
      </x:children>
    </group>
  </x:children>
</xswt>
"""

####

params = [
	{ "name": "charTitle", "type": "string" },
	{ "name": "foobar", "type": "int", "min": 0, "max": 20 },
]

params = [
	("charTitle", "string"),
	("foobar", "int", {"min": 0, "max": 20 }),
]


ax = plt.subplot(111)

plt.title(parameters["chartTitle"])

t = np.arange(0.0, 5.0, 0.01)
s = np.cos(2*np.pi*t)
line, = plt.plot(t, s, lw=2)

plt.annotate('local max', xy=(2, 1), xytext=(3, 1.5),
            arrowprops=dict(facecolor='black', shrink=0.05),
            )

plt.ylim(-2,2)
plt.show()

