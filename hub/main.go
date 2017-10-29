package main

import (
	"context"
	"net/http"
	"os"
	"os/signal"
	"syscall"
	"time"

	"github.com/edfungus/eevee"
	"github.com/kelseyhightower/envconfig"
	logging "github.com/op/go-logging"
)

/*
A uni-directional example routing rest calls to mqtt
*/

var (
	log            = logging.MustGetLogger("eevee")
	fb             *FacebookAuth
	jsonTranslator = NewJsonTranslator()
)

type envVars struct {
	FbID         string   `envconfig:"FB_ID" required:"true"`
	FbSecret     string   `envconfig:"FB_SECRET" required:"true"`
	AllowedUsers []string `envconfig:"FB_ALLOWED_USERS" required:"true"`
	MqttUsername string   `envconfig:"MQTT_USERNAME" required:"true"`
	MqttPassword string   `envconfig:"MQTT_PASSWORD" required:"true"`
}

func main() {
	eevee.StartLogger()
	log.Notice("Homebase is starting up")

	var env envVars
	err := envconfig.Process("homebase", &env)
	if err != nil {
		log.Fatal(err.Error())
	}

	restConnector := createRESTConnector()
	mqttConnector := createMQTTConnector(env.MqttUsername, env.MqttPassword)
	fb = NewFacebookAuth(env.FbID, env.FbSecret, env.AllowedUsers)

	bridge := eevee.NewUniBridge(restConnector, mqttConnector)

	ctx := context.Background()
	ctx, cancel := context.WithCancel(ctx)
	bridge.Start(ctx)

	sigchan := make(chan os.Signal, 1)
	signal.Notify(sigchan, syscall.SIGINT, syscall.SIGTERM)

	<-sigchan
	cancel()
	time.Sleep(time.Second * 1)
	log.Noticef("Homebase says goodbye")
}

func createRESTConnector() *eevee.Connector {
	restConfig := eevee.RestConnectionConfig{
		Topics: map[string]eevee.RestRoute{
			"open": {
				Path:   "/open",
				Method: http.MethodPost,
			},
			"close": {
				Path:   "/close",
				Method: http.MethodPost,
			},
			"toggle": {
				Path:   "/toggle",
				Method: http.MethodPost,
			},
		},
		Port:    "80",
		Router:  nil,
		Wrapper: fbAuthWrapper,
	}
	restConnection, err := eevee.NewRestConnection(restConfig)
	if err != nil {
		log.Fatalf("Could not create Rest Connection with err: %s", err.Error())
	}

	return &eevee.Connector{
		Connection: restConnection,
		Translator: jsonTranslator,
		IDStore:    NewMapStore(),
	}
}

func createMQTTConnector(username string, password string) *eevee.Connector {
	mqttConfig := eevee.MqttConnectionConfig{
		Server:   "tcp://192.168.12.30:1883",
		Topics:   []string{},
		ClientID: "raspberrypi",
		Qos:      byte(1),
		Username: username,
		Password: password,
	}
	mqttConnetion, err := eevee.NewMqttConnection(mqttConfig)
	if err != nil {
		log.Fatalf("Could not create MQTT Client with err: %s", err.Error())
	}

	return &eevee.Connector{
		Connection: mqttConnetion,
		Translator: jsonTranslator,
		IDStore:    NewMapStore(),
	}

}

func fbAuthWrapper(h http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		urlParams := r.URL.Query()
		userToken := urlParams.Get("token")
		if userToken == "" {
			w.WriteHeader(http.StatusBadRequest)
			return
		}
		ok, err := fb.ValidateUserAccess(userToken)
		if err != nil {
			log.Errorf("Error validating user access. Error: %v", err)
			w.WriteHeader(http.StatusInternalServerError)
			return
		}
		if ok {
			h.ServeHTTP(w, r)
			return
		}
		w.WriteHeader(http.StatusUnauthorized)
		return
	})
}
