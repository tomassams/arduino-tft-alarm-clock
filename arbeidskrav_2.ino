#if 1

#include <Adafruit_GFX.h>
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>

#define MINPRESSURE 200
#define MAXPRESSURE 1000

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

//240x320 ID=0x9341
const int XP = 8,
          XM = A2,
          YP = A3,
          YM = 9;

const int TS_LEFT = 122,
          TS_RT   = 898,
          TS_TOP  = 78,
          TS_BOT  = 880;

MCUFRIEND_kbv tft;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
Adafruit_GFX_Button on_btn;
Adafruit_GFX_Button off_btn;

// "ring" the alarm at this time
String alarmValue = "19:41";

// we "fake" the current time, start at these values
int hh = 19;
int mm = 40;
int ss = 45;

// button text
char* onBtnText = "ACTIVATE";
char* offBtnText = "DISABLE";

// Touch_getXY() updates these global vars
int pixel_x, pixel_y;

// current alarm status
bool alarmActive = false;

void setup(void)
{
  uint16_t ID = tft.readID();
  if (ID == 0xD3D3) ID = 0x9486; // write-only shield

  tft.begin(ID);
  tft.setRotation(0); //PORTRAIT
  tft.fillScreen(BLACK);

  on_btn.initButton(&tft,  60, 200, 100, 40, WHITE, GREEN, WHITE, onBtnText, 2);
  off_btn.initButton(&tft, 180, 200, 100, 40, WHITE, RED, WHITE, offBtnText, 2);

  on_btn.drawButton(false);
  off_btn.drawButton(false);

  updateTimeDisplay();
  updateStatusDisplay();
}

void loop(void)
{
  // look for button clicks every "second"
  for (int i = 0; i < 10; i++) {
    handleButtonClick();
    delay(100);
  }

  // update onscreen text/time/display
  updateTimeDisplay();
  updateStatusDisplay();
}

void handleButtonClick() {
  bool down = Touch_getXY();

  on_btn.press(down && on_btn.contains(pixel_x, pixel_y));
  off_btn.press(down && off_btn.contains(pixel_x, pixel_y));

  if (on_btn.justReleased())
    on_btn.drawButton();

  if (off_btn.justReleased())
    off_btn.drawButton();

  if (on_btn.justPressed()) {
    on_btn.drawButton(true);
    alarmActive = true;
  }

  if (off_btn.justPressed()) {
    off_btn.drawButton(true);
    alarmActive = false;
  }
}

bool Touch_getXY(void)
{
  TSPoint p = ts.getPoint();
  pinMode(YP, OUTPUT);      //restore shared pins
  pinMode(XM, OUTPUT);
  digitalWrite(YP, HIGH);   //because TFT control pins
  digitalWrite(XM, HIGH);
  bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
  if (pressed) {
    pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width());
    pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
  }
  return pressed;
}

void updateStatusDisplay() {

  tft.fillRect(0, 230, 240, 60, BLACK);
  tft.setCursor(70, 240);
  tft.setTextSize(1);

  if (alarmActive) {
    tft.setTextColor(GREEN);
    tft.print("ALARM ACTIVATED");
  }
  else {
    tft.setTextColor(RED);
    tft.print("ALARM DISABLED");
  }

  // reset to our default
  tft.setTextColor(GREEN);

}

void updateTimeDisplay() {

  // since we are faking time we also need to handle time increase
  ss++;
  if (ss == 60) {
    mm++;
    ss = 0;
  }
  if (mm == 60) {
    hh++;
    mm = 0;
  }
  if (hh == 24) {
    hh = 0;
  }

  // format the time in a neat way
  String timeString = formatTime(hh) + ":" + formatTime(mm) + ":" + formatTime(ss);

  tft.fillRect(0, 0, 240, 160, BLACK);
  tft.setTextSize(4);

  if (timeString.startsWith(alarmValue) && alarmActive) {
    tft.fillRect(0, 0, 240, 140, WHITE);
    tft.setTextColor(RED);
    tft.setCursor(25, 30);
    tft.print("ALARM!!");
  } else {
    tft.setTextColor(GREEN);
  }

  tft.setCursor(25, 90);
  tft.print(timeString);

}

// helper
String formatTime(int digit) {
  if (digit < 10) {
    return String(0, DEC) + String(digit);
  } else {
    return String(digit);
  }
}

#endif
