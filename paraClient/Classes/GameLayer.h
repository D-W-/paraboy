#ifndef __GAMELAYER_H__
#define __GAMELAYER_H__

#include "cocos2d.h"
#include "ParaBoy.h"
#include "RSA.cpp"
#include "Millionaire.h"
#include "network/WebSocket.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

#include "string"
#include "unordered_map"
#include "sstream"

USING_NS_CC;

using namespace std;
using namespace rapidjson;
typedef rapidjson::Value JsonValue;

class GameLayer : public Layer, public cocos2d::network::WebSocket::Delegate
{
public:
	GameLayer();
	~GameLayer();

	static Scene* createScene(string id);
	//static string id;

	CREATE_FUNC(GameLayer);
	void createButton(Ref * pSender, string id);
	void createOpenBoxButton(Ref* pSender);
	void onIdentify(Ref* pSender, string sender, string receiver);
	void onCompare(Ref* pSender, string sender, string receiver);
	void onOpenBox(Ref* pSender);

	void sendMessage(String message);
	void closeSocket();

	virtual bool init() override;
	virtual bool onTouchBegan(Touch *touch, Event *unused) override;
	virtual void onKeyReleased(EventKeyboard::KeyCode keycode, Event* event)override;
	virtual void onOpen(cocos2d::network::WebSocket* ws);
	virtual void onMessage(cocos2d::network::WebSocket* ws, const cocos2d::network::WebSocket::Data& data);
	virtual void onClose(cocos2d::network::WebSocket* ws);
	virtual void onError(cocos2d::network::WebSocket* ws, const cocos2d::network::WebSocket::ErrorCode& error);
private:
	Label *label;
	Sprite* gameBackground;
	Menu* menu;
	//point to current user, can get publicKey, privateKey, ID, SSID from here
	ParaBoy* me;
	MenuItemImage* buttonIdentify;
	MenuItemImage* buttonCompare;
	MenuItemImage* buttonOpenBox;
	MenuItemImage* box;
	int userCount;
	unordered_map<string, ParaBoy*> idMap;
	void jsonTest();
	cocos2d::network::WebSocket* _wsiClient;

	Vec2 toRealLocation(int x, int y);


// added by wangxiyang
public:
	// send actions
	void sendLogin(String id, String publicKey_d, String publicKey_n);
	void sendMove(int x, int y);
	void sendAuth(String targetId, String authMsg);
	void sendAuth2(String targetId, String auth2Msg);
	void sendCompare(String targetId, String compMsg);
	void sendCompare2(String targetId, list<string> &msgList);
	void sendVotes(int voteArr[], int n);
	void sendVotes2(int voteSum);
	void sendBox(int key);
	// receive actions
	void recvLogin2(JsonValue msg);
	void recvCreate(JsonValue msg);
	void recvMove(JsonValue msg);
	void recvAuth(JsonValue msg);
	void recvAuth2(JsonValue msg);
	void recvRemove(JsonValue msg);
	void recvCompare(JsonValue msg);
	void recvCompare2(JsonValue msg);
	void recvVotes(JsonValue msg);
	void recvVotes2(JsonValue msg);
	void recvBox(JsonValue msg);
	// response actions
	void doLogin2();
	void doCreate(string id, int px, int py, string publicKey_d,string publicKey_n, int level);
	void doMove(string id, int px, int py);
	void doAuth(string sourceId, string authMsg);
	void doAuth2(string sourceId, string auth2Msg);
	void doRemove(string id);
	void doCompare(string sourceId, string compMsg);
	void doCompare2(string sourceId, list<string> &msgList);
	void doVotes(string sourceId, int vote);
	void doVotes2(string sourceId, int voteSum);
	void doBox(int minNumber, list<string> &userList);
};
#endif /* defined(__GAMELAYER_H__) */