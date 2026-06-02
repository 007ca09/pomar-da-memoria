/*
 * ============================================================
 *  TESTE DE FRUTAS — 1 leitor, 2 objetos (cartão + tag)
 * ============================================================
 *  Conexões:
 *    SDA  → D10
 *    SCK  → D13
 *    MOSI → D11
 *    MISO → D12
 *    RST  → D9
 *    3.3V → 3.3V
 *    GND  → GND
 *
 * ============================================================
 */

#include <SPI.h>
#include <MFRC522.h>
#include <string.h>

#define SS_PIN 10
#define RST_PIN 9

//constantes globais
const byte FRUIT_COUNT = 5;
const byte SEQUENCE_SIZE = 3;
const byte PUSH_BUTTON_PIN = 2;

MFRC522 rfid(SS_PIN, RST_PIN);

//frutas e seus respectivos UIDs
const char* fruit_uids[FRUIT_COUNT] = {
  "D7A22506",
  "3BBCD411",
  "4B790A11",
  "13BA2506",
  "BA9A2506"
};

//nomes das frutas
const char* fruit_names[FRUIT_COUNT] = {
  "Banana",
  "Abacaxi",
  "Maçã",
  "Pera",
  "Morango"
};

//mesmo elementos de fruit_names porém aqui será usado para embaralhar o array
char shuffled_fruits[FRUIT_COUNT][12] = {
  "Banana",
  "Abacaxi",
  "Maçã",
  "Pera",
  "Morango"
};


char detected_fruit[12] = "";

byte sequence_index = 0;
bool previous_button_state = HIGH;

void read_uid(char* uid_buffer) {

  uid_buffer[0] = '\0';

  for (byte i = 0; i < rfid.uid.size; i++) {

    char temp[3];

    sprintf(temp, "%02X", rfid.uid.uidByte[i]);

    strcat(uid_buffer, temp);
  }
}

const char* get_fruit_by_uid(const char* uid) {

  for (byte i = 0; i < FRUIT_COUNT; i++) {

    if (strcmp(uid, fruit_uids[i]) == 0) {
      return fruit_names[i];
    }
  }

  return nullptr;
}

void show_sequence() {

  Serial.println("\nsequência: ");

  for (byte i = 0; i < SEQUENCE_SIZE; i++) {
    Serial.println(shuffled_fruits[i]);
  }

  Serial.println();
}

//faz o embaralhamento das frutas
void fisher_yates_shuffle() {

  for (int i = FRUIT_COUNT - 1; i > 0; i--) {

    int j = random(i + 1);

    char temp[12];

    strcpy(temp, shuffled_fruits[i]);
    strcpy(shuffled_fruits[i], shuffled_fruits[j]);
    strcpy(shuffled_fruits[j], temp);
  }
}

//verifica se a fruta lida é a mesma da sequência atual
void check_win() {

  if (strcmp(detected_fruit, shuffled_fruits[sequence_index]) == 0) {

    Serial.println("voce acertou!");
    sequence_index++;
    //se o numero de sequencia atual for igual a 3 então o jogador ganhou
    if (sequence_index >= SEQUENCE_SIZE) {

      Serial.println("voce completou a sequencia!");
      sequence_index = 0;
    }
  }
  else {

    Serial.println("voce errou!");
    sequence_index = 0;
  }
}

void setup() {

  Serial.begin(9600);

  SPI.begin();
  rfid.PCD_Init();

  pinMode(PUSH_BUTTON_PIN, INPUT_PULLUP);

  randomSeed(analogRead(0));

}


void loop() {

  bool current_button_state = digitalRead(PUSH_BUTTON_PIN);

  
  if (previous_button_state == HIGH && current_button_state == LOW) {

    sequence_index = 0;

    fisher_yates_shuffle();
    show_sequence();
  }

  previous_button_state = current_button_state;

  
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  char uid[20];

  read_uid(uid);

  const char* fruit = get_fruit_by_uid(uid);

  if (fruit != nullptr) {

    strcpy(detected_fruit, fruit);

    Serial.print("fruta: ");
    Serial.println(detected_fruit);

    check_win();
  }
  else {

    Serial.print("UID nao cadastrado: ");
    Serial.println(uid);
  }

  Serial.println("-----------------------");

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(500);
 
}