//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Simple block for displaying text. Delegating a text flow object for this
// task.

#ifndef TEXT_BLOCK_H_
#define TEXT_BLOCK_H_

#include "Block.h"

namespace eyegui
{
    class TextBlock : public Block
    {
    public:

        // Destructor
        virtual ~TextBlock();

        // Setter for content
        void setContent(std::u16string content);

        // Setter for key
        void setKey(std::string key);

    protected:

		// Factory is friend
		friend ElementFactory;

		// Protected constructor
		TextBlock(
			std::string id,
			std::vector<std::string> styles,
			Element* pParent,
			Layout const * pLayout,
			Frame* pFrame,
			AssetManager* pAssetManager,
			NotificationQueue* pNotificationQueue,
			float relativeScale,
			float border,
			bool dimming,
			bool adaptiveScaling,
			bool consumeInput,
			std::string backgroundFilepath,
			ImageAlignment backgroundAlignment,
			float innerBorder,
			FontSize fontSize,
			TextFlowAlignment alignment,
			TextFlowVerticalAlignment verticalAlignment,
			float textScale,
			std::u16string content,
			std::string key);

        // Drawing filled by subclasses
        virtual void specialDraw() const;

        // Transformation
        virtual void specialTransformAndSize();

    private:

        // Members
        std::string mKey;
        std::unique_ptr<TextFlow> mupTextFlow;
    };
}

#endif // TEXT_BLOCK_H_
