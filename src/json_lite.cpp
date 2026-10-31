/// This software has been written By Evgeniy Tukh
/// Feel free to use and/or modify it

#include <stdlib.h>
#include <string>
#include <vector>
#include <map>

#include "json_lite.h"

namespace json {
    node *parse (char *stream, int& offset);

    bool extractLiteral (char *stream, std::string& extraction, int& offset);
    node *extractLiteral (char *stream, int& offset);
    node *extractNumber (char *stream, int& offset);
    node *extractHash (char *stream, int& offset);
    node *extractArray (char *stream, int& offset);
    node *extractBoolean (char *stream, int& offset);

    node _nothing;
    node *nothing = & _nothing;
}

void json::removeWhiteSpaces (char *source, std::string& result) {
    bool insideString = false;

    result.clear ();

    for (auto chr = source; *chr; ++ chr) {
        if (*chr == '"') insideString = !insideString;
        
        if (insideString || *chr != ' ' && *chr != '\t' && *chr != '\r' && *chr != '\n')
            result += *chr;
    }
}

void json::removeWhiteSpaces (char *begin, char *end, std::string& result) {
    bool insideString = false;

    result.clear ();

    for (auto chr = begin; chr < end; ++ chr) {
        if (*chr == '"') insideString = !insideString;
        
        if (insideString || *chr != ' ' && *chr != '\t' && *chr != '\r' && *chr != '\n')
            result.append (1, *chr);
    }
}

bool json::extractLiteral (char *stream, std::string& extraction, int& offset) {
    extraction.clear ();

    if (stream [offset] != '"') return false;

    for (auto i = offset + 1; stream [i] && stream [i] != '"'; ++ i) extraction += stream [i];

    offset += (int) extraction.length () + 2;

    return true;
}

json::node *json::extractLiteral (char *stream, int& offset) {
    std::string extraction;

    if (!extractLiteral (stream, extraction, offset))
        return nullptr;

    return new stringNode (extraction.c_str ());
}

json::node *json::extractNumber (char *stream, int& offset) {
    if (!isdigit (stream [offset]) && stream [offset] != '-')
        return nullptr;

    std::string extraction;
    bool dotPassed = false;
    auto start= offset;

    if (stream [offset] == '-') {
        extraction += '-';
        start ++;
    }

    for (auto i = start; isdigit (stream [i]) || stream [i] == '.'; ++ i) {
        if (stream [i] == '.') {
            if (dotPassed)
                return nullptr;

            dotPassed = true;
        }

        extraction += stream [i];
    }

    offset += (int) extraction.length ();

    return new numberNode (extraction.c_str ());
}

json::node *json::extractBoolean (char *stream, int& offset) {
    if (!isalpha (stream [offset]))
        return nullptr;

    std::string extraction;

    while (isalpha (stream [offset])) {
        extraction += stream [offset++];
    }

    return new booleanNode (extraction.c_str ());
}

json::node *json::parse (const std::string& sourceString, int& nextChar) {
    return parse((char *) sourceString.data(), nextChar);
}

json::node *json::parse (char *stream, int& nextChar) {
    std::string streamHolder;

    removeWhiteSpaces (stream, streamHolder);
    
    return parseTrusted ((char *) streamHolder.data (), nextChar);
}

json::node *json::parse (char *begin, char *end, int& nextChar) {
    std::string streamHolder;

    removeWhiteSpaces (begin, end, streamHolder);
    
    return parseTrusted ((char *) streamHolder.data (), nextChar);
}

