#include <RTClib.h>            // Biblioteca para o Relógio em Tempo Real (RTC)
#include <EEPROM.h>            // Biblioteca para armazenamento na EEPROM
#include "dht.h"               // Biblioteca para o sensor DHT
#include <Wire.h>             // Biblioteca para comunicação I2C
#include <LiquidCrystal_I2C.h> // Biblioteca para comunicação com o display LCD 20x4 via I2C

// Definições para o display LCD
#define col 16    // Número de colunas do display LCD
#define lin  2    // Número de linhas do display LCD
#define ende  0x27 // Endereço I2C do display LCD

// Definições para o sensor DHT
#define DHTPIN 2     // Pino onde o sensor DHT está conectado
#define DHTTYPE DHT11 // Tipo do sensor DHT (DHT11)

// Opções de configuração
#define LOG_OPTION 1     // Opção para ativar a leitura do log (1 para ativado, 0 para desativado)
#define SERIAL_OPTION 0  // Opção de comunicação serial: 0 para desligado, 1 para ligado
#define UTC_OFFSET -3    // Ajuste de fuso horário para UTC-3

// Instanciação do objeto para o display LCD e para o RTC
LiquidCrystal_I2C lcd(ende, col, lin); // Configuração do display LCD
RTC_DS3231 RTC;                       // Instancia o objeto para o RTC

// Array com os nomes dos dias da semana
char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado"};

// Definição dos pinos dos LEDs e do buzzer
int ledVerde = 11;          // Pino do LED verde
int ledAmarelo = 12;        // Pino do LED amarelo
int ledVermelho = 13;       // Pino do LED vermelho
int buzzer = 7;             // Pino do buzzer
const int LDR_PIN = A0;     // Pino do sensor LDR (resistor dependente de luz)
const int pinoDHT11 = A2;   // Pino onde o sensor DHT11 está conectado

dht DHT;  // Instanciação do objeto para o sensor DHT

// Limiares para acionar LEDs e buzzer
int trigger_t_min = 15.0;       // Valor mínimo de temperatura para acionar o LED verde e buzzer
int trigger_t_max = 25.0;       // Valor máximo de temperatura para acionar o LED vermelho e buzzer
int trigger_u_min = 30.0;       // Valor mínimo de umidade para acionar o LED verde e buzzer
int trigger_u_max = 50.0;       // Valor máximo de umidade para acionar o LED vermelho e buzzer
float trigger_l_max = 30.0;     // Valor máximo de luminosidade para acionar o LED vermelho e buzzer
float trigger_l_min = 00.0;     // Valor mínimo de luminosidade para acionar o LED verde e buzzer

void setup() // Função de configuração inicial
{
  lcd.init(); // Inicializa o display LCD
  lcd.backlight(); // Liga a luz de fundo do display LCD
  lcd.clear(); // Limpa a tela do display LCD

  Serial.begin(9600); // Inicializa a comunicação serial a 9600 bps

  pinMode(LDR_PIN, INPUT); // Configura o pino do sensor LDR como entrada
  pinMode(buzzer, OUTPUT); // Configura o pino do buzzer como saída
  pinMode(ledVerde, OUTPUT); // Configura o pino do LED verde como saída
  pinMode(ledAmarelo, OUTPUT); // Configura o pino do LED amarelo como saída
  pinMode(ledVermelho, OUTPUT); // Configura o pino do LED vermelho como saída

  // Mensagem de boas-vindas no display LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SEJA BEM-VINDO");
  lcd.setCursor(0, 1);
  lcd.print("AO LUMITEMP :)");
  delay(2500); // Aguarda 2,5 segundos
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("INICIANDO...");
  delay(1000); // Aguarda 1 segundo
  lcd.clear();

  // Inicializa o RTC e ajusta a data e hora
  RTC.begin();
  RTC.adjust(DateTime(F(__DATE__), F(__TIME__))); // Ajusta para a data e hora de compilação
  RTC.adjust(DateTime(2024, 9, 16, 22, 22, 00)); // Ajusta para uma data e hora específica
}

