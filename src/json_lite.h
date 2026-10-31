#pragma once

/// This software has been written By Evgeniy Tukh
/// Feel free to use and/or modify it

#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>
#include <string>
#include <cstdint>
#include <type_traits>

#ifdef __linux__
#define _strdup strdup
#endif

namespace json {
    enum nodeType {
        null = 0,
        number,
        string,
        array,
        hash,
        boolean,
    };

    struct node {
        nodeType type;
        bool toBeFreed = true;

        node (nodeType _type = nodeType::null) : type (_type) {}
        virtual ~node () {}

        virtual void *get () { return nullptr; }
        virtual std::string serialize () { return "null"; }
    };

    extern node _nothing;
    extern node *nothing;

    struct stringNode: node {
        std::string value;

        stringNode (): node (nodeType::string) {}
        stringNode (const std::string& src): stringNode(src.c_str()) {}
        stringNode (const char *src): node (nodeType::string), value (src) {}

        void *get () override { return (void *) value.c_str (); }
        inline const char *getValue () { return value.c_str (); }

        std::string serialize () override {
            std::string result = "\"";

            result += value;
            result += '"';

            return result;
        }
    };

    struct booleanNode: node {
        bool value;

        bool isTrue (const char *src) {
            if (!src) return false;

            static constexpr const char *__TRUE = "true";

            for (int i = 0; __TRUE [i]; ++ i) {
                if (!src [i] || (tolower (src [i]) != __TRUE [i])) return false;
            }

            return true;
        }

        booleanNode (): node (nodeType::boolean), value (false) {}
        booleanNode (const bool src): node (nodeType::boolean), value (src) {}
        booleanNode (const char *src): node (nodeType::boolean), value (isTrue (src)) {}

        void *get () override { return (void *) & value; }
        inline bool getValue () { return value; }

        std::string serialize () override {
            return std::string (value ? "true" : "false");
        }
    };

    struct numberNode: node {
        double value;

        // redone atof version independent on the locale settings
        static double char2dbl (const char *src) {
            uint32_t integral = 0;
            uint32_t fractal = 0;
            uint32_t fractalDivider = 1;
            bool negative = false;
            bool sepFound = false;
            bool nonBlankFound = false;
            for (; *src; ++ src) {
                if (*src == '-') {
                    if (negative) break; // second minus found
                    nonBlankFound = true;
                    negative = true;
                } else if (*src == ' ' || *src == '\t') {
                    if (nonBlankFound) break; // an unexpected char found
                    continue;
                } else if (*src == ',' || *src == '.') {
                    if (sepFound) break; // second decimal point or comma found
                    sepFound = true;
                } else if (isdigit (*src)) {
                    if (sepFound) {
                        fractal = fractal * 10 + *src - '0';
                        fractalDivider *= 10;
                    } else {
                        integral = integral * 10 + *src - '0';
                    }
                } else {
                    break; // non-number char found (neither minus nor dot, comma or digit)
                }
            }
            double result = (double) integral + (double) fractal / (double) fractalDivider;
            return negative ? - result : result;
        }

        numberNode (): node (nodeType::number) {}
        numberNode (const char *src): node (nodeType::number), value (char2dbl (src)) {}
        numberNode (const double src): node (nodeType::number), value (src) {}

        void *get () override { return (void *) & value; }
        inline double getValue () { return value; }

        std::string serialize () override {
            char buffer [200];
            sprintf (buffer, "%f", value);

            // Make sure that dot is used but not comma
            char *dotPos = strchr (buffer, ',');

            if (dotPos) {
                *dotPos = '.';
            } else {
                dotPos = strchr (buffer, '.');
            }
            
            // Remove trailing zeros (if any)
            if (dotPos) {
                for (auto pos = buffer + strlen (buffer) - 1; pos > dotPos; -- pos) {
                    if (*pos == '0')
                        *pos = '\0';
                    else
                        break;
                }
                // if the value ended by "." so it is an integer
                if (dotPos [1] == '\0') *dotPos = '\0';
            }

            return std::string (buffer);
        }
    };

    struct arrayNode: node {
        std::vector<node *> value;

        arrayNode (): node (nodeType::array) {}
        ~arrayNode () override {
            for (auto& item: value) {
                delete item;
            }
            value.clear ();
        }

        void *get () override { return (void *) & value; }

        inline bool empty() const {
            return value.empty();
        }

        inline size_t size () const {
            return value.size ();
        }

        inline void add (node *val) {
            value.push_back (val);
        }

        inline node *&operator [] (size_t index) {
            return index < value.size () ? value [index] : json::nothing;
        }

        inline node *at (size_t index) {
            return index < value.size () ? value [index] : json::nothing;
        }

        inline void setAt (size_t index, node *nodeValue) {
            if (index < value.size ()) value [index] = nodeValue;
        }

        inline auto begin () { return value.begin (); }
        inline auto end () { return value.end (); }
        inline auto cbegin () const { return value.cbegin (); }
        inline auto cend () const { return value.cend (); }

        std::string serialize () override {
            if (value.empty())
                return "[]";
                
            std::string result = "[";

            for (auto i = 0; i < value.size (); ++ i) {
                result += value [i]->serialize ();

                if (i < (value.size () - 1)) result += ',';
            }

            result += ']';

            return result;
        }
    };

    struct hashNode: node {
        std::map<std::string, node *> value;

        hashNode (): node (nodeType::hash) {}
        ~hashNode () override {
            for (auto& item: value) {
                if (item.second && item.second->toBeFreed) {
                    delete item.second;
                }
            }
            value.clear ();
        }

        inline bool empty() const {
            return value.empty();
        }

        void *get () override { return (void *) & value; }

