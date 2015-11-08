void setup()
{
    Serial.begin(115200);
} 

String buff = "";
void loop()
{
    if(Serial.available())
    {
        char c = Serial.read();
        if(c == '\n')
        {
            float ratio = buff.toFloat();
            buff = "";
            float conc = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; 
            Serial.print(conc);
            Serial.print("\n");
        }
        else
            buff += c;
    }
}
