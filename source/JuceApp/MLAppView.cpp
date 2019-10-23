
// MadronaLib: a C++ framework for DSP applications.
// Copyright (c) 2013 Madrona Labs LLC. http://www.madronalabs.com
// Distributed under the MIT license: http://madrona-labs.mit-license.org/

#include "MLAppView.h"

const Colour defaultColor = Colours::grey;

MLAppView::MLAppView(MLWidget::Listener* pResp, MLReporter* pRep) :
	MLWidgetContainer(this, this), // MLAppView is its own widget root. TODO use external structure for tree, share w procs
	mInitialized(false),
	mpResponder(pResp),
	mpReporter(pRep)
{
	//setContainer(this);
	MLWidget::setComponent(this);
	
	LookAndFeel::setDefaultLookAndFeel (&(mResources.mLookAndFeel));		
	Desktop::getInstance().setDefaultLookAndFeel(&(mResources.mLookAndFeel));
	
	setOpaque(false);
	setInterceptsMouseClicks (false, true);
}

MLAppView::~MLAppView()
{
	deleteAllChildren();
}

void MLAppView::initialize()
{
	mInitialized = true;
}

void MLAppView::doPropertyChangeAction(ml::Symbol p, const MLProperty & newVal)
{
	int propertyType = newVal.getType();
	switch(propertyType)
	{
		case MLProperty::kFloatProperty:
		{
		}
		break;
		case MLProperty::kTextProperty:
		{
		}
		break;
		case MLProperty::kSignalProperty:
		{
			const MLSignal& sig = newVal.getSignalValue();
			if(p == ml::Symbol("view_bounds"))
			{
				setWindowBounds(sig);
			}
		}
		break;
		default:
		break;
	}
}

void MLAppView::addPropertyView(ml::Symbol modelProp, MLWidget* widget, ml::Symbol widgetProp)
{
	if(modelProp && widget && widgetProp)
		mpReporter->addPropertyViewToMap(modelProp, widget, widgetProp);
}

void MLAppView::addWidgetToView(MLWidget* pW, const MLRect& r, ml::Symbol name = ml::Symbol())
{
	////debug() << "ADDING widget: " << name << "\n";
	addWidget(pW, name);
	pW->setGridBounds(r);
	pW->addListener(mpResponder);
	addAndMakeVisible(pW->getComponent());
}

#pragma mark component add utility methods
//

MLDial* MLAppView::addDial(const char * displayName, const MLRect & r, const ml::Symbol p,
	const Colour& color, const float sizeMultiplier)
{
	MLDial* dial = new MLDial(this);
	dial->setTargetPropertyName(p);
	dial->setSizeMultiplier(sizeMultiplier);
	dial->setDialStyle (MLDial::Rotary);
	dial->setFillColor(color); 		

	addWidgetToView(dial, r, p);
	
	addPropertyView(p, dial, ml::Symbol("value"));
	
	if (strcmp(displayName, ""))
	{
		addLabelAbove(dial, displayName, "");		
	}
	return dial;
}

MLMultiSlider* MLAppView::addMultiSlider(const char * displayName, const MLRect & r, const ml::Symbol propName, 
	int numSliders, const Colour& color)
{
	MLMultiSlider* slider = new MLMultiSlider(this);
	slider->setNumSliders(numSliders);
	slider->setTargetPropertyName(propName);
	slider->setFillColor(color);
	addWidgetToView(slider, r, propName);
	
	for(int i=0; i<numSliders; ++i)
	{
		addPropertyView(ml::textUtils::addFinalNumber(propName, i), slider, ml::textUtils::addFinalNumber("value", i));
	}
	
	if (strcmp(displayName, ""))
	{
		addLabelAbove(slider, displayName, "");
	}
	return slider;
}

