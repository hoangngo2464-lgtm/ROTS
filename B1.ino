#include <Arduino_FreeRTOS.h>
#include <semphr.h>  // Để sử dụng mutex bảo vệ shared variables

/* --- 1. KHAI BÁO CHÂN (Sửa theo sơ đồ mạch) --- */
// Động cơ bước (Nối qua ULN2003, thứ tự IN1-D6, IN2-D5, IN3-D4, IN4-D3)
#define IN1 A1
#define IN2 A0
#define IN3 A2
#define IN4 A3
// Nút nhấn & LED đơn
#define BTN_DIR_PIN 4 // Nút đảo chiều
#define BTN_SPEED_PIN 5 // Nút tốc độ (sửa từ 3 thành 7 theo sơ đồ)
#define LED_SINGLE 3 // LED đơn báo chiều (TX pin, dùng như output, sửa từ 12 thành 1)
// LED 7 Đoạn (a-A0, b-13, c-12, d-11, e-10, f-9, g-8)
const int segPins[] = {6, 7, 8, 9, 10, 11, 12};

/* --- 2. BIẾN TOÀN CỤC (SHARED VARIABLES) --- */
volatile int speedLevel = 1; // Cấp tốc độ: 1, 2, 3, 4 (thêm cấp 4 để tăng tốc độ)
volatile bool isClockwise = true; // True: Thuận (LED sáng), False: Nghịch (LED tắt)
// Mutex để bảo vệ shared variables (từ blog RTOS: mutex cho shared resource)
SemaphoreHandle_t sharedMutex;

/* --- 3. BẢNG MÃ --- */
// Mã LED 7 đoạn (Anode chung: 0 là sáng - LOW) - Chỉ cần số 1, 2, 3, 4
const byte numbers[5][7] = {
  {1,1,1,1,1,1,1}, // 0 (Không dùng)
  {1,0,0,1,1,1,1}, // 1 (bc on)
  {0,0,1,0,0,1,0}, // 2 (abdeg on)
  {0,0,0,0,1,1,0}, // 3 (abcdg on)
  {1,0,0,1,1,0,0}  // 4 (bfg on) - Thêm mã cho số 4
};
// Mã Half-step (8 bước) cho động cơ chạy mịn
// Thứ tự kích: A(IN1) -> AB -> B(IN2) -> BC -> C(IN3) -> CD -> D(IN4) -> DA
// (Nếu chiều quay sai, đảo thứ tự cột hoặc dùng sequence reverse)
const int steps[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

/* --- 4. KHAI BÁO TASKS --- */
void TaskInput( void *pvParameters );
void TaskStepper( void *pvParameters ); // Điều khiển motor
void TaskDisplay( void *pvParameters ); // Hiển thị LED

void setup() {
  // Cấu hình Output
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(LED_SINGLE, OUTPUT);
  for(int i=0; i<7; i++) pinMode(segPins[i], OUTPUT);
  // Cấu hình Input (Sử dụng trở treo nội)
  pinMode(BTN_DIR_PIN, INPUT_PULLUP);
  pinMode(BTN_SPEED_PIN, INPUT_PULLUP);

  // Tạo mutex cho shared variables
  sharedMutex = xSemaphoreCreateMutex();

  // Tạo Task RTOS
  // Task 1: Xử lý nút nhấn (Ưu tiên trung bình)
  xTaskCreate(TaskInput, "Input", 128, NULL, 2, NULL);
  // Task 2: Điều khiển Động cơ (Ưu tiên cao nhất để chạy mượt)
  xTaskCreate(TaskStepper, "Stepper", 128, NULL, 3, NULL);
  // Task 3: Hiển thị LED (Ưu tiên thấp)
  xTaskCreate(TaskDisplay, "Display", 128, NULL, 1, NULL);

  // Bắt đầu scheduler RTOS (sửa: thêm dòng này, thiếu trong code gốc)
  vTaskStartScheduler();
}

void loop() {
  // RTOS quản lý, loop để trống
}

/* --- 5. NỘI DUNG CÁC TASK --- */
// --- TASK 1: ĐỌC NÚT NHẤN ---
void TaskInput(void *pvParameters) {
  int lastDirState = HIGH;
  int lastSpeedState = HIGH;
  for (;;) {
    int currDir = digitalRead(BTN_DIR_PIN);
    int currSpeed = digitalRead(BTN_SPEED_PIN);
    // Xử lý nút đảo chiều (Bắt sườn xuống)
    if (lastDirState == HIGH && currDir == LOW) {
      xSemaphoreTake(sharedMutex, portMAX_DELAY);
      isClockwise = !isClockwise;
      xSemaphoreGive(sharedMutex);
    }
    // Xử lý nút tốc độ (Bắt sườn xuống)
    if (lastSpeedState == HIGH && currSpeed == LOW) {
      xSemaphoreTake(sharedMutex, portMAX_DELAY);
      speedLevel++;
      if (speedLevel > 4) speedLevel = 1; // Thêm cấp 4, vòng lặp đến 4
      xSemaphoreGive(sharedMutex);
    }
    lastDirState = currDir;
    lastSpeedState = currSpeed;
   
    // Delay 50ms để chống rung phím
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

// --- TASK 2: ĐIỀU KHIỂN ĐỘNG CƠ (HALF-STEP) ---
void TaskStepper(void *pvParameters) {
  int stepIndex = 0;
  for (;;) {
    // 1. Xuất tín hiệu ra 4 chân
    digitalWrite(IN1, steps[stepIndex][0]);
    digitalWrite(IN2, steps[stepIndex][1]);
    digitalWrite(IN3, steps[stepIndex][2]);
    digitalWrite(IN4, steps[stepIndex][3]);
    // 2. Tăng/Giảm bước theo chiều quay (đọc an toàn với mutex)
    xSemaphoreTake(sharedMutex, portMAX_DELAY);
    if (isClockwise) {
      stepIndex++;
      if (stepIndex > 7) stepIndex = 0;
    } else {
      stepIndex--;
      if (stepIndex < 0) stepIndex = 7;
    }
    int localSpeed = speedLevel;  // Copy để tránh thay đổi giữa delay
    xSemaphoreGive(sharedMutex);
    // 3. Delay tạo tốc độ (Logic delay bằng tick RTOS, giả sử tick 1ms)
    int delayTicks = 0;
    switch(localSpeed) {
      case 1: delayTicks = 3; break; // Chậm ~10ms/step
      case 2: delayTicks = 2; break;  // Vừa ~6ms/step
      case 3: delayTicks = 1; break;  // Nhanh ~2ms/step
     
    } 
   
    vTaskDelay(delayTicks);
  }
}

// --- TASK 3: HIỂN THỊ LED ---
void TaskDisplay(void *pvParameters) {
  for (;;) {
    xSemaphoreTake(sharedMutex, portMAX_DELAY);
    bool localDir = isClockwise;
    int localSpeed = speedLevel;
    xSemaphoreGive(sharedMutex);
    // 1. Điều khiển LED Đơn: Thuận = Sáng, Nghịch = Tắt
    digitalWrite(LED_SINGLE, localDir ? HIGH : LOW);
    // 2. Điều khiển LED 7 Đoạn: Hiện cấp tốc độ
    for(int i=0; i<7; i++) {
      digitalWrite(segPins[i], numbers[localSpeed][i]);
    }
    // Cập nhật mỗi 100ms
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}