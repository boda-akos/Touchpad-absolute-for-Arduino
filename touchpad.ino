//Adafruit touchpad library modified for Synaptics touchpads, absolute values
//To determine two dividers in program, check the min/max X and Y values in the corners
//of your touchpad, uncomment and call "showCoord()" to show the values either on Serial or on TFT
//then divide X and Y range on your TFT dots range, for example 160x128 :
//Xmin = 1350 Ymin=1150 Xmax=5850 Ymax=4750 
//deltaX = 4500 divide on 160 LCD points = 28 counts /point 
//deltaY = 3600 divide on 128 LCD points = 28 counts /point
 
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

byte dpin=2,cpin=3;   //nano pins touchpad , TFT pins 11 SPI data, 13 SPI clock
  #define TFT_CS        10
  #define TFT_RST       8
  #define TFT_DC        9
  #define ST77XX_GREY 0xBDF7

 /*byte dpin=DB0,cpin=DB1;   //stm32 pins
  #define TFT_CS        PB12
  #define TFT_RST       PB14
  #define TFT_DC        PB13
  // TFT pins PA7 SPI data, PA5 SPI clock
*/ 

int x,y,z,sel=7;  //start with black pen
unsigned int col[]={ST77XX_RED,ST77XX_YELLOW ,ST77XX_GREEN,ST77XX_MAGENTA, 
                    ST77XX_CYAN,ST77XX_BLUE,ST77XX_WHITE,ST77XX_BLACK};

boolean finger;  //!< If a finger is currently pressing
boolean gesture,left,right,middle; //!< If a gesture occured
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  
  // disableDebugPorts();  //uncomment for STM32
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip
  tft.fillScreen(ST77XX_RED); // if program stops here, your touchpad is not responding -> swap wires
  Serial.begin(9600); Serial.println();Serial.println();Serial.println();
    if (!ps2_reset()) Serial.println(" reset fail");  else Serial.println(" reset OK");
E6Report();

 //absolute mode
e8x4(0x80); /* Send “Set Modes” sequence (see section 4.2). */
write_(0xF3); /* Set Sample Rate command. */
write_(0x14); /* Sample Rate argument of 20. */
write_(0xF4); /* Enable command. */
E8Report(0x01);  //see 0x80 response, absolute mode
  
tft.setRotation(1);
tft.fillScreen(ST77XX_GREY);
  tft.setTextColor(ST77XX_WHITE, ST77XX_GREY);  // Adding  background colour erases previous text automatically
  draw();
}

uint16_t lasttap_x = 0, lasttap_y = 0;

void loop() {
  static int cnt;
  delay(25);
  if (! readData_()) { return; }
  if (z>10) //z 10-40 sense
  { int xx,yy; if (y>1150) yy=(y-1150)/28;  if (x>1350) xx=(x-1350)/28; 
  //int xx,yy; if (y>520) yy=(y-520)/38;if (x>995) xx=(x-1000)/30; // values for touchpad X 526 to 5360, Y 996 to 5820  
   
       if(xx>10) tft.fillCircle(xx,127-yy,1, col[sel]);
        else { if (y<1400 && z>10) {tft.fillScreen(ST77XX_GREY); draw(); } //clear
                       else { sel=(127-yy)/15; draw();}  //color select
               }
 //showCoord();
  }

}

void draw(){
  for (byte i=0; i<8; i++)  tft.fillRect(0,i*15,10,15,col[i]);
            tft.fillCircle(4,sel*15+7,3,ST77XX_GREY);
            tft.setCursor(0,122);tft.print("X");//delay(50);
  }

boolean ps2_reset(void) {
  Serial.print("Reset time ms: ");
 unsigned long x=millis();
  idle_();
  if (!command(0xf5)) return false;
  if (!command(0xff)) return false;
  if (read_() != 0xAA)  return false;
  if (read_() != 0x00)  return false;
  if (!command(0xf4)) return false;
  Serial.println(millis()-x); 
  inhibit();
  return true;
}

void e8x4(byte arg)
{
for (byte i = 0; i < 4; i++) {write_(0xE8); write_((arg >> (6-2*i)) & 0x03);}
}


