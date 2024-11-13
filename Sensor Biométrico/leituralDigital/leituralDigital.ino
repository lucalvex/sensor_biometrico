
// INCLUSÃO DAS BIBLIOTECAS
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <PushButton.h>

SoftwareSerial mySerial(3, 2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// DECLARAÇÃO DAS VARIÁVEIS E FUNCOES
bool gravar = true;
bool buttonState = false;
bool lastButtonState = false;

// PINOS 
const int buttonPin = 4;
const int ledPin = 13;

void setup() {

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  
  Serial.begin(57600);
  finger.begin(57600);

  Serial.println("Deu certo");

  if(finger.verifyPassword()){
    
    Serial.println("Sensor biometricoo !");

  } else {

    Serial.println("Sensor biometrico não encontrado! Verifique a conexão e reinicie o sistema");
    
    while(true) { 
      delay(1); 
      }
  }

  Serial.println("Fim do Setup!");
}

void loop() {

  String escolha;
  uint8_t numID;

  buttonState = !digitalRead(buttonPin);

  // botao.waitForButton();

  if (buttonState && !lastButtonState) {

    Serial.print("Digite o ID para gravar a impressão digital: ");

    while (Serial.available() == 0); // Aguarda o valor do ID do usuário
    numID = Serial.parseInt();

    Serial.println("Modo gravação ativado \n");
    uint8_t resultado = modoGravacaoID(numID);

    if (resultado == FINGERPRINT_OK) {
      Serial.println("Impressão digital gava com sucesso!");

    } else {
      Serial.println("Falha na gravação da impressão digital!");
    }

    delay(50);

  }

  lastButtonState = buttonState;
  
  getFingerprintIDez();
  
}

uint8_t modoGravacaoID(uint8_t IDgravar) {

  int p = -1;

  Serial.print("Esperando uma leitura válida para gravar #");
  delay(5000);
  Serial.println(IDgravar);
  

  // Primeira etapa: Captura da digital
  while (p != FINGERPRINT_OK) {

    p = finger.getImage();

    switch (p) {
      
      // Valor da dedo foi capturado bem sucessido!
      case FINGERPRINT_OK:
        Serial.println("Leitura concluída");
        break;

      // Indica que não foi capturado nenhum valor
      case FINGERPRINT_NOFINGER:
        Serial.println("Nenhum valor capturado");
        delay(200);
        break;

      // Significa que o valor já é existente e houver um erro na comunicação com o arduino
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Erro comunicação");
        break;

      // Erro na captura da imagem
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Erro de leitura");
        break;

      default:
        Serial.println("Erro desconhecido");
        break;
      }
    
    delay(5000);
  }

  // Segunda etapa: Verificação e conversão da primeira Imagem
  p = finger.image2Tz(1);

  switch (p) {

    case FINGERPRINT_OK:
      Serial.println("Leitura convertida");
      break;

    case FINGERPRINT_IMAGEMESS:
      Serial.println("Leitura suja");
      return p;

    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de comunicação");
      return p;

    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Não foi possível encontrar propriedade da digital");
      return p;

    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Não foi possível encontrar propriedade da digital");
      return p;

    default:
      Serial.println("Erro desconhecido");
      return p;
  }
  
  Serial.println("Remova o dedo");
  delay(5000);

  // Este loop nos garante que o usuário já removeu o dedo do sensor, senão ele continuará preso no loop
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
    delay(5000);
  }


  // Terceiro etapa: Realiza uma segunda leitura do mesmo dedo 
  Serial.print("ID "); 
  Serial.println(IDgravar);
  Serial.println("Coloque o Mesmo dedo novamente");

  delay(5000);

  p = -1;
  while (p != FINGERPRINT_OK) {

    p = finger.getImage();
    switch (p) {

      case FINGERPRINT_OK:
        Serial.println("Leitura concluída");
        break;

      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        delay(200);
        break;

      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Erro de comunicação");
        break;

      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Erro de Leitura");
        break;

      default:
        Serial.println("Erro desconhecido");
        break;
      }
    
    delay(5000);
  }

  Serial.println("Remova o dedo");
  delay(5000);

  // Quarta etapa: Verificação e conversão da segunda imagem
  p = finger.image2Tz(2);

  switch (p) {

    case FINGERPRINT_OK:
      Serial.println("Leitura convertida");
      break;

    case FINGERPRINT_IMAGEMESS:
      Serial.println("Leitura suja");
      return p;

    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de comunicação");
      return p;

    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Não foi possível encontrar as propriedades da digital");
      return p;

    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Não foi possível encontrar as propriedades da digital");
      return p;

    default:
      Serial.println("Erro desconhecido");
      return p;
  }
  
  // OK convertido!
  // Quinta etapa: Criando o modelo
  Serial.print("Criando modelo para #");  Serial.println(IDgravar);
  delay(5000);
  
  p = finger.createModel();

  if (p == FINGERPRINT_OK) {
    Serial.println("As digitais batem!");

  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Erro de comunicação");
    return p;

  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("As digitais não batem");
    return p;

  } else {
    Serial.println("Erro desconhecido");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(IDgravar);
  p = finger.storeModel(IDgravar);

  if (p == FINGERPRINT_OK) {
    Serial.println("Armazenado!");

  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Erro de comunicação");
    return p;

  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Não foi possível gravar neste local da memória");
    return p;

  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Erro durante escrita na memória flash");
    return p;

  } else {
    Serial.println("Erro desconhecido");
    return p;
  }  

  return FINGERPRINT_OK;
}

int getFingerprintIDez() {

  uint8_t p = finger.getImage();

  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  //  digitalWrite(pinTrava, LOW);
  Serial.print("ID encontrado #"); 
  Serial.print(finger.fingerID); 
  Serial.print(" com confiança de "); 
  Serial.println(finger.confidence);
  delay(500);

  digitalWrite(ledPin, HIGH);

  delay(200);

  digitalWrite(ledPin, LOW);

  return finger.fingerID;

}


