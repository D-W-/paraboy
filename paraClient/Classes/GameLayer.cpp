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
	//������ʼ��
	gameBackground= Sprite::create("HelloWorld.png");
	gameBackground->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(gameBackground, 0);

	//�����ʼ��
	me = Sprite::create("CloseSelected.png");
	me->setPosition(0, 0);
	this->addChild(me, 1);

	// bind touch eventʵ�ִ���Ч��
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(GameLayer::onTouchBegan, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	//��Ӧ������Ϣ
	auto keyListener = EventListenerKeyboard::create();
	keyListener->onKeyReleased = CC_CALLBACK_2(GameLayer::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);
	
	//����websocket
	_wsiClient = new cocos2d::network::WebSocket();
	_wsiClient->init(*this, "ws://166.111.80.54:7000/echo");
	return true;
}



bool GameLayer::onTouchBegan(Touch* touch, Event* unused) 
{
	auto location = touch->getLocation();
	me->runAction(MoveTo::create(0.3, location));	
	sendMessage("Test usage");
	sendMessage("broadcast:Test usage");
	jsonTest();
	return true;
}

void GameLayer:: jsonTest(){
	Document doc;
	Document::AllocatorType& allocator = doc.GetAllocator();
	doc.SetObject();
	JsonValue username;
	username = "wangxiyang";
	doc.AddMember("username", username, allocator);
	//doc.AddMember("username:", JsonValue().SetString("wangxiyang").Move(), allocator);
	doc.AddMember("x", JsonValue(1).Move(), allocator);
	doc.AddMember("y", JsonValue(2).Move(), allocator);
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	sendMessage(buffer.GetString());
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
		//	�ر�socket����,��ɺ����ô�����onClose()
		_wsiClient->close();
	}
}

// ��ʼsocket����
void GameLayer::onOpen(cocos2d::network::WebSocket* ws)
{
    CCLOG("OnOpen");
	srand(time(0));
	std::string name = StringUtils::format("name:wxy%d", rand() % 100);
	sendMessage(name);
}
 
// ������Ϣ������
void GameLayer::onMessage(cocos2d::network::WebSocket* ws, const cocos2d::network::WebSocket::Data& data)
{
        std::string textStr = data.bytes;
        textStr.c_str();
        CCLOG(textStr.c_str());
}
 
// ���ӹر�
void GameLayer::onClose(cocos2d::network::WebSocket* ws)
{
    if (ws == _wsiClient)
    {
        _wsiClient = NULL;
    }
    CC_SAFE_DELETE(ws);
    CCLOG("onClose");
}
 
// ��������
void GameLayer::onError(cocos2d::network::WebSocket* ws, const cocos2d::network::WebSocket::ErrorCode& error)
{
    if (ws == _wsiClient)
    {
        char buf[100] = {0};
        sprintf(buf, "an error was fired, code: %d", error);
    }
    CCLOG("Error was fired, error code: %d", error);
}