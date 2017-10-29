package main

import (
	"log"
	"net/http"

	"github.com/gorilla/mux"
)

func home(w http.ResponseWriter, r *http.Request) {
	http.ServeFile(w, r, "index.html")
}
func wolf(w http.ResponseWriter, r *http.Request) {
	http.ServeFile(w, r, "wolf.jpg")
}

func main() {
	r := mux.NewRouter()
	r.HandleFunc("/", home)
	r.HandleFunc("/wolf.jpg", wolf)

	log.Fatal(http.ListenAndServe(":80", r))
}
