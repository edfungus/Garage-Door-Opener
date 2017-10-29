package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
)

const (
	fbHost           = "https://graph.facebook.com"
	fbAccessToken    = "/oauth/access_token?client_id=%s&client_secret=%s&grant_type=client_credentials"
	fbCheckUserToken = "/debug_token?input_token=%s&access_token=%s"
)

type accessTokenResponse struct {
	AccessToken string `json:"access_token"`
}

type userResponse struct {
	Data struct {
		ID string `json:"user_id"`
	} `json:"data"`
}

type FacebookAuth struct {
	fbID           string
	fbSecret       string
	AllowedUserIDs []string
	accessToken    string
}

// NewFacebookAuth returns FacebookAuth which can verify if facebook user is allowed access
func NewFacebookAuth(fbID string, fbSecret string, users []string) *FacebookAuth {
	return &FacebookAuth{
		fbID:           fbID,
		fbSecret:       fbSecret,
		AllowedUserIDs: users,
	}
}

func (fa *FacebookAuth) ValidateUserAccess(userToken string) (bool, error) {
	err := fa.getAccessToken() // Perhaps cache later
	if err != nil {
		return false, err
	}
	userID, err := fa.getUserID(userToken)
	if err != nil {
		return false, err
	}
	return fa.isUserIDAllowed(userID), nil
}

func (fa *FacebookAuth) getAccessToken() error {
	resp, err := http.Get(fmt.Sprintf(fbHost+fbAccessToken, fa.fbID, fa.fbSecret))
	if err != nil {
		return err
	}
	defer resp.Body.Close()
	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		return err
	}
	var tokenResp accessTokenResponse
	err = json.Unmarshal(body, &tokenResp)
	if err != nil {
		return err
	}
	fa.accessToken = tokenResp.AccessToken
	return nil
}

func (fa *FacebookAuth) getUserID(userToken string) (string, error) {
	resp, err := http.Get(fmt.Sprintf(fbHost+fbCheckUserToken, userToken, fa.accessToken))
	if err != nil {
		return "", err
	}
	defer resp.Body.Close()
	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		return "", err
	}
	var userResp userResponse
	err = json.Unmarshal(body, &userResp)
	if err != nil {
		return "", err
	}
	return userResp.Data.ID, nil
}

func (fa *FacebookAuth) isUserIDAllowed(userID string) bool {
	for _, v := range fa.AllowedUserIDs {
		if userID == v {
			return true
		}
	}
	return false
}
