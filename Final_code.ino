// library SPFD5408 
#include <SPFD5408_Adafruit_GFX.h>    // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library
#include <SPFD5408_TouchScreen.h>     // Touch library
#include <MD5.h>
#include <AESLib.h>
#include <SPI.h>
#include <SD.h>



//Define LCD pins
#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

//Colors
#define BLACK   0x0000
#define BROWN   0x7980
#define RED     0xF800
#define ORANGE  0xFBE0
#define YELLOW  0xFFE0
#define GREEN   0x07E0
#define BLUE    0x001F
#define VIOLET  0xA81F
#define GREY    0x7BEF
#define WHITE   0xFFFF
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define LGREEN   0xAFE0

// Init LCD
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

//Pressure Range
#define MINPRESSURE 10
#define MAXPRESSURE 1200

//Calibrate TFT LCD
int TS_MINX = 125; 
int TS_MINY = 83; 
int TS_MAXX = 912; 
int TS_MAXY = 865; 

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

int i=0;
int j=0;
int a=84;
int b=0;
int c=0;
int X,Y;
int z=0;
int k=0;

boolean Caps=false;
boolean keypadIndex=true;
boolean passPhraseMode=false;
int numberofWords = 1;

boolean modeState;

String symbolset1[4][5] = {
  { "1", "2", "3", "4", "5"},
  { "Q", "W", "E", "R", "T"},
  { "A", "S", "D", "F", "G"},
  { ".", "Z", "X", "C", "V"}
};

String symbolset2[4][5] = {
  {"6", "7", "8", "9", "0" },
  {"Y", "U", "I", "O", "P" },
  {"H", "J", "K", "L", "." },
  {".", "B", "N", "M", "." }
};

String words = "";
String allWords;

String passPhrase1 = "";
String passPhrase2 = "";
String passPhrase3 = "";

String key1 = "";
String key2 = "";
String key3 = ""; 
char data[30];
char data1[30];
String sdString;

uint8_t charpass1[40];
uint8_t charpass2[40];
uint8_t charpass3[40];

String wordString = "apple|mango|white|red|green|black|blue|brown|pink|yellow|nut|max|min|orange|violate|grey|cyan|magenta";
char charpass[30] ;
int indexArray[35];




String make_hash()
{
   //initialize serial
  Serial.begin(9600);
  //give it a second
  delay(1000);
  //generate the MD5 hash for our string
  unsigned char* hash = MD5::make_hash(charpass); 
 
  //generate the digest (hex encoding) of our hash
  char *md5str = MD5::make_digest(hash, 16);
  free(hash);
  //print it on our serial monitor
  
  String passPhrase = String(md5str);
  Serial.println(md5str);
  Serial.println(passPhrase);
  //Give the Memory back to the System if you run the md5 Hash generation in a loop
  free(md5str); 

  return passPhrase;
}

void encrypt_words()
{
   allWords.toCharArray(data, (allWords.length()+1));
   key1.toCharArray(charpass1, (key1.length()+1));
   key2.toCharArray(charpass2, (key2.length()+1));
   key3.toCharArray(charpass3, (key3.length()+1));

   aes128_enc_single(charpass1, data);
   aes128_enc_single(charpass2, data);
   aes128_enc_single(charpass3, data);

   Serial.println("Ciphered text:");
   for (int i = 0; i < 16; i++) 
   {
    char str[3];
    sprintf(str, "%02x", (int)data[i]);
    Serial.print(str);
   } 

}

void decrypt_words()
{   
  File myFile;
  if (!SD.begin(22)) 
  {
    Serial.println("initialization failed!");
    while (1);
  } 
  myFile = SD.open("string.txt");

  if (myFile) {
   for (int i = 0; i < 16; i++) 
   {
    data1[i] = myFile.read();
    Serial.write(data1[i]);
   }
   myFile.close();
  }
  else 
  {
    // if the file didn't open, print an error:
    Serial.println("error opening string.txt");
  }
   Serial.println();

   aes128_dec_single(charpass3, data1);
   aes128_dec_single(charpass2, data1);
   aes128_dec_single(charpass1, data1);
  
   Serial.println("\n\nDeciphered text:");
   for (int i = 0; i < 16; i++) 
   {
     Serial.print((char)data1[i]);
   }
}


