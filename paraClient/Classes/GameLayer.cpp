#include "GameLayer.h"

USING_NS_CC;

GameLayer::GameLayer(): buttonCompare(NULL), buttonIdentify(NULL), buttonOpenBox(NULL), userCount(0)
{

}

GameLayer::~GameLayer()
{
	closeSocket();
}

Scene* GameLayer::createScene(string id)
{
	auto scene = Scene::create();
	auto layer = GameLayer::create();
	scene->addChild(layer);
	layer->me = new ParaBoy();
	layer->me->setId(id);
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
	gameBackground= Sprite::create("bg.jpg");
	gameBackground->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(gameBackground, 0);

	menu = CCMenu::create(NULL);
	menu->setPosition(0, 0);
	this->addChild(menu, 1);


	//����websocket
	_wsiClient = new cocos2d::network::WebSocket();
	_wsiClient->init(*this, "ws://166.111.80.54:7000/echo");

	//�����ʼ��

	//me = ParaBoy::create("icon4.jpg", "icon4.jpg", CC_CALLBACK_1(GameLayer::createButton, this));
	//me->setPosition(0, 0);
	//this->addChild(me, 1);

	//init box

	// bind touch eventʵ�ִ���Ч��
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(GameLayer::onTouchBegan, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	//��Ӧ������Ϣ
	auto keyListener = EventListenerKeyboard::create();
	keyListener->onKeyReleased = CC_CALLBACK_2(GameLayer::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);
	

	return true;
}

bool GameLayer::onTouchBegan(Touch* touch, Event* unused) 
{
	//CCLOG("Click");
	if (buttonIdentify != NULL) {
		buttonIdentify->removeFromParentAndCleanup(true);
		buttonIdentify = NULL;
	}
	if (buttonCompare != NULL) {
		buttonCompare->removeFromParentAndCleanup(true);
		buttonCompare = NULL;
	}
	if (buttonOpenBox != NULL) {
		buttonOpenBox->removeFromParentAndCleanup(true);
		buttonOpenBox = NULL;
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
	if (me->getID() == id)
		return;
	string temp = "sender: " + me->getID() + "rev:" + id;
	CCLOG(temp.c_str());
	if (buttonIdentify == NULL) {
		buttonIdentify = MenuItemImage::create("button0.png", "button0b.png", CC_CALLBACK_1(GameLayer::onIdentify, this, me->getID(), id));
		buttonIdentify->setPosition(600, 100);
		menu->addChild(buttonIdentify, 2);
	}
	if (buttonCompare == NULL) {
		buttonCompare = MenuItemImage::create("button1.png", "button1b.png", CC_CALLBACK_1(GameLayer::onCompare, this, me->getID(), id));
		buttonCompare->setPosition(600, 200);
		menu->addChild(buttonCompare, 2);
	}
}

void GameLayer::onIdentify(Ref * pSender, string sender, string receiver)
{
	CCLOG("Clikcked identify");
	sendAuth(receiver, me->getCipher());
}

void GameLayer::onCompare(Ref * pSender, string sender, string receiver)
{
	CCLOG("Clikcked compare");
	String msg = "";
	ParaBoy* bob = idMap[sender], *alice = idMap[receiver];
	string d, n;
	alice->getPublicKey(d, n);
	BigNum* bd = new BigNum(d), *bn = new BigNum(n);
	vector<BigNum> result = millionSendMessage(bob->getLevel(), make_pair(*bn, *bd));
	if (result.size() > 1) {
		msg = RSA::bigNumToStr(result[1]);
		me->setX(result[0]);
		sendCompare(receiver, msg);
	}
}

void GameLayer::onOpenBox(Ref * pSender, string id)
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
		//	�ر�socket����,��ɺ����ô�����onClose()
		_wsiClient->close();
	}
}

// ��ʼsocket����
void GameLayer::onOpen(cocos2d::network::WebSocket* ws)
{
    CCLOG("OnOpen");
	srand(time(0));
	//std::string name = StringUtils::format("wxy%d", rand() % 100);
	string name = me->getID();
	//me = new ParaBoy();
	//me->setId(name);
	me->initRSA();
	string d, n;
	me->getPublicKey(d, n);
	sendLogin(name, d, n);
}
 
