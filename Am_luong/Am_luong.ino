const int buzzerPin = D6; // Chân PWM kết nối với buzzer

void setup() {
  pinMode(buzzerPin, OUTPUT); // Thiết lập chân buzzer là đầu ra
}

void loop() {
  // Tăng âm lượng từ 0 đến 255
  for (int volume = 0; volume <= 255; volume++) {
    analogWrite(buzzerPin, volume); // Ghi giá trị PWM
    delay(100); // Delay 10ms giữa các bước
  }

  // Giảm âm lượng từ 255 về 0
  for (int volume = 255; volume >= 0; volume--) {
    analogWrite(buzzerPin, volume); // Ghi giá trị PWM
    delay(100); // Delay 10ms giữa các bước
  }
}
