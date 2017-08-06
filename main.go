package main

import (
	"log"
	"net/http"

	"github.com/gorilla/mux"
)

func home(w http.ResponseWriter, r *http.Request) {
	http.ServeFile(w, r, "index.html")
}

func main() {
	r := mux.NewRouter()
	r.HandleFunc("/", home)

	log.Fatal(http.ListenAndServe(":80", r))
}
