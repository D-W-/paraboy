#pragma once
#ifndef __BEGINLAYER_H__
#define __BEGINLATER_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "GameLayer.h"

USING_NS_CC_EXT;
USING_NS_CC;

class BeginLayer: public cocos2d::Layer, public EditBoxDelegate
{
private:
	
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(BeginLayer);

	void editBoxEditingDidBegin(EditBox* editBox);

	// This method is called when an edit box loses focus after keyboard is hidden.  
	void editBoxEditingDidEnd(EditBox* editBox);

	// This method is called when the edit box text was changed.  
	void editBoxTextChanged(EditBox* editBox, const std::string& text);

	// This method is called when the return button was pressed or the outside area of keyboard was touched.  
	void editBoxReturn(EditBox* editBox);
};

#endif // __BEGINLAYER_H__
