# Ghi chú khái niệm & pattern

Ghi nhanh các pattern/gotcha học được trong lúc làm MiniRBS — không cần hiểu sâu gốc rễ mỗi lần, chỉ cần nhớ đúng pattern để dùng, và có sẵn để ôn lại hoặc trả lời khi phỏng vấn hỏi tới.

---

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

## Quy trình 1 TCP server luôn làm theo (y hệt cách nhấc điện thoại lên nghe cuộc gọi)

1. socket() — tạo ra "cái điện thoại" (một điểm giao tiếp mạng)

2. bind() — gán "số điện thoại" cụ thể cho nó (địa chỉ IP + port)

3. listen() — bật chế độ "chờ cuộc gọi tới"

4. accept() — "nhấc máy" khi có ai gọi tới (client kết nối)

5. recv() / send() — nói chuyện qua lại (nhận/gửi dữ liệu)

5 bước này sẽ là khung xương cho toàn bộ RBS server sau này. 

---

## Kiểu tự định nghĩa (struct, enum)

### C luôn bắt phải nhắc lại `struct`/`enum` mỗi lần dùng làm kiểu dữ liệu

Khác với Java/C++ (có thể lược bỏ), trong C thuần, khi khai báo biến/tham số/trường có kiểu là 1 `struct` hoặc `enum` tự định nghĩa, **luôn phải viết kèm từ khoá gốc phía trước tên đó** — viết tên suông sẽ không biên dịch được.

```c
enum UEState {
    IDLE,
    ATTACHED
};

struct UE {
    int id;
    enum UEState state;   // ← phải có "enum" phía trước, không viết "UEState state;" suông
};
```

Tương tự với `struct`, đã quen từ trước:

```c
struct UE *p = &ue1;   // luôn "struct UE", không viết "UE *p" suông
```

Quy tắc gọn: **mọi kiểu tự định nghĩa (`struct X`, `enum X`) đều phải nhắc lại từ khoá gốc mỗi lần dùng.**

---

## Chuỗi ký tự (string.h)

### `strspn()` và `strcspn()` — tên gần giống nhau, ý nghĩa NGƯỢC NHAU

Dễ gõ nhầm vì tên chỉ khác 1 chữ `c`, nhưng hoạt động ngược hẳn nhau:

- **`strspn(s, set)`** — trả về độ dài đoạn đầu chuỗi `s` **CHỈ GỒM** ký tự nằm trong `set`. Gặp ký tự **không thuộc** `set` là dừng ngay.
- **`strcspn(s, set)`** — trả về vị trí đầu tiên trong `s` **GẶP** ký tự nằm trong `set` ("complement" = ngược lại). Dùng hàm này để cắt `\n` do `nc`/`telnet` tự thêm vào cuối dữ liệu:
```c
buffer[strcspn(buffer, "\n")] = '\0';   // ĐÚNG — cắt tại vị trí gặp \n đầu tiên
buffer[strspn(buffer, "\n")] = '\0';    // SAI — nếu ký tự đầu không phải \n, trả về 0
                                          // → cắt ngay từ đầu, xoá sạch buffer
```

Triệu chứng khi dùng nhầm `strspn`: `buffer` bị rỗng ngay cả khi `recv()` nhận dữ liệu bình thường (vì bị cắt tại vị trí 0) — dễ tưởng nhầm là lỗi ở `recv()`, nhưng thực ra lỗi nằm ở dòng xử lý chuỗi ngay sau đó.
 
---

*(Thêm ghi chú mới bên dưới khi học được pattern/gotcha khác trong lúc làm dự án — không cần format cầu kỳ, ghi nhanh là được)*