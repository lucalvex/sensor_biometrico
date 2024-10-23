#include <Adafruit_Fingerprint.h>

// Configurações da comunicação Serial ------------------------
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(4, 2); // mySerial(RX, TX)
// RX - Recepção
// TX - Transmissão
#define mySerial Serial
#endif

// Criação do objeto do sensor  ----------------------------
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id; // Variavel global

void setup()
{

  Serial.begin(9600);
  delay(100);
  Serial.println("\n\nRegistro do sensor de Impressão Digital");

  // Inicializaçao do Sensor -------------------------
  // Defina a tava de dados a porta serial do sensor
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Sensor de impressão digital encontrado!");
  } else {
    Serial.println("Não encontrei o sensor de impressão digital :(");
    while (1) { delay(1); }
  }

  // Leitura de parâmetro do sensor ---------------------------------

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
}

// Está função aguarda a entrada do usuário e lê um número inteiro a partir
// da comunicação serial
uint8_t readnumber(void) {

  uint8_t num = 0;

  while (num == 0) {

    while (! Serial.available());
    num = Serial.parseInt();

  }

  return num;
}

void loop()                     // run over and over again
{

  Serial.println("Pronto para registrar uma impressão digital!");
  Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");

  id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
     return;
  }

  Serial.print("Enrolling ID #");
  Serial.println(id);

  while (! getFingerprintEnroll() );
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);

  while (p != FINGERPRINT_OK) {

    p = finger.getImage();

    switch (p) {

    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;

    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;

    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;

    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;

    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);

  switch (p) {

    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;

    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;

    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;

    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;

    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;

    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove finger");
  delay(2000);
  p = 0;

  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }

  Serial.print("ID "); Serial.println(id);
  p = -1;

  Serial.println("Place same finger again");

  while (p != FINGERPRINT_OK) {

    p = finger.getImage();
    switch (p) {

    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;

    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;

    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;

    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;

    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);

  switch (p) {

    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;

    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;

    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;

    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;

    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;

    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);

  p = finger.createModel();

  if (p == FINGERPRINT_OK) {

    Serial.println("Prints matched!");

  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {

    Serial.println("Communication error");
    return p;

  } else if (p == FINGERPRINT_ENROLLMISMATCH) {

    Serial.println("Fingerprints did not match");
    return p;

  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(id);

  p = finger.storeModel(id);

  if (p == FINGERPRINT_OK) {

    Serial.println("Stored!");

  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;

  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;

  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;

  } else {
    Serial.println("Unknown error");
    return p;
  }

  return true;
}
