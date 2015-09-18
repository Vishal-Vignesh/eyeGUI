//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE 
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "DropButton.h"

#include "Layout.h"
#include "Helper.h"

namespace eyegui
{
	DropButton::DropButton(
		std::string id,
		std::string styleName,
		Element* pParent,
		Layout* pLayout,
		AssetManager* pAssetManager,
		float relativeScale,
		float border,
		std::string iconFilepath,
		float space) : BoxButton(
			id,
			styleName,
			pParent,
			pLayout,
			pAssetManager,
			relativeScale,
			border,
			iconFilepath,
			true)
	{
		mType = Type::DROP_BUTTON;

		// Fill members
		mSpace = space;

		// Initial values
		mInnerAlpha = 0;
		mInnerElementVisible = false;
	}

	DropButton::~DropButton()
	{
		// Nothing to do here
	}

	void DropButton::down(bool immediately)
	{
		// Super call
		BoxButton::down(immediately);

		mInnerElementVisible = true;

		// Immediately
		if (immediately)
		{
			mInnerAlpha = 1;
		}
	}

	void DropButton::up(bool immediately)
	{
		// Super call
		BoxButton::up(immediately);

		mInnerElementVisible = false;

		// Immediately
		if (immediately)
		{
			mInnerAlpha = 0;
		}
	}

	void DropButton::attachInnerElement(std::unique_ptr<Element> upElement)
	{
		// Condition to allow only one inner child
		if (mChildren.size() == 0)
		{
			// Should be the only one
			mChildren.push_back(std::move(upElement));

			// Register in layout for updating before everything else and drawing after everything else
			mpLayout->registerFrontElementForUpdateAndDraw(getInnerElement(), isDown());
		}
	}

	std::unique_ptr<Element> DropButton::replaceAttachedElement(
		Element* pTarget,
		std::unique_ptr<Element> upReplacement)
	{
		// Replace the only inner element
		// (assumption is guaranteed by condition in attachInnerElement method)
		std::unique_ptr<Element> upElement = BoxButton::replaceAttachedElement(
			pTarget,
			std::move(upReplacement));

		// Only continue, if element was really replaced
		if (upElement.get() != NULL)
		{
			// Unregistering of old inner element in layout as front
			// element and all children was done in layout during replacement

			// Register new element
			mpLayout->registerFrontElementForUpdateAndDraw(getInnerElement(), isDown());
		}

		return std::move(upElement);
	}

	InteractiveElement* DropButton::nextInteractiveElement()
	{
		// Go into inner element if visible
		if (mInnerElementVisible)
		{
			return mChildren[0].get()->internalNextInteractiveElement(NULL);
		}

		// Otherwise, do standard behaviour
		return BoxButton::nextInteractiveElement();
	}

	void DropButton::setActivity(bool active, bool setImmediately)
	{
		// Member set, so use it
		if (!active)
		{
			up(setImmediately);
		}

		// Super call after up(), because up() tests for active
		BoxButton::setActivity(active, setImmediately);
	}

	void DropButton::specialUpdate(float tpf, Input * pInput)
	{
		// Super call
		BoxButton::specialUpdate(tpf, pInput);

		// Update alpha of inner element
		if (mInnerElementVisible)
		{
			mInnerAlpha += tpf / mpLayout->getConfig()->animationDuration;
		}
		else
		{
			mInnerAlpha -= tpf / mpLayout->getConfig()->animationDuration;
		}
		mInnerAlpha = clamp(mInnerAlpha, 0, 1);

		mpLayout->setFrontElementAlpha(getInnerElement(), mInnerAlpha * mAlpha);
	}

	void DropButton::specialTransformAndSize()
	{
		// Super call
		BoxButton::specialTransformAndSize();

		// Determine used space
		int usedX, usedY, usedWidth, usedHeight;

		// Reserve some values
		int elemWidth, elemHeight;

		// Calculate available space
		if (getParent() != NULL
			&& getParent()->getOrientation() == Element::Orientation::HORIZONTAL)
		{
			// Horizontal origin, so search in upper or lower area of layout for space
			int upperSpace = mY;
			int lowerSpace = mpLayout->getLayoutHeight() - mY - mHeight;

			if (upperSpace > lowerSpace)
			{
				// Use upper space!
				elemHeight = (int)(upperSpace * mSpace);
				getInnerElement()->evaluateSize(mWidth, elemHeight, usedWidth, usedHeight);
				usedY = mY - usedHeight;
			}
			else
			{
				// Use lower space!
				elemHeight = (int)(lowerSpace * mSpace);
				getInnerElement()->evaluateSize(mWidth, elemHeight, usedWidth, usedHeight);
				usedY = mY + mHeight;
			}

			usedX = mX + (mWidth - usedWidth) / 2;
		}
		else
		{
			// Search left and right for space
			int leftSpace = mX;
			int rightSpace = mpLayout->getLayoutWidth() - mX - mWidth;

			if (leftSpace > rightSpace)
			{
				// Use left space!
				elemWidth = (int)(leftSpace * mSpace);
				getInnerElement()->evaluateSize(elemWidth, mHeight, usedWidth, usedHeight);
				usedX = mX - usedWidth;
			}
			else
			{
				// Use right space!
				elemWidth = (int)(rightSpace * mSpace);
				getInnerElement()->evaluateSize(elemWidth, mHeight, usedWidth, usedHeight);
				usedX = mX + mWidth;
			}

			usedY = mY + (mHeight - usedHeight) / 2;

		}

		// Tell inner element about it
		getInnerElement()->transformAndSize(usedX, usedY, usedWidth, usedHeight);
	}

	void DropButton::specialReset()
	{
		// Super call
		BoxButton::specialReset();

		mInnerAlpha = 0;
		mInnerElementVisible = false;
	}

	Element* DropButton::getInnerElement()
	{
		// Assumption is guaranteed by condition in attachInnerElement method
		return mChildren[0].get();
	}
}