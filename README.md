# 🚀 Data Logger para Monitoramento Ambiental (FESA)

Este projeto consiste em um sistema de monitoramento ambiental baseado em um microcontrolador (MCU) Atmega 328P, montado em uma placa Arduino Uno R3. Ele realiza a medição de luminosidade, temperatura e umidade, utilizando sensores LDR e DHT-11. Além disso, utiliza um RTC (Real Time Clock) para marcar a data e a hora de cada leitura, com o objetivo de armazenar os dados coletados e realizar ações como a ativação de LEDs e buzzer em função dos valores medidos.

## 🛠️ Especificações Técnicas:

Lista de componentes usados:

-> **Microcontrolador:** Atmega 328P - Arduino Uno R3

-> **Sensor de Luminosidade:** LDR (resistor dependente de luz) + Resistor 10KOhm

-> **Sensor de Temperatura e Umidade:** DHT-11

-> **Display:** LCD 16x2 - Interface I2C

-> **Bateria:** 9V (com suporte para a bateria)

-> **RTC (Real Time Clock):** DS3231

-> **Atuadores:**
       _1 Buzzer_
       _3 LEDs (Verde, Amarelo, Vermelho)_
      
-> **Protoboard e Jumpers**

## ⚡ Diagrama Elétrico:

![Diagrama Elétrico](https://github.com/user-attachments/assets/063c4789-e729-45cd-8191-659d4cc04cc2)

## 📋 Manual de operação e funcionamento geradl do sistema:

_**1. INICIALIZAÇÃO**_

O sistema é alimentado por uma bateria de 9V e, ao ser energizado, inicia a medição de temperatura, umidade e luminosidade automaticamente. O RTC (DS3231) mantém o registro preciso da data e hora, que é utilizado para marcar os dados de leitura e exibi-los em um LCD 16x2. Ao iniciar, o sistema exibe uma mensagem de boas-vindas no LCD e aguarda alguns segundos antes de começar as medições.

_**2. MEDIÇÃO DE PARÂMETROS**_

**Temperatura e Umidade:** O sensor DHT-11 é responsável por medir os níveis de temperatura e umidade do ambiente.

**Luminosidade:** O sensor LDR mede a quantidade de luz no ambiente, que é convertida para uma escala de 0 a 100%.

_**3. AÇÃO DOS LED's E BUZZER**_

Com base nas medições feitas pelos sensores, o sistema atua da seguinte maneira:

O **LED vermelho e o buzzer** são acionados quando a temperatura, umidade ou luminosidade ultrapassam os limites **máximos** definidos. O **LED amarelo e o buzzer** são acionados quando os valores ficam **abaixo** dos limites mínimos. O **LED verde** indica que os parâmetros estão **dentro dos limites aceitáveis e as condições são consideradas ideais.**

_**4. EXIBIÇÃO DE INFORMAÇÕES**_

_#1 -_ As informações de temperatura, umidade e luminosidade são exibidas ciclicamente no LCD 16x2;

_#2 -_ Quando um dos parâmetros ultrapassa os limites estabelecidos, uma mensagem específica é exibida no LCD, indicando o status do ambiente:

"FRIO", "QUENTE" para temperatura

"SECO", "ABAFADO" para umidade

"ESCURO", "CLARO" para luminosidade

_#3 -_ A data e hora corrente também são exibidas no display a cada ciclo de leitura.

_**5. MODELOS DE ARMAZENAMENTO E RESET DO SISTEMA**_

O sistema utiliza a memória EEPROM do Arduino para registrar os valores de temperatura, umidade e luminosidade sempre que um dos parâmetros ultrapassa os limites definidos. Esses dados podem ser visualizados no monitor serial para acompanhamento posterior. Se o sistema precisar ser reiniciado, isso pode ser feito pressionando o botão de reset presente na placa Arduino UNO R3.

_**---###---###---###---###---###---###---###---###---###---###---###---###---###---###---###---###---###---###---###---###---###---**_

Este sistema de Data-Logger permite monitorar e registrar o comportamento ambiental em tempo real, fornecendo feedback visual (via LEDs e LCD) e sonoro (via buzzer) com base em parâmetros configuráveis para temperatura, umidade e luminosidade. Ele é útil em aplicações onde o controle do ambiente é essencial, como em estufas, armazéns ou ambientes industriais. O projeto pode ser expandido para incluir mais sensores e funcionalidades, como a comunicação com um servidor remoto ou a adição de armazenamento externo para maior capacidade de dados.

## 🤝 Integrantes do Projeto:

- Erikson Vieira Queiroz - RA: 082220021
- Enzo Brito Alves de Oliveira - RA: 082220040
- Guilherme Alves Barbosa - RA: 082220014
- Heitor Santos Ferreira - RA: 081230042
- Tainara do Nascimento Casimiro - RA: 082220011
- William Santim - RA: 082220033

- Link do Vídeo Demonstrativo: 
