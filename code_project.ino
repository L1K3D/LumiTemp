#include <LiquidCrystal_I2C.h>  // Biblioteca para LCD com interface I2C
#include <RTClib.h>            // Biblioteca para o Relógio em Tempo Real
#include <Wire.h>              // Biblioteca para comunicação I2C
#include <EEPROM.h>            // Biblioteca para armazenamento na EEPROM
#include "DHT.h"               // Biblioteca para o sensor DHT

#define LOG_OPTION 1           // Opção para ativar a leitura do log
#define SERIAL_OPTION 0        // Opção de comunicação serial: 0 para desligado, 1 para ligado
#define UTC_OFFSET -3          // Ajuste de fuso horário para UTC-3

// Configurações do sensor DHT22
#define DHTPIN 2               // Pino onde o sensor DHT22 está conectado
#define DHTTYPE DHT22          // Tipo do sensor DHT
DHT dht(DHTPIN, DHTTYPE);     // Criação do objeto para o sensor DHT

LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço I2C para o LCD (0x3F ou 0x27), com 16 colunas e 2 linhas
RTC_DS1307 RTC;                   // Objeto para o relógio em tempo real

// Configurações da EEPROM
const int maxRecords = 100;       // Máximo de registros na EEPROM
const int recordSize = 8;         // Tamanho de cada registro em bytes (4 bytes para timestamp e 4 bytes para dados)
int startAddress = 0;             // Endereço inicial para a gravação na EEPROM
int endAddress = maxRecords * recordSize; // Endereço final para a gravação na EEPROM
int currentAddress = 0;           // Endereço atual para gravação

const int ledVerde = 11;          // Pino do LED verde
const int ledAmarelo = 12;        // Pino do LED amarelo
const int ledVermelho = 13;       // Pino do LED vermelho
const int buzzer = 7;             // Pino do buzzer
const int LDR_PIN = A0;           // Pino do sensor LDR (resistor dependente de luz)

int lastLoggedMinute = -1;        // Último minuto registrado para controle do log

// Triggers para temperatura, umidade e luminosidade
float trigger_t_min = 15.0;       // Valor mínimo de temperatura para acionar o LED verde e buzzer
float trigger_t_max = 25.0;       // Valor máximo de temperatura para acionar o LED vermelho e buzzer
float trigger_u_min = 30.0;       // Valor mínimo de umidade para acionar o LED verde e buzzer
float trigger_u_max = 50.0;       // Valor máximo de umidade para acionar o LED vermelho e buzzer
float trigger_l_max = 30.0;       // Valor máximo de luminosidade para acionar o LED vermelho e buzzer
float trigger_l_min = 00.0;       // Valor mínimo de luminosidade para acionar o LED verde e buzzer

void setup() {
    pinMode(ledVerde, OUTPUT);    // Configura pinos dos LEDs e buzzer como saída
    pinMode(ledAmarelo, OUTPUT);
    pinMode(ledVermelho, OUTPUT);
    pinMode(buzzer, OUTPUT);
    pinMode(LDR_PIN, INPUT);      // Configura o pino do LDR como entrada
    dht.begin();                 // Inicializa o sensor DHT
    Serial.begin(9600);          // Inicializa a comunicação serial
    lcd.init();                  // Inicializa o LCD
    lcd.backlight();             // Liga o backlight do LCD
    RTC.begin();                 // Inicializa o relógio em tempo real
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__))); // Ajusta o relógio para a data e hora de compilação
    // RTC.adjust(DateTime(2024, 5, 6, 08, 15, 00));  // Ajuste inicial de data e hora
    EEPROM.begin();              // Inicializa a EEPROM
}

