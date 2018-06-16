#include "leitura.h"

//#define debug


//global local
String resposta = "";

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 100);
TSPoint p;
Adafruit_TFTLCD tft(A3, A2, A1, A0, A4);
int x_inicial = 250;
String string_lida = "";
int numero_pergunta = 1;


#define MINPRESSURE 1
#define MAXPRESSURE 1000

uint8_t leitura() {

  if (gb::subState == GET_USER_INFO_MAIN) return getUserInfoTouch();

  else if (gb::subState == GET_USER_INFO_START) return getInfoStartTouch();

  else if (gb::subState == GET_USER_INFO_PERSONALIZADO) return getUserInfoTouchPersonalizado();

}

void configTouch() {

  tft.reset();
  delay(500);
  tft.begin(0x9325);
  tft.fillScreen(BLACK);
  tft.setRotation(1);
  initTouch();

}
void initTouch() {

  tft.fillScreen(BLACK);
  tft.setRotation(1);

  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(2.2);
  tft.setCursor(15, 30);
  tft.println("Hipertrofia Iniciante");
  tft.setCursor(15, 85);
  tft.println("Hipertrofia Avancado");
  tft.setCursor(15, 140);
  tft.println("Resistencia");
  tft.setCursor(15, 195);
  tft.println("Personalizar");

  // Inicio - Texto e botoes

  //Linha
  tft.drawRoundRect(5, 15, 312, 50, 5, WHITE);
  //Linha 2
  tft.drawRoundRect(5, 70, 312, 50, 5, WHITE);
  //Linha 3
  tft.drawRoundRect(5, 125, 312, 50, 5, WHITE);
  //Linha 4
  tft.drawRoundRect(5, 180, 312, 50, 5, WHITE);



}

void initTouchPersonalizado() {

  tft.fillScreen(BLACK);
  tft.setRotation(1);

  // Inicio - Texto e botoes

  //Linha
  tft.drawRoundRect(5, 15, 312, 50, 5, WHITE);

  //1
  tft.drawRoundRect(21, 70, 62, 50, 5, WHITE);
  //4
  tft.drawRoundRect(21, 125, 62, 50, 5, WHITE);
  //7
  tft.drawRoundRect(21, 180, 62, 50, 5, WHITE);
  //2
  tft.drawRoundRect(93, 70, 62, 50, 5, WHITE);
  //5
  tft.drawRoundRect(93, 125, 62, 50, 5, WHITE);
  //8
  tft.drawRoundRect(93, 180, 62, 50, 5, WHITE);
  //3
  tft.drawRoundRect(165, 70, 62, 50, 5, WHITE);
  //6
  tft.drawRoundRect(165, 125, 62, 50, 5, WHITE);
  //9
  tft.drawRoundRect(165, 180, 62, 50, 5, WHITE);
  //Del
  tft.drawRoundRect(237, 70, 62, 50, 5, WHITE);
  //Enter
  tft.drawRoundRect(237, 125, 62, 50, 5, WHITE);
  //0
  tft.drawRoundRect(237, 180, 62, 50, 5, WHITE);


  //tft.drawRoundRect(255, 15, 62, 50, 5, WHITE);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.setCursor(44, 85);
  tft.println("1");
  tft.setCursor(44, 140);
  tft.println("4");
  tft.setCursor(44, 195);
  tft.println("7");
  tft.setCursor(116, 85);
  tft.println("2");
  tft.setCursor(116, 140);
  tft.println("5");
  tft.setCursor(116, 195);
  tft.println("8");
  tft.setCursor(188, 85);
  tft.println("3");
  tft.setCursor(188, 140);
  tft.println("6");
  tft.setCursor(188, 195);
  tft.println("9");
  tft.setCursor(260, 195);
  tft.println("0");
  tft.setTextSize(2.2);
  tft.setCursor(251, 85);
  tft.println("Del");
  tft.setCursor(241, 140);
  tft.println("Enter");
  tft.setTextSize(2.2);
  tft.setCursor(15, 30);
  tft.println("Quantas series?");
}

