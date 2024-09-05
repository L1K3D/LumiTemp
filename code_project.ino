// Importando bibliotecas
#include "DHT.h"
#include <LiquidCrystal_I2C.h>

// Definindo os pinos e variáveis para o sensor DHT
#define DHTPIN 2
#define DHTTYPE DHT11

// Criando instâncias dos objetos LCD e DHT
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

// Definindo os pinos de entrada e saída
int ledVermelho = 13; //Define o pino do LED de cor vermelha para o pino 13
int ledAzul = 12; // Define o pino do LED de cor azul para o pino 12
int ledVerde = 11; // Define o pino do LED de cor verde para o pino 11
int buzzer = 7; // Define o pino do buzzer para o pino 7

// Variáveis para controle de exibição
unsigned long tempoAtual = 0; // Cria variavel inicial de tempoAtual para um valor de 0 (zero). Essa variavel é responsável por calcular o tempo atual de execução do software dentro do sistema Arduino
unsigned long tempoAnterior = 0; // Cria variavel inicial de tempoAnterior para um valor de 0 (zero). Essa variavel irá armazenar o status da variavel de tempoAtual quando a troca de informações na tela LCD do Arduino for realizada
const unsigned long intervalo = 5000; // Intervalo de 5 segundos
enum Exibicao { LUMINOSIDADE, TEMPERATURA, UMIDADE }; // Enum para controle de exibição
Exibicao modoExibicao = LUMINOSIDADE; // Cria uma variavel de modoExibicao a partir dos valores enumerados na variavel 'Exibicao' criada anteriormente. Essa variavel recebe em momento inicial o valor de 'LUMINOSIDADE' para exibir os dados de luminosidade em um primeiro momento de execução do software

// Configurando os bytes para construção do foguete em tela LCD
// Montando o primeiro range de bytes de exibição
byte foguete[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00001,
  B00111,
  B11001
};

// Montando o segundo range de bytes de exibição
byte foguete2[8] = {
  B01100,
  B01010,
  B01001,
  B01000,
  B01111,
  B11000,
  B01000,
  B01000
};

// Montando o terceiro range de bytes de exibição
byte foguete3[8] = {
  B00000,
  B00000,
  B00000,
  B10000,
  B11111,
  B00000,
  B00110,
  B01001
};


// Montando o quarto range de bytes de exibição
byte foguete4[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B11100,
  B00110,
  B00011,
  B00001
};


// Montando o quinto range de bytes de exibição
byte foguete5[8] = {
  B00111,
  B00001,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};


// Montando o sexto range de bytes de exibição
byte foguete6[8] = {
  B01000,
  B11000,
  B01111,
  B01000,
  B01001,
  B01010,
  B01100,
  B00000
};


// Montando o setimo range de bytes de exibição
byte foguete7[8] = {
  B00110,
  B00000,
  B11111,
  B10000,
  B00000,
  B00000,
  B00000,
  B00000
};


// Montando o oitavo range de bytes de exibição
byte foguete8[8] = {
  B00011,
  B00110,
  B11100,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

// Executa a varredura e configuração inicial dos componentes e suas saidas ou entradas
void setup() {

  // Iniciliza, configura e reconhece o display LCD
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);

  // Configura os caracteres iniciais para exibição do foguete em um modelo inicial
  lcd.createChar(0, foguete); 
  lcd.createChar(1, foguete2);
  lcd.createChar(2, foguete3);
  lcd.createChar(3, foguete4);
  lcd.createChar(4, foguete5);
  lcd.createChar(5, foguete6);
  lcd.createChar(6, foguete7);
  lcd.createChar(7, foguete8);

  // Configura as entradas e saidas dos dispositivos
  pinMode(buzzer, OUTPUT); //Configura o buzzer como um dispositivo de saída
  pinMode(ledVermelho, OUTPUT); //Configura o ledVermelho como um dispositivo de saída
  pinMode(ledAzul, OUTPUT); //Configura o ledAzul como um dispositivo de saída
  pinMode(ledVerde, OUTPUT); //Configura o ledVerde como um dispositivo de saída

  //Inicializa o sensor dht
  dht.begin();

  //Realiza um delay de dois segundos no processamento do modelo para não sobrecarregamento do sistema
  delay(2000);

}

