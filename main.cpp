#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // Без пина сброса
#define SCREEN_ADDRESS 0x3C  // Адрес I2C для OLED
#define DOWN_BUTTON 1  // Пин для кнопки вниз
#define UP_BUTTON 2    // Пин для кнопки вверх
#define MIDDLE_BUTTON 0 // Пин для кнопки выбора

#define FRAME_DELAY (42)
#define FRAME_WIDTH (48)
#define FRAME_HEIGHT (48)
#define FRAME_COUNT (sizeof(frames) / sizeof(frames[0]))


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Переменные для меню
int currentSelection = 0; // Текущая выбранная строка меню
const int menuItems = 4;  // Количество пунктов меню

// Wi-Fi данные
const char* ssid = "Summator WiFi"; // Замените на имя вашей сети
const char* password = "YaseminBerkman"; // Замените на пароль вашей сети


void setup() {
  // Инициализация I2C на ESP32
  Wire.begin(8, 9); // SDA = GPIO 8, SCK = GPIO 9
  Wire.setClock(100000); // Установим частоту I2C на 100kHz

  // Инициализация дисплея
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Остановить выполнение если дисплей не инициализирован
  } else {
    Serial.println(F("OLED initialized"));
  }

  // Очистка экрана
  display.clearDisplay();

  // Настройка пинов кнопок
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(MIDDLE_BUTTON, INPUT_PULLUP);

  // Подключение к Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  
  sayHello();
  // Отображаем начальное меню
  displayMenu();
}

void loop() {
  // Проверка нажатия кнопки вниз
  if (digitalRead(DOWN_BUTTON) == LOW) {
    currentSelection = (currentSelection + 1) % menuItems; // Цикл по меню
    displayMenu();
    delay(200); // Задержка для предотвращения дребезга кнопок
  }

  // Проверка нажатия кнопки вверх
  if (digitalRead(UP_BUTTON) == LOW) {
    currentSelection = (currentSelection - 1 + menuItems) % menuItems; // Цикл по меню
    displayMenu();
    delay(200); // Задержка для предотвращения дребезга кнопок
  }

  // Проверка нажатия кнопки выбора (MIDDLE_BUTTON)
  if (digitalRead(MIDDLE_BUTTON) == LOW) {
    handleSelection(); // Обрабатываем выбор
    delay(200); // Задержка для предотвращения дребезга кнопок
  }
}

void sayHello(){
    display.clearDisplay(); // Очистка экрана

  // Вывод пунктов меню
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

  // Отображение приветственного сообщения
    display.setCursor(16, 32);
    display.print(F("Welcome to AydOS")); 
    display.display(); // Обновите дисплей для отображения текста
    // Задержка 3 секунды
    delay(3000);

    // Очистка дисплея
    display.clearDisplay();
}

// Функция для отображения меню
void displayMenu() {
  display.clearDisplay(); // Очистка экрана перед обновлением меню

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  if (currentSelection == 0) {
    display.print(F("1)Wifi networks<")); // Пункт выбран
  } else {
    display.print(F("1)Wifi networks"));
  }

  display.setCursor(0, 16);
  if (currentSelection == 1) {
    display.print(F("2)Second option<")); // Пункт выбран
  } else {
    display.print(F("2)Second option"));
  }
  
  display.setCursor(0, 32);
  if (currentSelection == 2) {
    display.print(F("3)Third option<")); // Пункт выбран
  } else {
    display.print(F("3)Third option"));
  }

  display.setCursor(0, 48);
  if (currentSelection == 3) {
    display.print(F("4)Fourth option<")); // Пункт выбран
  } else {
    display.print(F("4)Fourth option"));
  }

  display.display(); // Обновляем экран
}

// Функция для обработки выбора пункта меню
void handleSelection() {
  display.clearDisplay(); // Очистка экрана

  display.setCursor(0, 0); // Устанавливаем курсор в начало

  if (currentSelection == 0) {
    show_wifi();
  }// else if (currentSelection == 1) {
  //}

  display.display(); // Обновляем экран
}
void show_wifi() {
  display.clearDisplay(); // Очистка экрана
  display.setTextSize(1); // Установка размера текста
  display.setTextColor(SSD1306_WHITE); // Установка цвета текста
  display.setCursor(16, 32); // Установка курсора в центр экрана

  // Запускаем анимацию и асинхронное сканирование Wi-Fi сетей
  displayLoading(); 

  // После завершения сканирования
  display.clearDisplay(); // Очищаем экран для вывода результатов
  display.setCursor(0, 0); // Установка курсора в начало

  // Получаем количество найденных сетей
  int n = WiFi.scanComplete(); 
  
  if (n == 0) {
    display.println(F("No networks found")); // Если сетей нет
  } else {
    int currentNetwork = 0; // Текущая отображаемая сеть
    const int maxDisplayNetworks = 4; // Максимальное количество сетей на экране за раз

    while (true) {
      display.clearDisplay(); // Очистка экрана перед обновлением

      display.setCursor(0, 0);
      display.print(F("Networks found: "));
      display.println(n); // Выводим количество найденных сетей
      
      // Отображаем список сетей, начиная с текущей
      for (int i = currentNetwork; i < min(currentNetwork + maxDisplayNetworks, n); i++) {
        display.print(WiFi.SSID(i)); // Имя сети (SSID)
        display.print(F(" ("));
        display.print(WiFi.RSSI(i)); // Уровень сигнала
        display.print(F(" dBm)"));
        display.println();
      }

      display.display(); // Обновляем экран

      // Обрабатываем кнопки
      if (digitalRead(DOWN_BUTTON) == LOW) {
        if (currentNetwork < n - maxDisplayNetworks) {
          currentNetwork++; // Прокрутка вниз, если не достигнут конец списка
        }
        delay(200); // Задержка для предотвращения дребезга кнопок
      }

      if (digitalRead(UP_BUTTON) == LOW) {
        if (currentNetwork > 0) {
          currentNetwork--; // Прокрутка вверх, если не достигнуто начало списка
        }
        delay(200); // Задержка для предотвращения дребезга кнопок
      }

      // Выход из списка сетей по нажатию на MIDDLE_BUTTON и возврат в главное меню
      if (digitalRead(MIDDLE_BUTTON) == LOW) {
        displayMenu(); // Возвращаемся в главное меню
        break; // Прерываем цикл и возвращаемся в главное меню
      }
    }
  }

  display.display(); // Обновляем экран после вывода всех сетей
}


void displayLoading() {
  int n = -1; // Инициализируем переменную для хранения результата сканирования

  // Запускаем асинхронное сканирование Wi-Fi сетей
  WiFi.scanDelete(); // Удаляем предыдущие результаты сканирования
  WiFi.scanNetworks(true); // Асинхронное сканирование

  while (n == -1) { // Пока сканирование не завершено
    static int frame = 0; // Сохранение текущего кадра между вызовами функции
    display.clearDisplay(); // Очистка экрана
    display.setCursor(16, 32); // Центрирование текста
    display.print(F("Loading"));
    for (int i = 0; i <= frame; i++) { // Добавляем точки в зависимости от кадра
      display.print(F("."));
    }
    display.display(); // Обновление экрана
    frame = (frame + 1) % 4; // Цикл кадров (0, 1, 2, 3)

    delay(500); // Задержка между кадрами

    // Проверяем, завершилось ли сканирование
    n = WiFi.scanComplete(); // -1 означает, что сканирование еще не завершено
  }
}