void initRepeticao() {
  tft.fillScreen(WHITE);
  tft.fillRoundRect(0, 0, 320, 240, 5, WHITE);
  tft.setCursor(45, 30);
  tft.setTextSize(4);
  tft.setTextColor(BLACK);
  tft.println("Repeticoes");
}

void initDescanso() {
  tft.fillScreen(WHITE);
  tft.fillRoundRect(0, 0, 320, 240, 5, WHITE);
  tft.setCursor(65, 30);
  tft.setTextSize(4);
  tft.setTextColor(BLUE);
  tft.println("Descanso");
}


void updateRepeticao(int numero) {

  //Serial.println(numero);

  //tft.fillRoundRect(100, 90, 100, 60, 5, WHITE);
  tft.fillRoundRect(90, 130, 120, 80, 5, WHITE);
  tft.setTextSize(8);
  tft.setTextColor(BLACK);

  if (numero == gb::repeticoes) {
    tft.setTextSize(5);
    tft.setCursor(70, 150);
    tft.println("Ultima!");
  }
  else {

    if (numero >= 10) {
      tft.setCursor(110, 150);

    }
    else {
      tft.setCursor(140, 150);
    }

    tft.println(numero);
  }

}

void initInicializaComeco() {
  tft.fillScreen(WHITE);
  tft.setCursor(80, 30);
  tft.setTextSize(4);
  tft.setTextColor(RED);

  tft.print("Serie ");
  tft.println(gb::serieAtual + 1);
  tft.setCursor(45, 80);
  tft.println("Prepare-se");
}

void updateDescanso(int numero) {

  if (numero <= 15) {

    if (numero >= 10) {
      tft.fillRoundRect(21, 125, 179, 105, 5, WHITE);
      tft.setCursor(110, 150);
    }
    else {
      //x -> 21 ate 300   //y ->  125 ate 230
      tft.fillRoundRect(21, 125, 179, 105, 5, WHITE);
      tft.setCursor(140, 150);
    }

    tft.setTextColor(RED);
    tft.setTextSize(8);
    tft.println(numero);


  }
  else {

    if (numero >= 10) {


      tft.fillRoundRect(21, 125, 260, 155, 5, WHITE);
      tft.setCursor(110, 150);
      tft.setTextColor(BLUE);
      tft.setTextSize(8);
      tft.println(numero);

    }
  }
}

//listener pagina inicial
uint8_t getUserInfoTouch() {

  p = ts.getPoint();
  pinMode(XM, OUTPUT);
  digitalWrite(XM, LOW);
  pinMode(YP, OUTPUT);
  digitalWrite(YP, HIGH);
  pinMode(YM, OUTPUT);
  digitalWrite(YM, LOW);
  pinMode(XP, OUTPUT);
  digitalWrite(XP, HIGH);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
  {
    p.x = tft.width() - (map(p.x, TS_MINX, TS_MAXX, tft.width(), 0));
    p.y = tft.height() - (map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));





#ifdef DRIVER_BUG
    p.y = abs(p.y - 220);
#else
    p.x = abs(p.x - 330);
#endif

#ifdef DEBUG
    Serial.print(p.x);
    Serial.print(" ");
    Serial.println(p.y);
#endif

    //Teste toque linha 1 hipertrofia iniciante
    if (p.y > 5 & p.y < 40) {

      tft.fillRoundRect(5, 15, 312, 50, 5, RED);
      tft.setTextSize(2.2);
      tft.setCursor(15, 30);
      tft.println("Hipertrofia Iniciante");
      delay(500);
      gb::modo = MODO_HIPERTROFIA_INICIANTE;
      init_info();
      gb::subState = GET_USER_INFO_START;

    } else if ( p.y > 50 & p.y < 105) {
      tft.fillRoundRect(5, 70, 312, 50, 5, RED);
      tft.setTextSize(2.2);
      tft.setCursor(15, 85);
      tft.println("Hipertrofia Avancado");
      delay(500);
      gb::modo = MODO_HIPERTROFIA_AVANCADO;
      init_info();
      gb::subState = GET_USER_INFO_START;
    } else if ( p.y > 115 & p.y < 170) {
      tft.fillRoundRect(5, 125, 312, 50, 5, RED);
      tft.setTextSize(2.2);
      tft.setCursor(15, 140);
      tft.println("Resistencia");
      delay(500);
      gb::modo = MODO_RESISTENCIA;
      init_info();
      gb::subState = GET_USER_INFO_START;
    } else if ( p.y > 180 & p.y < 235) {
      tft.fillRoundRect(5, 180, 312, 50, 5, RED);
      tft.setTextSize(2.2);
      tft.setCursor(15, 195);
      tft.println("Personalizar");
      delay(500);
      gb::modo = MODO_PERSONALIZADO;
      initTouchPersonalizado();
      gb::subState = GET_USER_INFO_PERSONALIZADO;
    }
  }

  return LEITURA;

}

