#ifndef __GAMELAYER_H__
#define __GAMELAYER_H__

#include "cocos2d.h"
#include "ParaBoy.h"
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

	static Scene* createScene();
	CREATE_FUNC(GameLayer);
	void createButton(Ref * pSender, string id);
	void onIdentify(Ref* pSender);
	void onCompare(Ref* pSender);
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
	Sprite* gameBackground;
	//point to current user, can get publicKey, privateKey, ID, SSID from here
	ParaBoy* me;
	MenuItemImage* buttonIdentify;
	MenuItemImage* buttonCompare;
	MenuItemImage* buttonOpenBox;
	//Vector<MenuItemImage*> others;
	int userCount;
	unordered_map<string, ParaBoy*> idMap;
	//String my_id;   // 用户id，新建伞兵类之后可删去
	void jsonTest();
	cocos2d::network::WebSocket* _wsiClient;


// added by wangxiyang
public:
	// send actions
	void sendLogin(String id, int publicKey_d, int publicKey_n);
	void sendMove(int x, int y);
	void sendAuth(String targetId, String authMsg);
	void sendAuth2(String targetId, String auth2Msg);

	// receive actions
	void recvCreate(JsonValue msg);
	void recvMove(JsonValue msg);
	void recvAuth(JsonValue msg);
	void recvAuth2(JsonValue msg);

	// response actions
	void doCreate(string id, int px, int py, int publicKey_d,int publicKey_n);
	void doMove(string id, int px, int py);
	void doAuth(string sourceId, string authMsg);
	void doAuth2(string sourceId, string auth2Msg);
	void doRemove(string id);
};
#endif /* defined(__GAMELAYER_H__) */