//__________________________________ programme élaboré par Karl Jubénot étudiant en BTS _____________________________________//
//_________ le but du programme est de pouvoir récupérer des information de vitesse et de distance en UDP ___________________//
//___ le programme doit ensuite transformer les informations de signaux, après calcul de distance de sécurité ua véhicule ___//
//_____________________ enfin le programme doit être diagnostiqué en TCP par un autre programme _____________________________//

//_________________________________ Program made by Karl Jubénot two years degree student ____________________________________//
//_________ the goal of this program is to receive speed and values from UDP B ___________________//
//___ le programme doit ensuite transformer les informations de signaux, après calcul de distance de sécurité ua véhicule ___//
//_____________________ enfin le programme doit être diagnostiqué en TCP par un autre programme _____________________________//



/////////////////////////////////////////////////////////////
//_________________________CAN TX__________________________//
/////////////////////////////////////////////////////////////

// ce programme permet l'envoie de trames CAN à un véhicule
// le véhicule simulé a pour ID de trames CAN 0x108 [108]
// l'ID de trames de trames sera visible lors d'une étude des trames du véhicule 

#include <SPI.h>                      // Inclusion de la librairie SPI.h
#include <Ethernet.h>                 // Inclusion de la librairie Ethernet afin de pouvoir communiquer avec le sheild ethernet
#include <EthernetUdp.h>              // Inclusion de la librairie Ethernet UDP afin de pouvoir communiquer avec le sheild ethernet des trames UDP
#include "SR04.h"                     // Inclusion de la librairie SR04 lié au capteur HC-SR04
#include <String.h>

#include <Wire.h>
#include <mcp_can.h>
#include <SPI.h>

const int spiCSPin = 53;
const byte buttonPin = 3;             // Numéro du pin pour le bouton de poussoir
const byte buttonPin2 = 18;           // Numéro du pin pour le bouton de poussoir
volatile byte state = LOW;            // Mets en low  volatile = varibale suceptible de changer sans que le main soit au courant
volatile byte stateAcc = LOW; 

// Afin de réaliser des signaux de distance on réalise le montage d'une LED RGB

#define RED 6                         // Definie la couleur rouge sur la PIN 6
#define GREEN 5                       // Definie la couleur verte sur la PIN 5
#define BLUE 4                        // Definie la couleur bleue sur la PIN 4
#define TRIG_PIN 38                   // Renvoie les information pour pouvoir capter un echo
#define ECHO_PIN 39                   // Récupère les information d'écho
#define buzzer 2                      // Affectation des broches
SR04 sr04 = SR04(ECHO_PIN, TRIG_PIN); // Lie ECHO_PIN et TRIG_PIN du capteur HC-SR04
long distance;                        // On donne une variable [distance] prend en entrée, pour les distance


byte mac[] = {0x90, 0xA2, 0xDA, 0x11, 0x1C, 0x99};
///___________________IP____________________///
IPAddress ip(192, 168, 1, 23);        // Donne une adresse IP a notre arduino pour le bon fonctionnement des trames UDP
IPAddress server(192,168,1,1);    //ip du serveur

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
unsigned int localPortTCP = 5500;     // j'alloue un port local
unsigned int localPortUDP = 3698;     // j'alloue un port local
EthernetServer serverTCP(5500);
EthernetServer serverUDP(3698);
EthernetClient client;                //créer un client



