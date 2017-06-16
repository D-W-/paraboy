#include "GameLayer.h"

USING_NS_CC;

GameLayer::GameLayer(): buttonCompare(NULL), buttonIdentify(NULL), buttonOpenBox(NULL)
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
	gameBackground= Sprite::create("bg.jpg");
	gameBackground->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(gameBackground, 0);

	//人物初始化
	me = MenuItemImage::create("icon4.jpg", "icon4.jpg", CC_CALLBACK_1(GameLayer::createButton, this));
	me->setPosition(0, 0);
	//this->addChild(me, 1);

	CCMenu* menu = CCMenu::create(me, NULL);
	menu->setPosition(0, 0);
	this->addChild(menu, 1);

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
	_wsiClient->init(*this, "ws://166.111.80.54:7000/echo");
	return true;
}

bool GameLayer::onTouchBegan(Touch* touch, Event* unused) 
{
	if (buttonIdentify != NULL) {
		//buttonIdentify->removeAllChildrenWithCleanup(true);
		buttonIdentify->removeFromParentAndCleanup(true);
		buttonIdentify = NULL;
	}
	auto location = touch->getLocation();
	me->runAction(MoveTo::create(0.3, location));	
	sendMove(location.x,location.y);
	return true;
}

void GameLayer::onKeyReleased(EventKeyboard::KeyCode keycode, Event * event)
{
	int offsetX = 0, offsetY = 0;
	switch (keycode) {
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
			offsetX = -144;
			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
			offsetX = 144;
			break;
		case EventKeyboard::KeyCode::KEY_UP_ARROW:
			offsetY = 144;
			break;
		case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
			offsetY = -144;
	}
	auto moveTo = MoveTo::create(0.3, Vec2(me->getPositionX() + offsetX, me->getPositionY() + offsetY));
	me->runAction(moveTo);
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

void GameLayer::createButton(Ref* pSender)
{
	if (buttonIdentify == NULL) {
		buttonIdentify = MenuItemImage::create("icon4.jpg", "icon4.jpg", CC_CALLBACK_1(GameLayer::createButton, this));
		buttonIdentify->setPosition(500, 10);
		this->addChild(buttonIdentify, 2);
	}
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
	srand(time(0));
	std::string name = StringUtils::format("wxy%d", rand() % 100);
	my_id = name;
	sendLogin(name, 9, 6);
	sendMove(5, 5);
}
 
// 接收消息处理函数
void GameLayer::onMessage(cocos2d::network::WebSocket* ws, const cocos2d::network::WebSocket::Data& data)
{
        std::string textStr = data.bytes;
		Document doc;
		doc.Parse(textStr.c_str());
		const char *action = doc["action"].GetString();
		if (strcmp(action,"create") == 0){
			recvCreate(doc["msg"].GetObjectW());
		}
		else if (strcmp(action, "move") == 0){
			recvMove(doc["msg"].GetObjectW());
		}
		CCLOG(action);
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

// added by wangxiyang
void GameLayer::sendLogin(String id, int publicKey_d, int publicKey_n){
	Document doc;
	Document::AllocatorType& allocator = doc.GetAllocator();
	doc.SetObject();
	doc.AddMember("id", JsonValue(StringRef(id.getCString())).Move(), allocator);
	doc.AddMember("action", JsonValue("login").Move(), allocator);
	Document msg;
	msg.SetObject();
	msg.AddMember("d", JsonValue(publicKey_d).Move(), msg.GetAllocator());
	msg.AddMember("n", JsonValue(publicKey_n).Move(), msg.GetAllocator());
	doc.AddMember("msg", msg, allocator);
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	sendMessage(buffer.GetString());
}
void GameLayer::sendMove(int x, int y){
	Document doc;
	Document::AllocatorType& allocator = doc.GetAllocator();
	doc.SetObject();
	doc.AddMember("id", JsonValue(StringRef(my_id.getCString())).Move(), allocator);
	doc.AddMember("action", JsonValue("move").Move(), allocator);
	Document msg;
	msg.SetObject();
	msg.AddMember("x", JsonValue(x).Move(), msg.GetAllocator());
	msg.AddMember("y", JsonValue(y).Move(), msg.GetAllocator());
	doc.AddMember("msg", msg, allocator);
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	sendMessage(buffer.GetString());
}

void GameLayer::recvCreate(JsonValue msg){
	String c_id = msg["id"].GetString();
	int x = msg["x"].GetInt();
	int y = msg["y"].GetInt();
	int d = msg["d"].GetInt();
	int n = msg["n"].GetInt();
	doCreate(c_id, x, y, d, n);
	//CCLOG(c_id.getCString());
}
void GameLayer::recvMove(JsonValue msg){
	String c_id = msg["id"].GetString();
	int x = msg["x"].GetInt();
	int y = msg["y"].GetInt();
	doMove(c_id, x, y);
	//CCLOG(c_id.getCString());
}

void GameLayer::doCreate(String id, int x, int y, int d, int n){
	//need wanghan to finish...
	CCLOG("doCreate:%s,%d,%d,%d,%d", id.getCString(), x, y, d, n);
}

void GameLayer::doMove(String id, int x, int y){
	//need wanghan to finish...
	CCLOG("doMove:%s,%d,%d", id.getCString(), x, y);
}