void write_string()
{ 
  File myFile;

  if (!SD.begin(22)) 
  { 
    Serial.println();
    Serial.println("initialization failed!");
    while (1);
  } 
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("string.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if(myFile) 
  { 
    Serial.print("Writing to string.txt...");
    for (int i = 0; i < 16; i++) 
    {
     myFile.print(data[i]);
     Serial.print(data[i]);
    } 
     Serial.println();
     myFile.println();
    // close the file:
    myFile.close();
    Serial.println("Written");
  } 
  else 
  {
    //if the file didn't open, print an error:
    Serial.println("error opening encryption.txt");
  } 
}




void setup() 
{
  Serial.begin(9600); //Serial monitor for debugging
  

  tft.reset(); //reset at start
  tft.begin(0x9341); //Adress of lil9341 Interface driver IC
  tft.setRotation(3); //Rotated according to the use
  tft.invertDisplay(1);
  //IntroScreen();
  home:
  modeSelection();

  if(modeState)
  {
  numberofWord();
  for(k = 1 ; k <=numberofWords ; k++)
  {
    keyPad();//Function for collecting the strings from the user
    tft.setCursor(0,0);
    DetectButtons(); 
  }
  for(z = 1 ; z <4 ; z++)
  { 
    a = 156;
    passPhraseMode = true;
    keyPad();//Function for collecting the strings from the user
    tft.setCursor(0,0);
    DetectButtons(); 
  }
  
  Serial.print("Final String:");
  Serial.println(allWords);
  Serial.print("Pass Phrase1:");
  Serial.println(passPhrase1);
  Serial.print("Pass Phrase2:");
  Serial.println(passPhrase2);
  Serial.print("Pass Phrase3:");
  Serial.println(passPhrase3);
  delay(2000);
  encrypt_words();
  writeReadConsole();
  write_string();
  delay(2000);
  goto home;
  }
  else
  {
   for(z = 1 ; z <4 ; z++)
   { 
    a = 156;
    passPhraseMode = true;
    keyPad();//Function for collecting the strings from the user
    tft.setCursor(0,0);
    DetectButtons(); 
   }
  writeReadConsole();
  delay(2000);
  decrypt_words();
  showListofWords();
  delay(10000); 
  goto home; 
  }
}

void loop() 
{
 /* for testing
  *TSPoint p = waitTouch();
 tft.setCursor (a,b);
 tft.setTextSize (2);
 tft.setTextColor(WHITE);
 DetectButtons();
 delay(300);*/
}


TSPoint waitTouch() 
{
  TSPoint p;
  do{
    p = ts.getPoint(); 
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
   }while((p.z < MINPRESSURE )|| (p.z > MAXPRESSURE));
   Serial.print(p.x); Serial.print('|'); Serial.println(p.y);
   p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
   p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);
   X = p.y; 
   Y = p.x;
   //Serial.print(X); Serial.print(','); Serial.println(Y);
   return p;
}


