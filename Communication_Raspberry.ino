// String nom = "Arduino";
// String msg = "";

//FONCTIONS>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

dat GetData() {
  dat data;
  String cons;
  long concatval;
  // data.mod=0;
  data.vit=vitesse;
  data.ang=angle;
  //Serial.println("avant");
  if (Serial2.available() > 0) {
    char first;
    //Serial.println("avant read");
    first = (char) Serial2.read();
    if (first == '0') {
      //Serial.println("avant readstring");
      cons = Serial2.readStringUntil('\n');
      Serial.println(cons.length());
      if (cons.length()==5) {
        Serial.println(cons);
        concatval = cons.toInt();
      // data.mod=0;
        data.vit=(float)(concatval/1000);
        data.ang=(float)(concatval%1000-180);
      }
    }
  }
  return data;
}

/*
void readSerial2Port()
{
  msg = "";
  if (Serial2.available())
  {
    delay(10);
    while(Serial2.available() > 0)
    {
      msg += (char) Serial2.read();
      Serial.print(msg);
    }
  }
  Serial2.flush();
  
}
void sendData()
{
  Serial2.print(nom);
  Serial2.print(" received: ");
  Serial2.print(msg);
}*/
