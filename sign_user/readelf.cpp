#include "readelf.h"
#include <iterator>
namespace readelf {
	ReadElf::ReadElf(const char *fileName) {
		//读取elf
		m_fElf.open(fileName);
		if (m_fElf) { //成功打开
			m_fElf.read(reinterpret_cast<char *>(&m_elf), sizeof(m_elf)); //读入elf结构体
			std::vector<Elf64_Shdr> arSections(m_elf.e_shnum);
			m_fElf.seekg(m_elf.e_shoff);
			m_fElf.read(reinterpret_cast<char *>(&arSections[0]), arSections.size() * m_elf.e_shentsize);
			m_fElf.seekg(arSections[m_elf.e_shstrndx].sh_offset);
			nameArray arSectionNames(arSections[m_elf.e_shstrndx].sh_size);
			m_fElf.read(&arSectionNames[0], arSectionNames.size());
			for (auto itSec = arSections.begin(), itEnd = arSections.end(); 
				 itSec != itEnd; 
				 ++itSec) {
				m_mpSections.insert(std::make_pair(&arSectionNames[0] + itSec->sh_name, *itSec));
			}
		}
	}

	SectionVec ReadElf::getSection(const char *szSectionName) { //获取相应的段
		SectionVec arRetval;
		auto itSec = m_mpSections.find(szSectionName);
		if (itSec != m_mpSections.end()) {
			m_fElf.seekg(itSec->second.sh_offset);
			arRetval.resize(itSec->second.sh_size);
			m_fElf.read(reinterpret_cast<char *>(&arRetval[0]), arRetval.size());
		}
		return arRetval;
	}
} // namespace readelf
