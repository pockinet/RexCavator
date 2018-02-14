#include <FUTABA_SBUS.h>  //Library zum Einlesen des SBUS-Signals
#include <Streaming.h>  //Vereinfacht zeilenweise Ausgabe im Serialmonitor

FUTABA_SBUS sBus; //sBus wird instanziert, verwendet den Serial1-RX-Pin

const int deadzone = 10; //Legt einen Totbereich in der Mitte jedes Proportionalventils fest
const int idle = 0; //Legt den Tastgrad im deadzone für den Leerlauf fest

int channel[] {0,1,2,3,4,5,6,7};  //Kanal-Eingangswerte nach Mapping

const byte backflowPin = 30; //Ausgangspin für das Rücklauf-Magnetventil

const byte encoderPin[] {A0 , A1, A2, A3};  //Analoge Eingangspins für die Absolutwinkelgeber für Oberwagen, Ausleger, Stiel und Löffel
int encoderPosition[]   {1  ,  2,  3,  4};  //Eingangswerte der Absolutwinkelgeber für Oberwagen, Ausleger, Stiel und Löffel
const int deadEnd[]     {900,200,200,200};  //Wert für Software-Endanschlag

int dutyCycle[]     {1,2,3,4,5,6,7,8}; //PWM-Werte für die 8 Analogausgänge
const byte pwmPin[] {2,3,4,5,6,7,8,9}; //Je zwei Ausgangspins für Oberwagen, Ausleger, Stiel und Löffel

void setup()
{
  sBus.begin();
  Serial.begin(115200);
}

void loop()
{
  //Liest die Kanäle aus und schreibt sie in sBus.channels[i]:
  sBus.FeedLine();
  if (sBus.toChannels == 1)
  {
    sBus.UpdateChannels();
    sBus.toChannels = 0;
  }  

  //Rechnent die Kanalrohwerte auf 0-255 um:
  for (byte i = 0; i < 8; i++)
  {
   channel[i] = map(sBus.channels[i], 170, 1811, 0, 255);
  }  

  //Erzeugt die PWM-Signale für die acht PWM-Ausgänge:
  dutyCycle[0] = channel[0] * 2 - 255;
  dutyCycle[1] = 255 - channel[0] * 2;
  dutyCycle[2] = channel[1] * 2 - 255;
  dutyCycle[3] = 255 - channel[1] * 2;
  dutyCycle[4] = channel[2] * 2 - 255;
  dutyCycle[5] = 255 - channel[2] * 2;
  dutyCycle[6] = channel[3] * 2 - 255;
  dutyCycle[7] = 255 - channel[3] * 2;

  //Software-deadzone wird erzeugt:
  for (byte i = 0; i < 8; i++) 
  {
    if (dutyCycle[i] < deadzone)
    {  
      dutyCycle[i] = idle;
    }
  }  

  //Sind alle acht PWM-Werte == 0, dann Rücklauf = HIGH:
  if (dutyCycle[0] == idle && dutyCycle[1] == idle && dutyCycle[2] == idle && dutyCycle[3] == idle && dutyCycle[4] == idle && dutyCycle[5] == idle && dutyCycle[6] == idle && dutyCycle[7] == idle)
  {
    digitalWrite(backflowPin, HIGH);
    //Serial<<"Rücklauf offen \t";
  }
  else
  {
    digitalWrite(backflowPin, LOW);
    //Serial<<"Rücklauf geschlossen \t";
  }  

/*  
  //Wenn der Software-Endanschlag des Auslegers erreicht ist wird das Ventil auf Leerlauf geschaltet:
  encoderPosition[1] = analogRead(encoderPin[0]);
  if (encoderPosition[1] >= deadEnd[1] && dutyCycle[1] >= idle)
  {
    dutyCycle[2] = idle;
    dutyCycle[3] = idle;
  }
*/

  //Analog-Write-Schleife zum Ansteuern der acht PWM-Pins:
  for (byte i = 0; i < 9; i++)
  {
  analogWrite(pwmPin[i], dutyCycle[i]);
  } 
 
  //Serial<<sBus.channels[0]<<"\t"<<sBus.channels[1]<<"\t"<<sBus.channels[2]<<"\t"<<sBus.channels[3]<<"\t"<<sBus.channels[4]<<"\t"<<sBus.channels[5]<<"\t"<<sBus.channels[6]<<"\t"<<sBus.channels[7]<<endl;
  //Serial<<channel[0]<<"\t"<<channel[1]<<"\t"<<channel[2]<<"\t"<<channel[3]<<"\t"<<channel[4]<<"\t"<<channel[5]<<"\t"<<channel[6]<<"\t"<<channel[7]<<endl; 
  //Serial<<dutyCycle[0]<<"\t"<<dutyCycle[1]<<"\t"<<dutyCycle[2]<<"\t"<<dutyCycle[3]<<"\t"<<dutyCycle[4]<<"\t"<<dutyCycle[5]<<"\t"<<dutyCycle[6]<<"\t"<<dutyCycle[7]<<endl;
  //Serial<<encoderPosition[0]<<"\t"<<dutyCycle[0]<<"\t"<<dutyCycle[1]<<endl;
}