// Inica o loop principal do modelo
void loop() {

  // Calcula o tempo atual do loop
  tempoAtual = millis(); // Atualiza o tempo atual

  // Caso a subtração resultante entre o tempo atual e o tempo anterior calculado seja maior ou igual ao tempo de intervalo pré-definido anteriormente, alterna entre luminosidade, temperatura e umidade
  if (tempoAtual - tempoAnterior >= intervalo) {
    
    // Cria a primeira condição -> Caso o 'modoExibicao' no momento da verificação seja equivalente a LUMINOSIDADE, o mesmo é trocado para TEMPERATURA
    if (modoExibicao == LUMINOSIDADE) {

      modoExibicao = TEMPERATURA;

    }

    // Cria a primeira condição -> Caso o 'modoExibicao' no momento da verificação seja equivalente a TEMPERATURA, o mesmo é trocado para HUMIDADE
    else if (modoExibicao == TEMPERATURA) {

      modoExibicao = UMIDADE;

    } 

    // Cria a primeira condição -> Caso o 'modoExibicao' no momento da verificação seja equivalente a HUMIDADE, o mesmo é trocado para LUMINOSIDADE
    else {

      modoExibicao = LUMINOSIDADE;

    }

    // Atualiza a variavel de tempo atual para o valor da variavel de tempo anterior
    tempoAnterior = tempoAtual;

  }

  // Realiza os modelos de exibição
  // Caso o 'modoExibição' seja equivalende a 'LUMINOSIDADE' executa a animação do foguete e executa a exibição dos dados de luminosidade
  if (modoExibicao == LUMINOSIDADE) {

    animarFoguete();
    mostrarLuminosidadeInfo();

  }

  // Caso o 'modoExibição' seja equivalende a 'TEMPERATURA' executa a animação do foguete e executa a exibição dos dados de temperatura
  else if (modoExibicao == TEMPERATURA) {

    animarFoguete();
    mostrarTemperaturaInfo();

  }

  // Caso o 'modoExibição' seja equivalende a 'HUMIDADE' executa a animação do foguete e executa a exibição dos dados de humidade
  else if (modoExibicao == UMIDADE) {

    animarFoguete();
    mostrarHumidadeInfo();

  }

  // Pequeno atraso para evitar a sobrecarga do processador
  delay(500); 

}

// Define e configura função que exibe e calcula dados de luminosidade
void mostrarLuminosidadeInfo() {

  // Lê o valor do sensor de luminosidade
  int luminosidade = analogRead(A0);

  // Cálculo da tensão e resistência do LDR
  float Vout = (luminosidade * 0.0048828125); // Conversão para tensão
  float RLDR = (10000.0 * (5 - Vout)) / Vout; // Cálculo da resistência

  // Realiza a configuração dos dados que serão exibidos no LCD
  lcd.clear(); // Limpa o LCD
  lcd.setCursor(0, 0); // Define o cursor de onde o print seguiinte deve ser exibido
  lcd.print("LUMINOSIDADE:"); // Exibe que o dado a ser exibido é o de 'LUMINOSIDADE' e está atracado na linha zero, coluna 0

  // Define ações para cada variação de valor do sensor LDR
  // Define ações para caso o valor do LDR seja menor ou igual a 0 (zero)
  if (RLDR <= 0) {

    // Define status dos LED's
    digitalWrite.(ledAzul, HIGH); // Ascende o LED azul
    digitalWrite.(ledVerde, LOW); // Apaga o LED verde
    digitalWrite.(ledVermelho, LOW); // Apaga o LED Vermelho

    // Configura a exibição do valor na tela LCD
    lcd.setCursor(0, 1); // Define o cursor de onde o print seguinte deve ser exibido
    lcd.print("ESCURO " + String(RLDR) + "%"); // Diz que o ambiente está escuro, e exibe o porcentual de luminosidade
    
    // Acionar o buzzer
    tone(buzzer, 1000); // Tocar o buzzer com frequência de 1000 Hz
    delay(2500); // Tempo do buzzer
    noTone(buzzer); // Parar o buzzer
    delay(2500); // Tempo de pausa
  
  } 
  
  // Define ações para caso o valor do LDR seja maior que zero e menor que 30
  else if ((RLDR > 0) && (RLDR < 30)) {

    // Define status dos LED's
    digitalWrite.(ledAzul, LOW); // Apaga o LED azul
    digitalWrite.(ledVerde, HIGH); // Ascende o LED verde
    digitalWrite.(ledVermelho, LOW); // Apaga o LED vermelho

    // Configura a exibição do valor na tela LCD
    lcd.setCursor(0, 1); // Define o cursor de onde o print seguinte deve ser exibido
    lcd.print("IDEAL " + String(RLDR) + "%"); // Diz que o ambiente está ideal, e exibe o porcentual de luminosidade

  } 
  
  // Define ações para caso o valor do LDR seja maior que 30
  else if (RLDR > 30) {

    // Define status dos LED's
    digitalWrite.(ledAzul, LOW); // Apaga o LED azul
    digitalWrite.(ledVerde, LOW); // Apaga o LED verde
    digitalWrite.(ledVermelho, HIGH); // Apaga o LED vermelho

    // Configura a exibição do valor na tela LCD
    lcd.setCursor(0, 1); // Define o cursor de onde o print seguinte deve ser exibido
    lcd.print("CLARO " + String(RLDR) + "%"); // Diz que o ambiente está claro, e exibe o porcentual de luminosidade
    
    // Acionar o buzzer
    tone(buzzer, 1000); // Tocar o buzzer com frequência de 1000 Hz
    delay(500); // Tempo do buzzer
    noTone(buzzer); // Parar o buzzer
    delay(500); // Tempo de pausa

  }

}