void loop() 
{
  // Leitura dos dados do sensor DHT
  DHT.read11(pinoDHT11);
  int umidade = DHT.humidity; // Lê a umidade
  int temperatura = DHT.temperature; // Lê a temperatura

  // Leitura do valor do sensor LDR e mapeamento para a faixa de 20 a 100
  int valorLuminosidade = analogRead(LDR_PIN);
  int luminosidade = map(valorLuminosidade, 0, 1023, 100, 20);

  ExibeDataHora(); // Exibe a data e hora no display LCD

  DateTime now = RTC.now(); // Obtém a data e hora atuais do RTC

  // Calcula o deslocamento do fuso horário em segundos
  int offsetSeconds = UTC_OFFSET * 3600; // Convertendo horas para segundos
  now = now.unixtime() + offsetSeconds; // Adiciona o deslocamento ao tempo atual

  // Converte o tempo ajustado para DateTime
  DateTime agora = DateTime(now);

  // Exibe a data e hora no display LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Data: ");
  lcd.print(agora.day(), DEC);
  lcd.print("/");
  lcd.print(agora.month(), DEC);
  lcd.print("/");
  lcd.print(agora.year(), DEC);
  
  lcd.setCursor(0, 1);
  lcd.print("Hora: ");
  lcd.print(agora.hour(), DEC);
  lcd.print(":");
  lcd.print(agora.minute(), DEC);
  lcd.print(":");
  lcd.print(agora.second(), DEC);

  delay(2500); // Aguarda 2,5 segundos

  // Exibe os dados gerais no display LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DADOS GERAIS:");
  delay(1500); // Aguarda 1,5 segundos

  // Exibe a umidade
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("UMID: ");
  lcd.print(umidade);
  lcd.print("%");
  delay(2500); // Aguarda 2,5 segundos

  // Exibe a temperatura
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TEMP: ");
  lcd.print(temperatura);
  lcd.print("C");
  delay(2500); // Aguarda 2,5 segundos

  // Exibe a luminosidade
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LUMI: ");
  lcd.print(luminosidade);
  lcd.print("%");
  delay(2500); // Aguarda 2,5 segundos

  // Envia os dados para o monitor serial
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

  // Verifica se os valores estão acima dos limites máximos
  if(umidade > trigger_u_max || temperatura > trigger_t_max || luminosidade > trigger_l_max) {

    String mensagem1 = "Valores ACIMA dos parametros registrados: ";
    String mensagem2 = "Informações gravadas com sucesso!";

    // Grava os dados na EEPROM
    EEPROM.write(0, temperatura);
    EEPROM.write(1, umidade);
    EEPROM.write(2, luminosidade);
    
    // Exibe as informações gravadas no monitor serial
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

    // Verifica e exibe mensagens de acordo com os valores de umidade, temperatura e luminosidade
    if (umidade > trigger_l_max) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("UMID: ");
      lcd.print(umidade);
      lcd.print("%");
      lcd.setCursor(0, 1);
      lcd.print("ABAFADO");
      delay(2500);
    }

    if (temperatura > trigger_t_max) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("TEMP: ");
      lcd.print(temperatura);
      lcd.print("C");
      lcd.setCursor(0, 1);
      lcd.print("QUENTE");
      delay(2500);
    }

    if (luminosidade > trigger_l_max) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("LUMI: ");
      lcd.print(luminosidade);
      lcd.print("%");
      lcd.setCursor(0, 1);
      lcd.print("CLARO");
      delay(2500);
    }

    ExibeDataHora(); // Exibe a data e hora no LCD
    delay(2500); // Aguarda 2,5 segundos

    digitalWrite(ledVermelho, LOW); // Apaga o LED vermelho

  }

  // Verifica se os valores estão abaixo dos limites mínimos
  else if (umidade < trigger_u_min || temperatura < trigger_t_min || luminosidade < trigger_l_min) {

    String mensagem1 = "Valores ABAIXO dos parametros registrados: ";
    String mensagem2 = "Informações gravadas com sucesso!";

    // Grava os dados na EEPROM
    EEPROM.write(0, temperatura);
    EEPROM.write(1, umidade);
    EEPROM.write(2, luminosidade);
    
    // Exibe as informações gravadas no monitor serial
    for(int endereco = 0; endereco < 3; endereco++)
      Serial.println(mensagem1 + EEPROM.read(endereco) + " " + mensagem2);

    digitalWrite(ledAmarelo, HIGH); // Acende o LED amarelo

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

    // Verifica e exibe mensagens de acordo com os valores de umidade, temperatura e luminosidade
    if (umidade < trigger_u_min) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("UMID: ");
      lcd.print(umidade);
      lcd.print("%");
      lcd.setCursor(0, 1);
      lcd.print("SECO");
      delay(2500);
    }

    if (temperatura < trigger_t_min) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("TEMP: ");
      lcd.print(temperatura);
      lcd.print("C");
      lcd.setCursor(0, 1);
      lcd.print("FRIO");
      delay(2500);
    }

    if (luminosidade < trigger_l_min) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("LUMI: ");
      lcd.print(luminosidade);
      lcd.print("%");
      lcd.setCursor(0, 1);
      lcd.print("ESCURO");
      delay(2500);
    }

    ExibeDataHora(); // Exibe a data e hora no LCD
    delay(2500); // Aguarda 2,5 segundos

    digitalWrite(ledAmarelo, LOW); // Apaga o LED amarelo

  }

  // Se os valores estiverem dentro dos limites
  else {
    digitalWrite(ledVerde, HIGH); // Acende o LED verde
    delay(500); // Aguarda 0,5 segundos
    digitalWrite(ledVerde, LOW); // Apaga o LED verde
    delay(500); // Aguarda 0,5 segundos

    // Exibe que os valores estão ideais
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("UMID: ");
    lcd.print(umidade);
    lcd.print("%");
    lcd.setCursor(0, 1);
    lcd.print("IDEAL");
    delay(2500);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TEMP: ");
    lcd.print(temperatura);
    lcd.print("C");
    lcd.setCursor(0, 1);
    lcd.print("IDEAL");
    delay(2500);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LUMI: ");
    lcd.print(luminosidade);
    lcd.print("%");
    lcd.setCursor(0, 1);
    lcd.print("IDEAL");
    delay(2500);

    ExibeDataHora(); // Exibe a data e hora no LCD
    delay(2500); // Aguarda 2,5 segundos

    digitalWrite(ledVerde, LOW); // Apaga o LED verde
  }
}