void DetectButtons()
{ 
  while(1)
  {
   TSPoint p = waitTouch();
   tft.setCursor (a,b);
   tft.setTextSize (2);
   tft.setTextColor(WHITE);

   if(keypadIndex)
   {
    if (0<X && X<60 && 200<Y) //Caps lock
    {Caps = !Caps;
      Serial.print(Caps);
      a=a-12;
        if(Caps)
        {
         tft.setCursor (275, 58);
         tft.setTextSize (1);
         tft.setTextColor(WHITE);
         tft.println("CAPS ON");
         searchWord();
        }
        else 
         tft.fillRect (275,55,50,10,BLACK);
         delay(500);
     }
     else if ( 300<X && 0<Y && Y<60)
     {
        keypadIndex = !keypadIndex ;
        {
          keyPad();
          continue;
        }
     }
    //row 4 button detections
    else if ( 65<X && X<125 && 200<Y)
        {       
            if (Caps)
             {
             tft.print("Z");
             words += "Z";
             }
            else
             {
             tft.print("z");
             words += "z" ;
             }
        }
    else if ( 130<X && X<190 && 200<Y)    
        {       
             if (Caps)
             {
             tft.print("X");
             words =words + "X";
             }
            else
             {
             tft.print("x");
              words += "x";
             } 
          }
    else if ( 195<X && X<255 && 200<Y)    
        {       
             if (Caps)
             {
             tft.print("C");
             words += "C";
             }
            else
             {
             tft.print("c");
              words += "c";
             } 
         }

    else if ( 260<X && 200<Y)    
        {       
             if (Caps)
             {
             tft.print("V");
             words += "V";
             }
            else
             {
             tft.print("v");
             words += "v";
             }
          }
    //row 3 button detections
    else if ( 0<X && X<60 && 155<Y && Y<195)    
        {       
             if (Caps)
             {
             tft.print("A");
             words += "A";
             }
            else
             {
             tft.print("a");
              words += "a";
             }
          }
    else if ( 65<X && X<125 && 155<Y && Y<195)    
        {       
             if (Caps)
             {
             tft.print("S");
             words += "S";
             }
            else
             {
             tft.print("s");
              words += "s";
             } 
          }
    else if ( 130<X && X<190 && 155<Y && Y<195)    
        {       
             if (Caps)
             {
             tft.print("D");
             words += "D";
             }
            else
             {
             tft.print("d");
              words += "d";
             }
          }
    else if ( 195<X && X<255 && 155<Y && Y<195)    
        {       
             if (Caps)
             {
             tft.print("F");
             words += "F";
             }
            else
             {
             tft.print("f");
              words += "f";
             } 
          }
    else if ( 260<X && X<320 && 155<Y && Y<195)    
        {       
             if (Caps)
             {
             tft.print("G");
             words += "G";
             }
            else
             {
             tft.print("g");
              words += "g";
             } 
          }
     //row 2 button detections
   else if ( 0<X && X<60 && 105<Y && Y<145)    
        {       
             if (Caps)
             {
             tft.print("Q");
             words += "Q";
             }
            else
             {
             tft.print("q");
              words += "q";
             } 
          }
    else if ( 65<X && X<125 && 105<Y && Y<145)    
        {       
             if (Caps)
             {
             tft.print("W");
             words += "W";
             }
            else
             {
             tft.print("w");
              words += "w";
             } 
          }
    else if (130<X && X<190 && 105<Y && Y<145)    
        {       
             if (Caps)
             {
             tft.print("E");
             words += "E";
             }
            else
             {
             tft.print("e");
              words += "e";
             } 
          }
    else if ( 195<X && X<255 && 105<Y && Y<145)    
        {       
             if (Caps)
             {
             tft.print("R");
             words += "R";
             }
            else
             {
             tft.print("r");
              words += "r";
             } 
          }
    else if ( 260<X && X<320 && 105<Y && Y<145)    
        {       
             if (Caps)
             {
             tft.print("T");
             words += "T";
             }
            else
             {
             tft.print("t");
              words += "t";
             }
         }
     //row 1 button detections
    else if (0<X && X<60 && 66<Y && Y<100)    
        {       
            tft.print("1");
            words += "1" ;
        }
    else if (65<X && X<125 && 66<Y && Y<100)    
        {       
            tft.print("2");
            words += "2";
        }
    else if (130<X && X<190 && 66<Y && Y<100)    
        {       
            tft.print("3");
            words += "3";
        }
    else if (195<X && X<255 && 66<Y && Y<100)    
        {       
            tft.print("4");
            words += "4";
        }
    else if ( 260<X && X<320 && 66<Y && Y<100)    
        {       
            tft.print("5");
            words += "5" ;
        }
    else if ( 235<X && X<295 && 0<Y && Y<50)    
        {       
            Serial.println("Find");
            a=a-12;
            searchWord() ;
        }
    else
        {
          continue; 
        }
        a=a+12;
        if(a>225)
        {
          b=b+20;
          a=0;
          if(b >=60)
           b = 0;
        }
        delay(300);
   }
   else
   {
        if ( 290<X && 0<Y && Y<60)
        {
          keypadIndex = !keypadIndex ;
        {
          keyPad();
          continue;
        }
        }
        //row4
        else if (0<X && X<60 && 200<Y)//Space
        {       
             tft.print(" "); 
             words += " ";
        }
        else if ( 65<X && X<125 && 200<Y)    
        {       
             if (Caps)
             {
             tft.print("B");
             words += "B";
             }
            else
             {
             tft.print("b");
              words += "b";
             } 
          }
          
        else if (130<X && X<190 && 200<Y)    
        {       
             if (Caps)
             {
             tft.print("N");
             words += "N";
             }
            else
             {
             tft.print("n");
              words += "n";
             } 
          }

        else if(195<X && X<255 && 200<Y)    
        {       
             if (Caps)
             {
             tft.print("M");
             words += "M";
             }
            else
             {
             tft.print("m");
              words += "m";
             } 
          }
        else if (260<X && X<320 && 200<Y)//Reset
        {       
             Serial.println("Reset"); 
             tft.fillRect(0,0,300,62,BLACK);
             a=-12;
             b=0;
             words = "";
             keyPad();
             if(passPhraseMode == false)
             {
              a = 72;
              b = 0;
             }
             else
             {
              a = 144;
              b = 0;
             }
             
        }

        //Row 3
        else if (0<X && X<60 && 155<Y && Y<195)    
        {       
             if (Caps)
             {
             tft.print("H");
             words += "H";
             }
            else
             {
             tft.print("h");
             words += "h";
             } 
          }
        else if (65<X && X<125 && 155<Y && Y<195)    
        {       
             if (Caps)
             {
             tft.print("J");
             words += "J";
             }
            else
             {
             tft.print("j");
             words += "j";
             }  
        }
        else if (130<X && X<190 && 155<Y && Y<195)    
        {       
            if (Caps)
             {
             tft.print("K");
             words += "K";
             }
            else
             {
             tft.print("k");
             words += "k";
             } 
          }
        else if (195<X && X<255 && 155<Y && Y<195)    
        {       
            if (Caps)
             {
             tft.print("L");
             words += "L";
             }
            else
             {
             tft.print("l");
             words += "l";
             }  
        }
        else if (260<X && X<320 && 155<Y && Y<195)//Enter
        {    
             Serial.print("word: ");  
             Serial.println(words);
             if(passPhraseMode == false)
             {
              if(k == numberofWords)
                {
                  allWords = allWords + words + "?";
                }
               else
                  allWords = allWords + words + "|";
                  
              Serial.println(allWords);
             }
             else
             {
              if( z == 1)
               {
                passPhrase1 = words;
                passPhrase1.toCharArray(charpass, (passPhrase1.length()+1));
                Serial.println(charpass);
                passPhrase1 = make_hash();
                passPhrase1.toCharArray(charpass1, (passPhrase1.length()+1));
                key1 = passPhrase1;
               }
              else if(z==2)
               {
                passPhrase2 = words;
                passPhrase2.toCharArray(charpass, (passPhrase2.length()+1));
                Serial.println(charpass);
                passPhrase2 = make_hash();
                passPhrase2.toCharArray(charpass2, (passPhrase2.length()+1));
                key2 = passPhrase2;
               }
              else if(z==3)
              {
               passPhrase3 = words;
               passPhrase3.toCharArray(charpass, (passPhrase3.length()+1));
               Serial.println(charpass);
               passPhrase3 = make_hash();
               passPhrase3.toCharArray(charpass3, (passPhrase3.length()+1));
               key3 = passPhrase3;
               passPhraseMode = false;
              }
             }
             words = ""; 
             if(passPhraseMode == false)
             {
              a = 84;
              b = 0;
             }
             else
             {
              a = 156;
              b = 0;
             }
             break;   
        }
        //row 2
        else if ( 0<X && X<60 && 105<Y && Y<145)    
        {       
            if (Caps)
             {
             tft.print("Y");
             words += "Y";
             }
            else
             {
             tft.print("y");
              words += "y";
             }  
          }
        else if ( 65<X && X<125 && 105<Y && Y<145)    
        {       
            if (Caps)
             {
             tft.print("U");
             words += "U";
             }
            else
             {
             tft.print("u");
              words += "u";
             }  
          }
        else if ( 130<X && X<190 && 105<Y && Y<145)    
        {       
            if (Caps)
             {
             tft.print("I");
             words += "I";
             }
            else
             {
             tft.print("i");
              words += "i";
             }  
           }
        else if ( 195<X && X<255 && 105<Y && Y<145)    
        {       
            if (Caps)
             {
             tft.print("O");
             words += "O";
             }
            else
             {
             tft.print("o");
              words += "o";
             }  
        }
        else if ( 260<X && X<320 && 105<Y && Y<145)    
        {       
            if (Caps)
             {
             tft.print("P");
             words += "P";
             }
            else
             {
             tft.print("p");
              words += "p";
             }  
        }
        //row1
        else if (0<X && X<60 && 66<Y && Y<100)    
        {       
            tft.print("6");
            words += "6";
        }
        else if (65<X && X<125 && 66<Y && Y<100)    
        {       
             tft.print("7");
             words += "7";
        }
        else if (130<X && X<190 && 66<Y && Y<100)    
        {       
             tft.print("8");
             words += "8";
        }
        else if (195<X && X<255 && 66<Y && Y<100)    
        {       
             tft.print("9");
             words += "9";
        }
        else if (250<X && X<320  && 66<Y && Y<100)   
        {       
             tft.print("0");
             words += "0";
        }
        else if ( 235<X && X<295 && 0<Y && Y<50)    
        {       
            Serial.println("Find");
            a=a-12;
            searchWord() ;
        }
        else
        {
        continue; 
        }
        a=a+12;
        if(a>225)
        {
         b=b+20;
         a=0;
         if(b >=60)
         b = 0;
        }
        delay(300);
   }
 }  
}

