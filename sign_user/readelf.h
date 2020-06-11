#ifndef __ReadElf_H__
#define __ReadElf_H__

#include <elf.h>
#include <fstream>
#include <map>
#include <string>
#include <vector>
namespace readelf {
	typedef std::vector<char> nameArray;
	typedef std::vector<unsigned char> SectionVec;
	typedef std::map<std::string, Elf64_Shdr> SectionMap;

	class ReadElf {
	public:
		ReadElf(const char *fileName);
		SectionVec getSection(const char *szSectionName);
	protected:
		Elf64_Ehdr m_elf;
		SectionMap m_mpSections;
		std::ifstream m_fElf; //文件输入流
	};

} // namespace readelf
#endif