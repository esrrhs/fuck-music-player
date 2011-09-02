#ifndef __PLUG_IN_CONTAINER__
#define __PLUG_IN_CONTAINER__

#include "globledefine.h"
#include "plugin.h"
#include <string>
#include <vector>
#include "plheader.h"
#include <boost/tokenizer.hpp>

namespace PluginSys
{

	class PluginContainer
	{
	public:
		typedef std::vector<Plugin*> Container;
		typedef Container::iterator Iter;
		typedef Container::const_iterator ConstIter;
		PluginContainer(const STRING & plugin_file_str_list, const STRING & plugin_name_str_list)
		{
			boost::char_separator<ZCHAR> sep(PLUGIN_CONTAINER_SEP_STR);
			typedef boost::tokenizer<boost::char_separator<ZCHAR>, STRING::const_iterator, STRING> Token;
			Token file_tok(plugin_file_str_list, sep);
			Token name_tok(plugin_name_str_list, sep);
			for (Token::iterator f_it = file_tok.begin(), n_it = name_tok.begin(); 
				f_it != file_tok.end() && n_it != name_tok.end();
				f_it++, n_it++)
			{
				Plugin * p = new Plugin(*f_it, *n_it);
				m_container.push_back(p);
			}
		}
		~PluginContainer()
		{
			for (Container::const_iterator it = m_container.begin(); it != m_container.end(); it++)
			{
				delete *it;
			}
			m_container.clear();
		}
		bool IsOK() const
		{
			for (Container::const_iterator it = m_container.begin(); it != m_container.end(); it++)
			{
				Plugin * p = *it;
				if (!p->IsOK())
				{
					return false;
				}
			}
			return true;
		}
		bool Ini()
		{
			for (Container::const_iterator it = m_container.begin(); it != m_container.end(); it++)
			{
				Plugin * p = *it;
				if (!p->Ini())
				{
					return false;
				}
			}
			return true;
		}
		bool Run()
		{
			for (Container::const_iterator it = m_container.begin(); it != m_container.end(); it++)
			{
				Plugin * p = *it;
				if (!p->Run())
				{
					return false;
				}
			}
			return true;
		}
		bool Quit()
		{
			for (Container::const_iterator it = m_container.begin(); it != m_container.end(); it++)
			{
				Plugin * p = *it;
				if (!p->Quit())
				{
					return false;
				}
			}
			return true;
		}
		PLUGIN_HANDLE_INPUT_STATUS Input(void * type, void * param)
		{
			for (Container::const_iterator it = m_container.begin(); it != m_container.end(); it++)
			{
				Plugin * p = *it;
				if (p->Input(type, param) == PLUGIN_HANDLE_INPUT_END)
				{
					return PLUGIN_HANDLE_INPUT_END;
				}
			}
			return PLUGIN_HANDLE_INPUT_CONTINUE;
		}
		bool Get(void * type, void * param)
		{
			for (Container::const_iterator it = m_container.begin(); it != m_container.end(); it++)
			{
				Plugin * p = *it;
				if (p->Get(type, param))
				{
					return true;
				}
			}
			return false;
		}
		bool Set(void * type, void * param)
		{
			for (Container::const_iterator it = m_container.begin(); it != m_container.end(); it++)
			{
				Plugin * p = *it;
				if (p->Set(type, param))
				{
					return true;
				}
			}
			return false;
		}
		ConstIter begin()
		{
			return m_container.begin();
		}
		ConstIter end()
		{
			return m_container.end();
		}
		bool AddFather(Plugin * f)
		{
			for (Container::const_iterator it = m_container.begin(); it != m_container.end(); it++)
			{
				Plugin * p = *it;
				f->AddSon(p);
			}
			return true;
		}
		bool DelFather(Plugin * f)
		{
			for (Container::const_iterator it = m_container.begin(); it != m_container.end(); it++)
			{
				Plugin * p = *it;
				f->DelSon(p);
			}
			return true;
		}
	private:
		Container m_container;
	};

}
#endif