MLMultiButton* MLAppView::addMultiButton(const char * displayName, const MLRect & r, const ml::Symbol propName, 
	int n, const Colour& color)
{
	MLMultiButton* b = new MLMultiButton(this);
	b->setNumButtons(n);
	b->setTargetPropertyName(propName);
	b->setFillColor(color);
	addWidgetToView(b, r, propName);
	
	for(int i=0; i<n; ++i)
	{
		addPropertyView(ml::textUtils::addFinalNumber(propName, i), b, ml::textUtils::addFinalNumber("value", i));
	}
	
	if (strcmp(displayName, ""))
	{
		addLabelAbove(b, displayName, "");
	}
	return b;
}

MLButton* MLAppView::addToggleButton(const char* displayName, const MLRect & r, const ml::Symbol propName,
                                     const Colour& color, const float sizeMultiplier)
{
	MLButton* button = new MLToggleButton(this);
	button->setSizeMultiplier(sizeMultiplier);
	button->setTargetPropertyName(propName);
	button->setFillColor(color);
	addWidgetToView(button, r, propName);
	addPropertyView(propName, button, ml::Symbol("value"));
	
	if (strcmp(displayName, ""))
	{
		addLabelAbove(button, displayName, propName+("_label"), sizeMultiplier);
	}
    
	return button;
}

MLButton* MLAppView::addTriToggleButton(const char* displayName, const MLRect & r, const ml::Symbol propName,
                                     const Colour& color, const float sizeMultiplier)
{
	MLButton* button = new MLTriToggleButton(this);
	button->setSizeMultiplier(sizeMultiplier);
	button->setTargetPropertyName(propName);
	button->setFillColor(color);
	addWidgetToView(button, r, propName);
	addPropertyView(propName, button, ml::Symbol("value"));
	
	if (strcmp(displayName, ""))
	{
		addLabelAbove(button, displayName, propName+("_label"), sizeMultiplier);
	}
    
	return button;
}

MLPanel* MLAppView::addPanel(const MLRect & r, const Colour& color)
{
	MLPanel* b = new MLPanel(this);
	b->setBackgroundColor(color);
	addWidgetToView(b, r);
	return b;
}

MLDebugDisplay* MLAppView::addDebugDisplay(const MLRect & r)
{
	MLDebugDisplay* b = new MLDebugDisplay(this);
	addWidgetToView(b, r);
	return b;
}

MLDrawableButton* MLAppView::addRawImageButton(const MLRect & r, const char * name, 
	const Colour& color, const Drawable* normalImg)
{
	MLDrawableButton* b = new MLDrawableButton(this);
	b->setTargetPropertyName(name);
	b->setProperty("toggle", false);
	b->setImage(normalImg);
	addWidgetToView(b, r, name);
	return b;
}

MLTextButton* MLAppView::addTextButton(const char * displayName, const MLRect & r, const char * name, const Colour& color)
{	
	MLTextButton* b = new MLTextButton(this);
	b->setTargetPropertyName(name);
	b->setProperty("toggle", false);
	b->setFillColor(color);
	b->setPropertyImmediate("text", displayName);
	addWidgetToView(b, r, name);
	return b;
}

MLMenuButton* MLAppView::addMenuButton(const char * displayName, const MLRect & r, const char * menuName, const Colour& color)
{	
	MLMenuButton* b = new MLMenuButton(this);
	b->setTargetPropertyName(menuName);
	b->setFillColor(color);
	b->setProperty("text", "---");
	addWidgetToView(b, r, menuName);
	addPropertyView(menuName, b, ml::Symbol("text"));
	
	if (strcmp(displayName, ""))
	{
		addLabelAbove(b, displayName, "");
	}
	return b;
}

MLLabel* MLAppView::addLabel(const char* displayName, const MLRect & r, const float sizeMultiplier, int font)
{
	MLLabel* label = new MLLabel(this, displayName);
	if (label)
	{
		MLLookAndFeel* myLookAndFeel = (&(getRootViewResources(this).mLookAndFeel));
		
		if (strcmp(displayName, ""))
		{
			Font f = myLookAndFeel->getFont(font);
			label->setFont(f);		
			label->setSizeMultiplier(sizeMultiplier);
			label->setJustification(Justification::centred);
		}
		
		label->setResizeToText(true);
		addWidgetToView(label, r, "");
	}
////debug() << "added label " << displayName << ", rect" << r << "\n";
	return label;
}

