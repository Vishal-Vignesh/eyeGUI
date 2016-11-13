//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Layout manages frames with elements. There is one main frame which is
// screen filling and a free number of floating frames. Does mapping of id to
// element pointer. Is owner of the stylesheet used by the roots in frames
// and all their children. Has notification queue where elements in frames
// can enqueue their notifications.

#ifndef LAYOUT_H_
#define LAYOUT_H_

#include "include/eyeGUI.h"
#include "Frame.h"
#include "Style.h"
#include "Config.h"
#include "Parser/StylesheetParser.h"
#include "Parser/BrickParser.h"
#include "NotificationQueue.h"
#include "src/Utilities/LerpValue.h"

#include <memory>
#include <map>
#include <set>
#include <utility>

namespace eyegui
{
    // Forward declaration
    class GUI;

    class Layout
    {
    public:

        // Friends (may use private methods)
        friend class NotificationQueue;

        // Constructor
        Layout(std::string name, GUI const * pGUI, AssetManager* pAssetManager, std::string stylesheetFilepath);

        // Destructor
        virtual ~Layout();

        // Get name
        std::string getName() const;

        // Updating
        void update(float tpf, Input* pInput);

        // Drawing
        void draw() const;

        // Make resize necessary
        void makeResizeNecessary(bool force, bool instant = false);

        // Attach element to main frame as root
        void attachElementToMainFrameAsRoot(
            std::unique_ptr<Element> upElement,
            std::unique_ptr<std::map<std::string, Element*> > upIds);

        // Get notificaton queue
        NotificationQueue* getNotificationQueue() const;

        // Get pointer to config of owning GUI
        Config const * getConfig() const;

        // Get used character set
        CharacterSet getCharacterSet() const;

        // Get main frame of layout
        Frame* getMainFrame();

        // Get layout resolution
        uint getLayoutWidth() const;
        uint getLayoutHeight() const;

        // Get time from GUI
        float getAccPeriodicTime() const;

        // Get pointer to styles of this layout. Returns NULL if not found
        Style const * getStyleFromStylesheet(std::string styleName) const;

        // Get string content from localization
        std::u16string getContentFromLocalization(std::string key) const;

        // Returns set of names of the available styles for this layout
        std::set<std::string> getNamesOfAvailableStyles() const;

        // Set visibility
        void setVisibility(bool visible, bool fade);

        // One may want not to use input for this layout
        void useInput(bool useInput);

        // Get relative position and size of element
        RelativePositionAndSize getRelativePositionAndSizeOfElement(std::string id) const;

        // Get absolute pixel position and size of element
        AbsolutePositionAndSize getAbsolutePositionAndSizeOfElement(std::string id) const;

        // Set element activity
        void setElementActivity(std::string id, bool active, bool fade);

        // Check activity of element
        bool isElementActive(std::string id) const;

        // Set whether element is dimming
        void setElementDimming(std::string id, bool dimming);

        // Set whether element is marking
        void setElementMarking(std::string id, bool marking, int depth);

        // Set hiding of element
        void setElementHiding(std::string id, bool hidden);

        // Check whether element is dimming
        bool isElementDimming(std::string id) const;

        // Check whether element is marking
        bool isElementMarking(std::string id) const;

        // Set style of element
        void setStyleOfElement(std::string id, std::string style);

        // Set interactive element as highlighted
        void highlightInteractiveElement(std::string id, bool doHighlight);

        // Set icon of icon element
        void setIconOfIconElement(std::string id, std::string iconFilepath);

        // Set icon of icon element
        void setIconOfIconElement(
            std::string id,
            std::string name,
            int width,
            int height,
            ColorFormat format,
            unsigned char const * pIconData,
            bool flipY);

        // Set image of picture
        void setImageOfPicture(
            std::string id,
            std::string name,
            int width,
            int height,
            ColorFormat format,
            unsigned char const * pData,
            bool flipY);

		// Set image of picture
		void setImageOfPicture(
			std::string id,
			std::string name);

		// Fetch image
		void fetchImage(
			std::string name,
			int width,
			int height,
			ColorFormat format,
			unsigned char const * pData,
			bool flipY);

        // Interact with interactive element
        void interactWithInteractiveElement(std::string id);

        // Hit button
        void hitButton(std::string id);

        // Button down
        void buttonDown(std::string id, bool immediately);

        // Button up
        void buttonUp(std::string id, bool immediately);

        // Is button a switch?
        bool isButtonSwitch(std::string id) const;

