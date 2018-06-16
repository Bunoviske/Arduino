#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library

//Definicao de cores
#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF

//CS, CD, WR, RD, RESET
Adafruit_TFTLCD tft(A3, A2, A1, A0, A4);

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Teste de LCD 8 Bits");
  tft.reset();
  delay(500);
  //tft.begin(0x9341) //Use esta linha para o controlador 9341
  tft.begin(0x9328);
}

void loop()
{
  //tft.setRotation(3); //Use esta linha para o controlador 9341
  tft.setRotation(1);
  teste_texto(RED);
  delay(2000);
  
  tft.sleep();
  while (true);
  //tft.wake();

//  tft.setRotation(0);
//  teste_linhas(CYAN);
//  delay(500);
//  teste_retangulo_cheio(YELLOW, MAGENTA);
//  delay(500);
//  tft.fillScreen(BLACK);
//  teste_circulos(10, MAGENTA);
//  delay(500);
//  teste_triangulos();
//  delay(500);
//  teste_retangulos();
//  delay(500);

}

void teste_texto(uint16_t color)
{
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.setCursor(0, 20);
  tft.println("FILIPEFLOP");
  tft.setTextColor(GREEN);
  tft.setCursor(0, 50);
  tft.println("Arduino");
  tft.setTextColor(BLUE);
  tft.setCursor(0, 80);
  tft.println("Raspberry");
  tft.setTextColor(YELLOW);
  tft.setCursor(0, 110);
  tft.println("Embarcados");
  tft.setTextColor(CYAN);
  tft.setTextSize(5);
  tft.setCursor(0, 150);
  tft.println("1234567890");
  delay(2000);
}

void teste_linhas(uint16_t color)
{
  tft.fillScreen(BLACK);
  for (uint16_t x = 0; x < tft.width(); x += 6)
  {
    tft.drawLine(0, 0, x, tft.height() - 1, color);
  }
  for (uint16_t y = 0; y < tft.height(); y += 6)
  {
    tft.drawLine(0, 0, tft.width() - 1, y, color);
  }
}

void teste_retangulo_cheio(uint16_t color1, uint16_t color2)
{
  tft.fillScreen(BLACK);
  for (uint16_t x = tft.width() - 1; x > 6; x -= 6)
  {
    tft.fillRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2 , x, x, color1);
    tft.drawRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2 , x, x, color2);
  }
}

void teste_circulos(uint8_t radius, uint16_t color)
{
  for (uint16_t x = radius; x < tft.width(); x += radius * 2)
  {
    for (uint16_t y = radius; y < tft.height(); y += radius * 2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
  for (uint16_t x = 0; x < tft.width() + radius; x += radius * 2)
  {
    for (uint16_t y = 0; y < tft.height() + radius; y += radius * 2)
    {
      tft.drawCircle(x, y, radius, WHITE);
    }
  }
}

void teste_triangulos()
{
  tft.fillScreen(BLACK);
  for (uint16_t i = 0; i < tft.width() / 2; i += 5)
  {
    tft.drawTriangle(tft.width() / 2, tft.height() / 2 - i,
                     tft.width() / 2 - i, tft.height() / 2 + i,
                     tft.width() / 2 + i, tft.height() / 2 + i, tft.Color565(0, 0, i));
  }
}

void teste_retangulos()
{
  tft.fillScreen(BLACK);
  for (uint16_t x = tft.width(); x > 20 ; x -= 6)
  {
    tft.fillRoundRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2 , x, x, x / 8,  tft.Color565(0, x, 0));
  }
}
