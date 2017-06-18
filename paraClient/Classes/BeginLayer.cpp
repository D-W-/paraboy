#include "BeginLayer.h"

Scene * BeginLayer::createScene()
{
	auto scene = Scene::create();
	auto layer = BeginLayer::create();
	scene->addChild(layer);
	return scene;
}

bool BeginLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();

	//±³¾°³õÊ¼»¯
	auto gameBackground = Sprite::create("bg0.jpg");
	gameBackground->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(gameBackground, 0);

	//ÊäÈë¿ò
	std::string pNormalSprite = "edit.png";
	auto _editName = EditBox::create(Size(250, 88), ui::Scale9Sprite::create(pNormalSprite));
	_editName->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height * 19 / 32));
	_editName->setFontName("Paint Boy");
	_editName->setFontSize(25);
	_editName->setFontColor(Color3B::WHITE);
	_editName->setPlaceholderFontColor(Color3B::WHITE);
	_editName->setMaxLength(8);
	_editName->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	_editName->setDelegate(this);
	addChild(_editName);

	return true;
}

void BeginLayer::editBoxEditingDidBegin(EditBox* editBox)
{
}

void BeginLayer::editBoxEditingDidEnd(EditBox* editBox)
{
}

void BeginLayer::editBoxTextChanged(EditBox* editBox, const std::string & text)
{
}

void BeginLayer::editBoxReturn(EditBox* editBox)
{
	string id(editBox->getText());
	//string id = "123";
	Director::sharedDirector()->replaceScene(CCTransitionCrossFade::create(0.3, GameLayer::createScene(id)));
}



