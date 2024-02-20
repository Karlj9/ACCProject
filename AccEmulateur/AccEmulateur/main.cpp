#include "accemul.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AccEmul w;
    w.show();
    a.setWindowIcon(QIcon("E:/AccEmulateur/LMT-Logo-Couleur-V1.png"));
    return a.exec();
}
