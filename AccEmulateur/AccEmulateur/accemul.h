#ifndef ACCEMUL_H
#define ACCEMUL_H

///========================| Inclusion de librairie |========================///
#include <QWidget>
#include <QDialog>
#include <QDialog>
#include <QList>
#include <QStringList>
#include <QString>
#include <QSharedPointer>
#include <QHash>
#include <QLineEdit>
#include <QTextEdit>
#include <QDataWidgetMapper>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QStandardItemModel>
#include <QDoubleValidator>
#include <QLCDNumber>
#include <QProgressBar>
#include <QSerialPort>
#include <QSerialPortInfo>

QT_BEGIN_NAMESPACE
namespace Ui { class AccEmul; }
QT_END_NAMESPACE


class AccEmul : public QWidget
{
    Q_OBJECT

public:
    AccEmul(QWidget *parent = nullptr);
    ~AccEmul();
private slots:
    void AfficheValeur();
    void Nettoyer();
    //void readSerial();

private:
    QLabel *TitreLabel;                                            // créé un label pour le titre
    QLabel *label;                                                      // créé un label pour le titre

    QLabel *VitesseLabel;                                       // créé un label de vitesse
    QLabel *AccelerationLabel;                             // créé un label d'accélération
    QLabel *FreinageLabel;                                   // créé un label de Freinage

    QLabel *labelVitesse;

    QLineEdit *VitesseEdit;                                     // créé une case ou l'utilisateur rentre la vitesse

    QSpinBox *AccelerationBox;                           // créé un volet avec l'accélération
    QSpinBox *FreinageBox;                                  // créé un volet avec le freinage

    QPushButton *ValidationButton;                   // créé un bouton de Validation
    QPushButton *NettoyageButton;                  // créé un bouton de Nettoyage
    QPushButton *boutonQuitter;                       // créé un bouton pour Quitter l'IHM
    QString MaVitesse1;
    QDataWidgetMapper *mapper;
    QDoubleValidator *AcceptedValue;

    QByteArray line;

///========================| Afficheur LCD |=======================///
    QLCDNumber lcd;
///========================| Les boutons pour d'images |=======================///
    QToolButton *boutonImage1;

///===========================| Lien avec arduino |==========================///
    QSerialPort *arduino;                            // cherche le port série de arduino
    QString arduino_uno_port_name;            // nom ou N° du port sur laquel l'Arduino est placée
    bool validation;                                   // renvoie un "validation" bouléenne [0 ou 1]

    Ui::AccEmul *ui;
};
#endif // ACCEMUL_H
