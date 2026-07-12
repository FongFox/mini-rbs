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

## Thói quen làm việc

### Khi nào copy-paste code cũ, khi nào gõ lại tay

**Đừng viết lại tay code đã chạy đúng và đã test, nếu chỉ cần sửa nhỏ — hãy copy rồi sửa đúng chỗ cần sửa.**

Ví dụ thực tế: khi chuyển logic xử lý giao thức (ATTACH/PING/DETACH) từ code `accept()` đơn giản (Mốc 3) sang code `select()` (Mốc 4), phần `if/else` bên trong **không đổi ý nghĩa gì cả** — chỉ đổi tên biến (`client_fd` → `clients[i].socket`, `ue.state` → `clients[i].state`). Gõ lại tay toàn bộ khối `if/else` đó có rủi ro gõ nhầm điều kiện, gõ sai tên bản tin — tức tự tạo bug mới cho phần code vốn đã đúng và đã test kỹ.

Cách làm an toàn hơn: copy nguyên khối cũ, dùng Find & Replace (`Ctrl+R` trong CLion) để đổi đúng tên biến cần đổi, rồi **đọc lại bằng mắt 1 lượt** để chắc không sót chỗ nào — bước đọc lại này không nên bỏ qua dù đã tin tưởng Find & Replace.

---

## select() với nhiều UE (Mốc 4)

### Edge case: server đầy (đủ `MAX_CLIENTS`) mà có UE mới xin kết nối → rò rỉ file descriptor

Khi dùng mảng cố định `struct UE clients[MAX_CLIENTS]` để lưu UE đang kết nối, nếu mảng đã đầy mà `accept()` vẫn nhận thêm 1 kết nối mới, vòng lặp tìm ô trống (`clients[i].socket == -1`) sẽ không tìm được chỗ nào — `new_fd` vừa tạo ra **không được lưu vào đâu cả, cũng không bị `close()`**. Đây là 1 dạng rò rỉ tài nguyên (giống rò rỉ bộ nhớ, nhưng là rò rỉ file descriptor) — dùng lâu server sẽ dần hết fd khả dụng.

**Cách vá:** dùng 1 biến cờ để biết đã tìm được chỗ chứa hay chưa; nếu không, chủ động `close()` kết nối mới ngay:

```c
int placed = 0;
for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i].socket == -1) {
        clients[i].socket = new_fd;
        clients[i].id = i + 1;
        clients[i].state = IDLE;
        placed = 1;
        break;
    }
}

if (!placed) {
    printf("Server full, rejecting new connection\n");
    close(new_fd);   // không có chỗ chứa → đóng luôn, tránh rò rỉ
}
```

Nguyên tắc chung rút ra: **bất kỳ tài nguyên nào được cấp phát (ở đây là fd từ `accept()`) đều cần có đường đi rõ ràng để được giải phóng ở MỌI nhánh có thể xảy ra** — kể cả nhánh "không có chỗ chứa", không chỉ nhánh happy-path.

### `break` vs `continue` trong vòng lặp `for` xử lý nhiều UE

Khi 1 UE cần rời khỏi hệ thống (rớt mạng: `bytes <= 0`, hoặc chủ động `DETACH` thành công), phải làm đủ 3 việc: `close()` socket, gán `-1` để đánh dấu ô trống, và dùng **`continue`** (không phải `break`) để chuyển sang UE tiếp theo trong cùng vòng `for` — vì vòng lặp này phục vụ chung cho *mọi* UE trong 1 lượt `select()`, dùng `break` sẽ bỏ sót các UE khác đang chờ xử lý trong cùng lượt đó.

---

*(Thêm ghi chú mới bên dưới khi học được pattern/gotcha khác trong lúc làm dự án — không cần format cầu kỳ, ghi nhanh là được)*