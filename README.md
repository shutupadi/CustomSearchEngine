# CustomSearchEngine

Description: A simple search engine that indexes text data and allows users to search through it. Implement data structures like tries, hash maps, and inverted indexes for efficient search queries.
Key Concepts: Tries, Hash Maps, Sorting, Search Algorithms.

Key Enhancements Explained
Ranking with TF-IDF:

TF (Term Frequency): The frequency of a word in a document divided by the total number of words in that document.
IDF (Inverse Document Frequency): Logarithmic measure of how common or rare a word is across all documents.
The searchWithRanking function computes a relevance score for each document based on the TF-IDF score of the query words, providing ranked search results.
Phrase Search:

The searchPhrase function handles multi-word queries by checking if all words in the phrase appear in the same document. It sums the TF-IDF scores of each word in the phrase for the final ranking.
Autocomplete:

The Trie is used to suggest completions based on a given prefix. The autocomplete method recursively traverses the Trie to gather all words that start with the prefix, providing suggestions to the user.
Summary
Ranking: TF-IDF is used to rank documents based on relevance to the search query.
Phrase Search: The engine can search