// Função para exibir data e hora no monitor serial
void ExibeDataHora() {
  DateTime now = RTC.now(); // Obtém a data e hora atuais do RTC
  Serial.print("Data: "); // Imprime "Data: " no monitor serial
  Serial.print(now.day(), DEC); // Imprime o dia no monitor serial
  Serial.print('/'); // Imprime o caractere '/' no monitor serial
  Serial.print(now.month(), DEC); // Imprime o mês no monitor serial
  Serial.print('/'); // Imprime o caractere '/' no monitor serial
  Serial.print(now.year() % 100); // Imprime o ano no monitor serial
  Serial.print(" / Dia: "); // Imprime " / Dia: " no monitor serial
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]); // Imprime o nome do dia da semana no monitor serial
  Serial.print(" / Horas: "); // Imprime " / Horas: " no monitor serial
  Serial.print(now.hour(), DEC); // Imprime a hora no monitor serial
  Serial.print(':'); // Imprime o caractere ':' no monitor serial
  Serial.print(now.minute(), DEC); // Imprime os minutos no monitor serial
  Serial.print(':'); // Imprime o caractere ':' no monitor serial
  Serial.println(now.second(), DEC); // Imprime os segundos no monitor serial
  Serial.println(); // Quebra de linha no monitor serial
  delay(1000); // Aguarda 1 segundo
}
