#ifndef QPRINTER_H
#define QPRINTER_H

#include <QDialog>
#include <QLCDNumber>


class QPrinter : public QLCDNumber
{
    Q_OBJECT
public:
    QPrinter(QDialog *parent = nullptr);
    ~QPrinter();
};

#endif // QPRINTER_H
