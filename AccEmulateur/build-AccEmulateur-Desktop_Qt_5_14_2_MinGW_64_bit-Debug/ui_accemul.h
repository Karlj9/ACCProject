/********************************************************************************
** Form generated from reading UI file 'accemul.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ACCEMUL_H
#define UI_ACCEMUL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AccEmul
{
public:

    void setupUi(QWidget *AccEmul)
    {
        if (AccEmul->objectName().isEmpty())
            AccEmul->setObjectName(QString::fromUtf8("AccEmul"));
        AccEmul->resize(800, 600);

        retranslateUi(AccEmul);

        QMetaObject::connectSlotsByName(AccEmul);
    } // setupUi

    void retranslateUi(QWidget *AccEmul)
    {
        AccEmul->setWindowTitle(QCoreApplication::translate("AccEmul", "AccEmul", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AccEmul: public Ui_AccEmul {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ACCEMUL_H
