/*
    This file is part of Countdown.

    Countdown is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Countdown is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Countdown.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <fstream> //Imports all dependencies
#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <thread>
#include <future>

template <typename IterLhs, typename IterRhs> //@josh  could you please comment, what these variables do? (entire block)
auto findUnionOf(IterLhs lhsIter, const IterLhs& lhsEnd,
                 const IterRhs rhsBegin, const IterRhs& rhsEnd) {
    std::vector<std::string> result;

    for(; lhsIter != lhsEnd; ++lhsIter) {
        if(std::find(rhsBegin, rhsEnd, *lhsIter) != rhsEnd) {
            result.push_back(*lhsIter);
        }
    }

    return result; //returns ther result of the for-loop
}

int main(int argc, char* argv[]) { //@josh what are the arguements supposed to be passed?
    // program name and word list
    if(argc < 3) {
        std::cout << "Please supply a word to check anagrams of and a newline delimited list of words as a file" << std::endl; //On Illegal string (too short)
        std::exit(EXIT_FAILURE); //If the string is too short, exit the program
    }

    const char* const wordlistPath = argv[2]; //Sets the path of the wordlist (To have words as comparison, maybe?)

    std::ifstream wordlistFile(wordlistPath, std::ios_base::in);

    if(! wordlistFile.is_open()) {
        std::cout << "Could not open file " << wordlistPath << std::endl; //If the File with the words is empty
        std::exit(EXIT_FAILURE); //Exits on exception FILEEMPTY
    }

    std::string currentWord; //creates a string variable with the current word for easier comparison
    std::vector<std::string> words; //array with ALL the words inside the wordlist

    std::cout << "Loading words..." << std::endl;

    int progressBarCounter = 0;
    
    while(wordlistFile >> currentWord) {
        words.emplace_back(std::move(currentWord));
        std::cout << "#"; //writes the character as Progress-bar
        if (progressBarCounter == 30) {
         
            for (int i = 0, i = 30, i++) { //If the Progress bar is at thirty, flush everything
                std::cout << "\b";
            }
        }
    }

    
    
    std::cout << words.size() << " words loaded!" << std::endl; //tells the user how many words got loaded

    std::string anagram = argv[1]; //creates an instance of string called anagram, filled with the passed arg for anagramsearch

    std::cout << "Searching for anagrams of " << anagram << std::endl; //message

    std::sort(anagram.begin(), anagram.end()); //@josh Idk this method, could you please explain it to me?

    std::vector<std::string> anagramPermutations;

    do {
        anagramPermutations.push_back(anagram); //anagramPermutations: symbol not defined
    } while(std::next_permutation(anagram.begin(), anagram.end()));

    std::cout << "There are " << anagramPermutations.size() << " possible words" << std::endl;

    std::vector<std::future<std::vector<std::string>>> futures;

    const size_t jump = 20000;
    auto wordIter = words.cbegin();

    for(;;) {
        decltype(words)::const_iterator nextEnd;

        if(std::distance(wordIter, words.cend()) <= jump) {
            nextEnd = words.cend();
        } else {
            nextEnd = wordIter + jump;
        }

        futures.emplace_back(std::async(std::launch::async, [&anagramPermutations, wordIter, nextEnd](){
            return findUnionOf(wordIter, nextEnd, anagramPermutations.cbegin(), anagramPermutations.cend());
        }));

        if(nextEnd == words.cend()) {
            break;
        }

        wordIter += jump;
    }

    bool matchFound = false;
    
    for(auto& future : futures) {
        for(auto& word : future.get()) {
            matchFound = true;
            std::cout << "MATCH: " << word << std::endl;
        }
    }

    if(! matchFound) {
        std::cout << "No matches found" << std::endl;
    }
}
