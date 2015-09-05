#ifndef QMCCAD_VGPARAMDIALOG_H
#define QMCCAD_VGPARAMDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

class Ui_VGParamDialog
{
public:
    QWidget *layoutWidget;
    QHBoxLayout *hboxLayout;
    QPushButton *acceptButton;
    QSpacerItem *spacerItem;
    QPushButton *cancelButton;
    QPushButton *pushButton;
    QFrame *line_2;
    QFrame *frame;
    QLabel *label;
    QLabel *label_2;
    QFrame *line;
    QLabel *label_3;
    QCheckBox *checkCollision;
    QCheckBox *checkDiscrete;
    QCheckBox *checkCM;
    QCheckBox *checkMM;
    QFrame *frame_2;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *label_11;
    QLabel *label_12;
    QLabel *label_13;
    QLabel *label_14;
    QLineEdit *minInputVolume;
    QLineEdit *minVoidVolume;
    QLineEdit *minDecFaceArea;
    QLineEdit *minReDecFaceArea;
    QLineEdit *maxPredecCells;
    QLineEdit *maxComplementCells;
    QLineEdit *minSamplePoints;
    QLineEdit *maxSamplePoints;
    QLineEdit *xRes;
    QLineEdit *yRes;
    QLineEdit *tolSet;
    QFrame *frame_3;
    QLabel *label_15;
    QLabel *label_16;
    QLineEdit *initSurfNb;
    QLineEdit *initCellNb;

