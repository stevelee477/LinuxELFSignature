#include <iterator>
#include <iterator>
#include "readelf.h"
namespace
{	//yourSection
	typedef std::vector<Elf64_Shdr> yourSection;
}
namespace readelf
{

CReadElf::CReadElf(const char *fileName)	//读取相应的elf文件
{
	m_fElf.open(fileName);
	if(m_fElf)		// 成功打开
	{
		m_fElf.read(reinterpret_cast<char*>(&m_elf), sizeof(m_elf));		//读入elf结构体
		yourSection arSections(m_elf.e_shnum);
		m_fElf.seekg(m_elf.e_shoff);
		m_fElf.read(reinterpret_cast<char*>(&arSections[0]), arSections.size() * m_elf.e_shentsize);
		m_fElf.seekg(arSections[m_elf.e_shstrndx].sh_offset);
		nameArray arSectionNames(arSections[m_elf.e_shstrndx].sh_size);
		m_fElf.read(&arSectionNames[0], arSectionNames.size());
		for(yourSection::iterator itSec = arSections.begin(), itEnd = arSections.end(); itSec != itEnd; ++itSec)
		{
			m_mpSections.insert(std::make_pair(&arSectionNames[0] + itSec->sh_name, *itSec));
		}
	}
}

CReadElf::~CReadElf()
{
}

yourVector CReadElf::getSection(const char *szSectionName)	//传入段名称
{
	yourVector arRetval;
	mapSection::iterator itSec = m_mpSections.find(szSectionName);
	if(itSec != m_mpSections.end())
	{
		m_fElf.seekg(itSec->second.sh_offset);
		arRetval.resize(itSec->second.sh_size);
		m_fElf.read(reinterpret_cast<char*>(&arRetval[0]), arRetval.size());
	}
	return arRetval;
}
}
