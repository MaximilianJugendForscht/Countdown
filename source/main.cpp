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

#include <fstream>
#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <thread>
#include <future>

template <class IterLhs, class IterRhs>
inline std::vector<std::string> findUnionOf(IterLhs lhsIter, const IterLhs &lhsEnd, const IterRhs rhsBegin, const IterRhs &rhsEnd)
{
    std::vector<std::string> result;
    for (; lhsIter != lhsEnd; ++lhsIter)
    {
        if (std::find(rhsBegin, rhsEnd, *lhsIter) != rhsEnd)
        {
            result.push_back(*lhsIter);
        }
    }
    return result;
}

void checkAmountOfArguments(const int argc)
{
    if (argc < 3)
    {
        std::cout << "Please supply a word to check anagrams of and a newline delimited list of words as a file" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void checkFileIsOpen(const std::ifstream &wordlistFile)
{
    if (!wordlistFile.is_open())
    {
        std::cout << "Could not open the wordlist";
        std::exit(EXIT_FAILURE);
    }
}

std::vector<std::string> getWordsFromFile(std::ifstream &file)
{

    std::vector<std::string> words;
    std::string currentWord;
    while (file >> currentWord)
    {
        words.emplace_back(std::move(currentWord));
    }

    return words;
}

std::string formatAnagram(std::string anagram)
{
    std::transform(anagram.begin(), anagram.end(), anagram.begin(), tolower);
    std::sort(anagram.begin(), anagram.end());
    return anagram;
}

std::ifstream setupWordlist(const char *argv[])
{
    std::ifstream list;
    list.open(std::string(argv[2]), std::ios_base::in);
    checkFileIsOpen(list);
    return list;
}

std::vector<std::string> findAllAnagramPermutations(std::string anagram)
{
    std::vector<std::string> anagramPermutations;
    do
    {
        anagramPermutations.push_back(anagram);
    } while (std::next_permutation(anagram.begin(), anagram.end()));
    return anagramPermutations;
}

inline std::vector<std::string> getMatchesFromFutures(std::vector<std::future<std::vector<std::string>>> &futures)
{
    std::vector<std::string> temp_vec;
    for (auto &future : futures)
    {
        for (auto &word : future.get())
        {
            temp_vec.push_back(word);
        }
    }
    return temp_vec;
}

std::vector<std::string> findAllMatches(const std::vector<std::string> &words, const std::vector<std::string> anagramPermutations)
{
    std::vector<std::future<std::vector<std::string>>> futures;
    const int jump = 2000;
    auto wordIter = words.cbegin();
    auto nextEnd = wordIter;

    for (;;)
    {
        if (std::distance(wordIter, words.cend()) <= jump)
        {
            nextEnd = words.cend();
        }
        else
        {
            nextEnd += jump;
        }
        futures.emplace_back(std::async(std::launch::async, [&anagramPermutations, wordIter, nextEnd]() {
            return findUnionOf(wordIter, nextEnd, anagramPermutations.cbegin(), anagramPermutations.cend());
        }));

        if (nextEnd == words.cend())
        {
            break;
        }

        wordIter += jump;
    }
    return getMatchesFromFutures(futures);
}

int main(const int argc, const char *argv[])
{
    checkAmountOfArguments(argc);
    std::ifstream wordListFile = setupWordlist(argv);
    std::vector<std::string> words = getWordsFromFile(wordListFile);
    std::string anagram = formatAnagram(std::string(argv[1]));
    std::vector<std::string> anagramPermutations = findAllAnagramPermutations(anagram);
    std::vector<std::string> matches = findAllMatches(words, anagramPermutations);
    for (const auto &i : matches)
    {
        std::cout << "found match: " << i << "\n";
    }
    if (matches.size() == 0)
    {
        std::cout << "no matches found\n";
    }
}
