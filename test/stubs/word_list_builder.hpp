/**
 * word_list_builder.hpp
 *
 * Convenience class for building a WORD_LIST linked list from a brace-enclosed
 * list of string literals, for use in unit tests.
 *
 * Usage:
 *   WordListBuilder wl{"newt", "DrawRootText", "0", "1", "hello"};
 *   WORD_LIST* args = wl.head();   // first node is the "command" sentinel
 *
 * The call_newt / wrap_* contract is that args->next is the first real
 * argument (args itself is the command name given to the builtin).
 */
#pragma once

#include <string>
#include <vector>

// Depends on bash_stubs.hpp being included first (for WORD_DESC / WORD_LIST).

class WordListBuilder {
public:
    // Construct from an initializer list of strings.
    WordListBuilder(std::initializer_list<const char*> words) {
        for (const char* w : words)
            words_.emplace_back(w);
        rebuild();
    }

    explicit WordListBuilder(const std::vector<std::string>& words)
        : words_(words) { rebuild(); }

    // Returns a pointer to the first node (the "command" sentinel).
    WORD_LIST* head() { return nodes_.empty() ? nullptr : &nodes_[0].item; }

    // Number of nodes (including sentinel).
    std::size_t size() const { return nodes_.size(); }

private:
    struct Node {
        WORD_DESC desc;
        WORD_LIST item;
    };

    std::vector<std::string> words_;
    std::vector<Node>        nodes_;

    void rebuild() {
        nodes_.resize(words_.size());
        for (std::size_t i = 0; i < words_.size(); ++i) {
            nodes_[i].desc.word  = const_cast<char*>(words_[i].c_str());
            nodes_[i].desc.flags = 0;
            nodes_[i].item.word  = &nodes_[i].desc;
            nodes_[i].item.next  = (i + 1 < words_.size())
                                       ? &nodes_[i + 1].item
                                       : nullptr;
        }
    }
};
