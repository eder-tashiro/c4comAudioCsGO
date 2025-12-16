💣 Arduino Bomb Game – Hardware Real

Projeto de bomba eletrônica estilo CS:GO, desenvolvido com Arduino UNO, LCD 16x2 I2C, Keypad 4x4, DFPlayer Mini, buzzer, LEDs e alto-falante.

Este projeto é voltado para montagem física real, podendo ser usado em:

Jogos de airsoft

Escape rooms

Projetos educacionais

Demonstrações eletrônicas

🎮 Funcionalidades

⏱️ Contagem regressiva configurável

🔢 Código de desarme editável pelo teclado

🔊 Beep progressivo (acelera com o tempo)

🔊 Áudio real via DFPlayer Mini

💥 Explosão com áudio sequencial

✅ Desarme com áudio sequencial

🔁 Reinício rápido do jogo

🔐 Alteração do código de desarme

🔉 Controle de volume pelo keypad

🧰 Componentes (BOM)
🔹 Controle

1× Arduino UNO R3

1× Keypad matricial 4x4

🔹 Interface

1× LCD 16x2 com módulo I2C (PCF8574)

1× Potenciômetro 10k (opcional, se LCD sem ajuste onboard)

🔹 Áudio

1× DFPlayer Mini

1× Cartão microSD (≤ 32GB, FAT32)

1× Alto-falante 8Ω / 3W

1× Resistor 1kΩ (RX do DFPlayer)

🔹 Sinalização

1× LED vermelho

1× LED verde

2× Resistores 220Ω

🔹 Outros

1× Buzzer ativo ou passivo

Jumpers, protoboard ou PCB

Fonte 5V / ≥1A

🔌 Ligações Elétricas
LCD I2C
LCD	Arduino UNO
VCC	5V
GND	GND
SDA	A4
SCL	A5

Endereço I2C padrão: 0x27
(se não funcionar, testar 0x3F)

Keypad 4x4
Linha / Coluna	Arduino
R1	5
R2	4
R3	3
R4	2
C1	A3
C2	A2
C3	A1
C4	A0
DFPlayer Mini
DFPlayer	Arduino
VCC	5V
GND	GND
TX	8
RX	9 (via resistor 1kΩ)
SPK+	Alto-falante
SPK-	Alto-falante
LEDs e Buzzer
Componente	Arduino
LED vermelho	7
LED verde	13
Buzzer	6
🔊 Arquivos de Áudio (OBRIGATÓRIO)

O cartão SD deve conter uma pasta chamada /mp3.

Estrutura correta:
/mp3/
├── 0001.mp3  (Armar bomba)
├── 0002.mp3  (Desarmar)
├── 0003.mp3  (Explosão)
├── 0004.mp3  (CT Win)
└── 0005.mp3  (Terrorist Win)


📌 Importante

Nome EXATO (0001.mp3, etc)

FAT32

Arquivos curtos e claros

Um som por arquivo

⌨️ Controles do Jogo
Tecla	Função
A	Armar bomba
B	Iniciar desarme
C	Alterar código
D	Ajustar volume
0–9	Digitar código
🧠 Estados do Sistema
IDLE → PLANTED → DEFUSING → DEFUSED
                 ↓
              EXPLODED

⚙️ Configurações Importantes no Código
const unsigned long PLANT_COUNTDOWN_MS = 40000;
const byte CODE_LENGTH = 6;
int volumeLevel = 20;


Tempo em milissegundos

Código numérico

Volume de 0 a 30

⚠️ Observações Importantes

Nunca compartilhe pinos entre LCD, Keypad e DFPlayer

DFPlayer RX precisa de resistor

Fonte fraca causa falha no áudio

LCD I2C deve ter pull-ups (normalmente já possui)

🚀 Como Usar

Grave o código no Arduino

Insira o cartão SD no DFPlayer

Alimente com 5V estável

Pressione A para iniciar

📜 Licença

Projeto open-source para fins educacionais e recreativos.

⚠️ Nunca utilize este projeto para simular dispositivos reais perigosos.

👤 Autor

Desenvolvido por Éder Tashiro
Arduino Bomb Game – Hardware Real
