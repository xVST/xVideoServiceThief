#include "changelogimpl.h"

ChangelogImpl::ChangelogImpl(QWidget *parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
    setupUi(this);
	// fill changelog
	QString plainText = rchChangelog->loadResource(QTextDocument::HtmlResource, QUrl("qrc:/changelog/changelog.txt")).toString();
	// set the changelog as html
	rchChangelog->setHtml(convertPlainTextToHtml(plainText));
	// signals
	connect(cmbVersions, SIGNAL(activated(int)), this, SLOT(cmbVersionsInActivated(int)));
}

QString ChangelogImpl::convertPlainTextToHtml(QString plainText)
{
	QString result = "";
	QStringList lines = plainText.split("\n", QString::KeepEmptyParts);

	int lastLevel = 0;

	foreach(QString line, lines)
	{
		// skip lines like this "---..."
		if (line.trimmed().indexOf("---") == 0)
			continue;

		int currentLevel = 0;

		if (line.trimmed().isEmpty())
		{
			for (int n = lastLevel; n > 0; n--)
				result += "</ul>";
			lastLevel = 0;
			continue;
		}

		while (currentLevel < line.length() && line.at(currentLevel).isSpace())
			currentLevel++;

		if (line.trimmed().indexOf("- ") == 0)
			currentLevel++;

		if (currentLevel > lastLevel)
			result += "<ul>";
		else if (currentLevel < lastLevel)
			for (int n = lastLevel; n > currentLevel; n--)
				result += "</ul>";

		lastLevel = currentLevel;

		// version
		if (line.trimmed().at(0) != '-' && line.trimmed().at(0).isNumber())
		{
			if (!result.isEmpty()) result += "<hr>";
			result += QString("<a name='%1' id='%1' />").arg(line.trimmed().remove(":"));
			result += QString("<h3>Version: %1</h3>").arg(line);
			cmbVersions->addItem(line.trimmed().remove(":"));
		}
		// sub-section
		else if (line.trimmed().at(0) != '-')
			result += QString("<h4>%1</h4>").arg(line);
		else // feature
		{
			QString color = "black";
			if (line.trimmed().indexOf("- Fixed bug (Error:") == 0)
			{
				line = line.replace("- Fixed bug (Error:", "- Fixed bug (<b>Error:</b>");
				color = "red";
			}
//			else if (line.trimmed().indexOf("- Added ") == 0) color = "green";
//			else if (line.trimmed().indexOf("- Changed") == 0) color = "orange";

			line = line.remove("- ");
			result += QString("<li><font color=%2>%1</font></li>").arg(line).arg(color);
		}
	}

	for (int n = lastLevel; n == 0; n--)
		result += "</ul>";

	return result;
}

void ChangelogImpl::cmbVersionsInActivated(int index)
{
	rchChangelog->scrollToAnchor(cmbVersions->itemText(index));
}
