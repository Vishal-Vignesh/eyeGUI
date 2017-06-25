//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Style class has maps of style property pointers. Some of those properties
// are just references to properties from parent classes and other are owned
// by this. If a property is owned, propagation of values of its type from
// parent class is stopped. Children are optionally stored as shared pointer,
// because this is desired for the style tree but not for element style
// classes. Those are already shared pointer in the corresponding element
// and would survive the deletion of the element if stored as shared here.

#ifndef STYLE_CLASS_H_
#define STYLE_CLASS_H_

#include "include/eyeGUI.h"
#include "src/Style/StyleProperty.h"
#include "src/Utilities/Helper.h"
#include "src/Defines.h"

#include <functional>
#include <map>

namespace eyegui
{
	// (ADD NEW PROPERTY TYPES HERE AS MAPPING)
	// Compile time mapping between style property and type
	template<typename Type> struct StylePropertyValue;
	template<> struct StylePropertyValue<StylePropertyFloat> { typedef float type; };
	template<> struct StylePropertyValue<StylePropertyVec4> { typedef glm::vec4 type; };
	template<> struct StylePropertyValue<StylePropertyString> { typedef std::string type; };

	// Style class, must be built by builder
	class StyleClass : public std::enable_shared_from_this<StyleClass> // enable shared pointer creation from this
	{
	public:

		// Add child
		std::shared_ptr<StyleClass> addChild(bool storeShared, std::string name = "");

		// Fetch value from property
		// template<typename Type> TODO

		// Fetch float property
		std::shared_ptr<const StyleProperty<float> > fetchProperty(StylePropertyFloat type) const;

		// Get float value from property
		float getValue(StylePropertyFloat type) const;

		// Fetch vec4 property
		std::shared_ptr<const StyleProperty<glm::vec4> > fetchProperty(StylePropertyVec4 type) const;

		// Get vec4 value from property
		glm::vec4 getValue(StylePropertyVec4 type) const;

		// Fetch string property
		std::shared_ptr<const StyleProperty<std::string> > fetchProperty(StylePropertyString type) const;

		// Get string value from property
		std::string getValue(StylePropertyString type) const;

		// Set value of float porperty and propagate to children (TODO: maybe do not set it at value parsing failure)
		void setValue(StylePropertyFloat type, std::string value) { setValue(type, stringToFloat(value)); }
		void setValue(StylePropertyFloat type, float value);
		
		// Set vec4 value and propagate to children
		void setValue(StylePropertyVec4 type, std::string value) { setValue(type, stringHexRGBAToVec4RGBA(value)); }
		void setValue(StylePropertyVec4 type, glm::vec4 value);

		// Set string value and propagate to children
		void setValue(StylePropertyString type, std::string value);

		// Fetch this or child by name. Return empty pointer if not found
		std::shared_ptr<StyleClass> fetchThisOrChild(std::string name);

		// Get name of this style class
		std::string getName() const;

	private:

		// Builder is friend class
		friend class StyleClassBuilder;

		// Constructor
		StyleClass(std::string name, std::weak_ptr<const StyleClass> wpParent, bool& rPleaseFill);

		// Private copy constuctor
		StyleClass(StyleClass const&) {}

		// Private assignment operator
		StyleClass& operator = (StyleClass const&) { return *this; }

		// Called only by builder
		void fill();

		// #################################
		// ### MAPS HELPER FOR TEMPLATES ###
		// #################################

		// Get map corresponding to property type
		template<typename Type> std::map<Type, std::shared_ptr<StyleProperty<typename StylePropertyValue<Type>::type> > >* getMap() { return NULL; } // fallback
		template<> std::map<StylePropertyFloat, std::shared_ptr<StyleProperty<typename StylePropertyValue<StylePropertyFloat>::type> > >* getMap() { return &mFloatMap; }
		template<> std::map<StylePropertyVec4, std::shared_ptr<StyleProperty<typename StylePropertyValue<StylePropertyVec4>::type> > >* getMap() { return &mVec4Map; }
		template<> std::map<StylePropertyString, std::shared_ptr<StyleProperty<typename StylePropertyValue<StylePropertyString>::type> > >* getMap() { return &mStringMap; }

