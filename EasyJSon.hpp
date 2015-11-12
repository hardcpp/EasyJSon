/// Copyright 2015 hardcpp
/// https://github.com/hardcpp/EasyJSon
#pragma once
#ifndef EASYJSON_HPP_INCLUDED
#define EASYJSON_HPP_INCLUDED

#include <sstream>
#include <string>
#include <map>
#include <type_traits>

namespace EasyJSon
{

    /// JSon node element type
    enum NodeType
    {
        NT_INDEXED_ARRAY,
        NT_ARRAY,
        NT_DATA,
        NT_DATA_INT
    };

    /// JSon node
    template <class StringAllocator> class Node
    {
        public:
            /// @brief Get node type
            /// @return Type of current node
            const NodeType & Type()
            {
                return m_Type;
            }
            /// Sub node accessors
            /// @param p_Name : Index name of child node
            /// @return The node, or null filled node
            Node<StringAllocator> & operator[](const StringAllocator & p_Name)
            {
                m_Type = NT_INDEXED_ARRAY;

                if (m_ChildNodes.find(p_Name) == m_ChildNodes.end())
                    m_ChildNodes[p_Name] = Node();

                return m_ChildNodes[p_Name];
            }
            /// Sub node accessors
            /// @param p_ID : Index of child node
            /// @return The node, or null filled node
            Node<StringAllocator> & operator[](const size_t & p_ID);
            /// Get sub node count if type == array || indexed_array
            /// @return Child count
            const size_t Size()
            {
                return m_ChildNodes.size();
            }

            /// Get data
            /// @return Data
            const StringAllocator & GetData()
            {
                return m_Data;
            }
            /// Set data
            /// @param p_Data : New data str
            /// @param p_IsArithmetic : Is numeric value
            void SetData(const StringAllocator & p_Data, bool p_IsArithmetic = false)
            {
                m_Type = p_IsArithmetic ? NT_DATA_INT : NT_DATA;
                m_Data = p_Data;
            }

            /// Get child nodes
            /// @return Map of childs
            const std::map<StringAllocator, Node<StringAllocator>> & GetChilds()
            {
                return m_ChildNodes;
            }

            /// Serialize
            /// @return String
            template<class StringAllocatorStream> StringAllocator Serialize();

            /// Data editor operator
            template<typename T> Node & operator=(T p_Data);

        private:
            /// Node type
            NodeType m_Type;
            /// Node data
            StringAllocator m_Data;
            /// Child nodes
            std::map<StringAllocator, Node<StringAllocator>> m_ChildNodes;

    };

    /// @brief Sub node accessors
    /// @param p_ID : Index of child node
    /// @return The node, or null filled node
    template <> inline Node<std::string> & Node<std::string>::operator[](const size_t & p_ID)
    {
        m_Type = NT_ARRAY;

        std::string l_KeyName = std::to_string((unsigned long)p_ID);

        if (m_ChildNodes.find(l_KeyName) == m_ChildNodes.end())
            m_ChildNodes[l_KeyName] = Node();

        return m_ChildNodes[l_KeyName];
    }
    /// @brief Sub node accessors
    /// @param p_ID : Index of child node
    /// @return The node, or null filled node
    template <> inline Node<std::wstring> & Node<std::wstring>::operator[](const size_t & p_ID)
    {
        m_Type = NT_ARRAY;

        std::wstring l_KeyName = std::to_wstring((unsigned long)p_ID);

        if (m_ChildNodes.find(l_KeyName) == m_ChildNodes.end())
            m_ChildNodes[l_KeyName] = Node();

        return m_ChildNodes[l_KeyName];
    }

