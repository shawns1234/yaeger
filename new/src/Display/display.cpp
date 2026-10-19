#include "display.h"
#include "roaster_config.h"
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void initDisplay()
{
  lcd.init(DISPLAY_DA, DISPLAY_CL);
  lcd.backlight();
}

void setWifiIP()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Yaeger online");
  lcd.setCursor(0, 1);
  lcd.print("IP:");
  lcd.setCursor(5, 1);
  lcd.print(WiFi.localIP());
}

void setDisplay(const char* title, float bt, float et, float setpoint, const char* mode)
{
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(title);

  lcd.setCursor(0, 1);
  lcd.print("BT:");
  lcd.setCursor(5, 1);
  lcd.print(bt, 1);
  lcd.print("C  ET:");
  lcd.setCursor(11, 1);
  lcd.print(et, 1);
  lcd.print("C");

  lcd.setCursor(0, 2);
  lcd.print("Set:");
  lcd.setCursor(5, 2);
  lcd.print(setpoint, 1);
  lcd.print("C  Mode:");
  lcd.setCursor(11, 2);
  lcd.print(mode);
}
