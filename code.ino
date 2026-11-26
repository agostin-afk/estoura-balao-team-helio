#include "BluetoothSerial.h"
#include "esp_system.h"

BluetoothSerial SerialBT;

// ---------- PINOS (iguais ao código original) ----------
const int IN1 = 25;
const int IN2 = 26;
const int IN3 = 27;
const int IN4 = 14;
const int LED_PLACA = 2;

const int freq = 5000;
const int res = 8;

// ---------- ESTADO DA CONEXAO ----------
// Estas flags são ligadas pelo callback do Bluetooth e tratadas no loop()
volatile bool eventoConectou = false;
volatile bool eventoDesconectou = false;
bool conectado = false;

// ---------- MANOBRAS SEM delay() ----------
enum Manobra { NENHUMA, FLANQUEAR_1, FLANQUEAR_2, GIRAR_90, MEIA_VOLTA };
Manobra manobraAtual = NENHUMA;
unsigned long manobraInicio = 0;

// ---------- PWM compativel com core ESP32 2.x e 3.x ----------
void pwmInicia(int pino, int canal) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(pino, freq, res);
#else
  ledcSetup(canal, freq, res);
  ledcAttachPin(pino, canal);
#endif
}

void pwmEscreve(int pino, int valor) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(pino, valor);
#else
  int canal = (pino == IN1) ? 0 : (pino == IN2) ? 1 : (pino == IN3) ? 2 : 3;
  ledcWrite(canal, valor);
#endif
}

// ---------- CALLBACK DO BLUETOOTH ----------
void aoEventoBluetooth(esp_spp_cb_event_t evento, esp_spp_cb_param_t *param) {
  if (evento == ESP_SPP_SRV_OPEN_EVT) {
    eventoConectou = true;
  } else if (evento == ESP_SPP_CLOSE_EVT) {
    eventoDesconectou = true;
  }
}

void mostrarMotivoReset() {
  esp_reset_reason_t motivo = esp_reset_reason();
  Serial.print("Motivo do ultimo reset: ");
  switch (motivo) {
    case ESP_RST_POWERON:  Serial.println("ligou na energia (normal)"); break;
    case ESP_RST_SW:       Serial.println("reset por software"); break;
    case ESP_RST_PANIC:    Serial.println("ERRO NO CODIGO (panic)"); break;
    case ESP_RST_BROWNOUT: Serial.println("QUEDA DE TENSAO (brownout) - problema de energia!"); break;
    case ESP_RST_WDT:
    case ESP_RST_INT_WDT:
    case ESP_RST_TASK_WDT: Serial.println("watchdog (programa travou)"); break;
    default:               Serial.println((int)motivo); break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);
  pinMode(LED_PLACA, OUTPUT);
  mostrarMotivoReset();

  pwmInicia(IN1, 0);
  pwmInicia(IN2, 1);
  pwmInicia(IN3, 2);
  pwmInicia(IN4, 3);
  parar(false);

  SerialBT.register_callback(aoEventoBluetooth);
  if (!SerialBT.begin("Carrinho_ESP32")) {
    Serial.println("[ERRO] Falha ao iniciar o Bluetooth!");
    while (true) delay(1000);
  }

  Serial.println("=========================================");
  Serial.println("   SISTEMA PRONTO - AGUARDANDO APP BT    ");
  Serial.println("   Nome do dispositivo: Carrinho_ESP32   ");
  Serial.println("=========================================");
}

void loop() {
  if (eventoConectou) {
    eventoConectou = false;
    conectado = true;
    Serial.println("\n>>> [STATUS] CELULAR CONECTADO VIA BLUETOOTH! <<<");
    digitalWrite(LED_PLACA, HIGH);
  }

  if (eventoDesconectou) {
    eventoDesconectou = false;
    conectado = false;
    Serial.println("\n>>> [STATUS] CELULAR DESCONECTADO! <<<");
    digitalWrite(LED_PLACA, LOW);
    manobraAtual = NENHUMA;
    parar(true);
  }

  atualizarManobra();

  while (SerialBT.available()) {
    char cmd = SerialBT.read();
    digitalWrite(LED_PLACA, LOW);
    executarComando(cmd);
    if (conectado) digitalWrite(LED_PLACA, HIGH);
  }
}

