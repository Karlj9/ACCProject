///=============================| Creat By KARL |============================/// 

#include "accemul.h"
#include "ui_accemul.h"

///========================| Inclusion de librairie |========================///
#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QString>
#include <QTextCursor>
#include <QTextEdit>
#include <QLineEdit>
#include <QGridLayout>
#include <QDebug>
#include <QAction>
#include <QLCDNumber>
#include <QGraphicsItem>
#include <QGraphicsEffect>
#include <QtGui>
#include <QProgressBar>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QLabel>
#include <sstream>
#include <string>
#include <QGroupBox>
#include <QToolButton>
#include <QFormLayout>
#include <QWidget>
#include <QDialog>
#include <QList>
#include <QStringList>
#include <QString>
#include <QSharedPointer>
#include <QHash>

using namespace std;

///========================| Mise en place de l'IHM |========================///

AccEmul::AccEmul(QWidget *parent) : QWidget(parent), ui(new Ui::AccEmul)
{
    ui->setupUi(this);

    //setFixedSize(650 , 400);                                       // change la taille de la fenêtre
    setFixedSize(this->width()*0.45,this->height()*0.43); //Ajuste la taille de la fenêtre

    QPalette pallette;
    pallette.setBrush(QPalette::Background, Qt::gray);   // change la couleur de fond
    this->setPalette(pallette);
    label = new QLabel(this);

    boutonImage1 = new QToolButton;
    boutonImage1->setIcon(QIcon(":/IMAGES/Images/led-off-100100.png")); // set une image
    boutonImage1 ->setIconSize(QSize(50,50));

    TitreLabel = new QLabel(tr("| LMT  Formation |"));
    TitreLabel->setStyleSheet("QLabel { color : black; font-size : 11.5pt; font-weight: bold }"); // met les écritures du label en blanches et gère la taille

    VitesseLabel = new QLabel(tr("Vitesse:"));
    VitesseLabel->setStyleSheet("QLabel { color : black; font-size : 10pt;}");  // met les écritures du label en blanches et gère la taille
    VitesseEdit = new QLineEdit();
    QString MaVitesse1 = VitesseEdit->text();

    VitesseEdit->setValidator( new QIntValidator(0, 300, this) );


    AccelerationLabel = new QLabel(tr("Acceleration (en pourcentage):"));
    AccelerationLabel->setStyleSheet("QLabel { color : black; font-size : 10pt; }");  // met les écritures du label en blanches et gère la taille
    AccelerationBox = new QSpinBox();

    FreinageLabel = new QLabel(tr("freinage (en pourcentage):"));
    FreinageLabel->setStyleSheet("QLabel { color : black; font-size : 10pt; }");  // met les écritures du label en blanches et gère la taille
    FreinageBox = new QSpinBox();

    ValidationButton = new QPushButton(tr("&Validation"));                             // création d'un bouton de Validation
    NettoyageButton = new QPushButton(tr("&Nettoyage"));                           // création d'un bouton de Nettoyage
    boutonQuitter = new QPushButton("Quitter",this);                                    // création d'un bouton pour Quitter

    VitesseLabel->setBuddy(VitesseLabel);
    AccelerationLabel->setBuddy(AccelerationBox);
    FreinageLabel->setBuddy(FreinageBox);

    ///====================| Création d'un écran LCD |========================///
    QLCDNumber *lcd = new QLCDNumber(3);
    lcd->setSegmentStyle(QLCDNumber::Outline);
    lcd->resize(90, 80);
    
    ///============te========| Conception des mappers |========================///
    mapper = new QDataWidgetMapper(this);
    mapper->addMapping(VitesseLabel, 0);
    mapper->addMapping(AccelerationLabel, 1);
    mapper->addMapping(FreinageLabel, 2);
    /*
     *Le mappage des données est déterminant pour mener à bien de nombreux processus liés aux données.
     *Une seule erreur de mappage des données peut avoir des répercussions sur l’ensemble de votre organisation,
     * entraînant une reproduction des erreurs et à terme, une analyse erronée.
     */

    ///====================| Connection des boutons |========================///
    connect(ValidationButton, &QAbstractButton::clicked, mapper, &QDataWidgetMapper::toPrevious);
    connect (ValidationButton, SIGNAL (clicked()), this, SLOT (AfficheValeur()));
    connect(NettoyageButton, &QAbstractButton::clicked, mapper, &QDataWidgetMapper::toNext);
    connect (NettoyageButton, SIGNAL (clicked()), this, SLOT (Nettoyer()));

    connect (boutonQuitter, SIGNAL (clicked()), qApp, SLOT (quit())); // permet de quitter le block

    ///======================| Couleur des styles |==========================///
    VitesseEdit->setStyleSheet ("color: #0a214c; background-color: #C4C3C2;");
    AccelerationBox->setStyleSheet ("color: #0a214c; background-color: #C4C3C2;");
    FreinageBox->setStyleSheet ("color: #0a214c; background-color: #C4C3C2;");
    lcd->setStyleSheet("color: #6ed527;");
    VitesseEdit->clear();

    ///====================| Conception des layouts |========================///
    QHBoxLayout *Ligne1 = new QHBoxLayout();
    Ligne1->addWidget(TitreLabel);                                    // positionne le Titre


    QHBoxLayout *Ligne2 = new QHBoxLayout();
    Ligne2->addWidget(VitesseLabel);                                 // positionne l'afficheur de vitesse
    Ligne2->addWidget(VitesseEdit);
    Ligne2->addWidget(ValidationButton);                       // positionne le boutton de validation

    QHBoxLayout *Ligne3 = new QHBoxLayout();
    Ligne3->addWidget(AccelerationLabel);                     // positionne l'afficheur d'accélération
    Ligne3->addWidget(AccelerationBox);                      // positionne l'afficheur d'accélération
    Ligne3->addWidget(NettoyageButton);                   // positionne le boutton de nettoyage (permettant de supprimer tout les valeurs entrées)

    QHBoxLayout *Ligne4 = new QHBoxLayout();
    Ligne4->addWidget(FreinageLabel);
    Ligne4->addWidget(FreinageBox);
    Ligne4->addWidget(boutonQuitter);

    QHBoxLayout *Ligne5 = new QHBoxLayout();
    Ligne5->addWidget(boutonImage1);
    Ligne5->addWidget(lcd);

    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout -> addLayout(Ligne1);
    mainlayout -> addLayout(Ligne2);
    mainlayout -> addLayout(Ligne3);
    mainlayout -> addLayout(Ligne4);
    mainlayout -> addLayout(Ligne5);
    setLayout(mainlayout);

    setContextMenuPolicy(Qt::ActionsContextMenu);

    QByteArray line;
    line = arduino -> readAll();
    int Distance = line.toInt();
    lcd->display(Distance);

    ///==============| condition de signaux |==============///
    if (Distance < 10)      // si la distance est trop faible un signal fait par un bouton rouge est mis
        {
        boutonImage1->setIcon(QIcon(":/IMAGES/Images/led-off-100100.png")); //Spécifier le chemin de l'image
        qDebug() << "Distance est inférieur à 10";
        }
    else                           // sinon l'icone est de couleur verte
        {
        boutonImage1->setIcon(QIcon(":/IMAGES/Images/led-on-100100.png")); //Spécifier le chemin de l'image
        qDebug() << "la distance est supérieur à 10";
        }
    if (Distance > 1000)
    {
        boutonImage1->setIcon(QIcon(":/IMAGES/Images/led-off-100100.png")); //Spécifier le chemin de l'image
        lcd->display("ERROR!!!");
        cout << "ERROR" << endl;
    }
    //qDebug()<<line;

    ///===============| mise en place des information de l'IHM |====================///
    setWindowTitle("Emulateur ACC");
    setWindowIcon(QIcon(":/IMAGES/Images/profilepicturev2.jpg"));


    ///==============| Mise en place d'un click droit pour quitter |====================///
    QAction *actionQuitter = new QAction("&Quitter", this);
    actionQuitter->setShortcut(QKeySequence(QKeySequence::Quit));
    addAction(actionQuitter);
    ///=====================| Connection avec Arduino |========================///
    validation = false;                                 // instancie automatiquement la validation à 0
    arduino_uno_port_name = "";            // le port de 'larduino est instancieé automatiquement à ""
    arduino = new QSerialPort;                 // récupère le port série de l'Arduino
    foreach (const QSerialPortInfo &serialPortInfo,QSerialPortInfo::availablePorts())               // pour ce qui est lu....
        {
        qDebug() << "je suis dans le foreach";
            if (serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier())        // récupère le numéro de production du produit
            {
                qDebug() << "je suis dans hasVendorIndentifier && hasProductIndentifier";
                qDebug() <<"id vendeur : " << serialPortInfo.vendorIdentifier();
                qDebug() <<"id produit : " <<  serialPortInfo.productIdentifier();
                if (serialPortInfo.vendorIdentifier() == serialPortInfo.vendorIdentifier())          // numéro du vendeur
                {
                    qDebug() << "je suis dans vendorId";
                    if (serialPortInfo.productIdentifier() == serialPortInfo.productIdentifier())    // récupère le numéro de production du produit
                    {
                        qDebug() << "je suis dans productIndentifier";
                     arduino_uno_port_name = serialPortInfo.portName();
                     qDebug() << "Name        : " << serialPortInfo.portName();
                     qDebug() << "Name        : " << serialPortInfo.hasVendorIdentifier();
                     qDebug() << "Name        : " << serialPortInfo.hasProductIdentifier();

                     validation = true;
                    }
                }
             }
         }

    ///=============|  Ouverture et configuration du port arduino si disponible |============///

        if (validation)
       {
              qDebug() << "Port arduino identifié....\n";
              arduino -> setPortName(arduino_uno_port_name);
              arduino -> open(QSerialPort::ReadOnly);
              arduino -> setBaudRate(QSerialPort::Baud115200);
              arduino -> setDataBits(QSerialPort::Data8);
              arduino -> setFlowControl(QSerialPort::NoFlowControl);
              arduino -> setParity(QSerialPort::NoParity);
              arduino -> setStopBits(QSerialPort::OneStop);
              qDebug() << "ca marche ....\n";
              QObject::connect(arduino,SIGNAL(readyRead()),this,SLOT(readSerial()));
              QMessageBox::information (this,"PORT Série", "Ouverture du port arduino effectuée");



        }
        else
        {
              qDebug() << "Impossible de trouver un port correct pour la carte arduino \n";
              QMessageBox::information (this,"Problème port série", "Ouverture du port arduino impossible");
        }
}

