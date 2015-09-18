//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE 
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Parser for bricks. Delegates element parser for actual work.

#ifndef BRICK_PARSER_H_
#define BRICK_PARSER_H_

#include "ElementParser.h"

namespace eyegui
{
	// Forward declaration
	class GUI;
	class Layout;

	class BrickParser
	{
	public:

		// Constructor
		BrickParser();

		// Destructor
		virtual ~BrickParser();

		// Parsing
		std::unique_ptr<elementsAndIds> parse(Layout* pLayout, AssetManager* pAssetManager, Element* pParent, std::string filepath);

	private:

		// Member
		ElementParser mElementParser;
	};
}

#endif // BRICK_PARSER_H_