void IntroScreen()
{ 
  tft.fillScreen(BLACK);
  tft.setCursor (60, 50);
  tft.setTextSize (3);
  tft.setTextColor(GREEN);
  tft.println("Welcome");
  tft.setTextColor(GREEN);
  tft.setCursor (60, 100);
  tft.setTextSize (3);
  tft.println("Loading...");
  delay(4000);
}

void numberofWord()
{ 
  tft.fillScreen(BLACK);
   
  tft.setCursor (25, 50);
  tft.setTextSize (2);
  tft.setTextColor(GREEN);
  tft.print("Select Number of Words");
  
  tft.fillRoundRect(70,110,20,20,3,GREEN);
  tft.setCursor (72, 110);
  tft.setTextSize (3);
  tft.setTextColor(BLACK);
  tft.print("+");
  
  tft.fillRoundRect(230,110,20,20,3,GREEN);
  tft.setTextColor(BLACK);
  tft.setCursor (232, 110);
  tft.setTextSize (3);
  tft.println("-");
  
  tft.setTextColor(GREEN);
  tft.setCursor (148, 110);
  tft.setTextSize (3);
  tft.println(numberofWords);
  
  tft.fillRoundRect(125,138, 62, 30, 3,WHITE);
  tft.setTextColor(GREEN);
  tft.setCursor (140, 140);
  tft.setTextSize (3);
  tft.println("OK");

  while(1)
  {
   TSPoint p = waitTouch();
   if ( 55<X && X<80 && 103<Y && Y<145)//Space
        {     
             numberofWords++; 
             tft.fillRect(148,110,40,25,BLACK); 
             tft.setTextColor(GREEN);
             tft.setCursor (148, 110);
             tft.setTextSize (3);
             tft.print(numberofWords); 
             delay(400);
        }
   else if ( 225<X && X<270 && 100<Y && Y<145)
        {       
             numberofWords--; 
             tft.fillRect(148,110,40,25,BLACK); 
             tft.setTextColor(GREEN);
             tft.setCursor (148, 110);
             tft.setTextSize (3);
             tft.print(numberofWords);
             delay(400); 
        }
   else if ( 120<X && X<200 && 135<Y && Y<185)
        {       
             Serial.println("break"); 
             break;   
        }
    
  }
}