		// Get style property by type
		template<typename Type>
		std::shared_ptr<StyleProperty<typename StylePropertyValue<Type>::type> > getStyleProperty(Type type) { return (*getMap<Type>())[type]; }

		// Set property pointer in map
		template<typename Type>
		void setMapValue(Type type, std::shared_ptr<StyleProperty<typename StylePropertyValue<Type>::type> > spProperty) { (*getMap<Type>())[type] = spProperty; }

		// ####################
		// ### VALUE SETTER ###
		// ####################

		// Set value as template implementation. Does create new property if not existing in instance
		template<typename Type>
		void genericSetValue(Type type, typename StylePropertyValue<Type>::type value)
		{
			// Check whether property is owned by me
			auto spStoredProperty = this->getStyleProperty(type);
			auto spStoredPropertyStyleClass = spStoredProperty->getStyleClass().lock();
			bool owned = false;
			if (spStoredPropertyStyleClass)
			{
				// Compare raw pointers
				owned = spStoredPropertyStyleClass.get() == this;
			}

			// Set value by either creating a property or just changing it
			if (owned)
			{
				// Just set it
				spStoredProperty->set(value);
			}
			else
			{
				// Add new owned property while copying constraint from existing since it is of the same type
				typedef StyleProperty<typename StylePropertyValue<Type>::type> PropertyType;
				spStoredProperty = std::shared_ptr<PropertyType>(new PropertyType(shared_from_this(), value, spStoredProperty->getConstraint(), true));
				this->setMapValue(type, spStoredProperty);

				// Propagate it to children
				for (auto& rwpChild : this->mWeakChildren)
				{
					if (auto rspChild = rwpChild.lock())
					{
						rspChild->genericPropagateProperty(type, spStoredProperty);
					}
				}
			}
		}

		// #########################
		// ### VALUE PROPAGATION ###
		// #########################

		// Propagate value from parent to this and children
		template<typename Type>
		void genericPropagateProperty(Type type, std::shared_ptr<StyleProperty<typename StylePropertyValue<Type>::type> > spProperty)
		{
			// Check whether property is owned by me
			auto spStoredProperty = this->getStyleProperty(type);
			auto spStoredPropertyStyleClass = spStoredProperty->getStyleClass().lock();
			bool owned = false;
			if (spStoredPropertyStyleClass)
			{
				// Compare raw pointers
				owned = spStoredPropertyStyleClass.get() == this;
			}

			// Only store and pass to children if no custom one owned by me
			if (!owned)
			{
				// Store pointer to property
				this->setMapValue(type, spProperty);

				// Propagate it to children
				for (auto& rwpChild : this->mWeakChildren)
				{
					if(auto rspChild = rwpChild.lock())
					{
						rspChild->genericPropagateProperty(type, spProperty);
					}
				}
			} // else: stop propagation here
		}

		// ###############
		// ### MEMBERS ###
		// ###############

		// Name
		std::string mName = "";

		// Maps (ADD NEW PROPERTY TYPES HERE AS MAP)
		std::map<StylePropertyFloat, std::shared_ptr<StyleProperty<float> > > mFloatMap;
		std::map<StylePropertyVec4, std::shared_ptr<StyleProperty<glm::vec4> > > mVec4Map;
		std::map<StylePropertyString, std::shared_ptr<StyleProperty<std::string> > > mStringMap;

		// Parent
		std::weak_ptr<const StyleClass> mwpParent; // empty for root

		// Children
		std::vector<std::shared_ptr<StyleClass> > mChildren; // empty for classes belonging to element, only used classes of style tree for storing them
		std::vector<std::weak_ptr<StyleClass> > mWeakChildren; // this is used for all calculations, as it is always contains the children
	};

	// Builder for style class. Takes care of some prerequisites, as style class is always used in shared pointer which produces problems in constructor
	class StyleClassBuilder
	{
	public:

		// Construct a style class (necessary because "shared from" this is used for StyleClass, but this may not be done in its constructor)
		std::shared_ptr<StyleClass> construct(std::string name = "") const;
	};
}

#endif // STYLE_CLASS_H_