//pagina de detalhes
void init_info() {
  //mode define qual treino foi escolhido

  tft.setTextColor(WHITE);
  tft.setTextSize(2.2);

  switch (gb::modo) {

    case MODO_HIPERTROFIA_INICIANTE:

      gb::qtdSeries = 1;
      gb::repeticoes = 10;
      gb::intervalo = 60;
      gb::TimeIda = 2;
      gb::TimeVolta = 3;

      tft.fillScreen(BLACK);
      tft.drawRoundRect(5, 10, 312, 40, 5, WHITE);
      tft.setCursor(15, 22);
      tft.println("Hipertrofia Iniciante");
      tft.drawRoundRect(5, 200, 50, 38, 5, WHITE);
      tft.setCursor(23, 212);
      tft.println("<");
      tft.drawRoundRect(268, 200, 50, 38, 5, WHITE);
      tft.setCursor(282, 212);
      tft.println("OK");

      tft.setCursor(15, 60);
      tft.print("Series: ");
      tft.println(gb::qtdSeries);
      tft.setCursor(15, 95);
      tft.print("Repeticoes: ");
      tft.println(gb::repeticoes);
      tft.setCursor(15, 130);
      tft.print("Descanso: ");
      tft.println(gb::intervalo);
      tft.setCursor(15, 165);
      tft.print("Cadencia: ");
      tft.print((int)gb::TimeIda);
      tft.print(":");
      tft.println((int)gb::TimeVolta);
      break;

    case MODO_HIPERTROFIA_AVANCADO:

      gb::qtdSeries = 1;
      gb::repeticoes = 10;
      gb::intervalo = 45;
      gb::TimeIda = 2;
      gb::TimeVolta = 4;

      tft.fillScreen(BLACK);
      tft.drawRoundRect(5, 10, 312, 40, 5, WHITE);
      tft.setCursor(15, 22);
      tft.println("Hipertrofia Avancado");
      tft.drawRoundRect(5, 200, 50, 38, 5, WHITE);
      tft.setCursor(23, 212);
      tft.println("<");
      tft.drawRoundRect(268, 200, 50, 38, 5, WHITE);
      tft.setCursor(282, 212);
      tft.println("OK");

      tft.setCursor(15, 60);
      tft.print("Series: ");
      tft.println(gb::qtdSeries);
      tft.setCursor(15, 95);
      tft.print("Repeticoes: ");
      tft.println(gb::repeticoes);
      tft.setCursor(15, 130);
      tft.print("Descanso: ");
      tft.println(gb::intervalo);
      tft.setCursor(15, 165);
      tft.print("Cadencia: ");
      tft.print((int)gb::TimeIda);
      tft.print(":");
      tft.println((int)gb::TimeVolta);
      break;

    case MODO_RESISTENCIA:

      gb::qtdSeries = 3;
      gb::repeticoes = 15;
      gb::intervalo = 60;
      gb::TimeIda = 2;
      gb::TimeVolta = 2;

      tft.fillScreen(BLACK);
      tft.drawRoundRect(5, 10, 312, 40, 5, WHITE);
      tft.setCursor(15, 22);
      tft.println("Resistencia");
      tft.drawRoundRect(5, 200, 50, 38, 5, WHITE);
      tft.setCursor(23, 212);
      tft.println("<");
      tft.drawRoundRect(268, 200, 50, 38, 5, WHITE);
      tft.setCursor(282, 212);
      tft.println("OK");

      tft.setCursor(15, 60);
      tft.print("Series: ");
      tft.println(gb::qtdSeries);
      tft.setCursor(15, 95);
      tft.print("Repeticoes: ");
      tft.println(gb::repeticoes);
      tft.setCursor(15, 130);
      tft.print("Descanso: ");
      tft.println(gb::intervalo);
      tft.setCursor(15, 165);
      tft.print("Cadencia: ");
      tft.print((int)gb::TimeIda);
      tft.print(":");
      tft.println((int)gb::TimeVolta);
      break;

    case MODO_PERSONALIZADO:

      tft.fillScreen(BLACK);
      tft.drawRoundRect(5, 10, 312, 40, 5, WHITE);
      tft.setCursor(15, 22);
      tft.println("Personalizado");
      tft.drawRoundRect(5, 200, 50, 38, 5, WHITE);
      tft.setCursor(23, 212);
      tft.println("<");
      tft.drawRoundRect(268, 200, 50, 38, 5, WHITE);
      tft.setCursor(282, 212);
      tft.println("OK");

      tft.setCursor(15, 60);
      tft.print("Series: ");
      tft.println(gb::qtdSeries);
      tft.setCursor(15, 95);
      tft.print("Repeticoes: ");
      tft.println(gb::repeticoes);
      tft.setCursor(15, 130);
      tft.print("Descanso: ");
      tft.println(gb::intervalo);
      tft.setCursor(15, 165);
      tft.print("Cadencia: ");
      tft.print((int)gb::TimeIda);
      tft.print(":");
      tft.println((int)gb::TimeVolta);

    default:
      break;
  }
}