void modeSelection()
{ 
  tft.fillScreen(BLACK);
  
  tft.fillRoundRect(50,60,220,50,5,GREEN);
  tft.setCursor (74, 74);
  tft.setTextSize (3);
  tft.setTextColor(BLACK);
  tft.print("ENCRYPTION");

  tft.fillRoundRect(50,130,220,50,5,GREEN);
  tft.setCursor (74, 142);
  tft.setTextSize (3);
  tft.setTextColor(BLACK);
  tft.print("DECRYPTION");

  while(1)
  {
   TSPoint p = waitTouch();
   if(50<X && X<270 && 60<Y && Y<110)
        {       
             Serial.println("Encryption");
             modeState = true; 
             break;   
        }
   else if(50<X && X<270 && 130<Y && Y<180)
        {       
             Serial.println("Decryption");
             modeState = false;
             break;   
        }
  }
}

void keyPad()
{     
      tft.fillScreen(BLACK);
      tft.setTextSize (2);
      tft.setCursor(0,0);
      if(passPhraseMode)
      {
       tft.setTextColor(WHITE);
       tft.print("Passphrase " + String(z) + ":");
       tft.print(words);
      }
      else
      {
       tft.setTextColor(WHITE);
       tft.print("Word " + String(k) + ":");
       tft.print(words);
      }
      tft.setTextSize (3);
      tft.setTextColor(BLACK);
      for( j=0; j<4;j++)
      {
       for ( i=0; i<5; i++)
       {
        tft.fillRoundRect((i*65)+1, j*45+66, 60, 40, 3,GREEN);
        tft.setCursor (i*65+18,j*45+73 );
        if(keypadIndex)
        {
         tft.print(symbolset1[j][i]);
         if( i == 0 && j == 3)
          {
           tft.fillRoundRect((i*65)+1, j*45+66, 60, 40, 3,WHITE);
           tft.setCursor (i*65+10,j*45+73 );
           tft.setTextSize(2);
           tft.setTextColor(GREEN);
           tft.print("Caps");
           tft.setTextSize(3);
           tft.setTextColor(BLACK);
          }
        }
        else
        {
         tft.print(symbolset2[j][i]);
         if(i == 4 && j == 3)
         {
           tft.fillRoundRect((i*65)+1, j*45+66, 60, 40, 3,WHITE);
           tft.setCursor (i*65+13,j*45+80);
           tft.setTextSize(1);
           tft.setTextColor(GREEN);
           tft.print("RESET");
           tft.setTextSize(3);
           tft.setTextColor(BLACK);
         }
         if(i == 4 && j == 2)
         {
           tft.fillRoundRect((i*65)+1, j*45+66, 60, 40, 3,WHITE);
           tft.setCursor (i*65+13,j*45+80);
           tft.setTextSize(1);
           tft.setTextColor(GREEN);
           tft.print("ENTER");
           tft.setTextSize(3);
           tft.setTextColor(BLACK);
         }
         if( i == 0 && j == 3)
          {
           tft.fillRoundRect((i*65)+1, j*45+66, 60, 40, 3,WHITE);
           tft.setCursor (i*65+13,j*45+80 );
           tft.setTextSize(1);
           tft.setTextColor(GREEN);
           tft.print("SPACE");
           tft.setTextSize(3);
           tft.setTextColor(BLACK);
          }
        }
       }
      }
      if(keypadIndex)
      {
       tft.fillRoundRect(300,0, 30, 60, 3,WHITE);
       tft.setCursor(301,15);
       tft.setTextSize(3);
       tft.setTextColor(GREEN);
       tft.print(">");
      }
      else
      {
       tft.fillRoundRect(300,0, 30, 60, 3,WHITE);
       tft.setCursor(301,15);
       tft.setTextSize(3);
       tft.setTextColor(GREEN);
       tft.print("<");
      }
      tft.fillRoundRect(235,0, 60, 25, 4,WHITE);
      tft.setCursor(240,5);
      tft.setTextSize(2);
      tft.setTextColor(GREEN);
      tft.print("Find");
        
}

