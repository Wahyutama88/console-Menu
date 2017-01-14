#pragma once
#define NOMINMAX
#include <Windows.h>
#include <vector>
#include <iostream>

namespace Menu {
	enum class RetVal
	{
		False,
		Pause,
		Menu,
	};
	template <typename T>
	class Console;

	template <typename T>
	using ConsoleCallback = Menu::RetVal(T::*)(LPARAM lParam);

	template <typename T>
	class Entry
	{
		friend class Console<T>;
	public:
		Entry() {}
		Entry(const char* desc, ConsoleCallback<T> proc, LPARAM lParam)
			: m_desc(desc)
			, m_pCallback(proc)
			, m_lParam(lParam)
		{
		}
		const char* Desc() { return m_desc; }
		Menu::RetVal Execute()
		{
			return (m_parent->*m_pCallback)(m_lParam);
		}

	private:
		typename T* m_parent;
		const char* m_desc;
		LPARAM m_lParam;
		ConsoleCallback<T> m_pCallback;
	};

	template<typename T>
	class Console
	{
	public:
		template <class... Y>
		void Emplace(Y&&... val)
		{
			m_entries.emplace_back(std::forward<Y>(val)...);
			m_entries.back().m_parent = static_cast<T*>(this);
		}

		void Push(Entry<T>& entry)
		{
			m_entries.push_back(entry);
			m_entries.back().m_parent = static_cast<T*>(this);
		}

		void Title(const char* title)
		{
			m_title = title;
		}
		void Menu()
		{
			Populate();
			do
			{
				system("cls"); // replace with system('clear') on non windows
				if (m_title != nullptr)
					std::cout << " " << m_title << std::endl;

				size_t index = 1;
				for (auto& entry : m_entries)
					std::cout << " " << index++ << " " << entry.Desc() << std::endl;

				std::cout << " 0. Exit" << std::endl;
				std::cout << " choice? ";
				size_t choice = m_entries.size() + 1;
				std::cin >> choice;

				CHAR cont = 'Y';
				if (choice > 0 && choice <= m_entries.size())
				{
					switch (m_entries[choice - 1].Execute())
					{
						case Menu::RetVal::False:
							std::cout
								<< "\nQuit? "
								<< std::endl
								<< "Y/N: ";
							std::cin >> cont;
							if (cont == 'Y' || cont == 'y')
								break;

						case Menu::RetVal::Menu:
							continue;

						case Menu::RetVal::Pause:
							std::cout << std::endl;
							system("pause");
							break;
					}
				}
				else if (choice == 0)
					break;
				else
				{
					std::cout << "Bad input!" << std::endl;
					system("pause");
					std::cin.clear(); //clear bad input flag
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

				}
			} while (true);
		}

		virtual void Populate() PURE;
	private:
		std::vector<Entry<T>> m_entries;
		const char* m_title;
	};
}
