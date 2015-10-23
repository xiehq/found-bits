# Checkout #

  1. **Checkout the repository trunk to hard drive.**
> > URL: http://found-bits.googlecode.com/svn/trunk/


> ![https://found-bits.googlecode.com/svn/wiki/Checkout.jpg](https://found-bits.googlecode.com/svn/wiki/Checkout.jpg)

  1. **Map the checkout folder to drive.**
> > Put
> > _subst <drive letter>: <path to checkout folder>_
> > to batch file and make file execute at startup or share checkout folder and map it as a network drive.
Work only in new mapped drive.

# Download #
  1. Download **CodeSourcery** and put it in \Tools\Compilers\CodeSourcery. This folder must contain only one version of CodeSourcery. Don't create any version subfolders. Single version folder are on this level. (arm-2011.09 when this was written)
> > ![https://found-bits.googlecode.com/svn/wiki/CodeSourcery.jpg](https://found-bits.googlecode.com/svn/wiki/CodeSourcery.jpg)
  1. Samples might use other sources than ones from repository. They need to be downloaded from the original sites. There is a list of external libraries in _\Sources\Projects\ManagementScripts\ExternalLibsList.py_ and there is a _GetExternalLibs.py_ script to help you downloading them.

# Install #
  1. Install **Visual Studio (Express)** (2010 when this was written)
  1. Install **Python** (2.7.3 when this was written)
  1. Install **Eclipse**. Standard edition is OK, so is version for C/C++ with CDT. (Indigo when this was written)

# Configure #
  1. Add Python to PATH.
> > ![https://found-bits.googlecode.com/svn/wiki/PythonToPATH.jpg](https://found-bits.googlecode.com/svn/wiki/PythonToPATH.jpg)
  1. Add **Pydev** to Eclipse and create default Python interpreter in Eclipse.
  1. Add **CDT** to Eclipse if you haven't downloaded Eclipse for C/C++ developers.
  1. Some optional [Eclipse configuration](EclipseConfig.md) steps if you are setting Eclipse from scratch.