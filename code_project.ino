// Importando bibliotecas
#include "DHT.h"
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <RTClib.h>
#include <Wire.h>

// Definindo os pinos e variáveis para os sensores
#define DHTPIN 2 // Configura o pino de entrada do sensor DHT
#define DHTTYPE DHT11 // Configura o tipo/versão do sensor DHT
#define LOG_OPTION 1 // Exibe o tipo de LOG que será exibido
#define SERIAL_OPTION 0 // Reserva a porta serial de número 0 para capotura das informações de data e hora
#define UTC_OFFSET -3 // Registra o modelo de correção de fuso-horário

// Criando instâncias dos objetos e sensores
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

// Configurando o EEPROM
const int maxRecords = 100; // Configura o tamanho total de registros
const int recordSize = 8; // Configura o tamanho de cada registro em bytes
int startAddress = 0; // Configura o endereço de 
int endAddress = maxRecords * recordSize;
int currentAddress = 0;

int lastLoggedMinute = -1;

// Definindo os pinos de entrada e saída
int ledVermelho = 13; //Define o pino do LED de cor vermelha para o pino 13
int ledAzul = 12; // Define o pino do LED de cor azul para o pino 12
int ledVerde = 11; // Define o pino do LED de cor verde para o pino 11
int buzzer = 7; // Define o pino do buzzer para o pino 7

// Variáveis para controle de exibição
unsigned long tempoAtual = 0; // Cria variavel inicial de tempoAtual para um valor de 0 (zero). Essa variavel é responsável por calcular o tempo atual de execução do software dentro do sistema Arduino
unsigned long tempoAnterior = 0; // Cria variavel inicial de tempoAnterior para um valor de 0 (zero). Essa variavel irá armazenar o status da variavel de tempoAtual quando a troca de informações na tela LCD do Arduino for realizada
const unsigned long intervalo = 11000; // Intervalo de 10 segundos
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
  pinMode(LED_BUILTIN, OUTPUT);
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

  //Configura e inicializa o sensor RTC
  RTC.begin(); // Inicialização do Relógio em Tempo Real
  RTC.adjust(DateTime(F(__DATE__), F(__TIME__))); // Configura o modelo de exibição da data e da hora
  RTC.adjust(DateTime(2024, 5, 6, 08, 15, 00)); // Ajusta os modelos de exibição de data e hora

  // Inicilizando a memória EEPROM
  EEPROM.begin()

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
    delay(5000);
    mostrarDataEHora();

  }

  // Caso o 'modoExibição' seja equivalende a 'TEMPERATURA' executa a animação do foguete e executa a exibição dos dados de temperatura
  else if (modoExibicao == TEMPERATURA) {

    animarFoguete();
    mostrarTemperaturaInfo();
    delay(5000);
    mostrarDataEHora();

  }

  // Caso o 'modoExibição' seja equivalende a 'HUMIDADE' executa a animação do foguete e executa a exibição dos dados de humidade
  else if (modoExibicao == UMIDADE) {

    animarFoguete();
    mostrarUmidadeInfo();
    delay(5000);
    mostrarDataEHora();

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

    // Converte valores para int para armazenamento
    int lumiInt = (int)(luminosidade * 100)

    // Escreve os dados na memória EEPROM
    EEPROM.put(currentAddress, now.unixtime()); // Armazena dados de data e hora no endereço disponível
    EEPROM.put(currentAddress + 4, lumiInt); // Armazena os dados de luminosidade

    // Atualiza o endereço para o próximo registro
    getNextAddress();
  
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

    // Converte valores para int para armazenamento
    int lumiInt = (int)(luminosidade * 100)

    // Escreve os dados na memória EEPROM
    EEPROM.put(currentAddress, now.unixtime()); // Armazena dados de data e hora no endereço disponível
    EEPROM.put(currentAddress + 4, lumiInt); // Armazena os dados de luminosidade

    // Atualiza o endereço para o próximo registro
    getNextAddress();
  

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

    // Converte valores para int para armazenamento
    int tempInt = (int)(temperatura * 100)

    // Escreve os dados na memória EEPROM
    EEPROM.put(currentAddress, now.unixtime()); // Armazena dados de data e hora no endereço disponível
    EEPROM.put(currentAddress + 6, tempInt); // Armazena os dados de temperatura

    // Atualiza o endereço para o próximo registro
    getNextAddress();
  
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

    // Converte valores para int para armazenamento
    int tempInt = (int)(temperatura * 100)

    // Escreve os dados na memória EEPROM
    EEPROM.put(currentAddress, now.unixtime()); // Armazena dados de data e hora no endereço disponível
    EEPROM.put(currentAddress + 6, tempInt); // Armazena os dados de temperatura

    // Atualiza o endereço para o próximo registro
    getNextAddress();

  }

}

