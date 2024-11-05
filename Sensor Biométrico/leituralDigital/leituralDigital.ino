/*
 * Q0000
 * AUTOR:   BrincandoComIdeias
 * LINK:    https://www.youtube.com/brincandocomideias ; https://cursodearduino.net/
 * SKETCH:  Leitorbiométrico
 * DATA:    21/11/2018
 */

// INCLUSÃO DAS BIBLIOTECAS
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <Pushbutton.h>

// // DEFINIÇÃO DO PINO DO BOTÃO
// #define pinBot 11

// DEFINIÇÃO DO PINO DA TRAVA
// #define pinTrava 4

// SoftwareSerial mySerial(3, 2);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial);

// Pushbutton botao(pinBot);

// DECLARAÇÃO DAS VARIÁVEIS E FUNCOES
bool gravar = true;

// uint8_t modoGravacaoID(uint8_t IDgravar);

void setup() {
  
  // pinMode(pinTrava, OUTPUT);
  // digitalWrite(pinTrava, HIGH);
  
  Serial.begin(57600);
  finger.begin(57600);

  Serial.println("Deu certo");

  if(finger.verifyPassword()){
    
    Serial.println("Sensor biometrico !");

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

  // botao.waitForButton();

  // if (botao.isPressed()){
  //   gravar = true;
  // }

  // if(gravar){

  //   Serial.println("Modo gravação\n");
  //   modoGravacaoID(0);
  //   gravar = false;
  // }

  Serial.println("Deseja gravar uma nova impressão digital (s/n): ");

  // Este while faz que ele aguarde até que o usuário insira algum valor
  // Usamos isso quando precisamos aguardar até o usuário insira alguma valor
  while (Serial.available() == 0) {
    Serial.println("Esta preso aqui");
    delay(2000);
  };

  escolha = Serial.readString();
  Serial.print("Você digitou: "); // Imprime uma mensagem de retorno
  Serial.println(escolha); // Exibe o número lido

  // Limpa o buffer do Serial
  while (Serial.available() > 0);
  Serial.read();

  if (escolha == 's' || escolha == 'S') {

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

  } else if (escolha == 'n' || escolha == 'N') {
    Serial.println(" Encerrando programa. ");

    while(true); // Para o loop principal, encerrando o programa
  }

  // getFingerprintIDez();
  
}

uint8_t modoGravacaoID(uint8_t IDgravar) {

  int p = -1;

  Serial.print("ISssso Esperando uma leitura válida para gravar #");
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

// int getFingerprintIDez() {
//   uint8_t p = finger.getImage();
//   if (p != FINGERPRINT_OK)  return -1;

//   p = finger.image2Tz();
//   if (p != FINGERPRINT_OK)  return -1;

//   p = finger.fingerFastSearch();
//   if (p != FINGERPRINT_OK)  return -1;
  
//   //Encontrou uma digital!
//   if (finger.fingerID == 0) {
//      Serial.print("Modo Administrador!");
     
//      numID++;
//      modoGravacaoID(numID);
//      return 0; 
  
//   } else {

//     //  digitalWrite(pinTrava, LOW);
//      Serial.print("ID encontrado #"); Serial.print(finger.fingerID); 
//      Serial.print(" com confiança de "); Serial.println(finger.confidence);
//      delay(500);
//     //  digitalWrite(pinTrava, HIGH);
//      return finger.fingerID;
//   } 
// }