    /// @brief Serialize
    /// @return String
    template <class StringAllocator>
    template <class StringAllocatorStream> StringAllocator Node<StringAllocator>::Serialize()
    {
        if (m_Type == NT_DATA)
        {
            return StringAllocator("\"" + m_Data + "\"");
        }
        else if (m_Type == NT_DATA_INT)
        {
            return StringAllocator(m_Data);
        }
        else if (m_Type == NT_ARRAY)
        {
            StringAllocatorStream l_Out;
            l_Out << "[";

            for (typename std::map<StringAllocator, Node<StringAllocator>>::iterator l_It = m_ChildNodes.begin(); l_It != m_ChildNodes.end(); l_It++)
            {
                if (l_It != m_ChildNodes.begin())
                    l_Out << ",";

                l_Out << l_It->second.template Serialize<StringAllocatorStream>();
            }

            l_Out << "]";

            return l_Out.str();
        }
        else
        {
            StringAllocatorStream l_Out;
            l_Out << "{";

            for (typename std::map<StringAllocator, Node<StringAllocator>>::iterator l_It = m_ChildNodes.begin(); l_It != m_ChildNodes.end(); l_It++)
            {
                if (l_It != m_ChildNodes.begin())
                    l_Out << ",";

                l_Out << "\"" << l_It->first << "\" : " << l_It->second.template Serialize<StringAllocatorStream>();
            }

            l_Out << "}";

            return l_Out.str();
        }

        return StringAllocator("");
    }
    /// @brief Serialize
    /// @return String
    template <>
    template <class StringAllocatorStream> std::wstring Node<std::wstring>::Serialize()
    {
        if (m_Type == NT_DATA)
        {
            return std::wstring(L"\"" + m_Data + L"\"");
        }
        else if (m_Type == NT_DATA_INT)
        {
            return std::wstring(m_Data);
        }
        else if (m_Type == NT_ARRAY)
        {
            StringAllocatorStream l_Out;
            l_Out << L"[";

            for (std::map<std::wstring, Node<std::wstring>>::iterator l_It = m_ChildNodes.begin(); l_It != m_ChildNodes.end(); l_It++)
            {
                if (l_It != m_ChildNodes.begin())
                    l_Out << L",";

                l_Out << l_It->second.Serialize<StringAllocatorStream>();
            }

            l_Out << L"]";

            return l_Out.str();
        }
        else
        {
            StringAllocatorStream l_Out;
            l_Out << L"{";

            for (std::map<std::wstring, Node<std::wstring>>::iterator l_It = m_ChildNodes.begin(); l_It != m_ChildNodes.end(); l_It++)
            {
                if (l_It != m_ChildNodes.begin())
                    l_Out << ",";

                l_Out << L"\"" << l_It->first << L"\" : " << l_It->second.Serialize<StringAllocatorStream>();
            }

            l_Out << L"}";

            return l_Out.str();
        }

        return std::wstring(L"");
    }

    /// Data editor operator
    template <>
    template<typename T> Node<std::string> & Node<std::string>::operator=(T p_Data)
    {
        SetData(std::to_string(p_Data), std::is_arithmetic<T>::value);
        return *this;
    }
    /// Data editor operator
    template <>
    template<typename T> Node<std::wstring> & Node<std::wstring>::operator=(T p_Data)
    {
        SetData(std::to_wstring(p_Data), std::is_arithmetic<T>::value);
        return *this;
    }
    /// Data editor operator
    template <>
    template<> Node<std::string> & Node<std::string>::operator=(const char * p_Data)
    {
        SetData(p_Data);
        return *this;
    }
    /// Data editor operator
    template <>
    template<> Node<std::string> & Node<std::string>::operator=(std::string p_Data)
    {
        SetData(p_Data);
        return *this;
    }
    /// Data editor operator
    template <>
    template<> Node<std::wstring> & Node<std::wstring>::operator=(const wchar_t * p_Data)
    {
        SetData(p_Data);
        return *this;
    }
    /// Data editor operator
    template <>
    template<> Node<std::wstring> & Node<std::wstring>::operator=(std::wstring p_Data)
    {
        SetData(p_Data);
        return *this;
    }

}   ///< namespace EasyJSon

#endif  ///< EASYJSON_HPP_INCLUDED
