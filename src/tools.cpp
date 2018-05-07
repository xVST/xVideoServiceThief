/*
*
* This file is part of xVideoServiceThief,
* an open-source cross-platform Video service download
*
* Copyright (C) 2007 - 2018 Xesc & Technology
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

#include "tools.h"

#ifdef Q_OS_WIN32
	#include <windows.h>
#endif

#ifdef Q_OS_MACX
	#include "mac_only/mac_tools.h"
#endif

QString copy(QString str, int start, int end)
{
	end = end > str.length() ? str.length() : end;

	if (end > start)
	{
		str.remove(end, str.length() - end);
		str.remove(0, start);
		return str;
	}
	else
		return "";
}

QString getToken(QString str, QString separator, int token)
{
	QStringList results = str.split(separator, QString::KeepEmptyParts);

	if (token >= 0 && token < results.count())
		return results.at(token);
	else
		return "";
}

int getTokenCount(QString str, QString separator)
{
	QStringList results = str.split(separator, QString::KeepEmptyParts);

	return results.count();
}

QString copyBetween(QString str, QString from, QString to)
{
	QString result = "";
	int start = str.indexOf(from, 0);
	int end;
	// if first token exists
	if (start != -1)
	{
		start += from.length();
		end = str.indexOf(to, start);
		// if second token exists
		if (end != -1) result = copy(str, start, end);
	}
	// return the between text
	return result;
}

QString fileSizeToString(const int bytes)
{
	float fbytes = bytes;
	QString result;

	if (fbytes <= 1024)
		result = floatToStr(fbytes) + " bytes";
	else if (fbytes / 1024 <= 1024)
		result = floatToStr(fbytes / 1024) + " KiB";
	else if (fbytes / 1024 / 1024 <= 1024)
		result = floatToStr(fbytes / powf(1024, 2)) + " MiB";
	else if (fbytes / 1024 / 1024 / 1024 <= 1024)
		result = floatToStr(fbytes / powf(1024, 3)) + " GiB";
	else if (fbytes / 1024 / 1024 / 1024 / 1024 <= 1024)
		result = floatToStr(fbytes / powf(1024, 4)) + " TiB";
	else if (fbytes / 1024 / 1024 / 1024 / 1024 <= 1024)
		result = floatToStr(fbytes / powf(1024, 5)) + " PiB";
	else if (fbytes / 1024 / 1024 / 1024 / 1024 <= 1024)
		result = floatToStr(fbytes / powf(1024, 6)) + " EiB";
	else if (fbytes / 1024 / 1024 / 1024 / 1024 <= 1024)
		result = floatToStr(fbytes / powf(1024, 7)) + " ZiB";
	else if (fbytes / 1024 / 1024 / 1024 / 1024 <= 1024)
		result = floatToStr(fbytes / powf(1024, 8)) + " YiB";
	// the result
	return result;
}

int fileSizeToBytes(const QString fileSize)
{
	// prepre the file size regExp parser
	QRegExp fileSizeRegExp("([\\d\\.]+)(.*)");
	// extract the file size (we don't know if is KB, Bytes...)
	if (fileSizeRegExp.indexIn(fileSize) > -1)
	{
		float size = fileSizeRegExp.capturedTexts().at(1).trimmed().toFloat();
		QString units = fileSizeRegExp.capturedTexts().at(2).trimmed();
		// MiB to Bytes
			 if (units == "KiB") size *= 1024;
		else if (units == "MiB") size *= powf(1024, 2);
		else if (units == "GiB") size *= powf(1024, 3);
		else if (units == "TiB") size *= powf(1024, 4);
		else if (units == "PiB") size *= powf(1024, 5);
		else if (units == "EiB") size *= powf(1024, 6);
		else if (units == "ZiB") size *= powf(1024, 7);
		else if (units == "YiB") size *= powf(1024, 8);
		// set file size
		return (int)size;
	}
	return 0;
}

QString uniqueFileName(const QString fileName)
{
	int n = 1;
	QString tmp = fileName;

	while (QFileInfo(tmp).exists())
	{
		tmp = QString("%1/%2(%3).%4")
			  .arg(QFileInfo(fileName).absolutePath())
			  .arg(QFileInfo(fileName).baseName())
			  .arg(n)
			  .arg(QFileInfo(fileName).completeSuffix());
		// next
		n++;
	}
	return tmp;
}

QString changeFileName(const QString file, const QString newName)
{
	return QFileInfo(file).absolutePath() + "/" + newName + "." + QFileInfo(file).completeSuffix();
}

QString changeFileExt(const QString file, const QString ext)
{
	return QFileInfo(file).absolutePath() + "/" + QFileInfo(file).completeBaseName() + ext;
}

QString cleanFileName(const QString fileName, QString replaceFor)
{
	QString invalids = "\\/:*?!\"<>|\n\a\f\r\t\v";
	QString result = fileName;
	// look-up for some invalid chars
	for (int n = 0; n < fileName.length(); n++)
	{
		if (invalids.indexOf(fileName.at(n), 0) != -1)
			result.replace(fileName.at(n), replaceFor);
	}
	// get file info
	QFileInfo fileInfo(result);
	QString ext = fileInfo.suffix();
	// get the base name (we cannot use the shipped baseName() function due to fake positives)
	result.replace(result.length() - 1 - ext.length(), ext.length() + 1, "");
	// remove accents
	result = result.normalized(QString::NormalizationForm_D);
	// cleanup non ascii chars
	result.remove(QRegExp("[^a-zA-Z\\d\\s\\.]"));
	// build back the file name
	return result.trimmed() + "." + ext;
}

QString extractFilePath(const QString fileName)
{
	QFileInfo file(fileName);
	return file.absolutePath();
}

QString extractFileName(const QString fileName)
{
	QFileInfo file(fileName);
	return file.fileName();
}

QString extractFileExt(const QString fileName)
{
	QFileInfo file(fileName);
	QString ext = file.suffix();
	return ext != "" ? "." + ext : "";
}

QString validPath(QString path)
{
	path.replace("\\", "/");
	//path.replace(" ", "\ ");
	return path;
}

QString secondsToHMS(const int seconds, const bool compact, const bool allowNegative)
{
	int sec = seconds;

	if (!allowNegative && seconds < 0)
		sec = 0;

	if (!compact)
		return QString("%1h %2m %3s")
			   .arg(sec/3600)	//h
			   .arg(sec/60%60)	//m
			   .arg(sec%60);	//s
	else
	{
		QString result("");
		// h
		if (sec/3600 != 0)
			result = QString("%1h").arg(sec/3600);
		// m
		if (sec/60%60 != 0)
			result += QString("%1m").arg(sec/60%60);
		// s
		if (sec%60 != 0)
			result += QString("%1s").arg(sec%60);
		// return result
		return result;
	}
}

QString secondsToHMS_formatted(const int seconds, const QString separator,
							   const bool autoHours, const bool autoMins,
							   const bool allowNegative)
{
	int sec = seconds;

	if (!allowNegative && seconds < 0)
		sec = 0;

	QString result;

	// h
	if (!autoHours || sec/3600 != 0)
		result = QString("%1" + separator).arg(sec/3600);
	// m
	if (!autoMins || sec/60%60 != 0 || !result.isEmpty())
		result += QString("%1" + separator).arg(intToMinLength(sec/60%60, 2));
	// s
	//if (sec%60 != 0)
		result += QString("%1").arg(intToMinLength(sec%60, 2));
	// return result
	return result;
}

QString intToMinLength(int value, int minLength)
{
	QString base = QString().fill('0', minLength);
	QString s = QString("%1").arg(value);
	if (s.length() >= base.length())
		return s;
	else // remove some zeros and append the value (ie: 00 -> 01)
		return base.remove(0, s.size()).append(s);
}

float calculePercent(const float value, const float total)
{
	if (total == 0)
		return 0;
	else
		return value/total*100;
}

QString floatToStr(const float value, const int precision)
{
	QLocale locale;
	return locale.toString(value, 'f', precision);
}

bool isWindowsVista()
{
#ifdef Q_OS_WIN32
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	// return if we are running the windows vista/7 or higher OS
	return (osvi.dwMajorVersion >= 6);// && (osvi.dwMinorVersion == 0));
#else
	return false;
#endif
}

int subVersionToInt(QString subVersion)
{
	subVersion = subVersion.toLower();

	if (subVersion == "alpha" || subVersion == "a")
		return 1;
	else if (subVersion == "beta" || subVersion == "b")
		return 2;
	else if (subVersion == "final" || subVersion == "f")
		return 3;
	else
		return 4;
}

QString convertEmbededSubVersionToStandardSubVersion(QString version)
{
	version = QString(version).replace("a", " Alpha");
	version = QString(version).replace("b", " Beta");
	version = QString(version).replace("f", " Final");
	return version;
}

int compareVersions(QString version1, QString version2)
{
	// are the same version?
	if (version1 == version2)
		return 0;
	else
	{
		QString s1 = version1;
		QString s2 = version2;
		// replace comas and "_" for dots
		s1 = s1.replace(",", ".");
		s1 = s1.replace("_", ".");
		s2 = s2.replace(",", ".");
		s2 = s2.replace("_", ".");
		// get the version with more "fields" (x.x.x.x)
		int num1 = getTokenCount(s1, ".");
		int num2 = getTokenCount(s1, ".");
		// set the max num
		int max = num1 != num2 ? (num1 > num2 ? num1 : num2) : num1;
		// get subversions
		QString subVersion1, subVersion2;
		// convert a embeded version to standard version (i.e: 2.0a -> 2.0 Alpha)
		s1 = convertEmbededSubVersionToStandardSubVersion(s1);
		s2 = convertEmbededSubVersionToStandardSubVersion(s2);
		// get major version and sub-version (version1)
		if (getTokenCount(s1, " ") > 1)
		{
			subVersion1 = getToken(s1, " ", 1);
			s1 = getToken(s1, " ", 0);
		}
		// get major version and sub-version (version2)
		if (getTokenCount(s2, " ") > 1)
		{
			subVersion2 = getToken(s2, " ", 1);
			s2 = getToken(s2, " ", 0);
		}
		// Check for number versions
		for (int level = 0; level <= max; level++)
		{
			num1 = getToken(s1, ".", level).toInt();
			num2 = getToken(s2, ".", level).toInt();

			if (num1 > num2)
				return -1;
			else if (num1 < num2)
				return 1;
		}
		// Check for subversion (alpha, beta, etc...)
		if (subVersionToInt(subVersion1) < subVersionToInt(subVersion2))
			return 1;
		else if (subVersionToInt(subVersion1) > subVersionToInt(subVersion2))
			return -1;
		else
			return 0;
	}
}

QString cleanURL(QString URL)
{
	if (URL.indexOf("?") != -1 && URL.indexOf("%") != -1)
		if (URL.indexOf("?") < URL.indexOf("%"))
			return URL;

	// replace hex chars to ascii chars
	while (URL.indexOf("%") != -1)
	{
		int p = URL.indexOf("%");
		QString hexChar = copy(URL, p + 1, p + 3);
		URL.replace("%" + hexChar, QString(hexChar.toInt(NULL, 16)));
	}
	return URL;
}

bool validURL(QString URL, bool acceptRTMP)
{
	// first at all, check if have the "http://"
	if (isHttpURL(URL))
		return QUrl(URL).isValid();
	// if rtmp urls are accepted and have the "rtmp://"
	else if (acceptRTMP && isRtmpURL(URL))
	{
		if (URL.indexOf("rtmp://") == 0) URL = URL.replace("rtmp://", "http://");
		else if (URL.indexOf("rtmpe://") == 0) URL = URL.replace("rtmpe://", "http://");
		else if (URL.indexOf("rtmpt://") == 0) URL = URL.replace("rtmpt://", "http://");
		else if (URL.indexOf("rtmps://") == 0) URL = URL.replace("rtmps://", "http://");
		else if (URL.indexOf("rtmpte://") == 0) URL = URL.replace("rtmpte://", "http://");
		else if (URL.indexOf("rtmpts://") == 0) URL = URL.replace("rtmpts://", "http://");
		else if (URL.indexOf("rtmfp://") == 0) URL = URL.replace("rtmfp://", "http://");
		// validate this "new one"
		return QUrl(URL).isValid();
	}
	else // invalid url
		return false;
}

bool isHttpURL(QString URL)
{
    return URL.indexOf("http://") == 0 || URL.indexOf("https://") == 0;
}

bool isRtmpURL(QString URL)
{
	return	URL.indexOf("rtmp://") == 0 || URL.indexOf("rtmpe://") == 0 ||
			URL.indexOf("rtmpt://") == 0 || URL.indexOf("rtmps://") == 0 ||
			URL.indexOf("rtmpte://") == 0 || URL.indexOf("rtmpte://") == 0 ||
			URL.indexOf("rtmfp://") == 0;
}

QString getPathAndQuery(QUrl URL)
{
	QString params = URL.toString();

	if (params.indexOf("?") != -1)
		params.remove(0, params.indexOf("?"));
	else
		params = "";

	return QUrl(URL.path()).toEncoded() + params;
}

QString stripHtml(QString html)
{
	return QTextEdit(html).toPlainText();
}

QString htmlCharToHtmlCode(QString html)
{
	bool ok;
	int index = QString(copyBetween(html, "#", ";")).toInt(&ok);

	if (ok)
		switch (index)
		{
			case 34:
				return getToken(HTML_Char_Conversion_Table[0], "=", 1);
			case 38:
				return getToken(HTML_Char_Conversion_Table[1], "=", 1);
			case 60:
				return getToken(HTML_Char_Conversion_Table[2], "=", 1);
			case 62:
				return getToken(HTML_Char_Conversion_Table[3], "=", 1);
			default:
				if (index <= 255)
					return getToken(HTML_Char_Conversion_Table[99 - (255 - index)], "=", 1);
				else // unknwon code
					return "";
		}
	else // error
		return "";
}

QString htmlCodeToHtmlChar(QString html)
{
	for (int n = 0; n < 100; n++)
		if (getToken(HTML_Char_Conversion_Table[n], "=", 1) == html)
			return getToken(HTML_Char_Conversion_Table[n], "=", 0);
	// not found
	return "";
}

QString charToHtml(QChar asciiChar, bool htmlCodes)
{
	if (asciiChar.isNull()) return "";

	if	(asciiChar == ' ' || asciiChar == '!' || asciiChar == '=' ||
		(asciiChar >= '#' && asciiChar <= '%') ||
		(asciiChar >= '\'' && asciiChar <= ';') ||
		(asciiChar >= '?' && asciiChar <= '~'))
		return asciiChar;
	else // is a "rare" char (nonstandard)
	{
		QString result = QString("&#%1;").arg(int(asciiChar.unicode()));
		// use html codes?
		if (htmlCodes)
		{
			// get the html char code
			QString htmlChar = htmlCharToHtmlCode(result);
			// we have results?
			if (!htmlChar.isEmpty())
				return htmlChar;
			else
				return result;
		}
		else
			return result;
	}
}

QString htmlToStr(QString html)
{
	QString result = "";
	// parse html text
	while (html.indexOf("&") > -1)
	{
		// copy text & delete it from HTML
		int n = html.indexOf("&");
		result += copy(html, 0, n);
		html.remove(0, n);
		// get the HTML char & delete it from HTML
		n = html.indexOf(";");
		if (n == -1) return result + html;
		QString htmlChar = copy(html, 0, n + 1);
		html.remove(0, n + 1);
		// convert
		result += htmlToChar(htmlChar);
	}
	// all has been parsed?
	if (!html.isEmpty())
		result += html;
	// did something?
	if (result.isEmpty())
		result = html;
	// final string
	return result;
}

QChar htmlToChar(QString html)
{
	// try to convert the html code to html decimal
	QString charString = htmlCodeToHtmlChar(html);
	// if we do not have results then get the original HTML passed by parameters
	if (charString.isEmpty())
		charString = html;
	// start the conversion
	charString = copyBetween(charString, "#", ";");
	// convert
	bool ok;
	int charValue = charString.toInt(&ok);
	// is a valid value
	if (ok)
		return QChar(charValue);
	else
		return ' ';
}

QString strToHtml(QString str, bool htmlCodes)
{
	str = str.fromUtf8(str.toLocal8Bit());
	QString result = "";

	for (int n = 0; n < str.length(); n++)
		result += charToHtml(str[n], htmlCodes);

	return result;
}

bool openFile(QString filePath)
{
	return QDesktopServices::openUrl(QUrl::fromLocalFile(validPath(filePath)));
}

bool openDirectory(QString directoryPath)
{
	return openFile(directoryPath);
}

int showModalDialog(QDialog *form)
{
#ifdef Q_OS_MACX
	// display this form as a Sheet
	form->open();
	// wait while addVideoForm is visible
	while (form->isVisible())
		qApp->processEvents(QEventLoop::WaitForMoreEvents);
	// return result
	return form->result();
#else
	return form->exec();
#endif
}

bool isRunningOn64bits()
{
	// 32bits = 4 ; 64bits = 8
	return sizeof(int*) == 8;
}

QString multiLineToSingleLine(QString text)
{
	// remove start and end spaces
	text = text.trimmed();
	// init result
	QString result = "";
	int spacesCount = 0;
	// normalize text
	for (int n = 0; n < text.length(); n++)
	{
		if (text.at(n).isSpace() && spacesCount == 0)
		{
			result += text.at(n);
			spacesCount++;
		}
		else if (!text.at(n).isSpace()) // not space
		{
			result += text.at(n);
			spacesCount = 0;
		}
	}
	// return the normalized text
	return result;
}

QString escapeJson(QJsonDocument json, bool includeJSONParser)
{
	// generate back your json
	QString str(json.toJson(QJsonDocument::Compact));
	// scape the json string
	str.replace("\\", "\\\\").replace("\"", "\\\"").replace("'", "\\'");
	// return the escaped json
	return includeJSONParser ? "JSON.parse('" + str + "');" : str;
}

int native_alert(QWidget *parent, QMessageBox::Icon icon, QString messageText, QString informativeText, QString defaultButton,
				 QString alternateButton, QString otherButton)
{
// native cocoa dialogs has been disabled due some Qt troubles
/*
#ifdef Q_OS_MACX
	return alert(icon, stripHtml(messageText), stripHtml(informativeText), defaultButton, alternateButton, otherButton);
#else
*/
	switch (icon)
	{
		case QMessageBox::Warning:
			return QMessageBox::warning(parent, messageText, informativeText, defaultButton, alternateButton, otherButton, 0, 1);
		case QMessageBox::Critical:
			return QMessageBox::critical(parent, messageText, informativeText, defaultButton, alternateButton, otherButton, 0, 1);
		case QMessageBox::Question:
			return QMessageBox::question(parent, messageText, informativeText, defaultButton, alternateButton, otherButton, 0, 1);
		case QMessageBox::Information: default:
			return QMessageBox::information(parent, messageText, informativeText, defaultButton, alternateButton, otherButton, 0, 1);
	}
//#endif
}
