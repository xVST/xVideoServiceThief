xVideoServiceThief (aka xVST)
=============================

xVideoServiceThief is a tool for downloading your favourite video clips from a lot of video websites. xVideoServiceThief also provide you the ability to convert each video in most popular formats: AVI, MPEG1, MPEG2, WMV, MP4, 3GP, MP3 file formats.

**Official site:** http://www.xvideothief.com/

**Project on GitHub:** https://github.com/xVST/xVideoServiceThief

**Project on SourceForge.net:** https://sourceforge.net/projects/xviservicethief/

---

Whats new in version 3.0?
=========================

Well, a long time has passed since our last official update (we prepared a 2.6 version which it was never released to the public due to lack of time). 

So, we analyzed the market, and xVST continue being a great tool, but sincerely, our plugins are very outdated and the time required to keep them updated is very high, so we decided to **integrate the great tool youtube-dl into xVST!**. 

Now, we can say that xVST **is also** a graphical interface for youtube-dl, it is a new "feature" non a "replacement".

###What is Youtube-dl and why this tool?
[Youtube-dl](https://rg3.github.io/youtube-dl/) is a fantastic open source command-line tool to download videos from Youtube and a lot of other video services (its [long list](https://rg3.github.io/youtube-dl/supportedsites.html) of supported sites is quite impressive).

The community is very active, and they maintain the tool very updated, great!! The decision to integrate youtube-dl into xVST was obvious and easy. We love youtube-dl and we love xVST plugins.

###Is this the end of xVST plugins?
**Totally NO**. xVST still require the plugins to work, but they now can use youtube-dl to get information, or simply indicate that a certain video service is totally handled by youtube-dl (including the download). All is done and configured using our loved plugins.

Is this the official version of xVST?
=====================================

**Yes**. We moved our source code to GitHub. Why? Easy, we want give the community the opportunity to collaborate more directly and interactively with us and GitHub is the best tool today for this.

We will continue using the SourceForge.net platform for hosting, binary releases, wiki, statistics, etc... because is a great tool and we love it.

**[Fork us on GitHub!!](https://github.com/xVST/xVideoServiceThief/fork)**

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

	> compile-tools/compile-vc++.bat	(static + VC++2010)
	> compile-tools/compile-mingw.bat	(dynamic + mingw)
