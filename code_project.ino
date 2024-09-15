// Importando bibliotecas
#include <LiquidCrystal_I2C.h>  // Biblioteca para LCD com interface I2C
#include <RTClib.h>            // Biblioteca para o Relógio em Tempo Real
#include <Wire.h>              // Biblioteca para comunicação I2C
#include <EEPROM.h>            // Biblioteca para armazenamento na EEPROM
#include <EduIntro.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço I2C para o LCD (0x3F ou 0x27), com 16 colunas e 2 linhas
RTC_DS3231 RTC;                   // Objeto para o relógio em tempo real
char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado"}; //dias da semana para o rtc

int ledVerde = 11;          // Pino do LED verde
int ledAmarelo = 12;        // Pino do LED amarelo
int ledVermelho = 13;       // Pino do LED vermelho
int buzzer = 7;             // Pino do buzzer
const int LDR_PIN = A0;           // Pino do sensor LDR (resistor dependente de luz)

DHT11 dht11(D2);
int temperatura;
int umidade;

int leituras = 0;

// Triggers para temperatura, umidade e luminosidade
int trigger_t_min = 15.0;       // Valor mínimo de temperatura para acionar o LED verde e buzzer
int trigger_t_max = 25.0;       // Valor máximo de temperatura para acionar o LED vermelho e buzzer
int trigger_u_min = 30.0;       // Valor mínimo de umidade para acionar o LED verde e buzzer
int trigger_u_max = 50.0;       // Valor máximo de umidade para acionar o LED vermelho e buzzer
float trigger_l_max = 30.0;       // Valor máximo de luminosidade para acionar o LED vermelho e buzzer
float trigger_l_min = 00.0;       // Valor mínimo de luminosidade para acionar o LED verde e buzzer

