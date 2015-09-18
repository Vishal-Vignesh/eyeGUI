//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE 
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "StylesheetParser.h"

#include "Defines.h"
#include "OperationNotifier.h"
#include "Helper.h"

#include <algorithm>
#include <sstream>
#include <fstream>
#include <set>

namespace eyegui
{
	StylesheetParser::StylesheetParser()
	{
		// Nothing to do here
	}

	std::unique_ptr<std::map<std::string, Style> > StylesheetParser::parse(std::string filepath)
	{
		// Create map which should be filled
		std::unique_ptr<std::map<std::string, Style> > styles = std::unique_ptr<std::map<std::string, Style> >(new std::map<std::string, Style>);

		// Check, whether default style exists in stylesheet
		bool defaultExists = false;

		// Parse only if there should be a file, otherwise just add the default style
		if (filepath != EMPTY_STRING_ATTRIBUTE)
		{
			// Check file name
			if (!checkFileNameExtension(filepath, STYLESHEET_EXTENSION))
			{
				throwError(OperationNotifier::Operation::PARSING, "Extension of file not as expected", filepath);
			}

			if (filepath != EMPTY_STRING_ATTRIBUTE)
			{
				// Read file
				std::ifstream in(filepath.c_str(), std::ios::in);

				// Check whether file was found
				if (!in)
				{
					throwError(OperationNotifier::Operation::PARSING, "Stylesheet file not found", filepath);
				}

				// Convert input file to string
				std::stringstream strStream;
				strStream << in.rdbuf();
				std::string content = strStream.str();

				// Close file
				in.close();

				// Get rid of whitespaces
				std::string::iterator end_pos = std::remove(content.begin(), content.end(), ' ');
				content.erase(end_pos, content.end());

				// Get rid of tabs
				end_pos = std::remove(content.begin(), content.end(), '\t');
				content.erase(end_pos, content.end());

				// Add some new line at the end of content because while loop does not read last line
				content += "\n";

				// Make some state variables for parsing
				enum class ParseState { OUTER, NAME_READ, INNER };
				ParseState state = ParseState::OUTER;
				std::string name;
				Style style;
				std::set<std::string> names;

				// Go through the lines
				std::string delimiter = "\n";
				size_t pos = 0;
				std::string line;
				while ((pos = content.find(delimiter)) != std::string::npos)
				{
					line = content.substr(0, pos);
					content.erase(0, pos + delimiter.length());

					switch (state)
					{
					case ParseState::OUTER:
						if (line == "")
						{
							continue;
						}
						else
						{
							// First guess, whole line is name
							name = line;

							// Create new style with fallback values
							style = Style();

							// Try to get left and right side
							std::string delimiter = ":";
							size_t pos = 0;
							std::string copyline = line;

							if ((pos = copyline.find(delimiter)) != std::string::npos)
							{
								name = copyline.substr(0, pos);
								copyline.erase(0, pos + delimiter.length());
								std::string superstyle = copyline;

								// Only do something, if superstyle has a name
								if (superstyle != "")
								{
									// Name of superstyle is known?
									bool check = false;
									for (const std::string& rName : names)
									{
										if (rName == superstyle)
										{
											check = true;
											break;
										}
									}

									// If name of superstyle is ok, copy values
									if (check)
									{
										style = (*(styles.get()))[superstyle];
									}
									else
									{
										throwError(OperationNotifier::Operation::PARSING, "Unkown style used as superstyle: " + superstyle, filepath);
									}
								}
							}

							// Allow name only if unique
							for (const std::string& rName : names)
							{
								if (rName == name)
								{
									throwError(OperationNotifier::Operation::PARSING, "Following name of style is not unique: " + name, filepath);
								}
							}

							// Change state
							state = ParseState::NAME_READ;
						}
						break;
					case ParseState::NAME_READ:
						if (line == "{")
						{
							state = ParseState::INNER;
						}
						else
						{
							throwError(OperationNotifier::Operation::PARSING, "'{' expected but not there", filepath);
						}
						break;
					case ParseState::INNER:
						if (line == "")
						{
							continue;
						}
						else if (line == "}")
						{
							// Copy new entry to map
							(*(styles.get()))[name] = style;

							// Add name to names
							names.insert(name);

							if (name == DEFAULT_STYLE_NAME)
							{
								defaultExists = true;
							}

							state = ParseState::OUTER;
						}
						else
						{
							// There seems to be some value, parse it!
							parseValue(line, style, filepath);
						}
						break;
					}
				}

				// State should be OUTER after parsing
				if (state != ParseState::OUTER)
				{
					throwError(OperationNotifier::Operation::PARSING, "'}' seems to be missing", filepath);
				}
			}
		}

		// Create default if non existing
		if (!defaultExists)
		{
			(*(styles.get()))[DEFAULT_STYLE_NAME] = Style();
		}

		// Return unique pointer to map
		return std::move(styles);
	}

	void StylesheetParser::parseValue(std::string value, Style& rStyle, std::string filepath) const
	{
		// Get left and right side
		std::string delimiter = "=";
		size_t pos = 0;

		if ((pos = value.find(delimiter)) == std::string::npos)
		{
			throwError(OperationNotifier::Operation::PARSING, "Following line could not be parsed: " + value, filepath);
		}

		std::string left = value.substr(0, pos);
		value.erase(0, pos + delimiter.length());
		std::string right = value;

		// Get color out of right side
		glm::vec4 color = parseColor(right);

		// Assign value to correct field of struct
		if (left == "color")
		{
			rStyle.color = color;
		}
		else if (left == "background-color")
		{
			rStyle.backgroundColor = color;
		}
		else if (left == "highlight-color")
		{
			rStyle.highlightColor = color;
		}
		else if (left == "separator-color")
		{
			rStyle.separatorColor = color;
		}
		else if (left == "selection-color")
		{
			rStyle.selectionColor = color;
		}
		else if (left == "icon-color")
		{
			rStyle.iconColor = color;
		}
		else
		{
			throwError(OperationNotifier::Operation::PARSING, "Unknown value on left side of '=': " + left, filepath);
		}
	}

	glm::vec4 StylesheetParser::parseColor(std::string value) const
	{
		uint hexInt;
		std::stringstream ss;
		ss << std::hex << value;
		ss >> hexInt;

		float red = ((float)((hexInt & 0xFF000000) >> 24)) / (255.0f);
		float green = ((float)((hexInt & 0x00FF0000) >> 16)) / (255.0f);
		float blue = ((float)((hexInt & 0x0000FF00) >> 8)) / (255.0f);
		float alpha = ((float)((hexInt & 0x000000FF) >> 0)) / (255.0f);

		return glm::vec4(red, green, blue, alpha);
	}

}