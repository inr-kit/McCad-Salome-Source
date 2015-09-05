#ifndef QMcCad_LogWindow_H
#define QMcCad_LogWindow_H
#include <QtGui/QTextEdit>


class QMcCad_LogWindow : public QTextEdit {
    Q_OBJECT
  public:
    
    QMcCad_LogWindow(QWidget* parent);
    
  public:
   
    void Append(QString theString);
    void Warning(QString theString);
    void Error(QString theString);
    void Heading(QString theString);

    void SetTextColor(int r, int g, int b);
    void SetTextColor(QColor& c);
   
    QSize sizeHint () const;
         
  private:

    QString myString;    
    QWidget* myParent;
    QColor myWColor;
    QColor myEColor;
    QColor myColor;
};

#endif     


