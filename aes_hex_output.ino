#include <AESLib.h>

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  uint8_t *key =  "abcdefghijklmnop";
  uint8_t *data = "Tech tutorials x";
  aes128_enc_single(key, data);

  Serial.println("Ciphered text:");
  for (int i = 0; i < 16; i++) 
  {
    char str[3];
    sprintf(str, "%02x", (int)data[i]);
    Serial.print(str);
  }

  aes128_dec_single(key, data);
  Serial.println("\n\nDeciphered text:");
  for (int i = 0; i < 16; i++) 
  {
    Serial.print((char)data[i]);
  }
}

void loop() 
{
  // put your main code here, to run repeatedly:

}
