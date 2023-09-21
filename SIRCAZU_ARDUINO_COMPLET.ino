  #include <Wire.h>
  #include <MPU6050.h>
  #include <TinyGPS++.h>
  #include <SoftwareSerial.h>
  #include <WiFi.h>
  #include <HTTPClient.h>
  #include <ArduinoJson.h>
  
  
  //const int RXPin = 14;
  //const int TXPin = 17;
  //const int GPSBaud = 9600;
  // Déclaration de la broche d'entrée du capteur du choc
  const int choc = 17;
  // Déclaration de la broche de sortie LED 
  const int led = 5 ;
  // Pin du buzzer Use buzzer for alert 
  const int buzzerPin = 23;
  // Pin du gaz
  const int pinGaz = 26;
  // Pin du bouton
  const int buttonPin = 32;
  // Instance MPU6050
  MPU6050 mpu;
  // Seuil d'inclinaison
  const int seuilInclinaison = 70; // À ajuster en fonction de votre application
  // This is for input pin flame
  int FlamePin = 14;
  int Flame;
  int Gaz;
  
  
  int chocValeur = HIGH;
  boolean chocAlarme = false;
  unsigned long prevChocTime;
  
  int chocAlarmTime = 100;



//SoftwareSerial gpsSerial(RXPin, TXPin);


void setup() {
    Wire.begin();
    Serial.begin(9600);
    WiFi.begin("Zukunft","Gouverneur_Brad@2001#");
    while(WiFi.status()!= WL_CONNECTED){
    Serial.print("connexion en cours... :");
    Serial.println(WiFi.status());
    }
  
  
   // Initialisation de la broche de sortie
    pinMode (led, OUTPUT) ; 
   // Initialisation de la broche du capteur
    pinMode (choc, INPUT) ;  
   // Initialisation du MPU6050
    mpu.initialize();
   // Initialisation du buzzer
    pinMode(buzzerPin, OUTPUT);
   //pinMode(flame, INPUT);
    pinMode(Flame, INPUT);
   //pinMode(bouton, INPUT_PULLUP);
    pinMode(buttonPin, INPUT_PULLUP);
   //pinMode(gaz, INPUT);
    pinMode(pinGaz, INPUT);
  
    //gpsSerial.begin(GPSBaud);

}



void loop() {
  
  chocValeur = digitalRead(choc);

  if (chocValeur == LOW) {
    prevChocTime = millis();
                                                                                                                                                                                                                         
    if (!chocAlarme) {
      digitalWrite(led,HIGH);
      digitalWrite(buzzerPin, HIGH);
      delay(2000); 
      Serial.print("choc+++: ");
      Serial.println(chocAlarme);
      requete_post(); 
      //delay(5000); // Facultatif : ajoutez un délai pour éviter les détections répétées.
      chocAlarme = true;
    } 
  } else {
    if ((millis() - prevChocTime) > chocAlarmTime && chocAlarme) {
      digitalWrite(led,LOW);
      digitalWrite (buzzerPin, LOW);
      Serial.print("pas de choc: ");
      Serial.println(chocAlarme); 
      chocAlarme = false;
    }
  }
  

}



void requete_post(){
   String url="http://192.168.1.122:5000";
   String route="/insertion_capteur";
   String urls = url + route;
 
   
   const size_t capacity = JSON_OBJECT_SIZE(5);
   StaticJsonDocument<capacity> jsonDoc;
   jsonDoc["valeur_gaz"] = gaz();
   jsonDoc["valeur_flamme"] = FLAME();
   jsonDoc["valeur_choc"] = chocAlarme;
   jsonDoc["valeur_mpu"] = MPU();
   jsonDoc["valeur_bouton"] = bouton();

   HTTPClient http;
   http.begin(urls);
   http.addHeader("Content-type","application/json");


   String jsonString;
   serializeJson(jsonDoc, jsonString);

   int httpResponseCode = http.POST(jsonString);
      if(httpResponseCode > 0){
        Serial.print(" HTTP code :");
        Serial.println(httpResponseCode);
              Serial.println(MPU());
              Serial.println(FLAME());
              Serial.println(gaz());
              Serial.println(bouton());
        }else{
        Serial.print("erreur d'envoi HTTP code :");
        Serial.println(httpResponseCode);
        }
        http.end();
        delay(1000); 
 
}

void wifi(){
  WiFi.begin("Zukunft","Gouverneur_Brad@2001#");
  while(WiFi.status()!= WL_CONNECTED){
  Serial.print("connexion en cours... :");
  Serial.println(WiFi.status());
  }
}


float MPU(){
    // Lire les données de l'accéléromètre du MPU6050
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Calcul de l'inclinaison en degrés
  float inclinaison = atan2(ay, az) * (180.0 / PI);

  // Vérification de l'inclinaison par rapport au seuil
  if (abs(inclinaison) > seuilInclinaison) {
    // Inclinaison détectée, activer le buzzer
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(led,HIGH);
    //tone(buzzerPin, 1000, 500); // Sonnerie pendant 500 ms à 1 kHz
  } else {
    // Pas d'inclinaison, désactiver le buzzer
    //noTone(buzzerPin);
    digitalWrite (buzzerPin, LOW);
  }

    // Afficher les données de l'accéléromètre
    Serial.print("Inclinaison :");
    return inclinaison;
  
}


boolean FLAME(){
   int Flame = digitalRead(FlamePin);
   if (Flame == LOW) // si la flamme est detectée
    { 
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(led,HIGH);
      delay(1000);
      Serial.print("Flamme Detectée:");
      return 1;
     
    }
   else //si il ya aucune flamme
   {
      digitalWrite(led,LOW);
      digitalWrite (buzzerPin, LOW);
      Serial.print("Aucune flamme:");
      return 0;
    }

}



boolean bouton(){
  
    int buttonState = digitalRead(buttonPin); // read new state
    
  
    if (buttonState == LOW) {
      Serial.print("bouton préssé:");
      return 1;
      
    }
    else{
      Serial.print("bouton non-pressé:");
      return 0;
    }
    
}



boolean gaz(){
    int Gaz = digitalRead(pinGaz); 
    //int sensorValue = analogRead(pinGaz);
    //int *gazptr = &sensorValue;
  
   if ( Gaz == HIGH) {
      digitalWrite(led,HIGH);
      digitalWrite(buzzerPin, HIGH);
      delay(1000);
      Serial.print("Gaz detecté:");
      return 1;
    }
    else {
      digitalWrite(led,LOW);
      digitalWrite (buzzerPin, LOW);
      Serial.print("aucun gaz:");
      return 0;
    
    }

}


/*void gps(){
    while (gpsSerial.available() > 0)
    if (gps.encode(gpsSerial.read()))
      displayInfo();

    if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println("No GPS detected");
    while(true);
  }
  
}



void displayInfo()
{
  if (gps.location.isValid())
  {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Altitude: ");
    Serial.println(gps.altitude.meters());
  }
  else
  {
    Serial.println("Location: Not Available");
  }
  
  Serial.print("Date: ");
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print("/");
    Serial.print(gps.date.day());
    Serial.print("/");
    Serial.println(gps.date.year());
  }
  else
  {
    Serial.println("Not Available");
  }

  Serial.print("Time: ");
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(":");
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(":");
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(".");
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.println(gps.time.centisecond());
  }
  else
  {
    Serial.println("Not Available");
  }

  Serial.println();
  Serial.println();
  delay(1000);
} 
*/
