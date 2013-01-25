xVideoServiceThief (aka xVST)
=============================

xVideoServiceThief is a tool for downloading your favourite video clips from a lot of video websites. xVideoServiceThief also provide you the ability to convert each video in most popular formats: AVI, MPEG1, MPEG2, WMV, MP4, 3GP, MP3 file formats.

**Official site:** http://xviservicethief.sourceforge.net

**Project on SourceForge.net:** https://sourceforge.net/projects/xviservicethief/

---

Is this the official version of xVST?
=====================================

**Yes**. We moved our source code to GitHub. Why? Easy, we want give the community the opportunity to collaborate more directly and interactively with us and GitHub is the best tool today for this.

We will continue using the SourceForge.net platform for hosting, binary releases, wiki, statistics, etc... because is a great tool and we love it.

**Fork us!!**

Is this project dead?
=====================

Evidently, no ;)

Where I can find more information?
==================================

https://sourceforge.net/apps/mediawiki/xviservicethief/index.php?title=Main_Page

How to compile xVST?
==================================

A compiler directrive (STATIC_BUILD) must be defined if you want to get a "valid" xVST compilation.

- **Static mode**:

  If you want compile the xVST in STATIC version, you should do:
		> qmake -set build_mode static_build
		> qmake
		> make


- **Dynamic mode**:

	If you want compile the xVST in DYNAMIC version, you should do:
		> qmake -set build_mode dynamic_build
		> qmake
		> make

- **Frameworks mode (only MacOSX)**:

	If you want compile the xVST in FRAMEWORKS version, you should do:
		> qmake -set build_mode frameworks_build
		> qmake
		> make

If you are using Windows, you can use the automated scripts for compile the xVST.

	> compile-tools/compile-vc++.bat	(static + VC++2008)
	> compile-tools/compile-mingw.bat	(dynamic + mingw)