void AccEmul::AfficheValeur()
{
    QString MaVitesse = VitesseEdit->text();                                                                      //récupère "VitesseEdit" et la place dans "MaVitesse"
    qDebug () << MaVitesse << "Km/h";                                                                             // renvoie "MaVitesse" et "Km/h"
    MaVitesse.split(" ")[0].toInt();                                                                                       //converti en entier ce qui est récupéré
}

/*void AccEmul::readSerial()
{
    ///==============| condition de signaux |==============///
    if (Distance < 10)      // si la distance est trop faible un signal fait par un bouton rouge est mis
        {
        boutonImage1->setIcon(QIcon(":/IMAGES/Images/led-off-100100.png")); //Spécifier le chemin de l'image
        qDebug() << "Distance est inférieur à 10";
        }
    else                           // sinon l'icone est de couleur verte
        {
        boutonImage1->setIcon(QIcon(":/IMAGES/Images/led-on-100100.png")); //Spécifier le chemin de l'image
        qDebug() << "la distance est supérieur à 10";
        }
    if (Distance > 1000)
    {
        boutonImage1->setIcon(QIcon(":/IMAGES/Images/led-off-100100.png")); //Spécifier le chemin de l'image
        lcd.display("ERROR!!!");
        cout << "ERROR" << endl;
    }
    qDebug()<<line;
}
*/

AccEmul::~AccEmul()
{
    delete ui;
}

void AccEmul::Nettoyer()
{
    VitesseEdit->clear();
    AccelerationBox->clear();
    FreinageBox->clear();
}
