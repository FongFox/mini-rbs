# MiniRBS — Mini Radio Base Station Simulator

*(English version: [README.md](README.md))*

Mô phỏng thu nhỏ một trạm phát sóng (RBS – Radio Base Station) trong mạng di động, viết bằng **C thuần** trên Linux. Dự án cá nhân xây dựng để luyện các kỹ năng cốt lõi của lập trình phần mềm viễn thông: socket programming, state machine, Makefile/Autotools, unit test, debug bằng GDB và kiểm tra memory bằng Valgrind.

## Bối cảnh

Trong mạng di động, RBS (gọi là eNB ở 4G, gNB ở 5G) là điểm giao tiếp vô tuyến giữa UE (User Equipment — thiết bị đầu cuối như điện thoại) và phần còn lại của mạng. Dự án này mô phỏng lại, ở quy mô rất nhỏ, cách một RBS quản lý kết nối với nhiều UE cùng lúc, dùng giao thức bản tin đơn giản (ATTACH / PING / DETACH) qua TCP socket.

## Trạng thái hiện tại

🚧 Đang trong quá trình xây dựng.

- [x] Môi trường build: biên dịch tay bằng `gcc`, sau đó chuyển sang `Makefile`
- [x] `struct UE` (id, state) và thao tác qua con trỏ — nền tảng để quản lý danh sách UE sau này
- [x] TCP echo server/client (đang làm)
- [x] Giao thức ATTACH / PING / DETACH + state machine
- [ ] Xử lý nhiều UE cùng lúc bằng `select()` (in progress)
- [ ] Unit test bằng Check
- [ ] Bash script build + test tự động
- [ ] Autotools hoá (`configure.ac` / `Makefile.am`) — nếu kịp thời gian
- [ ] Debug Journal: 1 bug tìm bằng GDB, 1 memory leak bắt bằng Valgrind

## Build & chạy

Yêu cầu: `gcc`, `make`, Linux (đã test trên Linux Mint).

```bash
make
./mini-rbs
```

`make` chỉ biên dịch lại khi mã nguồn thay đổi (so sánh timestamp), không rebuild nếu không cần.

## Công nghệ dự kiến

C, POSIX sockets, `select()`, Makefile (Autotools nếu kịp), Check (unit test), Bash, Git, GDB, Valgrind.

## Ghi chú

Đây là dự án học tập/luyện kỹ năng, xây dựng trong thời gian ngắn để chuẩn bị ứng tuyển vị trí phát triển phần mềm radio/viễn thông. Không triển khai giao thức 3GPP thật — chỉ mô phỏng ý tưởng cốt lõi (kết nối, trạng thái, trao đổi bản tin) ở mức đơn giản hoá, đủ để giải thích rõ trong phỏng vấn.