// Define e configura função que exibe e calcula dados de temperatura
void mostrarUmidadeInfo() {

  // Define uma variavel para a leitura de humidade e configura o sensor DHT para ler a Humidade do ambiente
  float umidade = dht.readHumidity();

  // Realiza a configuração dos dados que serão exibidos no LCD
  lcd.clear(); // Limpa o LCD
  lcd.setCursor(0, 0); // Define o cursor de onde o print seguiinte deve ser exibido
  lcd.print("UMIDADE:"); // Exibe que o dado a ser exibido é o de 'HUMIDADE' e está atracado na linha zero, coluna 0

    // Define ações para cada variação de valor do sensor DHT para humidade
    // Define ações para caso o valor do DHT em humidade seja menor ou igual a 30
  if (umidade <= 30) {

    // Define status dos LED's
    digitalWrite.(ledAzul, HIGH); // Ascende o LED azul
    digitalWrite.(ledVerde, LOW); // Apaga o LED verde
    digitalWrite.(ledVermelho, LOW); // Apaga o LED vermelho

    // Configura a exibição do valor na tela LCD
    lcd.setCursor(0, 1); // Define o cursor de onde o print seguinte deve ser exibido
    lcd.print("SECO " + String(umidade) + "%"); // Diz que o ambiente está seco, e exibe a humidade

    // Acionar o buzzer
    tone(buzzer, 1000); // Tocar o buzzer com frequência de 1500 Hz
    delay(2500); // Tempo do buzzer
    noTone(buzzer); // Parar o buzzer
    delay(2500); // Tempo de pausa

    // Converte valores para int para armazenamento
    int umidInt = (int)(umidade * 100)

    // Escreve os dados na memória EEPROM
    EEPROM.put(currentAddress, now.unixtime()); // Armazena dados de data e hora no endereço disponível
    EEPROM.put(currentAddress + 8, umidInt); // Armazena os dados de umidade

    // Atualiza o endereço para o próximo registro
    getNextAddress();

  }

  // Define ações para caso o valor do DHT em humidade seja maior que 30 e menor que 50 graus celcius
  else if ((umidade > 30) && (umidade < 50)) {

    // Define status dos LED's
    digitalWrite.(ledAzul, LOW); // Apaga o LED azul
    digitalWrite.(ledVerde, HIGH); // Ascende o LED verde
    digitalWrite.(ledVermelho, LOW); // Apaga o LED vermelho

    // Configura a exibição do valor na tela LCD
    lcd.setCursor(0, 1); // Define o cursor de onde o print seguinte deve ser exibido
    lcd.print("IDEAL " + String(umidade) + "%"); // Diz que o ambiente está ideal, e exibe a humidade

  }

  // Define ações para caso o valor do DHT em humidade seja maior ou igual que 50
  else if (umidade >= 50) {

    // Define status dos LED's
    digitalWrite.(ledAzul, LOW); // Ascende o LED azul
    digitalWrite.(ledVerde, LOW); // Apaga o LED verde
    digitalWrite.(ledVermelho, HIGH); // Ascende o LED vermelho

    // Configura a exibição do valor na tela LCD
    lcd.setCursor(0, 1); // Define o cursor de onde o print seguinte deve ser exibido
    lcd.print("ABAFADO " + String(umidade) + "%"); // Diz que o ambiente está abafado, e exibe a humidade
    
    // Acionar o buzzer
    tone(buzzer, 1000); // Tocar o buzzer com frequência de 1000 Hz
    delay(500); // Tempo do buzzer
    noTone(buzzer); // Parar o buzzer
    delay(500); // Tempo de pausa

    // Converte valores para int para armazenamento
    int umidInt = (int)(umidade * 100)

    // Escreve os dados na memória EEPROM
    EEPROM.put(currentAddress, now.unixtime()); // Armazena dados de data e hora no endereço disponível
    EEPROM.put(currentAddress + 8, umidInt); // Armazena os dados de umidade

    // Atualiza o endereço para o próximo registro
    getNextAddress();

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

void mostrarDataEHora() {

  // Captura informações da data e da hora exata da execução da função
  Datatime now = RTC.now();

  // Calculando o deslocamento do fuso horário
  int offsetSeconds = UTC_OFFSET * 3600; // Convertendo horas para segundos
  now = now.unixtime() + offsetSeconds; // Adicionando o deslocamento ao tempo atual

  // Convertendo o novo tempo para DateTime
  DateTime adjustedTime = DateTime(now);

  // Realiza a impressão de data e hora no display LCD
  lcd.setCursor(0, 0); // Ancora a primeira linha do LCD
  lcd.print("DATA: "); // Registra uma exibição de data
  lcd.print(adjustedTime.day() < 10 ? "0" : ""); // Adiciona zero à esquerda se dia for menor que 10
  lcd.print(adjustedTime.day());
  lcd.print("/");
  lcd.print(adjustedTime.month() < 10 ? "0" : ""); // Adiciona zero à esquerda se mês for menor que 10
  lcd.print(adjustedTime.month());
  lcd.print("/");
  lcd.print(adjustedTime.year());
  lcd.setCursor(0, 1);
  lcd.print("HORA: ");
  lcd.print(adjustedTime.hour() < 10 ? "0" : ""); // Adiciona zero à esquerda se hora for menor que 10
  lcd.print(adjustedTime.hour());
  lcd.print(":");
  lcd.print(adjustedTime.minute() < 10 ? "0" : ""); // Adiciona zero à esquerda se minuto for menor que 10
  lcd.print(adjustedTime.minute());
  lcd.print(":");
  lcd.print(adjustedTime.second() < 10 ? "0" : ""); // Adiciona zero à esquerda se segundo for menor que 10
  lcd.print(adjustedTime.second());

}

void getNextAddress() {

    // Avança o endereço atual para o próximo local de armazenamento na EEPROM
    currentAddress += recordSize;

    // Verifica se o endereço atual excedeu o limite de armazenamento
    if (currentAddress >= endAddress) {

      // Se o endereço ultrapassou o limite (endAddress), redefine o endereço para o início
      currentAddress = 0; // Volta para o começo se atingir o limite
    
    }

}


void get_log() {

  // Imprime um cabeçalho indicando que os dados armazenados na EEPROM serão exibidos
  Serial.println("Dado armazenado na memória EEPROM:");
  Serial.println("Timestamp\t\tLuminosidade\tTemperatura\tUmidade");

  // Loop para percorrer todos os registros armazenados na EEPROM
  for (int address = startAddress; address < endAddress; address += recordSize) {
    
    long timeStamp; // Variável para armazenar o timestamp (data e hora)
    int lumiInt; // Variável para armazenar o valor da luminosidade (em inteiro)
    int tempInt; // Variável para armazenar o valor da temperatura (em inteiro)
    int umidInt; // Variável para armazenar o valor da umidade (em inteiro)

    // Ler os dados da EEPROM usando a função EEPROM.get()
    EEPROM.get(address, timeStamp); // Lê o timestamp do endereço atual
    EEPROM.get(address + 4, lumiInt); // Lê o valor da luminosidade (início do próximo registro)
    EEPROM.get(address + 6, tempInt); // Lê o valor da temperatura (continuação do registro)
    EEPROM.get(address + 8, humiInt); // Lê o valor da umidade (final do registro)

    // Converter valores de inteiros para floats para facilitar a leitura
    float luminosidade = lumiInt / 100.0;  // Converte o valor de luminosidade para float
    float temperatura = tempInt / 100.0;   // Converte o valor de temperatura para float
    float umidade = umidInt / 100.0;       // Converte o valor de umidade para float

    // Verificar se os dados são válidos antes de imprimir
    if (timeStamp != 0xFFFFFFFF) { // 0xFFFFFFFF é o valor padrão de uma EEPROM não inicializada
      // Cria um objeto DateTime a partir do timestamp para formatação legível
      DateTime dt = DateTime(timeStamp);
      
      // Imprime o timestamp no formato completo
      Serial.print(dt.timestamp(DateTime::TIMESTAMP_FULL));
      Serial.print("\t");
      
      // Imprime a luminosidade com unidade de porcentagem
      Serial.print(luminosidade);
      Serial.print(" %\t\t");
      
      // Imprime a temperatura com unidade de Celsius
      Serial.print(temperatura);
      Serial.print(" C\t\t");
      
      // Imprime a umidade com unidade de porcentagem
      Serial.print(umidade);
      Serial.println(" %");  // Adiciona uma nova linha após o último dado
    }

  }

}