        inline auto add (const char *key, node *val, bool toBeFreed = true) {
            auto elem = value.insert (value.end (), std::pair<std::string, node *> (std::string (key), val));

            elem->second->toBeFreed = toBeFreed;

            return elem;
        }

        inline auto add (const std::string& key, node *val, bool toBeFreed = true) {
            return add(key.c_str(), val, toBeFreed);
        }

        inline node *&operator [] (char *key) {
            for (auto& item: value) {
                auto result = strcmp (item.first.c_str (), key);
                
                if (result == 0) return item.second;
            }

            return nothing;
        }

        inline node *&operator [] (const char *key) {
            if (!key || !*key) return nothing;

            for (auto& item: value) {
                auto result = strcmp (item.first.c_str (), key);
                
                if (result == 0) return item.second;
            }

            return nothing;
        }

        inline node *at (const char *key) {
            return at((char *) key);
        }

        inline node *at (char *key) {
            if (!key || !*key) return nothing;

            for (auto& item: value) {
                auto result = strcmp (item.first.c_str (), key);
                
                if (result == 0) return item.second;
            }

            return nullptr;
        }

        inline void setAt (char *key, node *nodeValue) {
            if (!key || !*key || !nodeValue) return;

            for (auto& item: value) {
                auto result = strcmp (item.first.c_str (), key);
                
                if (result == 0) {
                    item.second = nodeValue; break;
                }
            }
        }

        inline auto begin () { return value.begin (); }
        inline auto end () { return value.end (); }
        inline auto cbegin () const { return value.cbegin (); }
        inline auto cend () const { return value.cend (); }

        std::string serialize () override {
            if (value.empty())
                return "{}";

            std::string result = "{";

            for (auto& element: value) {
                result += "\"";
                result += element.first;
                result += "\":";

                if (element.second) {
                    result += element.second->serialize ();
                } else {
                    result += "null";
                }

                result += ',';
            }

            if (result.length () > 2)
                result.back () = '}';

            return result;
        }
    };

    struct valueKey {
        size_t arrayIndex;
        std::string hashKey;

        static const size_t noIndex = 0xFFFFFFFF;

        valueKey (): arrayIndex (noIndex) {}
    };

    struct nodeValue {
        std::string stringValue;
        double numericValue;
        bool booleanValue;
        std::vector<node *> arrayValue;
        std::map<std::string, node *> hashValue;
    };

    node *parse (const std::string& sourceString, int& nextChar);
    node *parse (char *sourceString, int& nextChar);
    node *parse (char *begin, char *end, int& nextChar);
    node *parseTrusted (char *stream, int& offset);
    void removeWhiteSpaces (char *source, std::string& result);
    void removeWhiteSpaces (char *begin, char *end, std::string& result);
    void getValue (node *_node, nodeValue& value);
    std::string makePretty(const std::string& source);

    numberNode *childAsNumberNodeOrNull(node *parent, const char *key);
    numberNode *childAsNumberNodeOrNull(node *parent, const std::string& key);
    stringNode *childAsStringNodeOrNull(node *parent, const char *key);
    stringNode *childAsStringNodeOrNull(node *parent, const std::string& key);
    booleanNode *childAsBooleanNodeOrNull(node *parent, const char *key);
    booleanNode *childAsBooleanNodeOrNull(node *parent, const std::string& key);
    arrayNode *childAsArrayNodeOrNull(node *parent, const char *key);
    arrayNode *childAsArrayNodeOrNull(node *parent, const std::string& key);
    hashNode *childAsHashNodeOrNull(node *parent, const char *key);
    hashNode *childAsHashNodeOrNull(node *parent, const std::string& key);

    numberNode *childAsNumberNodeOrNull(node *parent, size_t index);
    stringNode *childAsStringNodeOrNull(node *parent, size_t index);
    booleanNode *childAsBooleanNodeOrNull(node *parent, size_t index);
    arrayNode *childAsArrayNodeOrNull(node *parent, size_t index);
    hashNode *childAsHashNodeOrNull(node *parent, size_t index);
    
    template<typename Cb> void walkThrough (node *_node, Cb cb, valueKey& key, uint16_t level) {
        nodeValue val;

        if (!_node) {
            cb (_node, val, key, level); return;
        }

        // populate node value and make a first, very general call of the callback
        // for hashes and arrays we will call callback recursively later
        getValue (_node, val);
        cb (_node, val, key, level); 

        switch ((*(_node)).type) {
            case nodeType::array: {
                valueKey itemKey;

                // go through all array elements
                for (itemKey.arrayIndex = 0; itemKey.arrayIndex < val.arrayValue.size (); ++ itemKey.arrayIndex) {
                    walkThrough (val.arrayValue [itemKey.arrayIndex], cb, itemKey, level + 1);
                }
                break;
            }
            case nodeType::hash: {
                valueKey itemKey;

                // go through all hash elements
                for (auto& element: val.hashValue) {
                    itemKey.hashKey = element.first;
                    walkThrough (element.second, cb, itemKey, level + 1);
                }
                break;
            }
        }
    }

    template<typename N, typename R, typename K>
    R childNumericValueOr(N parent, K key, R defValue = (R) 0) {
        auto numNode = childAsNumberNodeOrNull((json::node *) parent, key);

        return numNode ? (R) numNode->value : defValue;
    }

    template<typename N, typename K>
    bool childBoolValueOr(N parent, K key, bool defValue = false) {
        auto boolNode = childAsBooleanNodeOrNull((json::node *) parent, key);

        return boolNode ? boolNode->value : defValue;
    }

    template<typename N, typename K>
    inline std::string childStringValueOr(N parent, K key, const std::string defValue = "") {
        auto strNode = childAsStringNodeOrNull((json::node *) parent, key);

        return strNode ? strNode->value : defValue;
    }
}
