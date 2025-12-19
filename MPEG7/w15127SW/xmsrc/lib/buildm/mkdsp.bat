#!/bin/csh -f
set DSP=XMLib.dsp
type Start1.dsp >! $DSP
type Release.define >> $DSP
#type ../BA/Version.define >> $DSP
find ../BA -name \*. -exec ./catdefine {} \; >> $DSP
#find ../Audio -name \*. -exec catdefine {} \; >> $DSP
#find ../Video -name \*. -exec catdefine {} \; >> $DSP
#find ../MDS -name \*. -exec catdefine {} \; >> $DSP
type Debug.define >>$DSP
find ../BA -name \*. -exec catdefine {} \; >> $DSP
#find ../Audio -name \*. -exec catdefine {} \; >> $DSP
#find ../Video -name \*. -exec catdefine {} \; >> $DSP
#find ../MDS -name \*. -exec catdefine {} \; >> $DSP
type Start2.dsp >> $DSP
type BA1.dsp >>$DSP
test -f ../BA/XML. && type ../BA/XML.dsp >> $DSP
type BA2.dsp >>$DSP
find ../Audio -name \*. -exec catdsp {} \; >> $DSP
find ../Video -name \*. -exec catdsp {} \; >> $DSP
find ../MDS -name \*. -exec catdsp {} \; >> $DSP
type Tail.dsp >>$DSP


