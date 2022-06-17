## `wc` 加速實驗
`wc` 是一個相當經典的指令，藉由追蹤與上一個空白字符的狀態來計算單詞數量。

這個實驗希望夠過完全不同的語言/方法來實現平行化。

受到這一篇[文章](https://chrispenner.ca/posts/wc)啟發，其作者提出用 80 行 Haskell 來重現 `wc` 的功能。

接著引來各路系統程式相關工程師挑戰，引出這篇[專案](https://github.com/expr-fi/fastlwc/)，該作者利用 SIMD 並搭配特殊的演算法把程式向量化了。

算法如下：
```
Just a sample (multiple   spaces).
1111010111111011111111100011111111             =[1]
0111101011111101111111110001111111 SHIFT [1]   =[2]
1000010100000010000000001110000000 NOT [2]     =[3]
1000010100000010000000000010000000 [1] AND [3]
```

然而在我突然想到我的機器是 ARM 架構，該專案則是採用 Intel 的 SIMD 指令集 SSE2 等。
因此我又費了不小的功夫把他翻譯成 NEON 指令，其中參考了 jserv 的專案 [sse2neon](https://github.com/DLTcollab/sse2neon) 當表來查，還有學生的好朋友技術文件[手冊](https://developer.arm.com/documentation/102581/latest)。
其實這部分也不是太難，直接使用 gcc 能相對簡單的幫我們轉成 vector type，只能說編譯器都那麼聰明了...就交給它了！

而我最近都在寫 go，大腦相對熟悉，所以我就用 go goroutine 並行機制搭配 channel 來重現 `wc` 的效果，結果出乎意料簡單，與 SIMD 版相比更令我大之一驚，竟然比辛辛苦苦刻的 SIMD 還快，行數又少，更是跨平台(go 能 cross compile)，這個 runtime 實在有點猛呀！

為了計算效能，我們必須測量執行時間，可惜簡單的`time`指令並無支援 millisecond 以下單位。

所以我們自己寫了一套小 script 來用：`ts=$(date +%s%N) ; $@ ; tt=$((($(date +%s%N) - $ts)/1000000)) ; echo "Time taken: $tt milliseconds"`

不過在 OS X 的機器上，`date`指令沒有支援`%N`flag，所以我們需要另外安裝 GNU 的 coreutils，用 homebrew 就行了，然後再把`date`換成用`gdate`。

最後附上我們的實驗結果：
|  語言/方法  |  字元數量  |  完成耗時  | 目標當案 |
|  ----  |  ----  |  ----  | ----  |
|  C/SIMD  |  1024000  |  12 ms  | xv6/fs.img  |
|  GO/Goroutine  |  1024000  |  11 ms  | xv6/fs.img  |
|  C/SIMD  |  311  |  15 ms  | xv6/xv6.out  |
|  GO/Goroutine  |  311  |  10 ms  | xv6/xv6.out  |

另外第二次執行兩種作法皆明顯加快了超多(約40%)，猜測是 cache 的緣故，暫時就先不繼續 trace 下去了。