void loop() {
    DateTime now = RTC.now();   // Obtém a data e hora atual do relógio

    // Calculando o deslocamento do fuso horário
    int offsetSeconds = UTC_OFFSET * 3600; // Converte horas para segundos
    now = now.unixtime() + offsetSeconds; // Adiciona o deslocamento ao tempo atual

    // Convertendo o tempo ajustado para DateTime
    DateTime adjustedTime = DateTime(now);

    if (LOG_OPTION) get_log(); // Se LOG_OPTION estiver ativado, lê o log da EEPROM

    float umidade = dht.readHumidity();      // Lê a umidade do sensor DHT
    float temperatura = dht.readTemperature(); // Lê a temperatura do sensor DHT

    int valor_luminosidade = analogRead(LDR_PIN); // Lê o valor do sensor LDR
    int luminosidade = map(valor_luminosidade, 0, 1023, 0, 100); // Mapeia o valor lido para uma escala de 0 a 100

    // Verifica se os valores lidos excedem os limites superiores
    if(umidade > trigger_u_max || temperatura > trigger_t_max || luminosidade > trigger_l_max) {
      int tempInt = (int)(temperatura * 100); // Converte a temperatura para inteiro
      int umidInt = (int)(umidade * 100);     // Converte a umidade para inteiro
      int lumiInt = (int)(luminosidade * 100); // Converte a luminosidade para inteiro

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

      // Armazena os dados na EEPROM
      EEPROM.put(currentAddress, now.unixtime());  // Armazena o timestamp
      EEPROM.put(currentAddress + 4, tempInt);    // Armazena a temperatura
      EEPROM.put(currentAddress + 6, umidInt);    // Armazena a umidade
      EEPROM.put(currentAddress + 8, lumiInt);    // Armazena a luminosidade

      getNextAddress(); // Atualiza o endereço para o próximo registro

      ExibeDataHora(); // Exibe a data e hora no LCD
      delay(2500);

      digitalWrite(ledVermelho, LOW); // Apaga o LED vermelho
    }
    // Verifica se os valores lidos estão abaixo dos limites inferiores
    else if (umidade < trigger_u_min || temperatura < trigger_t_min || luminosidade < trigger_l_min) {
      int tempInt = (int)(temperatura * 100); // Converte a temperatura para inteiro
      int umidInt = (int)(umidade * 100);     // Converte a umidade para inteiro
      int lumiInt = (int)(luminosidade * 100); // Converte a luminosidade para inteiro

      digitalWrite(ledVerde, HIGH); // Acende o LED verde

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

      // Armazena os dados na EEPROM
      EEPROM.put(currentAddress, now.unixtime());  // Armazena o timestamp
      EEPROM.put(currentAddress + 4, tempInt);    // Armazena a temperatura
      EEPROM.put(currentAddress + 6, umidInt);    // Armazena a umidade
      EEPROM.put(currentAddress + 8, lumiInt);    // Armazena a luminosidade

      getNextAddress(); // Atualiza o endereço para o próximo registro

      ExibeDataHora(); // Exibe a data e hora no LCD
      delay(2500);

      digitalWrite(ledVerde, LOW); // Apaga o LED verde
    }
    // Se os valores lidos estiverem dentro dos limites ideais
    else {
      digitalWrite(ledAmarelo, HIGH); // Acende o LED amarelo
      delay(500);
      digitalWrite(ledAmarelo, LOW); // Apaga o LED amarelo
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
    }

    if (SERIAL_OPTION) {
        // Exibe a data e hora ajustada no monitor serial
        Serial.print(adjustedTime.day());
        Serial.print("/");
        Serial.print(adjustedTime.month());
        Serial.print("/");
        Serial.print(adjustedTime.year());
        Serial.print(" ");
        Serial.print(adjustedTime.hour() < 10 ? "0" : ""); // Adiciona zero à esquerda se a hora for menor que 10
        Serial.print(adjustedTime.hour());
        Serial.print(":");
        Serial.print(adjustedTime.minute() < 10 ? "0" : ""); // Adiciona zero à esquerda se o minuto for menor que 10
        Serial.print(adjustedTime.minute());
        Serial.print(":");
        Serial.print(adjustedTime.second() < 10 ? "0" : ""); // Adiciona zero à esquerda se o segundo for menor que 10
        Serial.print(adjustedTime.second());
        Serial.print("\n");
    }

    delay(1000); // Atraso de 1 segundo antes da próxima leitura
}

// Função para exibir a data e hora no LCD
void ExibeDataHora() {
  DateTime now = RTC.now();

  // Calculando o deslocamento do fuso horário
  int offsetSeconds = UTC_OFFSET * 3600; // Converte horas para segundos
  now = now.unixtime() + offsetSeconds; // Adiciona o deslocamento ao tempo atual

  DateTime adjustedTime = DateTime(now);

  lcd.setCursor(0, 0);
  lcd.print("DATA: ");
  lcd.print(adjustedTime.day() < 10 ? "0" : ""); // Adiciona zero à esquerda se o dia for menor que 10
  lcd.print(adjustedTime.day());
  lcd.print("/");
  lcd.print(adjustedTime.month() < 10 ? "0" : ""); // Adiciona zero à esquerda se o mês for menor que 10
  lcd.print(adjustedTime.month());
  lcd.print("/");
  lcd.print(adjustedTime.year());
  lcd.setCursor(0, 1);
  lcd.print("HORA: ");
  lcd.print(adjustedTime.hour() < 10 ? "0" : ""); // Adiciona zero à esquerda se a hora for menor que 10
  lcd.print(adjustedTime.hour());
  lcd.print(":");
  lcd.print(adjustedTime.minute() < 10 ? "0" : ""); // Adiciona zero à esquerda se o minuto for menor que 10
  lcd.print(adjustedTime.minute());
  lcd.print(":");
  lcd.print(adjustedTime.second() < 10 ? "0" : ""); // Adiciona zero à esquerda se o segundo for menor que 10
  lcd.print(adjustedTime.second());
}

// Função para atualizar o endereço na EEPROM
void getNextAddress() {
    currentAddress += recordSize;  // Avança o endereço para o próximo registro
    if (currentAddress >= endAddress) {
        currentAddress = 0; // Volta ao início se atingir o final
    }
}

// Função para exibir o log armazenado na EEPROM no monitor serial
void get_log() {
    Serial.println("Data stored in EEPROM:");
    Serial.println("Timestamp\t\tTemperatura\tUmidade\tLuminosidade");

    for (int address = startAddress; address < endAddress; address += recordSize) {
        long timeStamp;
        int tempInt, umidInt, lumiInt;

        // Ler dados da EEPROM
        EEPROM.get(address, timeStamp);
        EEPROM.get(address + 4, tempInt);
        EEPROM.get(address + 6, umidInt);
        EEPROM.get(address + 8, lumiInt);

        // Converter valores
        float temperatura = tempInt / 100.0;
        float umididade = umidInt / 100.0;
        float luminosidade = lumiInt / 100.0;

        // Verificar se os dados são válidos antes de imprimir
        if (timeStamp != 0xFFFFFFFF) { // 0xFFFFFFFF é o valor padrão de uma EEPROM não inicializada
            DateTime dt = DateTime(timeStamp);
            Serial.print(dt.timestamp(DateTime::TIMESTAMP_FULL));
            Serial.print("\t");
            Serial.print(temperatura);
            Serial.print(" C\t\t");
            Serial.print(umididade);
            Serial.print(" %\t\t");
            Serial.print(luminosidade);
            Serial.println(" %\t\t");
        }
    }
}
