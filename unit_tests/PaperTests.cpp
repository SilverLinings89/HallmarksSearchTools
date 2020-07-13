#include "gtest/gtest.h"
#include "../src/Paper.h"
#include <string>

TEST(PaperTests, OccuringWordsBase) {
    Paper p = {"uid", "title", "This is an abstract for an article.", 0};
    std::set<std::string> in_words;
    in_words.insert("me");
    in_words.insert("you");
    in_words.insert("an");
    in_words.insert("an");
    std::set<std::string> ret = p.OccuringWordsFromWordSet(&in_words);
    ASSERT_TRUE(ret.contains("an"));
    ASSERT_EQ(ret.size(), 1);
}

TEST(PaperTests, IllegalChar) {
    ASSERT_TRUE(isIllegalChar('.'));
}

TEST(PaperTests, OccuringWordsExtended) {
    Paper p = {"uid", "title", "This is an abstract for an article.", 0};
    WordSet in_words;
    in_words.insert("this");
    in_words.insert("you");
    in_words.insert("article");
    in_words.insert("an");
    WordSet ret = p.OccuringWordsFromWordSet(&in_words);
    ASSERT_TRUE(ret.contains("an"));
    ASSERT_TRUE(ret.contains("this"));
    ASSERT_TRUE(ret.contains("article"));
    ASSERT_EQ(ret.size(), 3);
}

TEST(HistogramTests, BasicHistogramTests) {
    Paper p = {"uid", "title", "This is an abstract for an article. It is a great abstract and I like this a lot.", 0};
    WordSet in_words;
    in_words.insert("this");
    in_words.insert("you");
    in_words.insert("article");
    in_words.insert("abstract");
    in_words.insert("an");
    in_words.insert("I");
    in_words.insert("lot");
    WordHistogram histogram = p.GetWordHistogram(&in_words);
    ASSERT_EQ(histogram.getCountForWord("I"),1);
    ASSERT_EQ(histogram.getCountForWord("abstract"),2);
    ASSERT_EQ(histogram.getCountForWord("lot"), 1);
    ASSERT_EQ(histogram.getCountForWord("this"), 2);
}