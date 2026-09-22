/*********
  EKP-CV01c v2026
  ESP32+OLED1306
  Zobrazuje na displeji OLED text. Tento program slouží k demonstraci práce s malým grafickým monochromatickým OLED displejem řízeným čipem SSD1306 (o rozlišení 128×64 bodů). 
  Postupně na něm ukazuje vykreslování znaků, různé velikosti textu, inverzní zobrazení, čísla a hardwarový posun (skrolování) textu.
  zdroj: https://randomnerdtutorials.com/esp32-ssd1306-oled-display-arduino-ide/
*********/
#include <Arduino.h>
/*********
  EKP-CV01d v2026
  ESP32+BME280+OLED1306
  Zobrazuje udaje ze senzoru BME280 na displeji OLED
*********/
// Všechno mezi lomítky je víceřádkový komentář. Mikrokontrolér tyto řádky úplně ignoruje; slouží pouze pro člověka jako poznámka o autorovi, verzi nebo zapojení.*/

// Příkaz #include připojuje externí knihovny (předpřipravené balíčky kódu):
#include <Wire.h> // Umožňuje komunikaci po sběrnici I2C (dvoudrátové rozhraní, přes které je senzor zapojen).
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h> // voláme všechny knihovny z příkladů CV01b a CV01c

#define SCREEN_WIDTH 128 // OLED display sirka, v bodech
#define SCREEN_HEIGHT 64 // OLED display vyska, v bodech

// Nastaveni parametru pro displej SSD1306 pripojeny pres I2C (SDA, SCL piny)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Adafruit_BME280 bme; // I2C

void setup() {
  Serial.begin(115200);

  bme.begin(0x76); // inicializace bme280

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 nepripojen nebo nemohu najit BME280"));
    for (;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
}

void loop() {
  delay(5000);  //opakuj mereni po 5 sekundach

  // Čti teplotu a vlhkost z BME280 a ulož do proměnných
  // Datový typ pro reálná čísla s plovoucí desetinnou čárkou (např. 23.45). 
  // V minulých příkladech se funkce senzoru volaly přímo do tisku; 
  // zde se hodnoty nejdříve uloží do proměnných t (teplota), h (vlhkost) a p (tlak), abychom s nimi mohli dále pracovat a zkontrolovat je.

  float t = bme.readTemperature();
  float h = bme.readHumidity();
  float p = bme.readPressure();
  if (isnan(t) || isnan(h) || isnan(p)) { // Kontrola platnosti naměřených dat
  // isnan(...): Funkce znamenající „Is Not a Number“ (není číslo). Zjišťuje, zda se čtení nezdařilo a senzor nevrátil neplatnou hodnotu.
  // ||: Logický operátor NEBO (OR). Podmínka platí, pokud je neplatná teplota, nebo vlhkost, nebo tlak.
  // Pokud ke chybě dojde, program vypíše varování do sériového monitoru.
    Serial.println("Chyba při čtení z BME280!");
  }

  int tlak = p / 100; // prepocet tlaku na hPa
  // Hodnota v pascalech p se vydělí stem, čímž vznikne tlak v hektopascalech (hPa) zaokrouhlený dolů na celé číslo.

  // vymaz displej
  display.clearDisplay();

  // zobraz teplotu
  display.setTextSize(1);     // nastav velikost pisma
  display.setCursor(0, 0);    // zacni psat od pozice 0,0
  display.print("Teplota: "); // vypis text
  display.setTextSize(2);
  display.setCursor(55, 0);
  display.print(t, 1);        // vypis obsah promenne t, jedno desetinne misto
  display.setTextSize(1);
  display.print(" ");
  display.cp437(true);        // Zapíná podporu kódové stránky Code Page 437 (původní znaková sada starých IBM PC). 
                              //To umožní používat speciální znaky a symboly, které nejsou v běžné základní ASCII tabulce.
  display.write(167);         // Vytiskne jeden konkrétní bajt/znak s číselným kódem 167 (0xA7), což je v tabulce CP437 právě symbol kroužku pro stupeň (°). 
                              // Spolu s následným C vytvoří značku °C.
  display.setTextSize(2);
  display.print("C");

  // zobraz vlhkost
  display.setTextSize(1);
  display.setCursor(0, 23);   // Vertikální souřadnice Y (0, 23, 45) rozdělují 64 pixelů vysoký displej na 3 přehledné řádky s mezerami.
  display.print("Vlhkost: ");
  display.setTextSize(2);
  display.setCursor(55, 23);  // Horizontální posun na X = 55 slouží jako odsazení: vlevo (od pozice 0) je malý popis popisku (Teplota:, Vlhkost:, Tlak:), 
                              //zatímco na pozici 55 začíná samotná hodnota vypsaná větším písmem.
  display.print(h, 1);
  display.print(" %");

  // zobraz tlak
  display.setTextSize(1);
  display.setCursor(0, 45);
  display.print("Tlak: ");
  display.setTextSize(2);
  display.setCursor(55, 45);
  display.print(tlak);
  display.setTextSize(1);
  display.print(" hPa");

  display.display();
}
