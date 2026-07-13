# MiniRBS — Mini Radio Base Station Simulator

*(English version: [README.md](README.md))*

Mô phỏng thu nhỏ một trạm phát sóng (RBS – Radio Base Station) trong mạng di động, viết bằng **C thuần** trên Linux. Dự án cá nhân xây dựng để luyện các kỹ năng cốt lõi của lập trình phần mềm viễn thông: socket programming, state machine, Makefile/Autotools, unit test, debug bằng GDB và kiểm tra memory bằng Valgrind.

## Bối cảnh

Trong mạng di động, RBS (gọi là eNB ở 4G, gNB ở 5G) là điểm giao tiếp vô tuyến giữa UE (User Equipment — thiết bị đầu cuối như điện thoại) và phần còn lại của mạng. Dự án này mô phỏng lại, ở quy mô rất nhỏ, cách một RBS quản lý kết nối với nhiều UE cùng lúc, dùng giao thức bản tin đơn giản (ATTACH / PING / DETACH) qua TCP socket.

## Trạng thái hiện tại

✅ Hoàn chỉnh — cả 6 Mốc theo kế hoạch đều đã xong.

- [x] Môi trường build: biên dịch tay bằng `gcc`, sau đó chuyển sang `Makefile`
- [x] `struct UE` (id, state) và thao tác qua con trỏ — nền tảng để quản lý danh sách UE sau này
- [x] TCP server: `socket` → `bind` → `listen` → `accept` → `recv`/`send`, có vòng lặp nhận nhiều bản tin trong 1 kết nối
- [x] Giao thức ATTACH / PING / DETACH + state machine (`enum UEState`, mỗi bản tin đều kiểm tra state trước khi accept/reject)
- [x] Xử lý nhiều UE cùng lúc bằng `select()` — mảng UE kích thước cố định, có nhánh từ chối khi đầy ("server full"), dọn dẹp đúng cách (không rò rỉ file descriptor)
- [x] Unit test bằng Check — tách logic giao thức ra `protocol.h`/`protocol.c` (không phụ thuộc socket), 8 test case phủ đủ các bản tin và nhánh accept/reject
- [x] Bash script (`build_and_test.sh`) tự động build + test, dùng `set -e` để dừng ngay khi có bước lỗi
- [x] Debug Journal: 1 bug tìm bằng GDB, 1 memory leak bắt bằng Valgrind — xem [docs/DEBUG_JOURNAL.vi.md](docs/DEBUG_JOURNAL.vi.md)
- [ ] Autotools hoá (`configure.ac` / `Makefile.am`) — cân nhắc là mục tiêu phụ (optional), quyết định bỏ qua để giữ build đơn giản với Makefile thuần

## Build & chạy

Yêu cầu: `gcc`, `make`, `check` (thư viện unit test), Linux (đã test trên Linux Mint).

```bash
make          # build server
./mini-rbs

make test     # build và chạy bộ unit test

./build_and_test.sh   # hoặc chạy cả 2 bước cùng lúc, dừng ngay nếu có lỗi
```

`make` chỉ biên dịch lại khi mã nguồn thay đổi (so sánh timestamp), không rebuild nếu không cần.

## Cấu trúc project

- `main.c` — điểm khởi chạy server: setup socket, vòng lặp `select()`, xử lý I/O
- `protocol.h` / `protocol.c` — logic giao thức/state machine (`process_message`), tách khỏi socket để test trực tiếp được
- `test_protocol.c` — unit test (dùng Check)
- `build_and_test.sh` — tự động hoá build + test chỉ với 1 lệnh
- `docs/notes.md` — ghi chú kỹ thuật và gotcha gặp phải trong quá trình làm
- `docs/DEBUG_JOURNAL.md` / `.vi.md` — 1 bug tìm bằng GDB, 1 memory leak tìm bằng Valgrind, có bằng chứng trước/sau

## Công nghệ dự kiến

C, POSIX sockets, `select()`, Makefile (Autotools nếu kịp), Check (unit test), Bash, Git, GDB, Valgrind.

## Ghi chú

Đây là dự án học tập/luyện kỹ năng, xây dựng trong thời gian ngắn để chuẩn bị ứng tuyển vị trí phát triển phần mềm radio/viễn thông. Không triển khai giao thức 3GPP thật — chỉ mô phỏng ý tưởng cốt lõi (kết nối, trạng thái, trao đổi bản tin) ở mức đơn giản hoá, đủ để giải thích rõ trong phỏng vấn.