json::node *json::parseTrusted (char *stream, int& offset) {
    json::node *item;

    switch (stream [offset]) {
        case '{': {
            item = extractHash (stream, offset);

            if (!item)
                return nullptr;

            break;
        }
        case '[': {
            item = extractArray (stream, offset);

            if (!item)
                return nullptr;

            break;
        }
        case '"': {
            item = extractLiteral (stream, offset);

            if (!item)
                return nullptr;

            break;
        }
        default: {
            if (isdigit (stream [offset]) || stream [offset] == '.' || stream [offset] == '-') {
                item = extractNumber (stream, offset);
            } else if (memcmp (stream + offset, "true", 4) == 0 || memcmp (stream + offset, "false", 5) == 0) {
                item = extractBoolean (stream, offset);
            } else if (memcmp (stream + offset, "null", 4) == 0) {
                item = new node ();
                offset += 4;
            } else {
                item = nullptr;
            }

            break;
        }
    }

    return item;
}

json::node *json::extractHash (char *stream, int& offset) {
    hashNode *result = new hashNode ();
    
    if (stream [offset] != '{') return  0;

    ++ offset;

    while (stream [offset] == '"') {
        std::string key;

        if (!extractLiteral (stream, key, offset))
            return nullptr;
        
            if (stream [offset] != ':')
                return nullptr;

        ++ offset;

        json::node *item = parseTrusted (stream, offset);

        if (!item)
            return nullptr;

        result->add (key.data (), item);

        if (stream [offset] == ',') ++ offset;
    }

    if (stream [offset] != '}')
        return nullptr;

    ++ offset;

    return result;
}

json::node *json::extractArray (char *stream, int& offset) {
    arrayNode *result = new arrayNode ();
    
    if (stream [offset] != '[') return  0;

    ++ offset;

    while (stream [offset] != ']') {
        json::node *item = parseTrusted (stream, offset);

        if (!item)
            return nullptr;

        result->add (item);

        if (stream [offset] == ',') ++ offset;
    }

    if (stream [offset] != ']')
        return nullptr;

    ++ offset;

    return result;
}

void json::getValue (node *_node, nodeValue& value) {
    switch ((*_node).type) {
        case nodeType::number: {
            value.numericValue = ((numberNode *)_node)->getValue (); break;
        }
        case nodeType::boolean: {
            value.booleanValue = ((booleanNode *)_node)->getValue (); break;
        }
        case nodeType::string: {
            value.stringValue = ((stringNode *)_node)->getValue (); break;
        }
        case nodeType::hash: {
            hashNode *hash = (hashNode *) _node;
            value.hashValue.insert ((*hash).begin (), (*hash).end ()); break;
        }
        case nodeType::array: {
            arrayNode *array = (arrayNode *) _node;
            value.arrayValue.insert (value.arrayValue.begin (), (*array).begin (), (*array).end ()); break;
        }
    }
}

std::string json::makePretty(const std::string& source) {
    std::string result;
    int indent = 0;
    bool insideQuote = false;

    auto makeIndent = [&indent, &result] () {
        for (int i = 0; i < indent; ++i)
            result += "    ";
    };

    for (char ch: source) {
        if (insideQuote) {
            if (ch == '"')
                insideQuote = false;

            result += ch;
            continue;
        }

        switch (ch) {
            case '\n':
            case '\r':
            case ' ':
                continue;

            case '{':
                result += "{\n";
                indent ++;
                makeIndent();
                break;

            case '[':
                result += "[\n";
                indent ++;
                makeIndent();
                break;

            case '}':
                result += '\n';
                indent --;
                makeIndent();
                result += "}";
                break;

            case ']':
                result += '\n';
                indent --;
                makeIndent();
                result += "]";
                break;

            case ',':
                result += ",\n";
                makeIndent();
                break;

            case ':':
                result += ": ";
                break;

            case '"':
                insideQuote = true;
                result += ch;
                break;

            default:
                result += ch;
        }
    }

    return result;
}

json::numberNode *json::childAsNumberNodeOrNull(json::node *parent, const char *key) {
    if (parent->type == json::nodeType::hash) {
        auto child = (*dynamic_cast<json::hashNode *>(parent))[key];

        if (child->type == json::nodeType::number)
            return dynamic_cast<numberNode *>(child);
    }

    return nullptr;
}

