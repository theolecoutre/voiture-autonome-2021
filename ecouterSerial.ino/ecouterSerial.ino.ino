String nom = "Arduino";
String msg = "";

void setup()
{
  Serial2.begin(9600);
  Serial.begin(9600);
}


void loop()
{
 if (Serial2.available() > 0)
 {
  char first;
  first = (char)  Serial2.read();
  if (first == '0')
  {
    String data = Serial2.readStringUntil('\n');
    Serial.println(data);
  }
 }
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