// Define e configura função que exibe e calcula dados de temperatura
void mostrarTemperaturaInfo() {

  // Define uma variavel para a leitura de temperatura e configura o sensor DHT para ler a Temperatura do ambiente
  float temperatura = dht.readTemperature();

  // Realiza a configuração dos dados que serão exibidos no LCD
  lcd.clear(); // Limpa o LCD
  lcd.setCursor(0, 0); // Define o cursor de onde o print seguiinte deve ser exibido
  lcd.print("TEMPERATURA:"); // Exibe que o dado a ser exibido é o de 'TEMPERATURA' e está atracado na linha zero, coluna 0

  // Define ações para cada variação de valor do sensor DHT para temperatura
  // Define ações para caso o valor do DHT em temperatura seja menor ou igual a 15 graus celcius
  if (temperatura <= 15.0) { 

    // Define status dos LED's
    digitalWrite.(ledAzul, HIGH); // Ascende o LED azul
    digitalWrite.(ledVerde, LOW); // Apaga o LED verde
    digitalWrite.(ledVermelho, LOW); // Apaga o LED vermelho

    // Configura a exibição do valor na tela LCD
    lcd.setCursor(0, 1); // Define o cursor de onde o print seguinte deve ser exibido
    lcd.print("FRIO " + String(temperatura) + "C"); // Diz que o ambiente está frio, e exibe a temperatura

    // Acionar o buzzer
    tone(buzzer, 1000); // Tocar o buzzer com frequência de 1000 Hz
    delay(2500); // Tempo do buzzer
    noTone(buzzer); // Parar o buzzer
    delay(2500); // Tempo de pausa
  
  }

  // Define ações para caso o valor do DHT em temperatura seja maior que 15 e menor que 25 graus celcius
  else if ((temperatura > 15) && (temperatura < 25)) {

    // Define status dos LED's
    digitalWrite.(ledAzul, LOW); // Apaga o LED azul
    digitalWrite.(ledVerde, HIGH); // Ascende o LED verde
    digitalWrite.(ledVermelho, LOW); // Apaga o LED vermelho

    // Configura a exibição do valor na tela LCD
    lcd.setCursor(0, 1); // Define o cursor de onde o print seguinte deve ser exibido
    lcd.print("IDEAL " + Sring(temperatura) + "C"); // Diz que o ambiente está ideal, e exibe a temperatura

  } 

  // Define ações para caso o valor do DHT em temperatura seja maior ou igual que 25 graus celcius
  else if (temperatura >= 25) {

    // Define status dos LED's
    digitalWrite.(ledAzul, LOW); // Apaga o LED azul
    digitalWrite.(ledVerde, LOW); // Apaga o LED verde
    digitalWrite.(ledVermelho, HIGH); // Ascende o LED Vermelho

    // Configura a exibição do valor na tela LCD
    lcd.setCursor(0, 1); // Define o cursor de onde o print seguinte deve ser exibido
    lcd.print("QUENTE " + String(temperatura) + "C"); // Diz que o ambiente está quente, e exibe a temperatura
    
    // Acionar o buzzer
    tone(buzzer, 1000); // Tocar o buzzer com frequência de 1000 Hz
    delay(500); // Tempo do buzzer
    noTone(buzzer); // Parar o buzzer
    delay(500); // Tempo de pausa

  }

}

