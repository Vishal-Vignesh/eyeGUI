//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO

#ifndef DICTIONARY_H_
#define DICTIONARY_H_

#include "src/Defines.h"

#include <string>
#include <vector>
#include <set>

// TODO:
// - Multiple dictionaries (URLs, Language....) -> vector of dicts in GUI
// - Do more more fuzzy search (optional!)
//  - pause input for one letter and do recursion on all letters in map (only one time)
//  - ignore letter of input (only one time)
// - Collect const defines and decide how to handle them

namespace eyegui
{
    class Dictionary
    {
    public:

        // Constructor
        Dictionary();

        // Destructor
        virtual ~Dictionary();

        // Check for exact word
        bool checkForWord(const std::u16string& rWord) const;

        // Give similar words. May return empty vector or similar words, ordered by probability (more or less)
        std::vector<std::u16string> similarWords(const std::u16string& rWord, uint maxCount) const;

    private:

        // Some typedef for the map
        struct Node;
        typedef std::map<char16_t, Node> NodeMap;

        // Possible states for word, using 1 byte char as structure. None is used for "here is no word"
        enum class WordState : char { NONE, LOWER_START, UPPER_START, BOTH_STARTS };

        // Add single word to dictionary
        void addWord(const std::u16string& rWord);

        // Convert to lower case. Returns word state
        WordState convertToLower(std::u16string& rWord) const;

        // Fuzzy search for words. Takes lower case word as input
        void fuzzyWordSearch(
            const std::u16string& rLowerWord,
            uint recursionDepth,
            std::set<std::u16string>& rFoundWords) const;
        void fuzzyWordSearch(
            const std::u16string& rLowerWord,
            uint wordStartIndex,
            std::u16string collectedWord,
            Node const * pNode,
            int remainingRecursions,
            std::set<std::u16string>& rFoundWords) const;

        // Add longer words which use collected word as prefix. One word can be lower and upper case, counted as one. Returns how many further words may be added
        int addLongerWords(const std::u16string& rCollectedWord, const Node& rNode, int remainingWords, std::set<std::u16string>& rFoundWords) const;

        // Add fuzzy word to found words in fuzzy search. Returns whether successful
        bool addFuzzyWord(const std::u16string& rCollectedWord, WordState collectedState, std::set<std::u16string>& rFoundWords) const;

        // Inner struct for node. Represented letter is implicit coded in map entry which stores node. Buildung up a tree by reusing existing nodes
        struct Node
        {
        public:

            // Members
            NodeMap children; // Further possible letters to form other words
            WordState wordState = WordState::NONE; // State of word formed by letters in nodes from root to here
        };

        // Members
        std::map<char16_t, Node> mRootMap; // Root map
    };
}

#endif // DICTIONARY_H_
