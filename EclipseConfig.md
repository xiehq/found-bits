This might be useful when setting up Eclipse
  * for the first time
  * the next time
  * you are C/C++ programmer in Windows

  1. Don't use tabs. Replace tabs with 2 spaces
> > ![https://found-bits.googlecode.com/svn/wiki/UseInsertSpacesForTabs.jpg](https://found-bits.googlecode.com/svn/wiki/UseInsertSpacesForTabs.jpg)
  1. Replace tabs with 2 spaces also in Python
> > ![https://found-bits.googlecode.com/svn/wiki/2SpacesForTabInPydev.jpg](https://found-bits.googlecode.com/svn/wiki/2SpacesForTabInPydev.jpg)
  1. Set VS key mappings
> > ![https://found-bits.googlecode.com/svn/wiki/UseVSKeys.jpg](https://found-bits.googlecode.com/svn/wiki/UseVSKeys.jpg)
  1. Import coding style from _\Sources\EclipseCCppCodingStyle.xml_
> > ![https://found-bits.googlecode.com/svn/wiki/ImportCCppCodingStyle.jpg](https://found-bits.googlecode.com/svn/wiki/ImportCCppCodingStyle.jpg)
  1. Save automatically before build
> > ![https://found-bits.googlecode.com/svn/wiki/SaveAutomaticallyBeforeBuild.jpg](https://found-bits.googlecode.com/svn/wiki/SaveAutomaticallyBeforeBuild.jpg)
  1. When Eclipse is ran it seems to create additional icon in Windows taskbar if it is already pinned there. To have only pinned icon in taskbar when Eclipse is running:
    1. Unpin eclipse from taskbar.
    1. Add following at the top of eclipse.ini (next to eclipse.exe):
```
     -vm
     C:\Program Files\Java\jre7\bin
```
> > ## UPDATE ##
> > New path for latest Java:
```
   -vm
   C:/Program Files/Java/jre1.8.0_25/bin/server
```
> > Add to the end of file for Eclipse Luna.
> > Notice the '//'
> > (Also added path to folder with javaw.exe at the beginning of %PATH%. Not sure if it helps.)
    1. Run Eclipse.
    1. Pin created icon to taskbar.