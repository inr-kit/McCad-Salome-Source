#include <QMcCad_LogWindow.h>

QMcCad_LogWindow::QMcCad_LogWindow(QWidget* parent) :
	QTextEdit(parent)
{
	myString = " ";
	myParent=parent;
	myWColor = QColor(125,25,225);
	myEColor = QColor(255,0,0);
	myColor = QColor(0,0,0);
}

void QMcCad_LogWindow::Append(QString theString)
{
	setTextColor(myColor);
	myString = theString;
	append(theString);
}

QSize QMcCad_LogWindow::sizeHint() const
{
	QSize theSize(1/25*myParent->width(), 1/25*myParent->height());
	return theSize;
}

void QMcCad_LogWindow::SetTextColor(int r, int g, int b)
{
	QColor c(r, g, b);
	setTextColor(c);
}

void QMcCad_LogWindow::SetTextColor(QColor& c)
{
	setTextColor(c);
}


void QMcCad_LogWindow::Error(QString theString)
{
	setTextColor(myEColor);
	append(theString);
	setTextColor(myColor);
}

void QMcCad_LogWindow::Warning(QString theString)
{
	setTextColor(myWColor);
	append(theString);
	setTextColor(myColor);
}

void QMcCad_LogWindow::Heading(QString theString)
{
	setFontWeight(QFont::Bold);
	append(theString);
	setFontWeight(QFont::Normal);
}