        // Penetrate sensor
        void penetrateSensor(std::string id, float amount);

        // Set content of text block
        void setContentOfTextBlock(std::string id, std::u16string content);
        void setContentOfTextBlock(std::string id, std::string content);

        // Set key of text block
        void setKeyOfTextBlock(std::string id, std::string key);

        // Set fast typing for keyboard
        void setFastTypingOfKeyboard(std::string id, bool useFastTyping);

        // Set case of keyboard
        void setCaseOfKeyboard(std::string id, KeyboardCase keyboardCase);

        // Get count of keymaps in keyboard
        uint getCountOfKeymapsInKeyboard(std::string id) const;

        // Set keymap of keyboard by index
        void setKeymapOfKeyboard(std::string id, uint keymapIndex);

        // Suggest words
        void suggestWords(std::string id, std::u16string input, uint dictionaryIndex, std::u16string& rBestSuggestion);
        void suggestWords(std::string id, std::string input, uint dictionaryIndex, std::string& rBestSuggestion);

        // Clear suggestions
        void clearSuggestions(std::string id);

        // Set space of flow
        void setSpaceOfFlow(std::string id, float space);

        // Set progress in progress bar
        void setProgress(std::string id, float progress);

		// Move cursor a certain amount of letters
		void moveCursorOverLettersInTextEdit(std::string id, int letterCount);

		// Move cursor a certain amount of words
		void moveCursorOverWordsInTextEdit(std::string id, int wordCount);

        // Move cursor to start
        void moveCursorToStartInTextEdit(std::string id);

        // Move cursor to end
        void moveCursorToEndInTextEdit(std::string id);

		// Add content at cursor
		void addContentAtCursorInTextEdit(std::string id, std::u16string content);

		// Set content
		void setContentOfTextEdit(std::string id, std::u16string content);

		// Get content
		std::u16string getContentOfTextEdit(std::string id) const;

		// Delete content
		void deleteContentAtCursorInTextEdit(std::string id, int letterCount);

		// Get content of acitve entity
		std::u16string getActiveEntityContentInTextEdit(std::string id) const;

        // Add brick to stack
        void addBrickToStack(
            std::string id,
            std::string filepath,
            std::map<std::string, std::string> idMapper);

        // Register button listener
        void registerButtonListener(std::string id, std::weak_ptr<ButtonListener> wpListener);

        // Register sensor listener
        void registerSensorListener(std::string id, std::weak_ptr<SensorListener> wpListener);

        // Register keyboard listener
        void registerKeyboardListener(std::string id, std::weak_ptr<KeyboardListener> wpListener);

        // Register word suggest listener
        void registerWordSuggestListener(std::string id, std::weak_ptr<WordSuggestListener> wpListener);

        // Select interactive element by id
        void selectInteractiveElement(std::string id);

        // Returns, whether successfull
        bool selectInteractiveElement(InteractiveElement* pInteractiveElement);

        // Deselect current element
        void deselectInteractiveElement();

        // Interact with selected interactive element
        void interactWithSelectedInteractiveElement();

        // Select next interactive element
        bool selectNextInteractiveElement();

        // Reset elements
        void resetElements();

        // Check for existence of id
        bool checkForId(std::string id) const;

		// Flash element
		void flash(std::string id);

        // Check whether element is highlighted
        bool isInteractiveElementHighlighted(std::string id) const;

        // Change value of style attribute
        void setValueOfStyleAttribute(std::string styleName, std::string attribute, std::string value);

        // Replace any element with block
        void replaceElementWithBlock(
            std::string id,
            bool consumeInput,
            std::string backgroundFilepath,
            ImageAlignment backgroundAlignment,
            bool fade);

        // Replace any element with picture
        void replaceElementWithPicture(std::string id, std::string filepath, ImageAlignment alignment, bool fade);

        // Replace any element with blank
        void replaceElementWithBlank(std::string id, bool fade);

        // Replace any element with circle button
        void replaceElementWithCircleButton(std::string id, std::string iconFilepath, std::u16string desc, std::string descKey, bool isSwitch, bool fade);

        // Replace any element with box button
        void replaceElementWithBoxButton(std::string id, std::string iconFilepath, std::u16string desc, std::string descKey, bool isSwitch, bool fade);

        // Replace any element with sensor
        void replaceElementWithSensor(std::string id, std::string iconFilepath, std::u16string desc, std::string descKey, bool fade);

