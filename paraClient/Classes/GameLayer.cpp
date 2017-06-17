#include "GameLayer.h"

USING_NS_CC;

GameLayer::GameLayer(): buttonCompare(NULL), buttonIdentify(NULL), buttonOpenBox(NULL), userCount(0)
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

	//建立websocket
	_wsiClient = new cocos2d::network::WebSocket();
	_wsiClient->init(*this, "ws://166.111.80.54:7000/echo");

	//人物初始化

	//me = ParaBoy::create("icon4.jpg", "icon4.jpg", CC_CALLBACK_1(GameLayer::createButton, this, "sss"));
	//me->setPosition(0, 0);
	//this->addChild(me, 1);

/*	CCMenu* menu = CCMenu::create(me, NULL);
	menu->setPosition(0, 0);
	this->addChild(menu, 1)*/;

	// bind touch event实现触摸效果
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(GameLayer::onTouchBegan, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	//响应键盘消息
	auto keyListener = EventListenerKeyboard::create();
	keyListener->onKeyReleased = CC_CALLBACK_2(GameLayer::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);
	

	return true;
}

bool GameLayer::onTouchBegan(Touch* touch, Event* unused) 
{
	if (buttonIdentify != NULL) {
		//buttonIdentify->removeAllChildrenWithCleanup(true);
		buttonIdentify->removeFromParentAndCleanup(true);
		buttonIdentify = NULL;
	}
	//auto location = touch->getLocation();
	//me->runAction(MoveTo::create(0.3, location));	
	//sendMove(location.x,location.y);
	return true;
}

