#include "GameLayer.h"

USING_NS_CC;

GameLayer::GameLayer()
{

}

GameLayer::~GameLayer()
{
	closeSocket();
}

Scene* GameLayer::createScene()
{
	auto scene = Scene::create();
	auto layer = GameLayer::create();
	scene->addChild(layer);
	return scene;
}

bool GameLayer::init()
{
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	if (!Layer::init())
	{
		return false;
	}
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();
	//背景初始化
	gameBackground= Sprite::create("HelloWorld.png");
	gameBackground->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(gameBackground, 0);

	//人物初始化
	me = Sprite::create("CloseSelected.png");
	me->setPosition(0, 0);
	this->addChild(me, 1);

	// bind touch event实现触摸效果
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(GameLayer::onTouchBegan, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	//响应键盘消息
	auto keyListener = EventListenerKeyboard::create();
	keyListener->onKeyReleased = CC_CALLBACK_2(GameLayer::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);
	
	//建立websocket
	_wsiClient = new cocos2d::network::WebSocket();
	_wsiClient->init(*this, "ws://localhost:8000");
	return true;
}

bool GameLayer::onTouchBegan(Touch* touch, Event* unused) 
{
	auto location = touch->getLocation();
	me->runAction(MoveTo::create(0.3, location));	
	sendMessage("Test usage");
	return true;
}

void GameLayer::sendMessage(String message)
{
	if (_wsiClient) {
		_wsiClient->send(message._string);
	}
}

void GameLayer::closeSocket()
{
	if (_wsiClient) {
		//	关闭socket连接,完成后会调用代理函数onClose()
		_wsiClient->close();
	}
}

// 开始socket连接
void GameLayer::onOpen(cocos2d::network::WebSocket* ws)
{
    CCLOG("OnOpen");
}
 
// 接收消息处理函数
void GameLayer::onMessage(cocos2d::network::WebSocket* ws, const cocos2d::network::WebSocket::Data& data)
{
        std::string textStr = data.bytes;
        textStr.c_str();
        CCLOG(textStr.c_str());
}
 
// 连接关闭
void GameLayer::onClose(cocos2d::network::WebSocket* ws)
{
    if (ws == _wsiClient)
    {
        _wsiClient = NULL;
    }
    CC_SAFE_DELETE(ws);
    CCLOG("onClose");
}
 
// 遇到错误
void GameLayer::onError(cocos2d::network::WebSocket* ws, const cocos2d::network::WebSocket::ErrorCode& error)
{
    if (ws == _wsiClient)
    {
        char buf[100] = {0};
        sprintf(buf, "an error was fired, code: %d", error);
    }
    CCLOG("Error was fired, error code: %d", error);
}