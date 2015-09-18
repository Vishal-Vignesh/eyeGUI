//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE 
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Picture.h"

namespace eyegui
{
	Picture::Picture(
		std::string id,
		std::string styleName,
		Element* pParent,
		Layout* pLayout,
		AssetManager* pAssetManager,
		float relativeScale,
		float border,
		std::string filepath,
		PictureAlignment alignment) : Element(
			id,
			styleName,
			pParent,
			pLayout,
			pAssetManager,
			relativeScale,
			border)
	{
		mType = Type::PICTURE;

		// Fill members
		mpQuad = mpAssetManager->fetchRenderItem(shaders::Type::PICTURE, meshes::Type::QUAD);
		mpImage = mpAssetManager->fetchTexture(filepath);
		mAlignment = alignment;

		// Aspect ratio of border should be preserved if necessary
		if (mAlignment == PictureAlignment::ORIGINAL)
		{
			mBorderAspectRatio = (float)(mpImage->getWidth()) / (float)(mpImage->getHeight());
		}

	}

	Picture::~Picture()
	{
		// Nothing to do
	}

	void Picture::evaluateSize(
		int availableWidth,
		int availableHeight,
		int& rWidth,
		int& rHeight) const
	{
		if (mAlignment == PictureAlignment::ORIGINAL)
		{
			float availableAspectRatio = ((float)availableWidth) / ((float)availableHeight);
			float aspectRatio = (float)(mpImage->getWidth()) / (float)(mpImage->getHeight());

			if (availableAspectRatio < aspectRatio)
			{
				// Horizontal space less than necessary
				rWidth = availableWidth;

				// Adjust vertical size
				rHeight = (int)((float)rWidth / aspectRatio);
			}
			else
			{
				// Vertical space less than necessary
				rHeight = availableHeight;

				// Adjust horizontal size
				rWidth = (int)((float)rHeight * aspectRatio);
			}
		}
		else
		{
			rWidth = availableWidth;
			rHeight = availableHeight;
		}
	}

	void Picture::specialUpdate(float tpf, Input* pInput)
	{
		// If mouse over picture, consume input (copied from INTERACTIVE element)
		if (pInput != NULL && !pInput->mouseUsed)
		{
			if (pInput->mouseCursorX >= mX
				&& pInput->mouseCursorX <= mX + mWidth
				&& pInput->mouseCursorY >= mY
				&& pInput->mouseCursorY <= mY + mHeight)
			{
				pInput->mouseUsed = true;
			}
		}
	}

	void Picture::specialDraw() const
	{
		// Bind render item before setting values and drawing
		mpQuad->bind();

		// Fill matrix in shader
		mpQuad->getShader()->fillValue("matrix", mDrawMatrix);

		// Fill alpha
		mpQuad->getShader()->fillValue("alpha", mAlpha);

		// Fill activity
		mpQuad->getShader()->fillValue("activity", mActivity);

		// Bind image
		mpImage->bind(0);

		// Draw render item
		mpQuad->draw();
	}

	void Picture::specialTransformAndSize()
	{
		// Nothing to do
	}

	void Picture::specialReset()
	{
		// Nothing to do
	}
}