        // Replace any element with text block
        void replaceElementWithTextBlock(
            std::string id,
            bool consumeInput,
            std::string backgroundFilepath,
            ImageAlignment backgroundAlignment,
            FontSize fontSize,
            TextFlowAlignment alignment,
            TextFlowVerticalAlignment verticalAlignment,
            float textScale,
            std::u16string content,
            float innerBorder,
            std::string key,
            bool fade);

        // Replace any element with a brick of elements
        void replaceElementWithBrick(std::string id, std::string filepath, std::map<std::string, std::string> idMapper, bool fade);

        // Add floating frame with brick
        uint addFloatingFrameWithBrick(
            std::string filepath,
            float relativePositionX,
            float relativePositionY,
            float relativeSizeX,
            float relativeSizeY,
            std::map<std::string, std::string> idMapper,
            bool visible,
            bool fade);

        // Set visibilty of floating frame
        void setVisibiltyOfFloatingFrame(uint frameIndex, bool visible, bool fade);

        // Reset elements of floating frame
        void resetFloatingFramesElements(uint frameIndex);

        // Remove floating frame
        void removeFloatingFrame(uint frameIndex, bool fade);

        // Translate floating frame
        void translateFloatingFrame(uint frameIndex, float translateX, float translateY);

        // Scale floating frame
        void scaleFloatingFrame(uint frameIndex, float scaleX, float scaleY);

        // Set position of floating frame
        void setPositionOfFloatingFrame(uint frameIndex, float relativePositionX, float relativePositionY);

        // Set size of floating frame
        void setSizeOfFloatingFrame(uint frameIndex, float relativeSizeX, float relativeSizeY);

        // Move floating frame to front
        void moveFloatingFrameToFront(uint frameIndex);

        // Move floating frame to back
        void moveFloatingFrameToBack(uint frameIndex);

        // Get relative position and size of floating frame
        RelativePositionAndSize getRelativePositionAndSizeOfFloatingFrame(unsigned int frameIndex) const;

        // Get absolute position and size of floating frame
        AbsolutePositionAndSize getAbsolutePositionAndSizeOfFloatingFrame(unsigned int frameIndex) const;

        // Get visibility of descriptions
        DescriptionVisibility getDescriptionVisibility() const;

        // Get font size for descriptions
        FontSize getDescriptionFontSize() const;

        // Experimental

        // Register future keyboard listener
        void registerFutureKeyboardListener(std::string id, std::weak_ptr<eyegui_experimental::FutureKeyboardListener> wpListener);

		// Set suggestions of line
		void setFutureKeyboardLineSuggestions(
			std::string id,
			std::u16string suggestionA,
			std::u16string suggestionB,
			std::u16string suggestionC);

        // Set suggestion in future key
        void setFutureKeySuggestion(
            std::string id,
            std::string keyId,
            std::u16string suggestion);

		// Get content of future keyboard
		std::u16string getFutureKeyboardContent(std::string id) const;

		// Set next future keyboard sentence
		void nextFutureKeyboardSentence(
			std::string id,
			std::u16string sentence);

		// Clear predisplay of future keyboard
		void clearFutureKeyboardPredisplay(std::string id);

    private:

        // Resize function
        void internalResizing(bool force, bool instant = false);

        // Fetch pointer to element by id
        Element* fetchElement(std::string id) const;

        // Internal replacement helper, returns whether successful
        bool replaceElement(Element* pTarget, std::unique_ptr<Element> upElement, bool fade);

        // Insert id of element
        void insertId(Element* pElement);

        // Insert map of ids
        void insertIds(std::unique_ptr<idMap> upIdMap);

        // Fetch pointer to frame
        Frame* fetchFloatingFrame(uint frameIndex) const;

        // Move floating frame by id
        void moveFloatingFrame(int oldIndex, int newIndex);

        // Members
        std::string mName;
        GUI const * mpGUI;
        AssetManager* mpAssetManager;
        std::unique_ptr<Frame> mupMainFrame;
        std::unique_ptr<std::map<std::string, Element*> > mupIds;
        std::vector<std::unique_ptr<Frame> > mFloatingFrames;
        std::vector<int> mFloatingFramesOrderingIndices;
        std::vector<int> mDyingFloatingFramesIndices;
        LerpValue mAlpha;
        bool mVisible;
        bool mResizeNecessary;
        bool mUseInput;
        std::unique_ptr<std::map<std::string, Style> > mupStyles;
        InteractiveElement* mpSelectedInteractiveElement;
        std::unique_ptr<NotificationQueue> mupNotificationQueue;
        bool mForceResize;
    };
}

#endif // LAYOUT_H_
