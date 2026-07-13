# Debug Journal

*(English version: [DEBUG_JOURNAL.md](DEBUG_JOURNAL.md))*

2 bug được cố ý tạo ra rồi tự tay tìm bằng GDB và Valgrind, để chứng minh quy trình debug thật, không chỉ mô tả suông về công cụ.

## Bug 1 — Thiếu `break`: 1 kết nối mới bị gán trùng vào mọi ô UE (tìm bằng GDB)

**Cách tạo bug:** comment lại dòng `break;` trong vòng lặp tìm ô trống cho UE mới kết nối.

**Triệu chứng:** chỉ mở 1 session `nc` duy nhất, nhưng server log ra 5 dòng "UE connected" riêng biệt, tất cả cùng chung 1 file descriptor.

**Nguyên nhân gốc:** thiếu `break`, vòng lặp tiếp tục quét hết các ô còn trống trong mảng `clients[]` sau khi đã tìm được ô đầu tiên, gán cùng 1 `new_fd` vào toàn bộ các ô còn lại thay vì dừng lại sau khi tìm thấy.

**Cách tìm bằng GDB:**

```
(gdb) break main.c:54
(gdb) run
# ... kết nối 1 lần qua nc ...
Breakpoint 1, main () at main.c:54
54          clients[i].socket = new_fd;
(gdb) print i
$1 = 0
(gdb) continue
Breakpoint 1, main () at main.c:54
(gdb) print i
$2 = 1
(gdb) continue
...
$5 = 4
```

Chỉ 1 kết nối duy nhất khiến breakpoint bị chạm 5 lần liên tiếp, `i` tăng dần từ 0 tới 4 mỗi lần — bằng chứng rõ ràng vòng lặp không dừng đúng chỗ.

**Cách sửa:** khôi phục lại dòng `break;`.

**Xác nhận đã sửa:** chạy lại đúng phiên GDB sau khi sửa. Breakpoint tại `main.c:54` chỉ bị chạm đúng 1 lần cho mỗi kết nối, sau đó chương trình chạy tiếp bình thường (vào xử lý ATTACH_REQUEST) mà không chạm lại breakpoint đó nữa.

## Bug 2 — Memory leak: `log_message` cấp phát nhưng không giải phóng (tìm bằng Valgrind)

**Cách tạo bug:** thêm 1 lệnh `malloc(100)` để tạo 1 log message ngắn cho mỗi kết nối, cố ý không kèm `free()` tương ứng.

```c
char *log_message = malloc(100);
sprintf(log_message, "Log for UE connected at fd=%d", new_fd);
printf("%s\n", log_message);
// cố ý thiếu: free(log_message);
```

**Cách tìm bằng Valgrind:**

```
valgrind --leak-check=full ./mini-rbs
```

Sau khi kết nối vài UE rồi dừng server (Ctrl+C), Valgrind báo:

```
200 bytes in 2 blocks are definitely lost in loss record 2 of 3
   at 0x4846828: malloc (...)
   by 0x10979E: main (main.c:69)

LEAK SUMMARY:
   definitely lost: 200 bytes in 2 blocks
```

`definitely lost` là mức nghiêm trọng nhất Valgrind phân loại — vùng nhớ không còn con trỏ nào trỏ tới, không thể cứu lại được. Báo cáo chỉ thẳng đúng dòng `malloc` gây ra (`main.c:69`).

**Cách sửa:** thêm `free(log_message);` ngay sau khi dùng xong, trước khi kết thúc khối xử lý kết nối đó.

**Xác nhận đã sửa:** chạy lại bằng Valgrind sau khi sửa:

```
total heap usage: 5 allocs, 4 frees, 1,424 bytes allocated

LEAK SUMMARY:
   definitely lost: 0 bytes in 0 blocks
   indirectly lost: 0 bytes in 0 blocks
   possibly lost: 0 bytes in 0 blocks
   still reachable: 1,024 bytes in 1 blocks
```

Cả 4 lần `malloc(100)` (mỗi UE kết nối 1 lần) đều đã được `free()` đúng. Block "still reachable" còn lại là cấp phát nội bộ của glibc, không liên quan tới code của mình — Valgrind không tính đây là leak.

## Rút ra được gì

Cả 2 bug đều tái hiện được, chẩn đoán bằng đúng công cụ thật (không đoán mò qua `printf`), và xác nhận đã sửa bằng cách chạy lại chính công cụ đó — đúng quy trình debug-sửa-xác nhận chuẩn trong công việc thực tế.
