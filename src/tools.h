/*
*
* This file is part of xVideoServiceThief,
* an open-source cross-platform Video service download
*
* Copyright (C) 2007 - 2017 Xesc & Technology
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with xVideoServiceThief. If not, see <http://www.gnu.org/licenses/>.
*
* Contact e-mail: Xesc <xeskuu.xvst@gmail.com>
* Program URL   : http://www.xvideothief.com/
*
*/

#ifndef __TOOLS_H__
#define __TOOLS_H__

#include <QtWidgets>

	/*! Copy a string */
	QString copy(QString str, int start, int end);

	/*! Get a token from the String */
	QString getToken(QString str, QString separator, int token);

	/*! Get the token count from the String */
	int getTokenCount(QString str, QString separator);

	/*! Get the text between two tokens */
	QString copyBetween(QString str, QString from, QString to);

	/*!  Convert the File Size to String (Bytes, KBytes, MBytes... */
	QString fileSizeToString(const int bytes);

	/*! Generates a UniqueFilename, makes sure the file does not exist before returning a result */
	QString uniqueFileName(const QString file);

	/*! Change the current file name (conserving the original extension) */
	QString changeFileName(const QString file, const QString newName);

	/*! Change the current file ext for the new extension */
	QString changeFileExt(const QString file, const QString ext);

	/*! Return a valid File Name */
	QString cleanFileName(const QString fileName, QString replaceFor = " ");

	/*! Return the file path */
	QString extractFilePath(const QString fileName);

	/*! Return the file name  */
	QString extractFileName(const QString fileName);

	/*! Return the file extension */
	QString extractFileExt(const QString fileName);

	/*! Return a valid file path */
	QString validPath(QString path);

	/*! Convert seconds to Hours minutes seconds */
	QString secondsToHMS(const int seconds, const bool compact, const bool allowNegative = false);

	/*! Convert seconds to Hours minutes seconds (semi-formated) */
	QString secondsToHMS_formatted(const int seconds, const QString separator,
								   const bool autoHours, const bool autoMins,
								   const bool allowNegative = false);

	/*! Fill with zeros */
	QString intToMinLength(int value, int minLength);

	/*! Calcule the % value */
	float calculePercent(const float value, const float total);

	/*! Float to String (using locale config) */
	QString floatToStr(const float value, const int precision = 2);

	/*! Get if we are running a Windows Vista*/
	bool isWindowsVista();

	/*! Compare versions */
	int compareVersions(QString version1, QString version2);

	/*! Clear the URL */
	QString cleanURL(QString URL);

	/*! Validate url */
	bool validURL(QString URL, bool acceptRTMP = false);

	/*! Return if is a HTTP url */
	bool isHttpURL(QString URL);

	/*! Return if is a RTMP url */
	bool isRtmpURL(QString URL);

	/*! Get the URL path + query */
	QString getPathAndQuery(QUrl URL);

	/*! Open the file with its associated program */
	bool openFile(QString filePath);

	/*! Open the directory into the defined explorer */
	bool openDirectory(QString directoryPath);

	/*! Open dialog and wait until his end (usefull on mac and Qt::Sheet property) */
	int showModalDialog(QDialog *form);

	/*! Returns if this application is running on a 64bits envirotment */
	bool isRunningOn64bits();

	/*! Convert a multi line string into a single line string (trimmed)  */
	QString multiLineToSingleLine(QString text);

	/*! Display a native dialog */
	int native_alert(QWidget *parent, QMessageBox::Icon icon, QString messageText, QString informativeText, QString defaultButton,
					 QString alternateButton = QString(), QString otherButton = QString());

	/*! Strip html tokens from QString */
	QString stripHtml(QString html);

	/*! Convert a Decimal HTML Char into the respective HTML Code */
	QString htmlCharToHtmlCode(QString html);

	/*! Convert a HTML Code into the respective Decimal HTML Char */
	QString htmlCodeToHtmlChar(QString html);

	/*! Convert a standard char into a HTML char */
	QString charToHtml(QChar asciiChar, bool htmlCodes = true);

	/*! Convert a HTML string into a "standard" string */
	QString htmlToStr(QString html);

	/*! Convert a HTML char into a "standard" char */
	QChar htmlToChar(QString html);

	/*! Convert a standard string into a HTML string */
	QString strToHtml(QString str, bool htmlCodes = true);

	/*! HTML conversion table */
	const QString HTML_Char_Conversion_Table[100] =
	{
		"&#34;=&quot;",		//!< Quotation mark
		"&#38=&amp;",		//!< Ampersand
		"&#60=&lt;",		//!< Less than
		"&#62=&gt;",		//!< Greater than
		"&#160;=&nbsp;",	//!< Non-breaking space
		"&#161;=&iexcl;",	//!< Inverted exclamation
		"&#162;=&cent;",	//!< Cent sign
		"&#163;=&pound;",	//!< Pound sterling
		"&#164;=&curren;",	//!< General currency sign
		"&#165;=&yen;",		//!< Yen sign
		"&#166;=&brvbar;",	//!< Broken vertical bar
		"&#167;=&sect;",	//!< Section sign
		"&#168;=&uml;",		//!< Umlaut (dieresis)
		"&#169;=&copy;",	//!< Copyright
		"&#170;=&ordf;",	//!< Feminine ordinal
		"&#171;=&laquo;",	//!< Left angle quote, guillemotleft
		"&#172;=&not;",		//!< Not sign
		"&#173;=&shy;",		//!< Soft hyphen
		"&#174;=&reg;",		//!< Registered trademark
		"&#175;=&macr;",	//!< Macron accent
		"&#176;=&deg;",		//!< Degree sign
		"&#177;=&plusmn;",	//!< Plus or minus
		"&#178;=&sup2;",	//!< Superscript two
		"&#179;=&sup3;",	//!< Superscript three
		"&#180;=&acute;",	//!< Acute accent
		"&#181;=&micro;",	//!< Micro sign
		"&#182;=&para;",	//!< Paragraph sign
		"&#183;=&middot;",	//!< Middle dot
		"&#184;=&cedil;",	//!< Cedilla
		"&#185;=&sup1;",	//!< Superscript one
		"&#186;=&ordm;",	//!< Masculine ordinal
		"&#187;=&raquo;",	//!< Right angle quote, guillemotright
		"&#188;=&frac14;",	//!< Fraction one-fourth
		"&#189;=&frac12;",	//!< Fraction one-half
		"&#190;=&frac34;",	//!< Fraction three-fourths
		"&#191;=&iquest;",	//!< Inverted question mark
		"&#192;=&Agrave;",	//!< Capital A, grave accent
		"&#193;=&Aacute;",	//!< Capital A, acute accent
		"&#194;=&Acirc;",	//!< Capital A, circumflex accent
		"&#195;=&Atilde;",	//!< Capital A, tilde
		"&#196;=&Auml;",	//!< Capital A, dieresis or umlaut mark
		"&#197;=&Aring;",	//!< Capital A, ring
		"&#198;=&AElig;",	//!< Capital AE dipthong (ligature)
		"&#199;=&Ccedil;",	//!< Capital C, cedilla
		"&#200;=&Egrave;",	//!< Capital E, grave accent
		"&#201;=&Eacute;",	//!< Capital E, acute accent
		"&#202;=&Ecirc;",	//!< Capital E, circumflex accent
		"&#203;=&Euml;",	//!< Capital E, dieresis or umlaut mark
		"&#204;=&Igrave;",	//!< Capital I, grave accent
		"&#205;=&Iacute;",	//!< Capital I, acute accent
		"&#206;=&Icirc;",	//!< Capital I, circumflex accent
		"&#207;=&Iuml;",	//!< Capital I, dieresis or umlaut mark
		"&#208;=&ETH;",		//!< Capital Eth, Icelandic
		"&#209;=&Ntilde;",	//!< Capital N, tilde
		"&#210;=&Ograve;",	//!< Capital O, grave accent
		"&#211;=&Oacute;",	//!< Capital O, acute accent
		"&#212;=&Ocirc;",	//!< Capital O, circumflex accent
		"&#213;=&Otilde;",	//!< Capital O, tilde
		"&#214;=&Ouml;",	//!< Capital O, dieresis or umlaut mark
		"&#215;=&times;",	//!< Multiply sign
		"&#216;=&Oslash;",	//!< Capital O, slash
		"&#217;=&Ugrave;",	//!< Capital U, grave accent
		"&#218;=&Uacute;",	//!< Capital U, acute accent
		"&#219;=&Ucirc;",	//!< Capital U, circumflex accent
		"&#220;=&Uuml;",	//!< Capital U, dieresis or umlaut mark
		"&#221;=&Yacute;",	//!< Capital Y, acute accent
		"&#222;=&THORN;",	//!< Capital THORN, Icelandic
		"&#223;=&szlig;",	//!< Small sharp s, German (sz ligature)
		"&#224;=&agrave;",	//!< Small a, grave accent
		"&#225;=&aacute;",	//!< Small a, acute accent
		"&#226;=&acirc;",	//!< Small a, circumflex accent
		"&#227;=&atilde;",	//!< Small a, tilde
		"&#228;=&auml;",	//!< Small a, dieresis or umlaut mark
		"&#229;=&aring;",	//!< Small a, ring
		"&#230;=&aelig;",	//!< Small ae dipthong (ligature)
		"&#231;=&ccedil;",	//!< Small c, cedilla
		"&#232;=&egrave;",	//!< Small e, grave accent
		"&#233;=&eacute;",	//!< Small e, acute accent
		"&#234;=&ecirc;",	//!< Small e, circumflex accent
		"&#235;=&euml;",	//!< Small e, dieresis or umlaut mark
		"&#236;=&igrave;",	//!< Small i, grave accent
		"&#237;=&iacute;",	//!< Small i, acute accent
		"&#238;=&icirc;",	//!< Small i, circumflex accent
		"&#239;=&iuml;",	//!< Small i, dieresis or umlaut mark
		"&#240;=&eth;",		//!< Small eth, Icelandic
		"&#241;=&ntilde;",	//!< Small n, tilde
		"&#242;=&ograve;",	//!< Small o, grave accent
		"&#243;=&oacute;",	//!< Small o, acute accent
		"&#244;=&ocirc;",	//!< Small o, circumflex accent
		"&#245;=&otilde;",	//!< Small o, tilde
		"&#246;=&ouml;",	//!< Small o, dieresis or umlaut mark
		"&#247;=&divide;",	//!< Division sign
		"&#248;=&oslash;",	//!< Small o, slash
		"&#249;=&ugrave;",	//!< Small u, grave accent
		"&#250;=&uacute;",	//!< Small u, acute accent
		"&#251;=&ucirc;",	//!< Small u, circumflex accent
		"&#252;=&uuml;",	//!< Small u, dieresis or umlaut mark
		"&#253;=&yacute;",	//!< Small y, acute accent
		"&#254;=&thorn;",	//!< Small thorn, Icelandic
		"&#255;=&yuml;"		//!< Small y, dieresis or umlaut mark
	};

#endif
