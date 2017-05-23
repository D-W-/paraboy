#ifndef __GAMELAYER_H__
#define __GAMELAYER_H__

#include "cocos2d.h"
#include "network/WebSocket.h"
#include "string"

USING_NS_CC;

class GameLayer : public Layer, public cocos2d::network::WebSocket::Delegate
{
public:
	GameLayer();
	~GameLayer();
	static Scene* createScene();
	CREATE_FUNC(GameLayer);

	void sendMessage(String message);
	void closeSocket();

	virtual bool init() override;
	virtual bool onTouchBegan(Touch *touch, Event *unused) override;
	//virtual void onKeyReleased(EventKeyboard::KeyCode keycode, Event* event)override;
	virtual void onOpen(cocos2d::network::WebSocket* ws);
	virtual void onMessage(cocos2d::network::WebSocket* ws, const cocos2d::network::WebSocket::Data& data);
	virtual void onClose(cocos2d::network::WebSocket* ws);
	virtual void onError(cocos2d::network::WebSocket* ws, const cocos2d::network::WebSocket::ErrorCode& error);


private:
	Sprite* gameBackground;
	Sprite* me;
	Vector<Sprite*> others;

	cocos2d::network::WebSocket* _wsiClient;

};
#endif /* defined(__GAMELAYER_H__) */