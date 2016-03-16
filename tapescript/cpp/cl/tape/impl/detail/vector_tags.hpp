/*
Copyright (C) 2015-present CompatibL

Performance test results and finance-specific examples are available at:

http://www.modval.org/adjoint

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef cl_tape_impl_detail_vector_tags_hpp
#define cl_tape_impl_detail_vector_tags_hpp

#include <map>
#include <vector>

namespace cl
{
    // Represents a bijection of vector indexes (0, ..., n-1) to tags.
    template <class Tag>
    struct vector_tags
    {
        typedef typename std::map<Tag, size_t>::iterator iterator;

        inline size_t operator [](Tag const& n) const
        {
            auto where = tags_.lower_bound(n);

            CL_ASSERT(where != tags_.end() && where->first == n, "Tag not found.");

            return where->second;
        }

        template <class Value>
        std::vector<Value> make_vector(std::map<Tag, Value> const& data, bool fill_defaults = false) const
        {
            if (fill_defaults)
            {
                return make_vector(data, Value());
            }

            std::vector<Value> result(tags_.size());
            auto tagged_data = data.begin();
            for (auto tagged_index : tags_)
            {
                if (tagged_data == data.end())
                    break;

                CL_ASSERT(tagged_data->first == tagged_index.first,
                    "Tags sets mismatch.");

                result[tagged_index.second] = (tagged_data++)->second;
            }
            CL_ASSERT(tagged_data == data.end(), "Unknown tag has been passed.");
            return result;
        }

        template <class Value>
        std::vector<Value> make_vector(std::map<Tag, Value> const& data, const Value& default_value) const
        {
            std::vector<Value> result(tags_.size(), default_value);
            auto tagged_data = data.begin();
            for (auto tagged_index : tags_)
            {
                if (tagged_data == data.end())
                    break;

                if (tagged_data->first == tagged_index.first)
                {
                    result[tagged_index.second] = (tagged_data++)->second;
                }
            }
            CL_ASSERT(tagged_data == data.end(), "Unknown tag has been passed.");
            return result;
        }

        template <class Value>
        std::map<Tag, Value> make_map(
            std::vector<Value> && data
            , size_t step = 1
            , size_t offset = 0) const
        {
            std::map<Tag, Value> result;

            auto hint = result.begin();
            for (auto tagged_index : tags_)
            {
                hint = result.insert(hint, std::make_pair(tagged_index.first
                    , std::move(data[tagged_index.second * step + offset])));
            }

            return result;
        }

        template <class Value>
        std::map<Tag, Value> make_map(
            std::vector<Value> const& data
            , size_t step = 1
            , size_t offset = 0) const
        {
            std::map<Tag, Value> result;

            auto hint = result.begin();
            for (auto tagged_index : tags_)
            {
                hint = result.insert(hint, std::make_pair(tagged_index.first
                    , data[tagged_index.second * step + offset]));
            }

            return result;
        }

        size_t size() const
        {
            return tags_.size();
        }

        // Extends tag set with one tag, returns an index asociated with this tag.
        size_t append(Tag const& tag)
        {
            size_t index = size();
            insert(tag);
            return index;
        }

        // Extends tag set with inputs keys, inputs values are copied to out.
        // The indexes asociated with tags are sequential and starts with curent size().
        template <class Value, class OutIt>
        OutIt append(std::map<Tag, Value> const& inputs, OutIt out)
        {
            auto hint = tags_.begin();
            for (auto& input : inputs)
            {
                hint = insert(hint, input.first);
                *out++ = input.second;
            }
            return out;
        }

        // PairContainer should be a container of pair<Key, Value>.
        // The method extends tag set with inputs keys, inputs values are copied to out.
        // The indexes asociated with tags are sequential and starts with curent size().
        template <class PairContainer, class OutIt>
        OutIt append(PairContainer const& inputs, OutIt out)
        {
            for (auto& input : inputs)
            {
                insert(input.first);
                *out++ = input.second;
            }
            return out;
        }

        iterator insert(iterator hint, Tag const& tag)
        {
            CL_ASSERT(tags_.find(tag) == tags_.end(), "Given tag already exist.");
            return tags_.insert(hint, std::make_pair(tag, size()));
        }

        std::pair<iterator, bool> insert(Tag const& tag)
        {
            CL_ASSERT(tags_.find(tag) == tags_.end(), "Given tag already exist.");
            return tags_.insert(std::make_pair(tag, size()));
        }

        std::map<Tag, size_t> tags_;
    };

    template <template <class > class Ho_, class Tag>
    struct class_fields<Ho_<vector_tags<Tag>>>
    {
        typedef vector_tags<Tag> Class;

        typedef fields<
            decl_mem(Class::tags_)
        > type;
    };
}

#endif

