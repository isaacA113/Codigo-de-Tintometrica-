#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <ESP32Servo.h>


const int PIN_BOMBA_NEGRO     = 32;
const int PIN_BOMBA_AMARILLO  = 33;
const int PIN_BOMBA_AZUL      = 25;
const int PIN_BOMBA_ROJO      = 26;

Servo servoNegro;
Servo servoAmarillo;
Servo servoAzul;
Servo servoRojo;

LiquidCrystal_I2C lcd(0x27, 20, 4);


const byte FILAS = 4;
const byte COLUMNAS = 4;

char keys[FILAS][COLUMNAS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte pinFilas[FILAS] = {19, 18, 5, 17};
byte pinColumnas[COLUMNAS] = {16, 4, 2, 15};

Keypad teclado = Keypad(
  makeKeymap(keys),
  pinFilas,
  pinColumnas,
  FILAS,
  COLUMNAS
);



const float Q_NEGRO    = 1.0;
const float Q_AMARILLO = 1.0;
const float Q_AZUL     = 1.0;
const float Q_ROJO     = 1.0;


struct RecetaColor {

  String codigo;
  String nombre;

  float volumenNegro;
  float volumenAmarillo;
  float volumenAzul;
  float volumenRojo;
};


const int NUM_RECETAS = 8;

RecetaColor baseDatosColores[NUM_RECETAS] = {

  {"00001", "Rojo",
   0, 0, 0, 180},

  {"00002", "Amarillo",
   0, 180, 0, 0},

  {"00003", "Gris oscuro",
   90, 0, 0, 0},

  {"00004", "Azul claro",
   0, 0, 45, 0},

  {"00005", "Naranja medio",
   0, 45, 0, 45},

  {"00006", "Morado claro",
   0, 0, 11.5, 11.5},

  {"00007", "Verde oliva",
   22.5, 45, 0, 45},

  {"00008", "Marrón",
   45, 22.5, 0, 22.5}
};


String codigoIngresado = "";



void setup() {

  Serial.begin(115200);


  servoNegro.attach(PIN_BOMBA_NEGRO);
  servoAmarillo.attach(PIN_BOMBA_AMARILLO);
  servoAzul.attach(PIN_BOMBA_AZUL);
  servoRojo.attach(PIN_BOMBA_ROJO);


  apagarTodasLasBombas();

  // Inicializar LCD
  lcd.init();
  lcd.backlight();

  mostrarMenuPrincipal();
}


void loop() {

  char tecla = teclado.getKey();

  if (tecla) {



    if (tecla >= '0' && tecla <= '9') {

      codigoIngresado += tecla;

      lcd.setCursor(0, 1);
      lcd.print("Cod: ");
      lcd.print(codigoIngresado);
    }

  

    else if (tecla == '*') {

      codigoIngresado = "";

      mostrarMenuPrincipal();
    }



    else if (tecla == '#') {

      buscarYPrepararColor(codigoIngresado);

      codigoIngresado = "";
    }
  }
}

void mostrarMenuPrincipal() {

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Ingrese codigo:");

  lcd.setCursor(0, 1);
  lcd.print("Cod: ");
}



void buscarYPrepararColor(String codigo) {

  bool encontrado = false;

  for (int i = 0; i < NUM_RECETAS; i++) {

    if (baseDatosColores[i].codigo == codigo) {

      encontrado = true;

      prepararColor(baseDatosColores[i]);

      break;
    }
  }


  if (!encontrado) {

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Codigo no valido");

    lcd.setCursor(0, 1);
    lcd.print("Color no existe");

    delay(2000);

    mostrarMenuPrincipal();
  }
}



void prepararColor(RecetaColor receta) {

 

  float tiempoNegro =
      receta.volumenNegro / Q_NEGRO;

  float tiempoAmarillo =
      receta.volumenAmarillo / Q_AMARILLO;

  float tiempoAzul =
      receta.volumenAzul / Q_AZUL;

  float tiempoRojo =
      receta.volumenRojo / Q_ROJO;


  unsigned long tiempoNegro_ms =
      tiempoNegro * 1000;

  unsigned long tiempoAmarillo_ms =
      tiempoAmarillo * 1000;

  unsigned long tiempoAzul_ms =
      tiempoAzul * 1000;

  unsigned long tiempoRojo_ms =
      tiempoRojo * 1000;


  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Preparando:");

  lcd.setCursor(0, 1);
  lcd.print(receta.nombre);

  if (receta.volumenNegro > 0)
    servoNegro.write(90);

  if (receta.volumenAmarillo > 0)
    servoAmarillo.write(90);

  if (receta.volumenAzul > 0)
    servoAzul.write(90);

  if (receta.volumenRojo > 0)
    servoRojo.write(90);


  unsigned long tiempoInicio = millis();


  bool finNegro =
      (receta.volumenNegro == 0);

  bool finAmarillo =
      (receta.volumenAmarillo == 0);

  bool finAzul =
      (receta.volumenAzul == 0);

  bool finRojo =
      (receta.volumenRojo == 0);

  while (!(finNegro &&
           finAmarillo &&
           finAzul &&
           finRojo)) {

    unsigned long tiempoTranscurrido =
        millis() - tiempoInicio;


    if (!finNegro &&
        tiempoTranscurrido >= tiempoNegro_ms) {

      servoNegro.write(0);

      finNegro = true;
    }



    if (!finAmarillo &&
        tiempoTranscurrido >= tiempoAmarillo_ms) {

      servoAmarillo.write(0);

      finAmarillo = true;
    }



    if (!finAzul &&
        tiempoTranscurrido >= tiempoAzul_ms) {

      servoAzul.write(0);

      finAzul = true;
    }



    if (!finRojo &&
        tiempoTranscurrido >= tiempoRojo_ms) {

      servoRojo.write(0);

      finRojo = true;
    }
  }



  apagarTodasLasBombas();

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Proceso");

  lcd.setCursor(0, 1);
  lcd.print("Finalizado!");

  delay(3000);

  mostrarMenuPrincipal();
}



void apagarTodasLasBombas() {

  servoNegro.write(0);

  servoAmarillo.write(0);

  servoAzul.write(0);

  servoRojo.write(0);
}
