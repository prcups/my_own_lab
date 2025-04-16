package main

import (
	"encoding/json"
	"fmt"
	"io"
	"log"
	"math/rand"
	"net/http"
	"net/url"
	"os"
	"os/signal"
	"strconv"
	"time"

	"github.com/gorilla/websocket"
)

const (
	HTTP_ADDRESS      = "http://127.0.0.1:5700"
	WEBSOCKET_ADDRESS = "ws://127.0.0.1:6700"
)

var err error

type GroupMemberInfo struct {
	NickName     string `json:"nickname"`
	UserID       int64  `json:"user_id"`
	LastSentTime int64  `json:"last_sent_time"`
	JoinTime     int64  `json:"join_time"`
	Level        string `json:"level"`
}

type HttpRes struct {
	RetCode int               `json: retcode`
	Msg     string            `json: msg`
	Data    []GroupMemberInfo `json: data`
}

func WSHandler() {
	wsurl, err := url.Parse("ws://localhost:6700")
	if err != nil {
		log.Fatal(err.Error())
	}

	ws, _, err := websocket.DefaultDialer.Dial(wsurl.String(), nil)
	if err != nil {
		log.Fatal(err.Error())
	}
	defer ws.Close()

	interrupt := make(chan os.Signal, 1)
	signal.Notify(interrupt, os.Interrupt)

	for {
		select {
		case <-interrupt:
			log.Println("interrupt")
			err := ws.WriteMessage(websocket.CloseMessage,
				websocket.FormatCloseMessage(websocket.CloseNormalClosure, ""))
			if err != nil {
				log.Fatal(err)
			}
			return
		default:
			_, resStr, err := ws.ReadMessage()
			if err != nil {
				log.Fatal(err)
			}
			var resMap map[string]interface{}
			err = json.Unmarshal(resStr, &resMap)
			if err != nil {
				log.Println(err)
			}

			log.Println(resMap)
		}
	}
}

func DeleteGroupMember() {
	resp, err := http.PostForm(
		HTTP_ADDRESS+"/get_group_member_list",
		url.Values{
			"group_id": {"746577276"},
		})
	if err != nil {
		log.Fatal(err)
	}
	resStr, err := io.ReadAll(resp.Body)
	var httpRes HttpRes
	httpRes.Data = make([]GroupMemberInfo, 0)
	err = json.Unmarshal(resStr, &httpRes)
	if err != nil {
		log.Fatal(err)
	}
	resp.Body.Close()
	dueDay, err := time.Parse("20060102", "20240601")
	if err != nil {
		log.Fatal(err)
	}
	var count int
	for _, i := range httpRes.Data {
		if time.Unix(i.LastSentTime, 0).Before(dueDay) {
			level, err := strconv.ParseInt(i.Level, 10, 64)
			if err != nil {
				log.Fatal(err)
			}
			if level < 50 {
				fmt.Println(i)
				_, err := http.PostForm(
					HTTP_ADDRESS+"/set_group_kick",
					url.Values{
						"group_id": {"746577276"},
						"user_id":  {strconv.FormatInt(i.UserID, 10)},
					})
				if err != nil {
					log.Fatal(err)
				}
				time.Sleep(time.Duration(2+rand.Intn(2)) * time.Second)
				count++
			}
		}
	}
	fmt.Println(count)
}

func main() {
	DeleteGroupMember()
}
