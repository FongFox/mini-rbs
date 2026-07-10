# Ghi chú khái niệm & pattern

Ghi nhanh các pattern/gotcha học được trong lúc làm MiniRBS — không cần hiểu sâu gốc rễ mỗi lần, chỉ cần nhớ đúng pattern để dùng, và có sẵn để ôn lại hoặc trả lời khi phỏng vấn hỏi tới.

## Socket programming

### Truyền địa chỉ struct vào hàm socket → luôn kèm `sizeof(...)`

**Mỗi lần ép kiểu một con trỏ struct địa chỉ (`sockaddr_in`, ...) để truyền vào hàm như `bind()`, luôn kèm `sizeof(...)` của đúng struct đó** ở tham số ngay sau:

```c
bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
```

**Vì sao:** con trỏ chỉ nói "bắt đầu đọc từ đâu", không nói "đọc bao nhiêu byte thì dừng". `struct sockaddr *` là kiểu con trỏ chung, có thể ẩn bên trong là `sockaddr_in` (IPv4, ~16 byte) hoặc `sockaddr_in6` (IPv6, ~28 byte) — kích thước khác nhau. `sizeof(addr)` là cách duy nhất báo cho hàm biết nên đọc đúng bao nhiêu byte.

## Ép kiểu con trỏ (cast)

- Biến **thường** (không phải con trỏ) → cần `&` trước khi ép kiểu con trỏ: `(struct sockaddr *)&addr`
- Biến **đã là con trỏ** sẵn → ép kiểu thẳng, không thêm `&`: `(void *)p`

---

**Quy trình 1 TCP server luôn làm theo (y hệt cách nhấc điện thoại lên nghe cuộc gọi):**

1. socket() — tạo ra "cái điện thoại" (một điểm giao tiếp mạng)

2. bind() — gán "số điện thoại" cụ thể cho nó (địa chỉ IP + port)

3. listen() — bật chế độ "chờ cuộc gọi tới"

4. accept() — "nhấc máy" khi có ai gọi tới (client kết nối)

5. recv() / send() — nói chuyện qua lại (nhận/gửi dữ liệu)

5 bước này sẽ là khung xương cho toàn bộ RBS server sau này. 

---

(Ghi chú thêm ở đây)