// Define e configura função que exibe e calcula dados de temperatura
void mostrarHumidadeInfo() {

  // Define uma variavel para a leitura de humidade e configura o sensor DHT para ler a Humidade do ambiente
  float humidade = dht.readHumidity();

  // Realiza a configuração dos dados que serão exibidos no LCD
  lcd.clear(); // Limpa o LCD
  lcd.setCursor(0, 0); // Define o cursor de onde o print seguiinte deve ser exibido
  lcd.print("HUMIDADE:"); // Exibe que o dado a ser exibido é o de 'HUMIDADE' e está atracado na linha zero, coluna 0

  // Define ações para cada variação de valor do sensor DHT para humidade
  // Define ações para caso o valor do DHT em humidade seja menor ou igual a 30
  if (humidade <= 30) { 

    // Define status dos LED's
    digitalWrite.(ledAzul, HIGH); // Ascende o LED azul
    digitalWrite.(ledVerde, LOW); // Apaga o LED verde
    digitalWrite.(ledVermelho, LOW); // Apaga o LED vermelho

    // Configura a exibição do valor na tela LCD
    lcd.setCursor(0, 1); // Define o cursor de onde o print seguinte deve ser exibido
    lcd.print("SECO " + String(humidade) + "%"); // Diz que o ambiente está seco, e exibe a humidade

    // Acionar o buzzer
    tone(buzzer, 1000); // Tocar o buzzer com frequência de 1500 Hz
    delay(2500); // Tempo do buzzer
    noTone(buzzer); // Parar o buzzer
    delay(2500); // Tempo de pausa
  
  } 

  // Define ações para caso o valor do DHT em humidade seja maior que 30 e menor que 50 graus celcius
  else if ((humidade > 30) && (humidade < 50)) {

    // Define status dos LED's
    digitalWrite.(ledAzul, LOW); // Apaga o LED azul
    digitalWrite.(ledVerde, HIGH); // Ascende o LED verde
    digitalWrite.(ledVermelho, LOW); // Apaga o LED vermelho

    // Configura a exibição do valor na tela LCD
    lcd.setCursor(0, 1); // Define o cursor de onde o print seguinte deve ser exibido
    lcd.print("IDEAL " + String(humidade) + "%"); // Diz que o ambiente está ideal, e exibe a humidade

  } 

  // Define ações para caso o valor do DHT em humidade seja maior ou igual que 50
  else if (humidade >= 50) {

    // Define status dos LED's
    digitalWrite.(ledAzul, LOW); // Ascende o LED azul
    digitalWrite.(ledVerde, LOW); // Apaga o LED verde
    digitalWrite.(ledVermelho, HIGH); // Ascende o LED vermelho

    // Configura a exibição do valor na tela LCD
    lcd.setCursor(0, 1); // Define o cursor de onde o print seguinte deve ser exibido
    lcd.print("ABAFADO " + String(humidade) + "%"); // Diz que o ambiente está abafado, e exibe a humidade
    
    // Acionar o buzzer
    tone(buzzer, 1000); // Tocar o buzzer com frequência de 1000 Hz
    delay(500); // Tempo do buzzer
    noTone(buzzer); // Parar o buzzer
    delay(500); // Tempo de pausa

  }

}

// Criando função para exibir a animação do foguete
void animarFoguete() {

  // Definindo um array bidimensional para armazenar os bytes que representam cada frame da animação do foguete.
  byte foguetes[8][8] = {foguete, foguete2, foguete3, foguete4, foguete5, foguete6, foguete7, foguete8};

  // Loop para percorrer cada frame da animação
  for (int i = 0; i < 8; i++) {

    // Limpa o display LCD antes de desenhar o novo frame
    lcd.clear();

    // Define o cursor na posição (0, 0) para a linha superior do LCD
    lcd.setCursor(0, 0);

    // Loop para escrever cada byte do frame na linha superior
    for (int j = 0; j < 8; j++) {
      // Escreve o byte correspondente à coluna 'j' no display LCD
      lcd.write(byte(j));
    }

    // Define o cursor na posição (0, 1) para a linha inferior do LCD
    lcd.setCursor(0, 1);

    // Loop para escrever cada byte do frame na linha inferior
    for (int j = 4; j < 8; j++) {
      // Escreve o byte correspondente à coluna 'j' no display LCD
      lcd.write(byte(j));
    }

    // Pausa de 500 milissegundos antes de atualizar o frame
    delay(500);
  }
  
}
