/******************************************
 * ESP32 SPI EEPROM Reader/Writer
 * @brief Reads and writes data to 25AA040A/25LC040A EEPROM using SPI
 ******************************************/

 #include <Arduino.h>
 #include <SPI.h>
 #include <Wire.h>
 
 #define CS_PIN 10
 #define MOSI_PIN 11
 #define SCK_PIN 12
 #define MISO_PIN 13
 #define WP_PIN 14
 #define HOLD_PIN 48
 
 #define EEPROM_READ  0x03
 #define EEPROM_WRITE 0x02
 #define EEPROM_WRDI  0x04
 #define EEPROM_WREN  0x06
 #define EEPROM_RDSR  0x05
 #define EEPROM_WRSR  0x01
 
 void eepromWriteEnable();
 uint8_t eepromReadStatus();
 void eepromWrite(uint16_t addr, uint8_t data);
 void eepromWriteString(uint16_t startAddr, const char* str);
 uint8_t eepromRead(uint16_t addr);
 void eepromReadAll();
 
 void setup() {
     Serial.begin(115200);
     SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
     pinMode(CS_PIN, OUTPUT);
     pinMode(HOLD_PIN, OUTPUT);
     pinMode(WP_PIN, OUTPUT);
     digitalWrite(CS_PIN, HIGH);
     digitalWrite(HOLD_PIN, HIGH);
     digitalWrite(WP_PIN, LOW);
     
     delay(1000);
     Serial.println("EEPROM Reader/Writer");
     
    //  digitalWrite(WP_PIN, HIGH);
    //  eepromWriteString(0x0A9, "Configure it as an input and read the HAL_GPIO_ReadPin, but I prefer HAL_GPIO_EXTI_Callback using interrupts."); // 108 bytes
    //  digitalWrite(WP_PIN, LOW);
 }
 
 void loop() {
     Serial.println("Looping...");
     eepromReadAll();
     delay(10000);
 }
 
 void eepromWriteEnable() {
     digitalWrite(CS_PIN, LOW);
     SPI.transfer(EEPROM_WREN);
     digitalWrite(CS_PIN, HIGH);
     delay(10);
 }
 
 uint8_t eepromReadStatus() {
     digitalWrite(CS_PIN, LOW);
     SPI.transfer(EEPROM_RDSR);
     uint8_t status = SPI.transfer(0x00);
     digitalWrite(CS_PIN, HIGH);
     return status;
 }

 uint8_t eepromWriteStatus() {
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(EEPROM_WRSR);
  uint8_t status = SPI.transfer(0x00);
  digitalWrite(CS_PIN, HIGH);
  return status;
}
 
 void eepromWrite(uint16_t addr, uint8_t data) {
  eepromWriteEnable(); 
  digitalWrite(CS_PIN, LOW);
  digitalWrite(WP_PIN, HIGH);
  delay(1);
  SPI.transfer(EEPROM_WRITE | (((addr >> 8) & 0x01) << 3));
  SPI.transfer((uint8_t)(addr & 0xFF));
  SPI.transfer(data);
  
  digitalWrite(CS_PIN, HIGH);
  delay(1);
  // Wait for write to complete
  uint8_t status;
  do {
      status = eepromWriteStatus();
  } while (status & 0x01); // Check WIP bit
  
  delay(5);
  digitalWrite(WP_PIN, LOW);
}
 
 void eepromWriteString(uint16_t startAddr, const char* str) {
    
  while (*str && startAddr < 0x200) {
         eepromWrite(startAddr++, (uint8_t)(*str++));
         delay(10);
     }
 }
 
 uint8_t eepromRead(uint16_t addr) {
     digitalWrite(CS_PIN, LOW);
     delay(0.1);
     SPI.transfer(EEPROM_READ | ((addr >> 8) & 0x01));
     SPI.transfer((uint8_t)(addr & 0xFF));
     uint8_t data = SPI.transfer(0x00);
     digitalWrite(CS_PIN, HIGH);
     delay(0.1);
     return data;
 }
 
 void eepromReadAll() {
     Serial.println("Reading entire EEPROM:");
     char eepromData[513]; // 512 bytes + null terminator
     memset(eepromData, 0, sizeof(eepromData));
     
    //  digitalWrite(CS_PIN, LOW);
     for (uint16_t addr = 0; addr < 0x200; addr++) {
         uint8_t data = eepromRead(addr);
         eepromData[addr] = data ? (char)data : ' ';
     }
    //  digitalWrite(CS_PIN, HIGH);

    //  eepromWriteString(0x1F4, "ID:ES321-326");
    // eepromWriteString(0x0A9, "Configure it as an input and read the HAL_GPIO_ReadPin, but I prefer HAL_GPIO_EXTI_Callback using interrupts."); // 108 bytes

     
     Serial.println("EEPROM Content:");
     Serial.println(eepromData);
 }
 