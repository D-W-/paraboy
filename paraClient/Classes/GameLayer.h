#ifndef __GAMELAYER_H__
#define __GAMELAYER_H__

#include "cocos2d.h"
#include "network/WebSocket.h"
#include "string"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

USING_NS_CC;
using namespace rapidjson;
typedef rapidjson::Value JsonValue;

class GameLayer : public Layer, public cocos2d::network::WebSocket::Delegate
{
public:
	GameLayer();
	~GameLayer();
	static Scene* createScene();
	CREATE_FUNC(GameLayer);

	void createButton(Ref* pSender);

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
	MenuItemImage* me;
	MenuItemImage* buttonIdentify;
	MenuItemImage* buttonCompare;
	MenuItemImage* buttonOpenBox;
	Vector<Sprite*> others;
	void jsonTest();
	cocos2d::network::WebSocket* _wsiClient;

// added by wangxiyang
public:
// send actions
	void sendLogin(String id, int publicKey_d, int publicKey_n);
	void sendMove(int x, int y);

// receive actions
	void recvCreate(JsonValue msg);
	void recvMove(JsonValue msg);
	
};
#endif /* defined(__GAMELAYER_H__) */