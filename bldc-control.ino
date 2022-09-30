#define HALL_PIN 2 // 定義霍爾元件感測腳位為 GPIO2
#define PWM_PIN 3  // 定義PWM訊號輸出腳位為 GPIO3 (供應給BLDC控制板，輸出範圍0~5V)

// 設置霍爾元件觸發次數，影響RPM的數值（更高提高準確性）
float hall_thresh = 100.0;

// 初始化執行一次
void setup()
{
  // 初始化序列阜通訊速率為 115200
  Serial.begin(115200);

  // 設定 HALL_PIN 腳位為輸入腳位
  pinMode(HALL_PIN, INPUT);

  // 設定 PWM_PIN 腳位為輸出腳位
  pinMode(PWM_PIN, OUTPUT);
}

// 持續重複執行
void loop()
{
  // 計數預設值
  float hall_count = 1.0;

  // 取得開始計數前系統時間，單位為微秒
  float start = micros();

  // 表示霍爾元件的觸發狀態, true 表示真，false 表示假
  bool on_state = false;

  // 當霍爾元件被觸發時計數
  // 但若還在觸發至狀態中則不計數
  // 在 break 前，此while迴圈會不斷執行
  while (true)
  {
    // 當磁鐵靠近霍爾元件，元件電位被拉低時執行內部程式
    if (digitalRead(HALL_PIN) == 0)
    {
      // 當霍爾元件觸發狀態為 false，將狀態設為 true 並計數+1
      if (on_state == false)
      {
        on_state = true;
        hall_count += 1.0;
      }
    }
    else
    {
      // 當磁鐵遠離霍爾元件時，觸發狀態設為 false ，表示磁鐵未接觸霍爾元件
      on_state = false;
    }

    // 當計數達設定的閾值時呼叫 break 跳出 while 迴圈
    if (hall_count >= hall_thresh)
    {
      break;
    }
  }

  // 取得計數完畢後的系統時間
  float end_time = micros();

  // 計算經過時間，單位為秒
  float time_passed = ((end_time - start) / 1000000.0);

  // 在 SerialMonitor 中印出文字與數值
  Serial.print("Time Passed: ");
  Serial.print(time_passed);
  Serial.println("s");

  // 計算 RPM(Revolution Per Minute) ，即每分鐘轉幾圈
  float rpm_val = (hall_count / time_passed) * 60.0;

  Serial.print(rpm_val);
  Serial.println(" RPM");

  // 建立算式，根據 RPM 的值來改變 PWM 輸出，這邊提供簡單的算式，實際須照實驗中調整算式
  // 需要將 RPM 值換算成 0~1023 範圍的 PWM 輸出，這邊假設最高 RPM 為 1000，所以當 RPM 為 500 時， PWM 輸出 511 ，在 0~5V 的電壓中約為 2.5V
  int output = rpm_val * 1.023;

  // 輸出類比訊號至 PWM 腳位，供應給 BLDC 控制板
  analogWrite(PWM_PIN, output);

  // 等待 1 毫秒持續執行程式
  delay(1);
}