void iniciarManobra(Manobra m) {
  manobraAtual = m;
  manobraInicio = millis();
}

// Controla o tempo das manobras sem travar o programa
void atualizarManobra() {
  if (manobraAtual == NENHUMA) return;
  unsigned long dt = millis() - manobraInicio;

  switch (manobraAtual) {
    case FLANQUEAR_1:
      if (dt >= 450) {
        Serial.println("   Passo 2: Fechando arco atras do oponente...");
        moverMotores(110, 255);
        iniciarManobra(FLANQUEAR_2);
      }
      break;

    case FLANQUEAR_2:
      if (dt >= 400) {
        Serial.println("   Passo 3: Concluido.");
        manobraAtual = NENHUMA;
        parar(true);
      }
      break;

    case GIRAR_90:
      if (dt >= 300) {
        manobraAtual = NENHUMA;
        parar(true);
      }
      break;

    case MEIA_VOLTA:
      if (dt >= 580) {
        manobraAtual = NENHUMA;
        parar(true);
      }
      break;

    default:
      break;
  }
}

void executarComando(char cmd) {
  if (cmd == '\n' || cmd == '\r') return;

  // Qualquer comando novo cancela uma manobra em andamento (o 'S' agora funciona sempre)
  manobraAtual = NENHUMA;

  Serial.print("[CMD: ");
  Serial.print(cmd);
  Serial.print("] -> ");

  switch (cmd) {
    // --- PILOTAGEM MANUAL ---
    case 'F':
      Serial.println("FRENTE (Precisao)");
      moverMotores(210, 210);
      break;

    case 'B':
      Serial.println("RE (Recuar)");
      moverMotores(-210, -210);
      break;

    case 'L':
      Serial.println("GIRAR ESQUERDA (No eixo)");
      moverMotores(-190, 190);
      break;

    case 'R':
      Serial.println("GIRAR DIREITA (No eixo)");
      moverMotores(190, -190);
      break;

    case 'S':
      Serial.println("PARAR (Freio Eletromagnetico)");
      parar(true);
      break;

    // --- MANOBRAS DE COMBATE ---
    case 'T':
      Serial.println("TURBO / INVESTIDA (100% Potencia!)");
      moverMotores(255, 255);
      break;

    case 'D':
      Serial.println("MANOBRA: FLANQUEAR (Pegar Costas)");
      Serial.println("   Passo 1: Abrindo arco pela direita...");
      moverMotores(255, 110);
      iniciarManobra(FLANQUEAR_1);
      break;

    case 'G':
      Serial.println("MANOBRA: GIRAR 90 GRAUS");
      moverMotores(230, -230);
      iniciarManobra(GIRAR_90);
      break;

    case 'U':
      Serial.println("MANOBRA: MEIA-VOLTA 180 GRAUS");
      moverMotores(240, -240);
      iniciarManobra(MEIA_VOLTA);
      break;

    case 'C':
      Serial.println("MANOBRA: ANDAR EM CIRCULO PEQUENO");
      moverMotores(255, 85);
      break;

    default:
      Serial.println("COMANDO DESCONHECIDO");
      break;
  }
}

void moverMotores(int velEsq, int velDir) {
  Serial.print("   PWM Aplicado -> Motor Esq: ");
  Serial.print(velEsq);
  Serial.print(" | Motor Dir: ");
  Serial.println(velDir);

  if (velEsq >= 0) {
    pwmEscreve(IN1, velEsq);
    pwmEscreve(IN2, 0);
  } else {
    pwmEscreve(IN1, 0);
    pwmEscreve(IN2, -velEsq);
  }

  if (velDir >= 0) {
    pwmEscreve(IN3, velDir);
    pwmEscreve(IN4, 0);
  } else {
    pwmEscreve(IN3, 0);
    pwmEscreve(IN4, -velDir);
  }
}

void parar(bool imprimirLog) {
  if (imprimirLog) {
    Serial.println("   PWM Aplicado -> FREIO TRAVADO (IN1..IN4 = 255)");
  }
  pwmEscreve(IN1, 255);
  pwmEscreve(IN2, 255);
  pwmEscreve(IN3, 255);
  pwmEscreve(IN4, 255);
}