void writeReadConsole()
 {  
    tft.fillScreen(BLACK);
    tft.setTextSize (2);
    tft.setCursor(50,0);
    tft.print("PLACE THE SD CARD"); 
    tft.fillCircle(160,120, 80, GREEN); 
    tft.setTextSize (3);
    tft.setCursor(100,110);
    if(modeState)
    tft.print("WRITING");
    else
    tft.print("READING");       
 }

void showListofWords()
 {
  tft.fillScreen(BLACK);
  tft.setTextSize (2);
  tft.setCursor(0,0);
  tft.setTextColor(GREEN);
  tft.print("List of Words:"); 
  int k = 18;
  tft.setTextColor(WHITE);
  int f = 0;
  for(int y = 1; y <= 24 ; y++)
  {  
    if(y <= 12)
     tft.setCursor(0,k);
    else if(y == 13)
     {
      k = 18;
      tft.setCursor(160,k);
     }
     else
      tft.setCursor(160,k);
    
    tft.print(String(y)+".");
    while( data1[f] != '|')
    {
       if(data1[f] == '?')
       {
         goto end;   
       }
      tft.print(data1[f]);
      delay(50);
      f++ ;
    }
    f++ ;   
    k = k + 18; 
  }
  end:
   Serial.println("List ends");  
 }

void searchWord()
{ 
  String stringAll = wordString;

  String stringPhrase = words;
  stringPhrase.toLowerCase();
  Serial.println(stringPhrase);

  int indexedWord = stringAll.indexOf(stringPhrase);
  Serial.println(indexedWord);

  if(indexedWord >= 0)
  {
   Serial.println("Word Found");
   indexArray[k] = indexedWord;
   for(int l = 0 ; l < 3 ; l++)
   {
      tft.setCursor(105,45);
      tft.setTextSize(2);
      tft.setTextColor(GREEN);
      tft.print("Found..!!!"); 
      delay(500);
      tft.fillRoundRect(100,45, 135, 15, 4,BLACK);  
      delay(500);
   } 
  }
  else
  {
   Serial.println("Not Found"); 
   for(int l = 0 ; l < 3 ; l++)
   {
      tft.setCursor(105,45);
      tft.setTextSize(2);
      tft.setTextColor(GREEN); 
      tft.print("Not Found..");
      delay(500);
      tft.fillRoundRect(100,45, 135, 15, 1,BLACK);  
      delay(500);
   }
  }
}
