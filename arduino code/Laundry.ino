#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#define DT 4
#define SCK 5
#define sw 2

long sample = 0;
float val = 0;
long count = 0;
float harga = 0;
static unsigned long timeoutStartTime = 0;
LiquidCrystal_I2C lcd(0x27, 16, 2);
bool state = false;

void setup()
{
  EEPROM.begin();
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(SCK, OUTPUT);
  pinMode(sw, INPUT_PULLUP);
  lcd.begin();
  lcd.print("Laundry System");
  Serial1.println("Laundry System");
}

void loop()
{
  val = EEPROM.read(0);
  count = readCount();
  int w = (((count - sample) / val) - 2 * ((count - sample) / val));
  if (w <= 0) {
    w = 0;
  }
  harga = w * 0.02;
  Serial.print("weight:");
  Serial.print((int)w);
  Serial.println("g");
  lcd.setCursor(0, 0);

  if (Serial1.available() ) {
    char data = Serial1.read();
    if (data == 'b') {
      Serial1.println("Berat=" + String(w) + "gram");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Laundry System");
      lcd.setCursor(0, 1);
      lcd.print("Berat=" + String(w) + "gram");
      timeoutStartTime = millis();
      state = true;
    }
    if (data == 'h') {
      Serial1.println("RM " + String(harga));
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Laundry System");
      lcd.setCursor(0, 1);
      lcd.print("Harga=RM" + String(harga));
      timeoutStartTime = millis();
      state = true;
    }
    if (data == 'c') {
      val = 0;
      sample = 0;
      w = 0;
      count = 0;
      calibrate();
    }
    data = "";
  }
  if ((millis() - timeoutStartTime >= 10000) && (state == true))
    {
      timeoutStartTime = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Laundry System");
      state = false;
    }
}

void calibrate()
{
  lcd.clear();
  lcd.print("Calibrating...");
  Serial1.println("Calibrating the system, Please wait");
  lcd.setCursor(0, 1);
  lcd.print("Please Wait...");
  for (int i = 0; i < 100; i++)
  {
    count = readCount();
    sample += count;
    Serial.println(count);
  }
  sample /= 100;
  lcd.clear();
  lcd.print("Put 100g & wait");
  Serial1.println("Put 100g & wait for system to calibrate");
  count = 0;
  while (count < 1000)
  {
    count = readCount();
    count = sample - count;
  }
  lcd.clear();
  lcd.print("Please Wait....");
  Serial1.println("Please Wait....");
  delay(2000);
  for (int i = 0; i < 100; i++)
  {
    count = readCount();
    val += sample - count;
  }
  val = val / 100.0;
  val = val / 100.0;    // put here your calibrating weight
  lcd.clear();
  EEPROM.put(0, val);
  Serial1.println("Done Calibrate");
}


unsigned long readCount(void)
{
  unsigned long Count;
  unsigned char i;
  pinMode(DT, OUTPUT);
  digitalWrite(DT, HIGH);
  digitalWrite(SCK, LOW);
  Count = 0;
  pinMode(DT, INPUT);
  while (digitalRead(DT));
  for (i = 0; i < 24; i++)
  {
    digitalWrite(SCK, HIGH);
    Count = Count << 1;
    digitalWrite(SCK, LOW);
    if (digitalRead(DT))
      Count++;
  }
  digitalWrite(SCK, HIGH);
  Count = Count ^ 0x800000;
  digitalWrite(SCK, LOW);
  return (Count);
}