// les buffers les buffer permentent d'envoyer ou de recvoir des données
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer permettant de tenir en memoire les valeurs,
char ReplyBuffer[] = "acknowledged"; 

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///______________________________________________________Void Setup_______________________________________________________________///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
    // initialisation des pin des deux boutons de poussoirs à un input :
    pinMode(buttonPin, INPUT); //Définir mon entree/sortie avec le boutonStart
    pinMode(buttonPin2, INPUT); //Définir mon entree/sortie avec le boutonAcc
    attachInterrupt(digitalPinToInterrupt(buttonPin), PushButton, RISING);    // interruption qui se déclenche quand on passe de 0 à 1 (RISING) appel la fonction PushButton
    attachInterrupt(digitalPinToInterrupt(buttonPin2), PushButtonAcc, RISING);
    Serial.println ("***Debut transmission***"); //Message pour prévenir la transmission
    
    SERIAL_PORT_MONITOR.begin(115200);
    while(!Serial){};

    #define CAN_500KBPS  16
    CAN.begin(CAN_500KBPS);

   // while (CAN_OK != CAN.begin(CAN_500KBPS)) {                    // Initilise les bauds ou bits par secondes à 500000
        //SERIAL_PORT_MONITOR.println("CAN init fail, retry..."); // Si le transfert ne se fait pas à 500000 bauds une erreur est renvoyée
        //delay(100);
    //}
    //SERIAL_PORT_MONITOR.println("CAN init ok!");                // Dans l'autre cas, un message de validation est envoyé
    Serial.begin(115200);
    //pinMode(A0,INPUT);

    pinMode(RED, OUTPUT);                                         // Met en sortie la PIN 6
    pinMode(GREEN, OUTPUT);                                       // Met en sortie la PIN 8
    pinMode(BLUE, OUTPUT);                                        // Met en sortie la PIN 4


    ///___________________TCP / UDP___________________///
    Ethernet.begin(mac,ip); // Démarage du sytsème Ethernet

    // Open serial communications and wait for port to open:
    while (!Serial) {
      ; // attente du port série. Nécessite seulement un port USB
    }
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      //Serial.println("le shield ethernet n'a pas était trouvé, le precessus ne peut pas fonctionner sans hardware :( ");
      while (true) {
        delay(10); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkON) {
      //Serial.println("câble ethernet connecté");      // Renvoie si le cable n'est pas connécté a la seconde arduino
    }

    Udp.begin(localPortUDP);   // Démarage de l'UDP

    // start the Ethernet connection and the server:
    Ethernet.begin(mac, ip);

    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      //Serial.println("Shield Ethernet non trouve.  Impossible de continuer.");
      while (true) {
      delay(10); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      //Serial.println("Cable Ethernet non connecte.");         // Renvoie si le cable n'est pas connécté a la seconde arduino
    }

    //  démarage du serveur TCP
    if (client.connect(server, 5500)) { 
    Serial.println("connecté");
    Serial.print("Mon adresse IP: ");
    Serial.println(ip);
  }
  else{
    // if you didn't get a connection to the server:
    Serial.println("connexion échouée");
  }
  }

unsigned char trames[8] = {0, 0, 0, 0, 0, 0, 0, 0};             // Initilaise la liste de données à 0, cette dernière est vide
                                                                // La liste que nous venons de creer permettra de composer une trames can


    
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///_______________________________________________________Void Loop_______________________________________________________________///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
   CAN.sendMsgBuf(0xB6, 0, 8, trames);      // Renvoie dans la trames l'id 108, LSB 0 et 8 qui est la taille du message enfin ce que nous avons écris 
  GREENColor();                             // fonction qui renvoie un signal vert sur la LED RGB
  YELLOWColor();                            // fonction qui renvoie un signal jaune sur la LED RGB
  REDColor();                               // fonction qui renvoie un signal bleu sur la LED RGB

///___________________TCP___________________///
  if (client.available()) 
    {
      char inChar = client.read();                        //lit se qui est envoyé
      delay(1);
      Serial.print(inChar);                               //affiche se qui est envoyé dans le moniteur série
      if (inChar == 's') client.print(state);  
      if (inChar == 'c') client.print(stateAcc);
      if (inChar == 'd' && packetBuffer[2] < packetBuffer[4]*0.6) client.print("distance de sécurité non respéctée");
      else if (inChar == 'd' && packetBuffer[2] > packetBuffer[4]*0.6) client.print("distance de sécurité respéctée");
      if (inChar == 'r' && packetBuffer[2] < packetBuffer[4]*0.4) client.print("Freinage en cours");
    }

///___________________UDP___________________///
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) 
    {
      //Serial.print("Received packet of size ");
      //Serial.println(packetSize);
      //Serial.print("From ");
      IPAddress remote = Udp.remoteIP();
      for (int i=0; i < 4; i++) 
        {
          //Serial.print(remote[i], DEC);
          if (i < 3) 
            {
              Serial.print(".");
            }
        }
    }

  // read the packet into packetBufffer
  Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
  //Serial.println("Contents:");
  //Serial.println(packetBuffer);
  //Serial.println(UDP_TX_PACKET_MAX_SIZE);
  Serial.print(packetBuffer[0]);                // Affinche l'ID
  Serial.print(packetBuffer[2]);                // Affiche les valeurs de distance 
  Serial.println(packetBuffer[5]);              // Affiche las valeurs de vitesses
  delay(50);
  if (packetBuffer[0]== 'C')
    {
      if (packetBuffer[1] < packetBuffer[4]*0.6)
        {
           trames[2]=10;            // La valeur que j'écris ici est la valeur en décimal de ce qui est transmit dans la trame sous forme de valeur hexadécimal     [a]
        }
      else if (packetBuffer[1] < packetBuffer[4]*0.4)
        {
           trames[2]=15;            // La valeur que j'écris ici est la valeur en décimal de ce qui est transmit dans la trame sous forme de valeur hexadécimal     [f]
        }
      else if (packetBuffer[1] > packetBuffer[4]*0.6)
        {
           trames[2]=11;            // La valeur que j'écris ici est la valeur en décimal de ce qui est transmit dans la trame sous forme de valeur hexadécimal     [b]
        }
    }
  delay(50);
  sendData();                         // Appel de fonction pour envoyer
  receiveddata();                     // Appel de fonction pour recevoir
 
}
void REDColor()
{
  if (distance < 10)
  {
    //Serial.println("distance faible");
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, LOW);
    delay (10);
    trames[3]=10;           // la valeur que j'écrit ici est la valeur en décimal de ce qui est transmis dans la trames sous forme de valeur hexadécimal     [a]
    trames[4]=10;           // la valeur que j'écrit ici est la valeur en décimal de ce qui est transmis dans la trames sous forme de valeur hexadécimal     [a]
    trames[5]=11;           // la valeur que j'écrit ici est la valeur en décimal de ce qui est transmis dans la trames sous forme de valeur hexadécimal     [b]
  }
}

