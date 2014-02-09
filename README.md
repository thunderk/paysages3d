Paysages 3D
===========

About
-----

[Paysages 3D](http://www.paysages3d.com) is a work-in-progress landscape generator, modeler and renderer software.

It is written in C++ 11, using Qt 5.2 as only dependency.

Build/Run
---------

At the moment, the build system is only well tested using the latest [QtSDK](http://qt-project.org/downloads) installation, and [QtCreator](http://qt-project.org/downloads#qt-creator). Later work will remove this dependency.

Simply open the *src/paysages.pro* file in QtCreator, and make a shadow build.

You can then run the compiled program **from the top project directory** (the one that contains *src* and *data*).

Licensing
---------

The source code is subject to the terms of the [Mozilla Public License, v. 2.0](http://mozilla.org/MPL/2.0/). Read the full terms in the LICENSE file.

Credits
-------

* [Michaël Lemaire](http://thunderk.net) - Main developer
* [Eric Bruneton and Fabrice Neyet](http://www-evasion.imag.fr/Membres/Eric.Bruneton/) - Publication and source code from *Precomputed Atmospheric Scattering (2008)*
* [Qt](http://qt-project.org/) - Library in use
* [GoogleTest](https://code.google.com/p/googletest/) - Included library
