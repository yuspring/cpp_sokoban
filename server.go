package main

import (
	"embed"
	"log"
	"net/http"
)

// 告訴 Go 把這些檔案包進執行檔
//
//go:embed game.html game.js game.wasm game.data
var content embed.FS

func main() {
	port := ":8080"

	// 如果檔案就在根目錄，我們直接使用 http.FS 包裝
	// 這樣即使你把編譯好的執行檔拿到別台電腦（沒有 game.wasm 的地方），它也能跑
	handler := http.FileServer(http.FS(content))

	log.Printf("Sokoban Embedded Server 啟動！\n")
	log.Printf("請訪問 http://localhost%s/game.html\n", port)

	err := http.ListenAndServe(port, handler)
	if err != nil {
		log.Fatal(err)
	}
}