void YELLOWColor()
{
  if (distance <= 20 &&  distance >= 10)
  {
    //Serial.println("distance faible, freinage en cours");
    analogWrite(RED, 255);
    analogWrite(GREEN, 255);
    analogWrite(BLUE, 0);
    delay (10);
    trames[4]=10;           // la valeur que j'écrit ici est la valeur en décimal de ce qui est transmis dans la trames sous forme de valeur hexadécimal     [a]
    trames[5]=12;           // la valeur que j'écrit ici est la valeur en décimal de ce qui est transmis dans la trames sous forme de valeur hexadécimal     [c]
  }
}

void GREENColor()
{
  if (distance > 20)
  {
    //Serial.println("distance ok");
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, LOW);
    delay (10);
    trames[4]=10;           // la valeur que j'écrit ici est la valeur en décimal de ce qui est transmis dans la trames sous forme de valeur hexadécimal     [a]
    trames[5]=13;           // la valeur que j'écrit ici est la valeur en décimal de ce qui est transmis dans la trames sous forme de valeur hexadécimal     [d]
  }
}

void PushButton()
{
  state = !state; // Change l'état "prends la négation"
}

void PushButtonAcc()
{
  stateAcc = !stateAcc; 
}

void sendData()
{
  /*
   * Fonction pour envoyer des données à mon IHM
   */
   
  //Condition pour démarrer et éteindre le système

  if (state == 0 && stateAcc == 0)
  {
    Serial.println("f");    //  bouton d'allumage du véhicule "Off"
    trames[3]=15;           // la valeur que j'écrit ici est la valeur en décimal de ce qui est transmis dans la trames sous forme de valeur hexadécimal
                            // cette trames permet l'activation du système
    return trames; // renvoie la trames
  }
  else if (state == 0 && stateAcc == 1)
  {
    Serial.println("a");
    stateAcc = 0;           //Quand le bouton allumage est sur "OFF", on éteint automatiquement l'Acc
    trames[3]=10;           // la valeur que j'écrit ici est la valeur en décimal de ce qui est transmis dans la trames sous forme de valeur hexadécimal
                            // cette trames permet l'exctionction automatique la première valeur est 100
   return trames; // renvoie la trames
  }
  
  //Condition pour démarrer l'Acc
  if (stateAcc == 0 && state ==1) { //si
    Serial.println("nD");   // bouton de démarrage ON et désactivation Acc de mon IHM
    trames[4]=15;           // la valeur que j'écrit ici est la valeur en décimal de ce qui est transmis dans la trames sous forme de valeur hexadécimal
                            // cette trames permet la désactivation de l'ACC après allumage
    } 
    else { if (stateAcc == 1 && state ==1)
    Serial.println("nA");   //  bouton de démarrage ON et activation Acc de mon IHM
    trames[4]=10;           // la valeur que j'écrit ici est la valeur en décimal de ce qui est transmis dans la trames sous forme de valeur hexadécimal
                            // cette trames permet l'activation de l'ACC après allumage
    return trames; // renvoie la trames
  }
  
  delay (100); //délai pour éviter que mon IHM crash car écriture dans le port série trop rapide

}

void receiveddata()
{
  /*
   * Fonction pour recvoir des données de mon IHM
   */
   
  const int received_data = Serial.read(); //Lecture de mon port série
  
  if (received_data=='o') //Signal pour allumer le système
  {
    state = 1;
    //Serial.println("Received 'o'"); //Pour du débug
  }
  else if (received_data=='k') //Signal pour éteindre le système
  {
    state = 0;
    //Serial.println("Received 'k'"); //Pour du débug
  }
  if (received_data == 'm') //Signal pour allumer l'ACC
  {
    stateAcc = 1;
    //Serial.println("Received 'b'"); //Pour du débug
  }
  else if (received_data =='l') //Signal pour éteindre l'ACC
  {
    stateAcc = 0;
    //Serial.println("Received 'l'"); //Pour du débug
  }
}
