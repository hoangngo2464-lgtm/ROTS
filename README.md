# ROTS

## Giới thiệu

ROTS là dự án thực hành lập trình hệ thống nhúng sử dụng hệ điều hành thời gian thực (Real-Time Operating System – RTOS) trên nền tảng Arduino, kết hợp mô phỏng mạch điện bằng Proteus.

Dự án tập trung vào việc tìm hiểu cách tổ chức chương trình thành nhiều tác vụ độc lập, quản lý việc thực thi các tác vụ và đồng bộ dữ liệu giữa chúng. Thông qua các bài tập thực hành, dự án áp dụng RTOS vào điều khiển thiết bị ngoại vi và xử lý các sự kiện đầu vào.

## Mục tiêu

* Tìm hiểu nguyên lý hoạt động của hệ điều hành thời gian thực RTOS.
* Làm quen với cơ chế tạo, lập lịch và quản lý các tác vụ.
* Thực hành lập trình Arduino với FreeRTOS.
* Tìm hiểu cơ chế đồng bộ dữ liệu giữa các tác vụ bằng Mutex.
* Ứng dụng RTOS trong điều khiển động cơ và các thiết bị ngoại vi.
* Mô phỏng, kiểm tra và đánh giá hoạt động của chương trình trên Proteus.

## Công nghệ sử dụng

* Vi điều khiển: Arduino Uno.
* Ngôn ngữ lập trình: C/C++.
* Hệ điều hành thời gian thực: FreeRTOS.
* Môi trường lập trình: Arduino IDE.
* Phần mềm mô phỏng: Proteus.

## Nội dung dự án

### Bài tập 1: Điều khiển động cơ bước bằng RTOS

Xây dựng chương trình điều khiển động cơ bước bằng Arduino, sử dụng FreeRTOS để phân chia chương trình thành các tác vụ riêng biệt.

Các chức năng chính:

* Đọc trạng thái nút nhấn để thay đổi chiều quay và tốc độ động cơ.
* Điều khiển trình tự kích động cơ bước.
* Hiển thị chiều quay bằng LED đơn.
* Hiển thị cấp tốc độ bằng LED 7 đoạn.
* Sử dụng Mutex để bảo vệ dữ liệu được chia sẻ giữa các tác vụ.

Các tác vụ được tổ chức riêng biệt nhằm giúp chương trình dễ quản lý và minh họa cơ chế lập lịch của RTOS.

### Bài tập 2: Thực hành lập trình RTOS

Bài tập thứ hai tiếp tục sử dụng Arduino và môi trường mô phỏng Proteus để thực hành các nội dung về RTOS.

Mã nguồn và sơ đồ mô phỏng được cung cấp trong các tệp `B2.ino` và `B2.pdsprj`.

## Cấu trúc repository

```text
ROTS/
├── B1.ino
├── B1.pdsprj
├── B2.ino
├── B2.pdsprj
├── BÁO CÁO.docx
├── Bài tập Thí nghiệm chuyên môn.pdf
├── RTOS là gì.pdf
└── README.md
```

Trong đó:

* Các tệp `.ino` chứa mã nguồn Arduino.
* Các tệp `.pdsprj` chứa dự án mô phỏng Proteus.
* Tệp báo cáo và các tài liệu PDF cung cấp nội dung bài tập, kiến thức nền tảng và kết quả thực hiện.

## Hướng dẫn sử dụng

### Yêu cầu

* Arduino IDE.
* Proteus.
* Thư viện FreeRTOS tương thích với môi trường Arduino được sử dụng.

### Thực hiện

1. Tải hoặc clone repository về máy tính.
2. Mở các tệp `B1.ino` hoặc `B2.ino` bằng Arduino IDE.
3. Cài đặt các thư viện cần thiết và kiểm tra cấu hình chân kết nối.
4. Biên dịch chương trình để tạo firmware.
5. Mở tệp Proteus tương ứng.
6. Nạp đường dẫn firmware vào thuộc tính chương trình của vi điều khiển trong Proteus.
7. Chạy mô phỏng và kiểm tra hoạt động của hệ thống.

## Kiến thức áp dụng

* Lập trình hệ thống nhúng với Arduino.
* Nguyên lý hoạt động của RTOS.
* Quản lý và lập lịch tác vụ.
* Đồng bộ hóa và bảo vệ tài nguyên dùng chung.
* Điều khiển động cơ bước.
* Giao tiếp với nút nhấn, LED đơn và LED 7 đoạn.
* Mô phỏng mạch điện và kiểm thử chương trình nhúng.

## Tác giả

Ngô Bỉnh Hoàng

GitHub: https://github.com/hoangngo2464-lgtm

## Giấy phép

Repository chưa xác định giấy phép phân phối. Vui lòng kiểm tra tệp LICENSE nếu có trước khi sử dụng hoặc phân phối lại mã nguồn.
