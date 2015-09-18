//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE 
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Grid offers rows and columns to arrange elements. Each row can have a different
// count of columns. Sizes are given in percent and verified by the parser.

#ifndef GRID_H_
#define GRID_H_

#include "Block.h"

#include <vector>
#include <memory>

namespace eyegui
{
	class Grid : public Block
	{
	public:

		// Constructor
		Grid(
			std::string id,
			std::string styleName,
			Element* pParent,
			Layout* pLayout,
			AssetManager* pAssetManager,
			float relativeScale,
			float border,
			int rows);

		// Destructor
		virtual ~Grid();

		// Get element (returns null if no element or does not exist)
		Element* getElement(int row, int column);

		// Prepare columns
		void prepareColumns(int row, int columns);

		// Attach element in cell
		void attachElement(
			int row,
			int column,
			std::unique_ptr<Element> upElement);

		// Check whether grid is completely filled
		bool gridCompletelyFilledBySizes() const;

		// Cell sizes
		void setRelativeHeightOfRow(int row, float height);
		void setRelativeWidthOfCell(int row, int column, float width);
		float getRelativeHeight(int row) const;
		float getRelativeWidth(int row, int column) const;

		// Tries to fetch next interactive element for selecting, returns NULL if fails
		virtual InteractiveElement* internalNextInteractiveElement(Element const * pChildCaller);

	protected:

		// Updating filled by subclasses
		virtual void specialUpdate(float tpf, Input* pInput);

		// Drawing filled by subclasses
		virtual void specialDraw() const;

		// Transformation filled by subclasses
		virtual void specialTransformAndSize();

	private:

		// Members
		std::vector<std::vector<int> > mCellIndices;
		std::vector<std::vector<float> > mElementWidths;
		std::vector<float> mElementHeights;
		std::vector<int> mColumns;
		int mRows = 0;
	};
}

#endif // GRID_H_