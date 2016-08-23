//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO

#ifndef FUTURE_KEY_H_
#define FUTURE_KEY_H_

#include "src/Utilities/LerpValue.h"

#include <memory>

namespace eyegui
{
	// Forward declaration
	class Layout;
	class AssetManager;
	class RenderItem;
    class TextSimple;
    class Input;

	// Class for key
	class FutureKey
	{
	public:

		// Constructor
        FutureKey(Layout const * pLayout, AssetManager* pAssetManager, std::u16string letter);

		// Destructor
		virtual ~FutureKey();

		// Simple transform and size to fit layout size
		virtual void transformAndSize(int x, int y, int width, int height);

		// Update
        virtual void update(float tpf, Input const * pInput);

		// Draw
		virtual void draw(float alpha) const;

		// Reset
		virtual void reset();

	private:

		// Members
		Layout const * mpLayout;
		AssetManager* mpAssetManager;

		int mX;
		int mY;
		int mWidth;
		int mHeight;
        RenderItem const * mpKeyItem;
        RenderItem const * mpSuggestionBackgroundItem;
		RenderItem const * mpThresholdItem;
        std::unique_ptr<TextSimple> mupLetter;
        std::unique_ptr<TextSimple> mupSuggestion;
        LerpValue mFirstThreshold;

        const float SUGGESTION_HEIGHT = 0.2f;
	};
}

#endif // FUTURE_KEY_H_