uint32_t E6Report(void) {
  write_(0xe8);
  //write_(0x0); // argument for setres
  write_(0xe6);
  write_(0xe6);
  write_(0xe6);
  
  if (!command(0xe9))  return false;

  int reply0, reply1, reply2;
  reply0 = read_();
  reply1 = read_();
  reply2 = read_();

  Serial.print("E6 Reply: 0x");
  Serial.print(reply0, HEX);
  Serial.print(" 0x");
  Serial.print(reply1, HEX);
  Serial.print(" 0x");
  Serial.println(reply2, HEX);

  uint32_t reply = reply0;
  reply <<= 8;
  reply |= reply1;
  reply <<= 8;
  reply |= reply2;

  return reply;  //32 bit
}

uint32_t E8Report(byte arg) {
 
  e8x4(arg);
  Serial.print("E8 Reply to ");Serial.println(arg,HEX);
  if (!command(0xe9)) {Serial.print("E8 fail"); return false;}

  int reply0, reply1, reply2;
  reply0 = read_();
  reply1 = read_();
  reply2 = read_();

  Serial.print("E8 Reply: 0x");
  Serial.print(reply0, HEX);
  Serial.print(" 0x");
  Serial.print(reply1, HEX);
  Serial.print(" 0x");
  Serial.println(reply2, HEX);

  uint32_t reply = reply0;
  reply <<= 8;
  reply |= reply1;
  reply <<= 8;
  reply |= reply2;
  return reply;
}

 void showCoord(void){ 
   
  Serial.print("X = "); Serial.print(x); Serial.print(" ");
  Serial.print("Y = "); Serial.print(y); Serial.print(" ");
  Serial.print("Z = "); Serial.println(z);

   tft.setCursor(0,10);tft.print("    ");tft.setCursor(0,10);tft.print(x);
   tft.setCursor(0,40);tft.print("    "); tft.setCursor(0,40);tft.print(y);
   tft.setCursor(0,70);tft.print("    "); tft.setCursor(0,70);tft.print(z);
  } 

void idle_(void) {
  pullhigh(cpin);
  pullhigh(dpin);
}

// pull high
void pullhigh(int p) {
  pinMode(p, INPUT_PULLUP);
  digitalWrite(p, HIGH);
}
// drive low
void drivelow(int p) {
  digitalWrite(p, LOW);
  pinMode(p, OUTPUT);
}

void inhibit(void) {
  pullhigh(dpin);
  drivelow(cpin);
}


boolean readData_(void) {
  //write_(0xff);
  byte packet[6];
  for (byte j = 0; j < 6; j++) {
  packet[j] = read_();
    //Serial.print("0x"); Serial.print(packet[j], HEX); Serial.print(" ");
  }
  //Serial.println();
/*
gesture = finger = left = right = middle = false;
  if (packet[0] & 0x01) left = true;
  if (packet[0] & 0x02) right = true;
  if (packet[0] & 0x04) gesture = true;
  if (packet[0] & 0x20) finger = true;
 */
  x = packet[4]; x |= ((packet[1] & 0x0f) << 8); x |= (packet[3] & 0x10) <<8;
  y = packet[5]; y |= ((packet[1] & 0xf0) << 4); y |= (packet[3] & 0x20) <<7;
  z = packet[2];
  return true;
}


int readID_(void) {
  if (!command(0xf2)) return -1;
  return read_();
}

boolean command(int cmd) {
  write_(cmd);
  return (read_() == 0xfa);
}

int read_(void) {
  uint16_t d = 0;

  idle_();
  delayMicroseconds(50);
  // wait for clock line to drop

  // start bit + 8 bits data + parity + stop = 11 bits
  for (int i = 0; i < 11; i++) {
    while (digitalRead(cpin));
    if (digitalRead(dpin)) d |= _BV(i);  //_BV macro : bit value 1<<i
    while (!digitalRead(cpin));
  }
  inhibit();
  // drop start bit
  d >>= 1;
  return d & 0xFF;
}

