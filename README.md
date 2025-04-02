# MiniShell 🐚

用 C++ 實作的簡易 Linux Shell，支援以下功能：

- 指令輸入與執行 (`execvp`)
- `cd` / `exit` 內建指令
- 管線 `|`
- 重導向 `<` `>`


## 使用方式
```bash
make
./minishell

---

### ✅ 你每完成一個功能：
1. 在 `main.cpp` 或 `parser.cpp` 裡實作功能
2. 測試沒問題就 git add + commit + push
3. 更新 `README.md` 補上新功能
4. GitHub 上自動顯示你的 commit、code、更新紀錄 🎯

```


## 未來工作
1. cd, pwd（內建指令）
2. jobs, &（背景執行）
3. history 歷史記錄
4. >> 追加重定向
5. 多層 | 管線（用 loop 實作）

