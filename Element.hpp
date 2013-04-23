#pragma once

#include <sstream>
#include <string>
#include <map>

namespace EasyJSon {

	/*
	 * @brief JSon node element type
	 */
	enum NodeType
	{
		NT_INDEXED_ARRAY,
		NT_ARRAY,
		NT_DATA
	};

	/*
	 * @brief JSon node
	 */
	template <class StringAllocator> class Node
	{
		public:
			/*
			 * @brief Get node type
			 * @return Type of cureent node
			 */
			const NodeType & Type()
			{
				return m_Type;
			}
			/*
			 * @brief Sub node accessor
			 * @param p_Name : Index name of child node
			 * @return The node, or null filled node
			 */
			Node<StringAllocator> & operator[](const StringAllocator & p_Name)
			{
				m_Type = NT_INDEXED_ARRAY;

				if (m_ChildNodes.find(p_Name) == m_ChildNodes.end())
					m_ChildNodes[p_Name] = Node();

				return m_ChildNodes[p_Name];
			}
			/*
			 * @brief Sub node accessor
			 * @param p_ID : Index of child node
			 * @return The node, or null filled node
			 */
			Node<StringAllocator> & operator[](const size_t & p_ID);
			/*
			 * @brief Get sub node count if type == array || indexed_array
			 * @return Child count
			 */
			const size_t Size()
			{
				return m_ChildNodes.size();
			}

			/*
			 * @brief Get data
			 * @return Data
			 */
			const StringAllocator & GetData()
			{
				return m_Data;
			}
			/*
			 * @brief Set data
			 * @param p_Data : New data str
			 */
			void SetData(const StringAllocator & p_Data)
			{
				m_Type = NT_DATA;
				m_Data = p_Data;
			}

			/*
			 * @brief Get child nodes
			 * @return Map of childs
			 */
			const std::map<StringAllocator, Node<StringAllocator>> & GetChilds()
			{
				return m_ChildNodes;
			}

			/*
			 * @brief Serialize
			 * @return String
			 */
			template<class StringAllocatorStream> StringAllocator Serialize();

			/*
			 * @brief Data editor operator
			 */
			Node & operator=(const StringAllocator & p_Data)
			{
				SetData(p_Data);
				return *this;
			}

		private:
			/*
			 * @brief Node type
			 */
			NodeType m_Type;
			/*
			 * @brief Node data
			 */
			StringAllocator m_Data;
			/*
			 * @brief Child nodes
			 */
			std::map<StringAllocator, Node<StringAllocator>> m_ChildNodes;

	};

	/*
	 * @brief Sub node accessor
	 * @param p_ID : Index of child node
	 * @return The node, or null filled node
	 */
	template <> Node<std::string> & Node<std::string>::operator[](const size_t & p_ID)
	{
		m_Type = NT_ARRAY;

		std::string l_KeyName = std::to_string((unsigned long)p_ID);

		if (m_ChildNodes.find(l_KeyName) == m_ChildNodes.end())
			m_ChildNodes[l_KeyName] = Node();

		return m_ChildNodes[l_KeyName];
	}
	/*
	 * @brief Sub node accessor
	 * @param p_ID : Index of child node
	 * @return The node, or null filled node
	 */
	template <> Node<std::wstring> & Node<std::wstring>::operator[](const size_t & p_ID)
	{
		m_Type = NT_ARRAY;

		std::wstring l_KeyName = std::to_wstring((unsigned long)p_ID);

		if (m_ChildNodes.find(l_KeyName) == m_ChildNodes.end())
			m_ChildNodes[l_KeyName] = Node();

		return m_ChildNodes[l_KeyName];
	}

	/*
	 * @brief Serialize
	 * @return String
	 */
	template <class StringAllocator> 
	template <class StringAllocatorStream> StringAllocator Node<StringAllocator>::Serialize()
	{
		if (m_Type == NT_DATA)
		{
			return StringAllocator("\"" + m_Data + "\"");
		}
		else if (m_Type == NT_ARRAY)
		{
			StringAllocatorStream l_Out;
			l_Out << "[";
			
			for (std::map<StringAllocator, Node<StringAllocator>>::iterator l_It = m_ChildNodes.begin() ; l_It != m_ChildNodes.end() ; l_It++)
			{
				if (l_It != m_ChildNodes.begin())
					l_Out << ",";

				l_Out << l_It->second.Serialize<StringAllocatorStream>();
			}

			l_Out << "]";

			return l_Out.str();
		}
		else
		{
			StringAllocatorStream l_Out;
			l_Out << "{";
			
			for (std::map<StringAllocator, Node<StringAllocator>>::iterator l_It = m_ChildNodes.begin() ; l_It != m_ChildNodes.end() ; l_It++)
			{
				if (l_It != m_ChildNodes.begin())
					l_Out << ",";

				l_Out << "\"" << l_It->first << "\" : " << l_It->second.Serialize<StringAllocatorStream>();
			}

			l_Out << "}";

			return l_Out.str();
		}
		
		return StringAllocator("");
	}
	/*
	 * @brief Serialize
	 * @return String
	 */
	template <> 
	template <class StringAllocatorStream> std::wstring Node<std::wstring>::Serialize()
	{
		if (m_Type == NT_DATA)
		{
			return std::wstring(L"\"" + m_Data + L"\"");
		}
		else if (m_Type == NT_ARRAY)
		{
			StringAllocatorStream l_Out;
			l_Out << L"[";
			
			for (std::map<std::wstring, Node<std::wstring>>::iterator l_It = m_ChildNodes.begin() ; l_It != m_ChildNodes.end() ; l_It++)
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
			
			for (std::map<std::wstring, Node<std::wstring>>::iterator l_It = m_ChildNodes.begin() ; l_It != m_ChildNodes.end() ; l_It++)
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

}