void write_(int x) {
  uint16_t tosend = x;
  int parity = 1;

  for (int i = 0; i < 8; i++) {
    parity ^= (tosend >> i);
  }
  if (parity & 0x1)
    tosend |= 0x100;

  idle_();
  delayMicroseconds(300);
  drivelow(cpin);
  delayMicroseconds(100);
  drivelow(dpin);
  delayMicroseconds(10);

  // we pull the clock line up to indicate we're ready
  pullhigh(cpin);
  // wait for the device to acknowledge by pulling it down
  while (digitalRead(cpin)) ;

  for (int i = 0; i < 9; i++) {
    if (tosend & 0x1) pullhigh(dpin);  else drivelow(dpin);

    // the clock lines are driven by the -DEVICE- so we wait
    while (!digitalRead(cpin)) ;
    while (digitalRead(cpin)) ;

    tosend >>= 1;
  }

  // send stop bit (high)
  pullhigh(dpin);
  delayMicroseconds(50);
  while (digitalRead(cpin));
  // wait for pad to switch modes
  while (!digitalRead(cpin) || !digitalRead(dpin));
  inhibit();
}
  /*
  if (finger) Serial.print("\tFinger");
  if (right) Serial.print("\tRight");
  if (middle) Serial.print("\tMiddle");
  if (left) Serial.print("\tLeft");
  
  if (gesture && !finger)  {
    if ((lasttap_x == x) && (lasttap_y == y)) {
      Serial.print("\tDouble tap!");
    } else {
      Serial.print("\tTap");
    }
    lasttap_x = x;
    lasttap_y = y;
  }
  delay(25);*//*
Adafruit_PS2_Trackpad::Adafruit_PS2_Trackpad(int c, int d) {
  constructor(c, d);
}

boolean Adafruit_PS2_Mouse::begin(void) {

  if (!reset())
    return false;

  idle();
  write(0xf0); // we will poll the mouse
  read_();
  delayMicroseconds(100);
  inhibit();

}*//*
boolean ps2_begin(void) {
Serial.print("trk begin,");
  if (!ps2_reset()) return false;
Serial.println(" reset done");
  // see kbd/scancodes-13.html sec 13.7
  uint32_t e6 = E6Report();
//Serial.print("32 bit"); Serial.println(e6,HEX);
  int s1, s2, s3;
  s1 = e6 >> 16;
  s2 = e6 >> 8;
  s3 = e6;

  if (s3 != 0x64) {Serial.print("s3 != 0x64"); return false;} // not a trackpad?
  //if (s2 == 0) {Serial.print("s2=0x00");return false;} // not a trackpad?

  Serial.print("Trackpad found with ");
  Serial.print(s2, DEC);
  Serial.println(" buttons");

  if (s1 & 0x80)  E8Report(1);   // supports the e7 command report
   
  if (!tapMode(false)) return false;
  Serial.println("Tap mode set");
  getStatus();
  if (!absoluteMode()) return false;
  Serial.println("Absolute mode set");

  if (!command(0xf0)) return false;
   delay(1);
  if (!command(0xea)) return false;
  inhibit();
  return true;
}
*//*
boolean tapMode(boolean enabl) {
  int cmd, arg;
  if (enabl) {cmd = 0xf3;arg = 0x0A;} else {cmd = 0xe8;arg = 0;}
  int info[3];
  if (!command(0xe9))
    return false;
  info[0] = read_();
  Serial.println(info[0], HEX);
  info[1] = read_();
  Serial.println(info[1], HEX);
  info[2] = read_();
  Serial.println(info[2], HEX);

  if (!command(0xf5))
    return false;
  if (!command(0xf5))
    return false;
  write_(cmd);
  write_(arg);
  // if (read() != 0xfa) return false;
  //Serial.println(read_(), HEX);
  read_();
  return true;
}

uint32_t getStatus(void) {
  int info[3];
  Serial.print("Status: ");

  if (!command(0xe8) || !command(0xe6) ||
      !command(0xe6) || !command(0xe6) ||
      !command(0xe9))
    return 0;

  info[0] = read_();
  Serial.print(" 0x");
  Serial.print(info[0], HEX);
  info[1] = read_();
  Serial.print(" 0x");
  Serial.print(info[1], HEX);
  info[2] = read_();
  Serial.print(" 0x");
  Serial.println(info[2], HEX);

  return 1;
} 
*/

/*
boolean absoluteMode(void) {
  // alps magic knock, 4 disables before an enable
  if (!command(0xf5) || !command(0xf5) ||
      !command(0xf5) || !command(0xf5) ||
      !command(0xf4))
    return false;

  if (!command(0xf0))
    return false;
  delayMicroseconds(100);
  return true;
}
*/
