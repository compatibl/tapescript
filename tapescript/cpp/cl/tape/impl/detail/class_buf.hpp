/*
Copyright (C) 2015-present CompatibL

Performance test results and finance-specific examples are available at:

http://www.tapescript.org

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

# ifndef cl_tape_impl_detail_class_buf_hpp
# define cl_tape_impl_detail_class_buf_hpp

#   pragma warning (push)
#   pragma warning (disable:4351)

namespace cl
{
    template <class Type>
    struct static_class_buf
    {
        static_class_buf()
        : buf_()
        {
            buf_[0] = 0;
        }

        void destroy()
        {
            if (buf_[0])
                reinterpret_cast<Type*>(&buf_[1])->~Type();

            buf_[0] = 0;
        }

        template <class... Args>
        inline Type* ctor(Args const&... args)
        {
            this->destroy();

            buf_[0] = 1;

            return new (&buf_[1]) Type(args...);
        }

        operator bool()
        {
            return buf_[0] != 0;
        }

        inline Type* operator -> ()
        {
            assert(get());
            return get();
        }

        inline Type* get()
        {
            return reinterpret_cast<Type*>(&buf_[1]);
        }

        ~static_class_buf()
        {
            this->destroy();
        }

        unsigned char buf_[sizeof(Type)+1];
    };


    template <template <class > class Ho_, class Ty>
    struct class_fields<Ho_<static_class_buf<Ty>>>
    {
        typedef static_class_buf<Ty> Class;

        typedef fields<> type;

        typedef void after;

        template <class Serializer>
        void after_io(Class& x, Serializer& ss)
        {
            ss & x.buf_[0];
            if (x)
            {
                if (!Serializer::is_out)
                {
                    new (x.get()) Ty();
                }

                ss & *x.get();
            }
        }
    };
}

#   pragma warning (pop)

# endif