package main

import (
	"embed"
	"io/fs"
	"log"
	"net"
	"net/http"
)

//go:embed dist/*
var content embed.FS

type loggingResponseWriter struct {
	http.ResponseWriter
	statusCode int
}

func (lrw *loggingResponseWriter) WriteHeader(code int) {
	lrw.statusCode = code
	lrw.ResponseWriter.WriteHeader(code)
}

func Logger(handler http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		lrw := &loggingResponseWriter{w, http.StatusOK}
		handler.ServeHTTP(lrw, r)
		host, _, err := net.SplitHostPort(r.RemoteAddr)
		if err != nil {
			host = r.RemoteAddr
		}
		log.Printf("%s - \"%s %s %s\" %d",
			host,
			r.Method,
			r.URL.Path,
			r.Proto,
			lrw.statusCode,
		)
	})
}

func main() {
	port := ":8080"

	distFS, err := fs.Sub(content, "dist")
	if err != nil {
		log.Fatal(err)
	}

	mux := http.NewServeMux()
	fileServer := http.FileServer(http.FS(distFS))

	mux.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		if r.URL.Path == "/" {
			r.URL.Path = "/game.html"
		}
		fileServer.ServeHTTP(w, r)
	})

	log.Printf("Server is Listening on http://localhost%s/", port)

	err = http.ListenAndServe(port, Logger(mux))
	if err != nil {
		log.Fatal(err)
	}
}
