#ifndef __CReadElf_H__
#define __CReadElf_H__

#include <elf.h>
#include <fstream>
#include <map>
#include <string>
#include <vector>
namespace readelf
{

	typedef std::vector<char> nameArray;
	typedef std::vector<unsigned char> yourVector;
	typedef std::map<std::string, Elf64_Shdr> mapSection;

	class CReadElf
	{
	public:
		CReadElf(const char *szFileName);
		virtual ~CReadElf();
		virtual yourVector getSection(const char *szSectionName);

	protected:
		Elf64_Ehdr m_elf;
		mapSection m_mpSections;
		std::ifstream m_fElf; //文件输入流
	};

} // namespace readelf
#endif