void GameLayer::onKeyReleased(EventKeyboard::KeyCode keycode, Event * event)
{
	int offsetX = 0, offsetY = 0;
	switch (keycode) {
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
			offsetX = -1;
			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
			offsetX = 1;
			break;
		case EventKeyboard::KeyCode::KEY_UP_ARROW:
			offsetY = 1;
			break;
		case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
			offsetY = -1;
	}
	//auto moveTo = MoveTo::create(0.3, Vec2(me->getPositionX() + offsetX, me->getPositionY() + offsetY));
	//me->runAction(moveTo);
	MenuItemImage* current = idMap[me->getID()];
	int x = current->getPositionX() / 144 + offsetX, y = current->getPositionY() / 144 + offsetY;
	sendMove(x, y);
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

Vec2 GameLayer::toRealLocation(int x, int y)
{
	return Vec2(x*144, y*144);
}

void GameLayer::createButton(Ref* pSender, string id)
{
	CCLOG(id.c_str());
	if (buttonIdentify == NULL) {
		buttonIdentify = MenuItemImage::create("icon4.jpg", "icon4.jpg", CC_CALLBACK_1(GameLayer::onIdentify, this));
		buttonIdentify->setPosition(500, 10);
		this->addChild(buttonIdentify, 2);
	}
}

void GameLayer::onIdentify(Ref * pSender)
{
	CCLOG("Clikcked");
}

void GameLayer::onCompare(Ref * pSender)
{
	CCLOG("Clikcked");
}

void GameLayer::onOpenBox(Ref * pSender)
{
	CCLOG("CLicked");
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
	me = new ParaBoy();
	me->setId(name);
	me->initRSA();
	string d, n;
	me->getPublicKey(d, n);
	sendLogin(name, d, n);
	//sendMove(5, 5);
	//sendAuth(name, "asjdhgfalkdjshfgaskdhfakldsfhakdfhlaldshf");
	//sendAuth2(name, "kljhlkhasdkflhasdjhflkjsflkdhsfagsdsfhlja");
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
		else if (strcmp(action, "auth") == 0){
			recvAuth(doc["msg"].GetObjectW());
		}
		else if (strcmp(action, "auth2") == 0){
			recvAuth2(doc["msg"].GetObjectW());
		}
		else if (strcmp(action, "login2") == 0){
			recvLogin2(doc["msg"].GetObjectW());
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
void GameLayer::sendLogin(String id, String publicKey_d, String publicKey_n){
	Document doc;
	Document::AllocatorType& allocator = doc.GetAllocator();
	doc.SetObject();
	doc.AddMember("id", JsonValue(StringRef(id.getCString())).Move(), allocator);
	doc.AddMember("action", JsonValue("login").Move(), allocator);
	Document msg;
	msg.SetObject();
	msg.AddMember("d", JsonValue(StringRef(publicKey_d.getCString())).Move(), msg.GetAllocator());
	msg.AddMember("n", JsonValue(StringRef(publicKey_n.getCString())).Move(), msg.GetAllocator());
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
	string tempStr = me->getID();
	doc.AddMember("id", JsonValue(StringRef(tempStr.c_str())).Move(), allocator);
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
void GameLayer::sendAuth(String targetId, String authMsg){
	Document doc;
	Document::AllocatorType& allocator = doc.GetAllocator();
	doc.SetObject();
	string tempStr = me->getID();
	doc.AddMember("id", JsonValue(StringRef(tempStr.c_str())).Move(), allocator);;
	doc.AddMember("action", JsonValue("auth").Move(), allocator);
	Document msg;
	msg.SetObject();
	msg.AddMember("target", JsonValue(StringRef(targetId.getCString())).Move(), msg.GetAllocator());
	msg.AddMember("authMsg", JsonValue(StringRef(authMsg.getCString())).Move(), msg.GetAllocator());
	doc.AddMember("msg", msg, allocator);
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	sendMessage(buffer.GetString());
}
void GameLayer::sendAuth2(String targetId, String auth2Msg){
	Document doc;
	Document::AllocatorType& allocator = doc.GetAllocator();
	doc.SetObject();
	string tempStr = me->getID();
	doc.AddMember("id", JsonValue(StringRef(tempStr.c_str())).Move(), allocator);
	doc.AddMember("action", JsonValue("auth2").Move(), allocator);
	Document msg;
	msg.SetObject();
	msg.AddMember("target", JsonValue(StringRef(targetId.getCString())).Move(), msg.GetAllocator());
	msg.AddMember("auth2Msg", JsonValue(StringRef(auth2Msg.getCString())).Move(), msg.GetAllocator());
	doc.AddMember("msg", msg, allocator);
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	sendMessage(buffer.GetString());
}

void GameLayer::recvLogin2(JsonValue msg){
	JsonValue users = msg["users"].GetArray();
	userCount = 0;
	for (SizeType i = 0; i < users.Size(); i++){
		doCreate(users[i]["id"].GetString(), users[i]["x"].GetInt(), users[i]["y"].GetInt(), users[i]["d"].GetString(), users[i]["n"].GetString());
	}
}

void GameLayer::recvCreate(JsonValue msg){
	string c_id = msg["id"].GetString();
	int x = msg["x"].GetInt();
	int y = msg["y"].GetInt();
	string d = msg["d"].GetString();
	string n = msg["n"].GetString();
	doCreate(c_id, x, y, d, n);
	//CCLOG(c_id.getCString());
}
void GameLayer::recvMove(JsonValue msg){
	string c_id = msg["id"].GetString();
	int x = msg["x"].GetInt();
	int y = msg["y"].GetInt();
	doMove(c_id, x, y);
	//CCLOG(c_id.getCString());
}

void GameLayer::recvAuth(JsonValue msg){
	string sourceId = msg["source"].GetString();
	string authMsg = msg["authMsg"].GetString();
	doAuth(sourceId, authMsg);
}

void GameLayer::recvAuth2(JsonValue msg){
	string sourceId = msg["source"].GetString();
	string auth2Msg = msg["auth2Msg"].GetString();
	doAuth2(sourceId, auth2Msg);
}

void GameLayer::doLogin2(){

}

void GameLayer::doCreate(string id, int x, int y, string d, string n){
	CCLOG("doCreate:%s,%d,%d,%s,%s", id.c_str(), x, y, d.c_str(), n.c_str());
	stringstream ss;
	ss << userCount;
	string userImage = "icon" + ss.str() + ".jpg", backImage = "bicon" + ss.str() + ".jpg";
	ParaBoy* current = ParaBoy::create(userImage, backImage, CC_CALLBACK_1(GameLayer::createButton, this, id));
	if (id == me->getID()){
		me = current;
	}
	current->setId(id);
	current->setPublicKey(d, n);
	current->setPosition(toRealLocation(x, y));
	this->addChild(current, 1);

	idMap[id] = current;
	userCount++;
}

void GameLayer::doMove(string id, int x, int y){
	CCLOG("doMove:%s,%d,%d", id, x, y);
	MenuItemImage* current = idMap[id];
	current->runAction(MoveTo::create(0.3, toRealLocation(x, y)));
}

void GameLayer::doAuth(string sourceId, string authMsg){
	//need wanghan to finish...
	CCLOG("doAuth:%s,%s", sourceId, authMsg);
}

void GameLayer::doAuth2(string sourceId, string auth2Msg){
	//need wanghan to finish...
	CCLOG("doAuth:%s,%s", sourceId, auth2Msg);
}

void GameLayer::doRemove(string id)
{
	
}