MLLabel* MLAppView::addLabelAbove(MLWidget* c, const char* displayName, ml::Symbol widgetName, const float sizeMultiplier, int font, Vec2 offset)
{
	MLLabel* label = new MLLabel(this, displayName);
	if (label)
	{
		MLLookAndFeel* myLookAndFeel = (&(getRootViewResources(this).mLookAndFeel));
		float labelHeight = myLookAndFeel->getLabelHeight()*sizeMultiplier;
		
		//float m = myLookAndFeel->getMargin();
		label->setResizeToText(true);
		
		MLRect r (c->getGridBounds());
		r.setHeight(labelHeight);
		r.stretchWidthTo(1.);
		
		Font f = myLookAndFeel->getFont(font);
		label->setFont(f);		
		label->setSizeMultiplier(sizeMultiplier);
		label->setJustification(Justification::centred);		
	
		MLRect rr = r.translated(Vec2(0, -labelHeight*c->getLabelVerticalOffset()) + offset);
		
		// quantize label top to 1/8 unit grid
		float gridSize = 0.125;
		float top = rr.top();
		top = (roundf(top/gridSize))*gridSize;
		rr.setTop(top);
		
		addWidgetToView(label, rr, widgetName);
	}
	return label;
}

MLDrawing* MLAppView::addDrawing(const MLRect & r)
{
	MLDrawing* drawing = new MLDrawing(this);
	addWidgetToView(drawing, r);
	return drawing;
}

MLProgressBar* MLAppView::addProgressBar(const MLRect & r)
{
	MLProgressBar* pb = new MLProgressBar(this);
	addWidgetToView(pb, r);
	return pb;
}

void MLAppView::resized()
{
	MLLookAndFeel* myLookAndFeel = (&(getRootViewResources(this).mLookAndFeel));
	int u = myLookAndFeel->getGridUnitSize(); 

  int w = getWidth();
  int h = getHeight();
  
	for(auto it = mWidgets.begin(); it != mWidgets.end(); ++it)
	{
		MLWidget* w = (*it).second;
		MLRect r = w->getGridBounds();
		MLRect scaled = (r*u).getIntPart();
		if (!w->wantsResizeLast())
		{
			w->setWidgetGridUnitSize(u);
			w->resizeWidget(scaled, u);
		}
	}
	
	for(auto it = mWidgets.begin(); it != mWidgets.end(); ++it)
	{
		MLWidget* w = (*it).second;
		MLRect r = w->getGridBounds();
		MLRect scaled = (r*u).getIntPart();
		if (w->wantsResizeLast())
		{
			w->setWidgetGridUnitSize(u);
			w->resizeWidget(scaled, u);
		}
	}
}

void MLAppView::setViewBoundsProperty()
{
	if(!mInitialized) return;
	
	MLSignal bounds(4);
	ComponentPeer* p = getPeer();
	if(!p) return;	
	Rectangle<int> peerBounds = p->getBounds();	
	bounds[0] = peerBounds.getX();
	bounds[1] = peerBounds.getY();
	bounds[2] = peerBounds.getWidth();
	bounds[3] = peerBounds.getHeight();

	// set property for saving, avoiding loop
	setPropertyImmediateExcludingListener("view_bounds", bounds, this);
}

void MLAppView::setWindowBounds(const MLSignal& bounds)
{
	ComponentPeer* p = getPeer();
	if(!p) return;
	bool fullScreen = 0;
	p->setBounds(Rectangle<int>(bounds[0], bounds[1], bounds[2], bounds[3]), fullScreen);
}

void MLAppView::windowMoved()
{
	setViewBoundsProperty();
}

void MLAppView::windowResized()
{
	setViewBoundsProperty();
}