    void setupUi(QDialog *VGParamDialog)
    {
    VGParamDialog->setObjectName(QString::fromUtf8("VGParamDialog"));
    VGParamDialog->resize(QSize(398, 420).expandedTo(VGParamDialog->minimumSizeHint()));
    layoutWidget = new QWidget(VGParamDialog);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    layoutWidget->setGeometry(QRect(10, 380, 382, 33));
    hboxLayout = new QHBoxLayout(layoutWidget);
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    pushButton = new QPushButton(layoutWidget);
    pushButton->setObjectName(QString::fromUtf8("pushButton"));

    hboxLayout->addWidget(pushButton);

    spacerItem = new QSpacerItem(101, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem);

    acceptButton = new QPushButton(layoutWidget);
    acceptButton->setObjectName(QString::fromUtf8("acceptButton"));

    hboxLayout->addWidget(acceptButton);

    cancelButton = new QPushButton(layoutWidget);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

    hboxLayout->addWidget(cancelButton);

    line_2 = new QFrame(VGParamDialog);
    line_2->setObjectName(QString::fromUtf8("line_2"));
    line_2->setGeometry(QRect(10, 370, 381, 16));
    line_2->setFrameShape(QFrame::HLine);
    frame = new QFrame(VGParamDialog);
    frame->setObjectName(QString::fromUtf8("frame"));
    frame->setGeometry(QRect(10, 10, 381, 61));
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);
    label = new QLabel(frame);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(10, 10, 121, 17));
    label_2 = new QLabel(frame);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setGeometry(QRect(10, 30, 141, 17));
    line = new QFrame(frame);
    line->setObjectName(QString::fromUtf8("line"));
    line->setGeometry(QRect(180, 0, 20, 61));
    line->setFrameShape(QFrame::VLine);
    label_3 = new QLabel(frame);
    label_3->setObjectName(QString::fromUtf8("label_3"));
    label_3->setGeometry(QRect(220, 10, 71, 17));
    checkCollision = new QCheckBox(frame);
    checkCollision->setObjectName(QString::fromUtf8("checkCollision"));
    checkCollision->setGeometry(QRect(160, 10, 21, 22));
    checkCollision->setChecked(true);
    checkDiscrete = new QCheckBox(frame);
    checkDiscrete->setObjectName(QString::fromUtf8("checkDiscrete"));
    checkDiscrete->setGeometry(QRect(160, 30, 16, 22));
    checkDiscrete->setChecked(true);
    checkCM = new QCheckBox(frame);
    checkCM->setObjectName(QString::fromUtf8("checkCM"));
    checkCM->setGeometry(QRect(300, 10, 41, 22));
    checkCM->setChecked(true);
    checkMM = new QCheckBox(frame);
    checkMM->setObjectName(QString::fromUtf8("checkMM"));
    checkMM->setGeometry(QRect(300, 30, 41, 22));
    frame_2 = new QFrame(VGParamDialog);
    frame_2->setObjectName(QString::fromUtf8("frame_2"));
    frame_2->setGeometry(QRect(10, 130, 381, 241));
    frame_2->setFrameShape(QFrame::StyledPanel);
    frame_2->setFrameShadow(QFrame::Raised);
    label_4 = new QLabel(frame_2);
    label_4->setObjectName(QString::fromUtf8("label_4"));
    label_4->setGeometry(QRect(10, 10, 181, 17));
    label_5 = new QLabel(frame_2);
    label_5->setObjectName(QString::fromUtf8("label_5"));
    label_5->setGeometry(QRect(10, 30, 171, 17));
    label_6 = new QLabel(frame_2);
    label_6->setObjectName(QString::fromUtf8("label_6"));
    label_6->setGeometry(QRect(10, 50, 211, 17));
    label_7 = new QLabel(frame_2);
    label_7->setObjectName(QString::fromUtf8("label_7"));
    label_7->setGeometry(QRect(10, 70, 225, 17));
    label_8 = new QLabel(frame_2);
    label_8->setObjectName(QString::fromUtf8("label_8"));
    label_8->setGeometry(QRect(10, 90, 271, 17));
    label_9 = new QLabel(frame_2);
    label_9->setObjectName(QString::fromUtf8("label_9"));
    label_9->setGeometry(QRect(10, 110, 238, 17));
    label_10 = new QLabel(frame_2);
    label_10->setObjectName(QString::fromUtf8("label_10"));
    label_10->setGeometry(QRect(10, 130, 209, 17));
    label_11 = new QLabel(frame_2);
    label_11->setObjectName(QString::fromUtf8("label_11"));
    label_11->setGeometry(QRect(10, 150, 213, 17));
    label_12 = new QLabel(frame_2);
    label_12->setObjectName(QString::fromUtf8("label_12"));
    label_12->setGeometry(QRect(10, 170, 75, 17));
    label_13 = new QLabel(frame_2);
    label_13->setObjectName(QString::fromUtf8("label_13"));
    label_13->setGeometry(QRect(10, 190, 73, 17));
    label_14 = new QLabel(frame_2);
    label_14->setObjectName(QString::fromUtf8("label_14"));
    label_14->setGeometry(QRect(10, 210, 71, 17));
    minInputVolume = new QLineEdit(frame_2);
    minInputVolume->setObjectName(QString::fromUtf8("minInputVolume"));
    minInputVolume->setGeometry(QRect(312, 10, 61, 20));
    minInputVolume->setAlignment(Qt::AlignRight);
    minVoidVolume = new QLineEdit(frame_2);
    minVoidVolume->setObjectName(QString::fromUtf8("minVoidVolume"));
    minVoidVolume->setGeometry(QRect(312, 30, 61, 20));
    minVoidVolume->setAlignment(Qt::AlignRight);
    minDecFaceArea = new QLineEdit(frame_2);
    minDecFaceArea->setObjectName(QString::fromUtf8("minDecFaceArea"));
    minDecFaceArea->setGeometry(QRect(312, 50, 61, 20));
    minDecFaceArea->setAlignment(Qt::AlignRight);
    minReDecFaceArea = new QLineEdit(frame_2);
    minReDecFaceArea->setObjectName(QString::fromUtf8("minReDecFaceArea"));
    minReDecFaceArea->setGeometry(QRect(312, 70, 61, 20));
    minReDecFaceArea->setAlignment(Qt::AlignRight);
    maxPredecCells = new QLineEdit(frame_2);
    maxPredecCells->setObjectName(QString::fromUtf8("maxPredecCells"));
    maxPredecCells->setGeometry(QRect(312, 90, 61, 20));
    maxPredecCells->setAlignment(Qt::AlignRight);
    maxComplementCells = new QLineEdit(frame_2);
    maxComplementCells->setObjectName(QString::fromUtf8("maxComplementCells"));
    maxComplementCells->setGeometry(QRect(312, 110, 61, 20));
    maxComplementCells->setAlignment(Qt::AlignRight);
    minSamplePoints = new QLineEdit(frame_2);
    minSamplePoints->setObjectName(QString::fromUtf8("minSamplePoints"));
    minSamplePoints->setGeometry(QRect(312, 130, 61, 20));
    minSamplePoints->setAlignment(Qt::AlignRight);
    maxSamplePoints = new QLineEdit(frame_2);
    maxSamplePoints->setObjectName(QString::fromUtf8("maxSamplePoints"));
    maxSamplePoints->setGeometry(QRect(312, 150, 61, 20));
    maxSamplePoints->setAlignment(Qt::AlignRight);
    xRes = new QLineEdit(frame_2);
    xRes->setObjectName(QString::fromUtf8("xRes"));
    xRes->setGeometry(QRect(312, 170, 61, 20));
    xRes->setAlignment(Qt::AlignRight);
    yRes = new QLineEdit(frame_2);
    yRes->setObjectName(QString::fromUtf8("yRes"));
    yRes->setGeometry(QRect(312, 190, 61, 20));
    yRes->setAlignment(Qt::AlignRight);
    tolSet = new QLineEdit(frame_2);
    tolSet->setObjectName(QString::fromUtf8("tolSet"));
    tolSet->setGeometry(QRect(312, 210, 61, 20));
    tolSet->setAlignment(Qt::AlignRight);
    frame_3 = new QFrame(VGParamDialog);
    frame_3->setObjectName(QString::fromUtf8("frame_3"));
    frame_3->setGeometry(QRect(10, 70, 381, 61));
    frame_3->setFrameShape(QFrame::StyledPanel);
    frame_3->setFrameShadow(QFrame::Raised);
    label_15 = new QLabel(frame_3);
    label_15->setObjectName(QString::fromUtf8("label_15"));
    label_15->setGeometry(QRect(10, 10, 131, 17));
    label_16 = new QLabel(frame_3);
    label_16->setObjectName(QString::fromUtf8("label_16"));
    label_16->setGeometry(QRect(10, 30, 107, 17));
    initSurfNb = new QLineEdit(frame_3);
    initSurfNb->setObjectName(QString::fromUtf8("initSurfNb"));
    initSurfNb->setGeometry(QRect(312, 10, 61, 20));
    initSurfNb->setAlignment(Qt::AlignRight);
    initCellNb = new QLineEdit(frame_3);
    initCellNb->setObjectName(QString::fromUtf8("initCellNb"));
    initCellNb->setGeometry(QRect(312, 30, 61, 20));
    initCellNb->setAlignment(Qt::AlignRight);
    retranslateUi(VGParamDialog);
    QObject::connect(cancelButton, SIGNAL(clicked()), VGParamDialog, SLOT(reject()));
    QObject::connect(acceptButton, SIGNAL(clicked()), VGParamDialog, SLOT(accept()));

    QMetaObject::connectSlotsByName(VGParamDialog);
    } // setupUi

    void retranslateUi(QDialog *VGParamDialog)
    {
    VGParamDialog->setWindowTitle(QApplication::translate("VGParamDialog", "Optional Conversion Parameters", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("VGParamDialog", "Write Collision File", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("VGParamDialog", "Write Discrete Model", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("VGParamDialog", "Input Units", 0, QApplication::UnicodeUTF8));
    checkCollision->setText(QApplication::translate("VGParamDialog", "", 0, QApplication::UnicodeUTF8));
    checkDiscrete->setText(QApplication::translate("VGParamDialog", "", 0, QApplication::UnicodeUTF8));
    checkCM->setText(QApplication::translate("VGParamDialog", "CM", 0, QApplication::UnicodeUTF8));
    checkMM->setText(QApplication::translate("VGParamDialog", "MM", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("VGParamDialog", "Minimum Input Solid Volume", 0, QApplication::UnicodeUTF8));
    label_5->setText(QApplication::translate("VGParamDialog", "Minimum Void Volume", 0, QApplication::UnicodeUTF8));
    label_6->setText(QApplication::translate("VGParamDialog", "Minimum Decomposition Face Area", 0, QApplication::UnicodeUTF8));
    label_7->setText(QApplication::translate("VGParamDialog", "Minimum Redecomposition Face Area", 0, QApplication::UnicodeUTF8));
    label_8->setText(QApplication::translate("VGParamDialog", "Maximum Number Of Predecomposition Cells", 0, QApplication::UnicodeUTF8));
    label_9->setText(QApplication::translate("VGParamDialog", "Maximum Number Of Complement Cells", 0, QApplication::UnicodeUTF8));
    label_10->setText(QApplication::translate("VGParamDialog", "Minimum Number Of Sample Points", 0, QApplication::UnicodeUTF8));
    label_11->setText(QApplication::translate("VGParamDialog", "Maximum Number Of Sample Points", 0, QApplication::UnicodeUTF8));
    label_12->setText(QApplication::translate("VGParamDialog", "X-Resolution", 0, QApplication::UnicodeUTF8));
    label_13->setText(QApplication::translate("VGParamDialog", "Y-Resolution", 0, QApplication::UnicodeUTF8));
    label_14->setText(QApplication::translate("VGParamDialog", "Tolerance", 0, QApplication::UnicodeUTF8));
    minInputVolume->setText(QApplication::translate("VGParamDialog", "10", 0, QApplication::UnicodeUTF8));
    minVoidVolume->setText(QApplication::translate("VGParamDialog", "1.0", 0, QApplication::UnicodeUTF8));
    minDecFaceArea->setText(QApplication::translate("VGParamDialog", "5", 0, QApplication::UnicodeUTF8));
    minReDecFaceArea->setText(QApplication::translate("VGParamDialog", "1", 0, QApplication::UnicodeUTF8));
    maxPredecCells->setText(QApplication::translate("VGParamDialog", "500", 0, QApplication::UnicodeUTF8));
    maxComplementCells->setText(QApplication::translate("VGParamDialog", "3", 0, QApplication::UnicodeUTF8));
    minSamplePoints->setText(QApplication::translate("VGParamDialog", "10", 0, QApplication::UnicodeUTF8));
    maxSamplePoints->setText(QApplication::translate("VGParamDialog", "50", 0, QApplication::UnicodeUTF8));
    xRes->setText(QApplication::translate("VGParamDialog", "50", 0, QApplication::UnicodeUTF8));
    yRes->setText(QApplication::translate("VGParamDialog", "50", 0, QApplication::UnicodeUTF8));
    tolSet->setText(QApplication::translate("VGParamDialog", "1e-7", 0, QApplication::UnicodeUTF8));
    label_15->setText(QApplication::translate("VGParamDialog", "Initial Surface Number", 0, QApplication::UnicodeUTF8));
    label_16->setText(QApplication::translate("VGParamDialog", "Initial Cell Number", 0, QApplication::UnicodeUTF8));
    initSurfNb->setText(QApplication::translate("VGParamDialog", "1", 0, QApplication::UnicodeUTF8));
    initCellNb->setText(QApplication::translate("VGParamDialog", "1", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("VGParamDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    pushButton->setText(QApplication::translate("VGParamDialog", "Load", 0, QApplication::UnicodeUTF8));
    acceptButton->setText(QApplication::translate("VGParamDialog", "Accept", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(VGParamDialog);
    } // retranslateUi

};

namespace Ui {
    class VGParamDialog: public Ui_VGParamDialog {};
} // namespace Ui

#endif // QMCCAD_VGPARAMDIALOG_H