//listener da pagina de detalhes
uint8_t getInfoStartTouch() {

  p = ts.getPoint();
  pinMode(XM, OUTPUT);
  digitalWrite(XM, LOW);
  pinMode(YP, OUTPUT);
  digitalWrite(YP, HIGH);
  pinMode(YM, OUTPUT);
  digitalWrite(YM, LOW);
  pinMode(XP, OUTPUT);
  digitalWrite(XP, HIGH);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
  {



    p.x = tft.width() - (map(p.x, TS_MINX, TS_MAXX, tft.width(), 0));
    p.y = tft.height() - (map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));



#ifdef DRIVER_BUG
    p.y = abs(p.y - 220);
#else
    p.x = abs(p.x - 330);
#endif

#ifdef DEBUG
    Serial.print(p.x);
    Serial.print(" ");
    Serial.println(p.y);
#endif



    if (p.y < 222 & p.y > 160) {
      //botao de ok
      if (p.x > 260 & p.x < 320) {
        tft.fillRoundRect(268, 200, 50, 38, 5, RED);
        tft.setCursor(282, 212);
        tft.println("OK");
        delay(500);
        tft.fillRoundRect(268, 200, 50, 38, 5, BLACK);
        tft.drawRoundRect(268, 200, 50, 38, 5, WHITE);
        tft.setCursor(282, 212);
        tft.println("OK");

        gb::firstTime = 1;
        return TREINO;

      } else if (p.x > 0 & p.x < 60) {  //botao de voltar
        tft.fillRoundRect(5, 200, 50, 38, 5, RED);
        tft.setCursor(23, 212);
        tft.println("<");
        delay(500);
        tft.fillRoundRect(5, 200, 50, 38, 5, BLACK);
        tft.drawRoundRect(5, 200, 50, 38, 5, WHITE);
        tft.setCursor(23, 212);
        tft.println("<");

        initTouch();
        gb::subState = GET_USER_INFO_MAIN;
      }
    }
  }
  return LEITURA;
}
uint8_t getUserInfoTouchPersonalizado() {

  p = ts.getPoint();
  pinMode(XM, OUTPUT);
  digitalWrite(XM, LOW);
  pinMode(YP, OUTPUT);
  digitalWrite(YP, HIGH);
  pinMode(YM, OUTPUT);
  digitalWrite(YM, LOW);
  pinMode(XP, OUTPUT);
  digitalWrite(XP, HIGH);



  if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
  {
    p.x = tft.width() - (map(p.x, TS_MINX, TS_MAXX, tft.width(), 0));
    p.y = tft.height() - (map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));

#ifdef DRIVER_BUG
    p.y = abs(p.y - 220);
#else
    p.x = abs(p.x - 330);
#endif

    //    Serial.print("py: ");
    //    Serial.print(p.y);
    //    Serial.print(" px: ");
    //    Serial.println(p.x);


    if (p.y > 70 & p.y < 120) {
      //Testa numero 1
      if (p.x > 21 & p.x < 83)
      {
        tft.fillRoundRect(21, 70, 62, 50, 5, RED);
        tft.setCursor(44, 85);
        tft.println("1");


        if (string_lida.length() < 3 ) {
          tft.setTextSize(2.2);
          tft.setCursor(x_inicial, 30);
          tft.println("1");
          x_inicial = x_inicial + espaco;
          string_lida += "1";
        }


        delay(500);
        tft.fillRoundRect(21, 70, 62, 50, 5, BLACK);
        delay(100);
        tft.setTextSize(3);
        tft.setCursor(44, 85);
        tft.println("1");
        //1
        tft.drawRoundRect(21, 70, 62, 50, 5, WHITE);
      }

      //Testa numero 2
      if (p.x > 93 & p.x < 155)
      {
        tft.fillRoundRect(93, 70, 62, 50, 5, RED);
        tft.setCursor(116, 85);
        tft.println("2");

        if (string_lida.length() < 3 ) {
          tft.setTextSize(2.2);
          tft.setCursor(x_inicial, 30);
          tft.println("2");
          x_inicial = x_inicial + espaco;
          string_lida += "2";
        }


        delay(500);
        tft.fillRoundRect(93, 70, 62, 50, 5, BLACK);
        delay(100);
        tft.setTextSize(3);
        tft.setCursor(116, 85);
        tft.println("2");
        //2
        tft.drawRoundRect(93, 70, 62, 50, 5, WHITE);

      }

      //Testa numero 3
      if (p.x > 165 & p.x < 227)
      {
        tft.fillRoundRect(165, 70, 62, 50, 5, RED);
        tft.setCursor(188, 85);
        tft.println("3");

        if (string_lida.length() < 3 ) {
          tft.setTextSize(2.2);
          tft.setCursor(x_inicial, 30);
          tft.println("3");
          x_inicial = x_inicial + espaco;
          string_lida += "3";
        }


        delay(500);
        tft.fillRoundRect(165, 70, 62, 50, 5, BLACK);
        delay(100);
        tft.setTextSize(3);
        tft.setCursor(188, 85);
        tft.println("3");
        //3
        tft.drawRoundRect(165, 70, 62, 50, 5, WHITE);

      }

      //Testa Delete
      if (p.x > 237 & p.x < 299)
      {
        tft.fillRoundRect(237, 70, 62, 50, 5, RED);
        tft.setCursor(251, 85);
        tft.setTextSize(2.2);
        tft.println("Del");


        if (string_lida.length() > 0 ) {
          x_inicial = x_inicial - espaco;
          tft.setTextSize(2.2);
          tft.setCursor(x_inicial, 30);
          tft.fillRoundRect(x_inicial, 20, 10, 30, 5, BLACK);
          string_lida.remove(string_lida.length() - 1);
        }


        delay(500);
        tft.fillRoundRect(237, 70, 62, 50, 5, BLACK);
        delay(100);
        tft.setTextSize(2.2);
        tft.setCursor(251, 85);
        tft.println("Del");
        //3
        tft.drawRoundRect(237, 70, 62, 50, 5, WHITE);

      }
    }

    if (p.y > 125 & p.y < 175) {

      //Testa numero 4
      if (p.x > 21 & p.x < 83)
      {
        tft.fillRoundRect(21, 125, 62, 50, 5, RED);
        tft.setCursor(44, 140);
        tft.println("4");

        if (string_lida.length() < 3 ) {
          tft.setTextSize(2.2);
          tft.setCursor(x_inicial, 30);
          tft.println("4");
          x_inicial = x_inicial + espaco;
          string_lida += "4";
        }


        delay(500);
        tft.fillRoundRect(21, 125, 62, 50, 5, BLACK);
        delay(100);
        tft.setTextSize(3);
        tft.setCursor(44, 140);
        tft.println("4");
        //1
        tft.drawRoundRect(21, 125, 62, 50, 5, WHITE);
      }

      //Testa numero 5
      if (p.x > 93 & p.x < 155)
      {
        tft.fillRoundRect(93, 125, 62, 50, 5, RED);
        tft.setCursor(116, 140);
        tft.println("5");

        if (string_lida.length() < 3 ) {
          tft.setTextSize(2.2);
          tft.setCursor(x_inicial, 30);
          tft.println("5");
          x_inicial = x_inicial + espaco;
          string_lida += "5";
        }


        delay(500);
        tft.fillRoundRect(93, 125, 62, 50, 5, BLACK);
        delay(100);
        tft.setTextSize(3);
        tft.setCursor(116, 140);
        tft.println("5");
        //2
        tft.drawRoundRect(93, 125, 62, 50, 5, WHITE);

      }

      //Testa numero 6
      if (p.x > 165 & p.x < 227)
      {
        tft.fillRoundRect(165, 125, 62, 50, 5, RED);
        tft.setCursor(188, 140);
        tft.println("6");

        if (string_lida.length() < 3 ) {
          tft.setTextSize(2.2);
          tft.setCursor(x_inicial, 30);
          tft.println("6");
          x_inicial = x_inicial + espaco;
          string_lida += "6";
        }


        delay(500);
        tft.fillRoundRect(165, 125, 62, 50, 5, BLACK);
        delay(100);
        tft.setTextSize(3);
        tft.setCursor(188, 140);
        tft.println("6");
        //3
        tft.drawRoundRect(165, 125, 62, 50, 5, WHITE);

      }

      //Testa Enter
      if (p.x > 237 & p.x < 299)
      {
        tft.fillRoundRect(237, 125, 62, 50, 5, RED);
        tft.setCursor(241, 140);
        tft.setTextSize(2.2);
        tft.println("Enter");
        delay(500);
        tft.fillRoundRect(237, 125, 62, 50, 5, BLACK);
        delay(100);
        tft.setTextSize(2.2);
        tft.setCursor(241, 140);
        tft.println("Enter");
        //3
        tft.drawRoundRect(237, 125, 62, 50, 5, WHITE);

        if (string_lida.length() > 0 && numero_pergunta == 1 && string_lida.toInt() > 0) {

          tft.fillRoundRect(5, 15, 312, 50, 5, BLACK);
          tft.drawRoundRect(5, 15, 312, 50, 5, WHITE);
          tft.setCursor(15, 30);
          tft.println("Quantas repeticoes?");
          numero_pergunta = 2;
          gb::qtdSeries = string_lida.toInt();
          string_lida = "";
          x_inicial = 250;

        }
        else if (string_lida.length() > 0 && numero_pergunta == 2 && string_lida.toInt() > 0) {

          tft.fillRoundRect(5, 15, 312, 50, 5, BLACK);
          tft.drawRoundRect(5, 15, 312, 50, 5, WHITE);
          tft.setCursor(15, 30);
          tft.println("Qual intervalo?");
          tft.setCursor(265, 30);
          tft.println("segs");
          numero_pergunta = 3;
          gb::repeticoes = string_lida.toInt();
          string_lida = "";
          x_inicial = 220;
        }
        else if (string_lida.length() > 0 && numero_pergunta == 3) {

          tft.fillRoundRect(5, 15, 312, 50, 5, BLACK);
          tft.drawRoundRect(5, 15, 312, 50, 5, WHITE);
          tft.setCursor(15, 30);
          tft.println("Tempo de descida?");
          tft.setCursor(265, 30);
          tft.println("segs");
          numero_pergunta = 4;
          gb::intervalo = string_lida.toInt();
          string_lida = "";
          x_inicial = 230;
        }
        else if (string_lida.length() > 0 && numero_pergunta == 4 && string_lida.toInt() > 0) {

          tft.fillRoundRect(5, 15, 312, 50, 5, BLACK);
          tft.drawRoundRect(5, 15, 312, 50, 5, WHITE);
          tft.setCursor(15, 30);
          tft.println("Tempo de subida?");
          tft.setCursor(265, 30);
          tft.println("segs");
          numero_pergunta = 5;
          gb::TimeIda = string_lida.toInt();
          x_inicial = 230;
          string_lida = "";
        }
        else if (string_lida.length() > 0 && numero_pergunta > 4 && string_lida.toInt() > 0) {

          tft.fillRoundRect(0, 0, 320, 240, 5, BLACK);
          gb::TimeVolta = string_lida.toInt();
          string_lida = "";
          x_inicial = 250;
          numero_pergunta = 1;
          gb::firstTime = 1;
          return TREINO;
        }

      }
    }

    if (p.y > 180 & p.y < 230) {

      //Testa numero 7
      if (p.x > 21 & p.x < 83)
      {
        tft.fillRoundRect(21, 180, 62, 50, 5, RED);
        tft.setCursor(44, 195);
        tft.println("7");

        if (string_lida.length() < 3 ) {
          tft.setTextSize(2.2);
          tft.setCursor(x_inicial, 30);
          tft.println("7");
          x_inicial = x_inicial + espaco;
          string_lida += "7";
        }


        delay(500);
        tft.fillRoundRect(21, 180, 62, 50, 5, BLACK);
        delay(100);
        tft.setTextSize(3);
        tft.setCursor(44, 195);
        tft.println("7");
        //1
        tft.drawRoundRect(21, 180, 62, 50, 5, WHITE);
      }

      //Testa numero 8
      if (p.x > 93 & p.x < 155)
      {
        tft.fillRoundRect(93, 180, 62, 50, 5, RED);
        tft.setCursor(116, 195);
        tft.println("8");

        if (string_lida.length() < 3 ) {
          tft.setTextSize(2.2);
          tft.setCursor(x_inicial, 30);
          tft.println("8");
          x_inicial = x_inicial + espaco;
          string_lida += "8";
        }


        delay(500);
        tft.fillRoundRect(93, 180, 62, 50, 5, BLACK);
        delay(100);
        tft.setTextSize(3);
        tft.setCursor(116, 195);
        tft.println("8");
        //2
        tft.drawRoundRect(93, 180, 62, 50, 5, WHITE);

      }

      //Testa numero 9
      if (p.x > 165 & p.x < 227)
      {
        tft.fillRoundRect(165, 180, 62, 50, 5, RED);
        tft.setCursor(188, 195);
        tft.println("9");

        if (string_lida.length() < 3 ) {
          tft.setTextSize(2.2);
          tft.setCursor(x_inicial, 30);
          tft.println("9");
          x_inicial = x_inicial + espaco;
          string_lida += "9";
        }

        delay(500);
        tft.fillRoundRect(165, 180, 62, 50, 5, BLACK);
        delay(100);
        tft.setTextSize(3);
        tft.setCursor(188, 195);
        tft.println("9");
        //3
        tft.drawRoundRect(165, 180, 62, 50, 5, WHITE);

      }

      //Testa numero 0
      if (p.x > 237 & p.x < 299)
      {
        tft.fillRoundRect(237, 180, 62, 50, 5, RED);
        tft.setCursor(260, 195);
        tft.setTextSize(3);
        tft.println("0");

        if (string_lida.length() < 3 ) {
          tft.setTextSize(2.2);
          tft.setCursor(x_inicial, 30);
          tft.println("0");
          x_inicial = x_inicial + espaco;
          string_lida += "0";
        }


        delay(500);
        tft.fillRoundRect(237, 180, 62, 50, 5, BLACK);
        delay(100);
        tft.setTextSize(3);
        tft.setCursor(260, 195);
        tft.println("0");
        //3
        tft.drawRoundRect(237, 180, 62, 50, 5, WHITE);

      }
    }
  }

  return LEITURA;

}