void setup() {

  RTC.begin();
  Serial.begin(9600);
  
  lcd.init();
  lcd.backlight();

  pinMode(LDR_PIN, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);

  if(!RTC.begin()) { // SE O RTC NÃO FOR INICIALIZADO, FAZ

    Serial.println("DS3231 não encontrado"); //IMPRIME O TEXTO NO MONITOR SERIAL

    while(1); //SEMPRE ENTRE NO LOOP

  }

  if(RTC.lostPower()){ //SE RTC FOI LIGADO PELA PRIMEIRA VEZ / FICOU SEM ENERGIA / ESGOTOU A BATERIA, FAZ

    Serial.println("DS3231 OK!"); //IMPRIME O TEXTO NO MONITOR SERIAL

    RTC.adjust(DateTime(F(__DATE__), F(__TIME__))); //CAPTURA A DATA E HORA EM QUE O SKETCH É COMPILADO

  }

  // Dando boas vindas ao usuário
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SEJA BEM-VINDO");
  lcd.print(umidade);
  lcd.setCursor(0, 1);
  lcd.print("AO LUMITEMP :)");
  delay(2500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("INICIANDO...");
  delay(1000);
  lcd.clear();

  delay(5000); //INTERVALO DE 100 MILISSEGUNDOS

}

void loop() {

  temperatura = dht11.readCelsius();
  umidade = dht11.readHumidity();

  int valorLuminosidade = analogRead(LDR_PIN);
  int luminosidade = map(valorLuminosidade, 0, 1023, 0 , 100);

  Serial.println(" ");
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" °C");
  Serial.print("Umidade: ");
  Serial.print(umidade);
  Serial.println(" %");
  Serial.print("Luminosidade: ");
  Serial.print(luminosidade);
  Serial.println(" %");

  if(umidade > trigger_u_max || temperatura > trigger_t_max || luminosidade > trigger_l_max) {

    String mensagem1 = "Valores ACIMA dos parametros registrados: ";
    String mensagem2 = "Informações gravadas com sucesso!";

    EEPROM.write(0, temperatura);
    EEPROM.write(1, umidade);
    EEPROM.write(2, luminosidade);
    int endereco = 0;
    for(int endereco = 0; endereco < 3; endereco++)
      Serial.println(mensagem1 + EEPROM.read(endereco) + " " + mensagem2);

    digitalWrite(ledVermelho, HIGH); // Acende o LED vermelho

    // Aciona o buzzer
    tone(buzzer, 1000); // Toca o buzzer com frequência de 1000 Hz
    delay(500); // Tempo ligado do buzzer
    noTone(buzzer); // Para o buzzer
    delay(500); // Tempo de pausa
    tone(buzzer, 1000);
    delay(500);
    noTone(buzzer);
    delay(500);
    tone(buzzer, 1000);
    delay(500);
    noTone(buzzer);

    // Verifica a umidade e exibe mensagem correspondente no LCD
    if (umidade > trigger_l_max) {

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("UMID: ");
      lcd.print(umidade);
      lcd.setCursor(0, 1);
      lcd.print("ABAFADO");
      delay(2500);

    }

    // Verifica a temperatura e exibe mensagem correspondente no LCD
    if (temperatura > trigger_t_max) {
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("TEMP: ");
      lcd.print(temperatura);
      lcd.setCursor(0, 1);
      lcd.print("QUENTE");
      delay(2500);

    }

    // Verifica a luminosidade e exibe mensagem correspondente no LCD
    if (luminosidade > trigger_l_max) {

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("LUMI: ");
      lcd.print(luminosidade);
      lcd.setCursor(0, 1);
      lcd.print("CLARO");
      delay(2500);

    }

    ExibeDataHora(); // Exibe a data e hora no LCD
    delay(2500);

    digitalWrite(ledVermelho, LOW); // Apaga o LED amarelo

  }

  // Verifica se os valores lidos estão abaixo dos limites inferiores
  else if (umidade < trigger_u_min || temperatura < trigger_t_min || luminosidade < trigger_l_min) {

    String mensagem1 = "Valores ABAIXO dos parametros registrados: ";
    String mensagem2 = "Informações gravadas com sucesso!";

    EEPROM.write(0, temperatura);
    EEPROM.write(1, umidade);
    EEPROM.write(2, luminosidade);
    int endereco = 0;
    for(int endereco = 0; endereco < 3; endereco++)
      Serial.println(mensagem1 + EEPROM.read(endereco) + " " + mensagem2);
    
    digitalWrite(ledAmarelo, HIGH); // Acende o LED verde

    // Aciona o buzzer
    tone(buzzer, 1000); // Toca o buzzer com frequência de 1000 Hz
    delay(1000); // Tempo ligado do buzzer
    noTone(buzzer); // Para o buzzer
    delay(500); // Tempo de pausa
    tone(buzzer, 1000);
    delay(1000);
    noTone(buzzer);
    delay(500);
    tone(buzzer, 1000);
    delay(1000);
    noTone(buzzer);

    // Verifica a umidade e exibe mensagem correspondente no LCD
    if (umidade < trigger_u_min) {

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("UMID: ");
      lcd.print(umidade);
      lcd.setCursor(0, 1);
      lcd.print("SECO");
      delay(2500);

    }

    // Verifica a temperatura e exibe mensagem correspondente no LCD
    if (temperatura < trigger_t_min) {

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("TEMP: ");
      lcd.print(temperatura);
      lcd.setCursor(0, 1);
      lcd.print("FRIO");
      delay(2500);

    }

    // Verifica a luminosidade e exibe mensagem correspondente no LCD
    if (luminosidade < trigger_l_min) {

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("LUMI: ");
      lcd.print(luminosidade);
      lcd.setCursor(0, 1);
      lcd.print("ESCURO");
      delay(2500);

    }

    ExibeDataHora(); // Exibe a data e hora no LCD
    delay(2500);

    digitalWrite(ledAmarelo, LOW); // Apaga o LED amarelo

  }

  else {

    digitalWrite(ledVerde, HIGH); // Acende o LED amarelo
    delay(500);
    digitalWrite(ledVerde, LOW); // Apaga o LED amarelo
    delay(500);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("UMID: ");
    lcd.print(umidade);
    lcd.setCursor(0, 1);
    lcd.print("IDEAL");
    delay(2500);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TEMP: ");
    lcd.print(temperatura);
    lcd.setCursor(0, 1);
    lcd.print("IDEAL");
    delay(2500);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LUMI: ");
    lcd.print(luminosidade);
    lcd.setCursor(0, 1);
    lcd.print("IDEAL");
    delay(2500);

    ExibeDataHora(); // Exibe a data e hora no LCD
    delay(2500);

    digitalWrite(ledVerde, LOW); // Apaga o LED amarelo

  }

}

void ExibeDataHora() {

  DateTime now = RTC.now(); //CHAMADA DE FUNÇÃO
  Serial.print("Data: "); //IMPRIME O TEXTO NO MONITOR SERIAL
  Serial.print(now.day(), DEC); //IMPRIME NO MONITOR SERIAL O DIA
  Serial.print('/'); //IMPRIME O CARACTERE NO MONITOR SERIAL
  Serial.print(now.month(), DEC); //IMPRIME NO MONITOR SERIAL O MÊS
  Serial.print('/'); //IMPRIME O CARACTERE NO MONITOR SERIAL
  Serial.print(now.year() % 100); //IMPRIME NO MONITOR SERIAL O ANO
  Serial.print(" / Dia: "); //IMPRIME O TEXTO NA SERIAL
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]); //IMPRIME NO MONITOR SERIAL O DIA
  Serial.print(" / Horas: "); //IMPRIME O TEXTO NA SERIAL
  Serial.print(now.hour(), DEC); //IMPRIME NO MONITOR SERIAL A HORA
  Serial.print(':'); //IMPRIME O CARACTERE NO MONITOR SERIAL
  Serial.print(now.minute(), DEC); //IMPRIME NO MONITOR SERIAL OS MINUTOS
  Serial.print(':'); //IMPRIME O CARACTERE NO MONITOR SERIAL
  Serial.println(now.second(), DEC); //IMPRIME NO MONITOR SERIAL OS SEGUNDOS
  Serial.println(); //QUEBRA DE LINHA NA SERIAL
  delay(1000); //INTERVALO DE 1 SEGUNDO

}