json::numberNode *json::childAsNumberNodeOrNull(json::node *parent, const std::string& key) {
    return childAsNumberNodeOrNull(parent, key.c_str());
}

json::stringNode *json::childAsStringNodeOrNull(json::node *parent, const char *key) {
    if (parent->type == json::nodeType::hash) {
        auto child = (*dynamic_cast<json::hashNode *>(parent))[key];

        if (child->type == json::nodeType::string)
            return dynamic_cast<stringNode *>(child);
    }

    return nullptr;
}

json::stringNode *json::childAsStringNodeOrNull(json::node *parent, const std::string& key) {
    return childAsStringNodeOrNull(parent, key.c_str());
}

json::booleanNode *json::childAsBooleanNodeOrNull(json::node *parent, const char *key) {
    if (parent->type == json::nodeType::hash) {
        auto child = (*dynamic_cast<json::hashNode *>(parent))[key];

        if (child->type == json::nodeType::boolean)
            return dynamic_cast<booleanNode *>(child);
    }

    return nullptr;
}

json::booleanNode *json::childAsBooleanNodeOrNull(json::node *parent, const std::string& key) {
    return childAsBooleanNodeOrNull(parent, key.c_str());
}

json::arrayNode *json::childAsArrayNodeOrNull(json::node *parent, const char *key) {
    if (parent->type == json::nodeType::hash) {
        auto child = (*dynamic_cast<json::hashNode *>(parent))[key];

        if (child->type == json::nodeType::array)
            return dynamic_cast<arrayNode *>(child);
    }

    return nullptr;
}

json::arrayNode *json::childAsArrayNodeOrNull(json::node *parent, const std::string& key) {
    return childAsArrayNodeOrNull(parent, key.c_str());
}

json::hashNode *json::childAsHashNodeOrNull(json::node *parent, const char *key) {
    if (parent->type == json::nodeType::hash) {
        auto child = (*dynamic_cast<json::hashNode *>(parent))[key];

        if (child->type == json::nodeType::hash)
            return dynamic_cast<hashNode *>(child);
    }

    return nullptr;
}

json::hashNode *json::childAsHashNodeOrNull(json::node *parent, const std::string& key) {
    return childAsHashNodeOrNull(parent, key.c_str());
}

json::numberNode *json::childAsNumberNodeOrNull(json::node *parent, size_t index) {
    if (parent->type == json::nodeType::array) {
        auto child = (*dynamic_cast<json::arrayNode *>(parent))[index];

        if (child->type == json::nodeType::number)
            return dynamic_cast<numberNode *>(child);
    }

    return nullptr;
}

json::stringNode *json::childAsStringNodeOrNull(json::node *parent, size_t index) {
    if (parent->type == json::nodeType::array) {
        auto child = (*dynamic_cast<json::arrayNode *>(parent))[index];

        if (child->type == json::nodeType::string)
            return dynamic_cast<stringNode *>(child);
    }

    return nullptr;
}

json::booleanNode *json::childAsBooleanNodeOrNull(json::node *parent, size_t index) {
    if (parent->type == json::nodeType::array) {
        auto child = (*dynamic_cast<json::arrayNode *>(parent))[index];

        if (child->type == json::nodeType::boolean)
            return dynamic_cast<booleanNode *>(child);
    }

    return nullptr;
}

json::arrayNode *json::childAsArrayNodeOrNull(json::node *parent, size_t index) {
    if (parent->type == json::nodeType::array) {
        auto child = (*dynamic_cast<json::arrayNode *>(parent))[index];

        if (child->type == json::nodeType::array)
            return dynamic_cast<arrayNode *>(child);
    }

    return nullptr;
}

json::hashNode *json::childAsHashNodeOrNull(json::node *parent, size_t index) {
    if (parent->type == json::nodeType::array) {
        auto child = (*dynamic_cast<json::arrayNode *>(parent))[index];

        if (child->type == json::nodeType::hash)
            return dynamic_cast<hashNode *>(child);
    }

    return nullptr;
}