// ������Ϣ������
void GameLayer::onMessage(cocos2d::network::WebSocket* ws, const cocos2d::network::WebSocket::Data& data)
{
        std::string textStr = data.bytes;
		
		Document doc;
		doc.Parse(textStr.c_str());
		const char *action = doc["action"].GetString();
		CCLOG(action);
		CCLOG(textStr.c_str());
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
		else if (strcmp(action, "remove") == 0){
			recvRemove(doc["msg"].GetObjectW());
		}
		else if (strcmp(action, "compare") == 0){
			recvCompare(doc["msg"].GetObjectW());
		}
		else if (strcmp(action, "compare2") == 0){
			recvCompare2(doc["msg"].GetObjectW());
		}
		else if (strcmp(action, "votes") == 0){
			recvVotes(doc["msg"].GetObjectW());
		}
		else if (strcmp(action, "votes2") == 0){
			recvVotes2(doc["msg"].GetObjectW());
		}
		else if (strcmp(action, "box")== 0){
			recvBox(doc["msg"].GetObjectW());
		}
		
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

void GameLayer::sendCompare(String targetId, String compMsg){
	Document doc;
	Document::AllocatorType& allocator = doc.GetAllocator();
	doc.SetObject();
	string tempStr = me->getID();
	doc.AddMember("id", JsonValue(StringRef(tempStr.c_str())).Move(), allocator);
	doc.AddMember("action", JsonValue("compare").Move(), allocator);
	Document msg;
	msg.SetObject();
	msg.AddMember("target", JsonValue(StringRef(targetId.getCString())).Move(), msg.GetAllocator());
	msg.AddMember("compMsg", JsonValue(StringRef(compMsg.getCString())).Move(), msg.GetAllocator());
	doc.AddMember("msg", msg, allocator);
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	sendMessage(buffer.GetString());
}

void GameLayer::sendCompare2(String targetId, list<string> &msgList){
	Document doc;
	Document::AllocatorType& allocator = doc.GetAllocator();
	doc.SetObject();
	string tempStr = me->getID();
	doc.AddMember("id", JsonValue(StringRef(tempStr.c_str())).Move(), allocator);
	doc.AddMember("action", JsonValue("compare2").Move(), allocator);
	Document msg;
	msg.SetObject();
	msg.AddMember("target", JsonValue(StringRef(targetId.getCString())).Move(), msg.GetAllocator());
	JsonValue msgs(kArrayType);
	for (string t : msgList){
		char * writable = new char[t.size() + 1];
		std::copy(t.begin(), t.end(), writable);
		writable[t.size()] = '\0';
		msgs.PushBack(JsonValue(StringRef(writable)).Move(), allocator);
	}
	msg.AddMember("msgs", msgs, msg.GetAllocator());
	doc.AddMember("msg", msg, allocator);
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	sendMessage(buffer.GetString());
}

void GameLayer::sendVotes(int voteArr[], int n){
	Document doc;
	Document::AllocatorType& allocator = doc.GetAllocator();
	doc.SetObject();
	string tempStr = me->getID();
	doc.AddMember("id", JsonValue(StringRef(tempStr.c_str())).Move(), allocator);
	doc.AddMember("action", JsonValue("votes").Move(), allocator);
	Document msg;
	msg.SetObject();
	JsonValue votes(kArrayType);
	for (int i = 0; i < n; i++){
		votes.PushBack(JsonValue(voteArr[i]).Move(), allocator);
	}
	msg.AddMember("votes", votes, msg.GetAllocator());
	doc.AddMember("msg", msg, allocator);
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	sendMessage(buffer.GetString());
}

void GameLayer::sendVotes2(int voteSum){
	Document doc;
	Document::AllocatorType& allocator = doc.GetAllocator();
	doc.SetObject();
	string tempStr = me->getID();
	doc.AddMember("id", JsonValue(StringRef(tempStr.c_str())).Move(), allocator);
	doc.AddMember("action", JsonValue("votes2").Move(), allocator);
	Document msg;
	msg.SetObject();
	msg.AddMember("voteSum", JsonValue(voteSum).Move(), msg.GetAllocator());
	doc.AddMember("msg", msg, allocator);
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	sendMessage(buffer.GetString());
}

void GameLayer::sendBox(int key){
	Document doc;
	Document::AllocatorType& allocator = doc.GetAllocator();
	doc.SetObject();
	string tempStr = me->getID();
	doc.AddMember("id", JsonValue(StringRef(tempStr.c_str())).Move(), allocator);
	doc.AddMember("action", JsonValue("box").Move(), allocator);
	Document msg;
	msg.SetObject();
	msg.AddMember("key", JsonValue(key).Move(), msg.GetAllocator());
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

void GameLayer::recvRemove(JsonValue msg){
	doRemove(msg["id"].GetString());
}

void GameLayer::recvCompare(JsonValue msg){
	string sourceId = msg["source"].GetString();
	string compMsg = msg["compMsg"].GetString();
	doCompare(sourceId, compMsg);
}
void GameLayer::recvCompare2(JsonValue msg){
	string sourceId = msg["source"].GetString();
	JsonValue msgs = msg["msgs"].GetArray();
	list<string> msgList;
	for (SizeType i = 0; i < msgs.Size(); i++){
		msgList.push_back(msgs[i].GetString());
	}
	doCompare2(sourceId, msgList);
}

void GameLayer::recvVotes(JsonValue msg){
	string sourceId = msg["source"].GetString();
	int vote = msg["vote"].GetInt();
	doVotes(sourceId, vote);
}

void GameLayer::recvVotes2(JsonValue msg){
	string sourceId = msg["source"].GetString();
	int voteSum = msg["voteSum"].GetInt();
	doVotes2(sourceId, voteSum);
}

void GameLayer::recvBox(JsonValue msg){
	int minNum = msg["minNum"].GetInt();
	JsonValue users = msg["users"].GetArray();
	list<string> userList;
	for (SizeType i = 0; i < users.Size();i++){
		userList.push_back(users[i].GetString());
	}
	doBox(minNum, userList);
}

void GameLayer::doLogin2(){

}

void GameLayer::doCreate(string id, int x, int y, string d, string n){
	CCLOG("doCreate:%s,%d,%d,%s,%s", id.c_str(), x, y, d.c_str(), n.c_str());
	stringstream ss;
	ss << userCount;
	string userImage = "icon" + ss.str() + ".jpg", backImage = "icon" + ss.str() + "b.jpg";
	ParaBoy* current = ParaBoy::create(userImage, backImage, CC_CALLBACK_1(GameLayer::createButton, this, id));
	if (id == me->getID()) {
		string e = me->getPrivateKey();
		current->setPrivateKey(e);
		me->removeFromParentAndCleanup(true);
		me = current;
	}
	current->setLevel(atoi(id.c_str()));
	current->setId(id);
	current->setPublicKey(d, n);
	current->setPosition(toRealLocation(x, y));
	this->menu->addChild(current, 1);

	idMap[id] = current;
	userCount++;
}

void GameLayer::doMove(string id, int x, int y){
	CCLOG("doMove:%s,%d,%d", id, x, y);
	MenuItemImage* current = idMap[id];
	current->runAction(MoveTo::create(0.3, toRealLocation(x, y)));
}

void GameLayer::doAuth(string sourceId, string authMsg) {
	//need wanghan to finish...
	CCLOG("doAuth:%s,%s", sourceId.c_str(), authMsg.c_str());
	ParaBoy* current = idMap[sourceId];
	string result = current->verifyCipher(authMsg);
	if (result == "helloworld")
		sendAuth2(sourceId, "1");
	else
		sendAuth2(sourceId, "0");
}

void GameLayer::doAuth2(string sourceId, string auth2Msg){
	//need wanghan to finish...

	CCLOG("doAuth2:%s,%s", sourceId.c_str(), auth2Msg.c_str());
	if (auth2Msg == "1")
		CCLOG("Success");
}

void GameLayer::doRemove(string id)
{
	ParaBoy* toRemove = idMap[id];
	toRemove->removeFromParentAndCleanup(true);
}

void GameLayer::doCompare(string sourceId, string compMsg){
	CCLOG("doCompare:%s,%s", sourceId.c_str(), compMsg.c_str());
	string pe = me->getPrivateKey(), pn, pd;
	me->getPublicKey(pd, pn);
	BigNum* e = new BigNum(pe), *n = new BigNum(pn);
	BigNum* msg = new BigNum(compMsg);
	vector<BigNum> result = millionDealMessage(me->getLevel(), make_pair(*n, *e), *msg);
	list<string> val;
	for (auto i : result) {
		val.push_back(RSA::bigNumToStr(i));
	}
	sendCompare2(sourceId, val);
}

void GameLayer::doCompare2(string sourceId, list<string> &msgList){
	CCLOG("doCompare2:%s,%s", sourceId.c_str(), msgList.front().c_str());
	vector<BigNum> val;
	for (string i : msgList) {
		BigNum temp(i);
		val.push_back(temp);
	}
	BigNum x(*(me->getX()));
	bool result = millionRich(me->getLevel(), x, val);
	if (!result)
		CCLOG("HIGHER");
	else
		CCLOG("LOWER");
}

void GameLayer::doVotes(string sourceId, int vote){
	CCLOG("doVotes:%s,%d", sourceId.c_str(), vote);
}

void GameLayer::doVotes2(string sourceId, int voteSum){
	CCLOG("doVotes2:%s,%d", sourceId.c_str(), voteSum);

}

void GameLayer::doBox(int minNumber, list<string> &userList){
	CCLOG("doBox:%d,%s",minNumber, userList.front().c_str());
}