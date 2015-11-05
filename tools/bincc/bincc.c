#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#if defined(__i386__)
#define BIT32
#define BIT 32
#define Ehdr Elf32_Ehdr
#define Shdr Elf32_Shdr
#define Sym Elf32_Sym
#elif defined(__x86_64__)
#define BIT64
#define BIT 64
#define Ehdr Elf64_Ehdr
#define Shdr Elf64_Shdr
#define Sym Elf64_Sym
#else
#error "Can't judge 32 or 64"
#endif

// Sections: .data, .symtab, shstrtab, .strtab
struct stat fs;
char *sym_name;

#define N_SECTION 5
#define IDX_DATA 1
#define IDX_SYMTAB 2
#define IDX_SHSTRTAB 3
#define IDX_STRTAB 4
#define SHSTRTAB_CONTENT "\0.data\0.symtab\0.shstrtab\0.strtab"
#define SECTION_TABLE_OFF sizeof(Ehdr)
#define RAWDATA_OFF sizeof(Ehdr) + sizeof(Shdr) * N_SECTION
#define DATA_SIZE fs.st_size
#define SYMTAB_SIZE sizeof(Sym)
#define SHSTRTAB_SIZE sizeof(SHSTRTAB_CONTENT)
#define STRTAB_SIZE strlen(sym_name) + 2

void prepare_elf_header(Ehdr *elf_header) {
	elf_header->e_ident[EI_MAG0] = ELFMAG0;
	elf_header->e_ident[EI_MAG1] = ELFMAG1;
	elf_header->e_ident[EI_MAG2] = ELFMAG2;
	elf_header->e_ident[EI_MAG3] = ELFMAG3;
#ifdef BIT32
	elf_header->e_ident[EI_CLASS] = ELFCLASS32;
#else
	elf_header->e_ident[EI_CLASS] = ELFCLASS64;
#endif
	elf_header->e_ident[EI_DATA] = ELFDATA2LSB;
	elf_header->e_ident[EI_VERSION] = EV_CURRENT;
	elf_header->e_ident[EI_OSABI] = ELFOSABI_LINUX;
	elf_header->e_ident[EI_ABIVERSION] = 0;
	elf_header->e_type = ET_REL;
#ifdef BIT32
	elf_header->e_machine = EM_386;
#else
	elf_header->e_machine = EM_X86_64;
#endif
	elf_header->e_version = EV_CURRENT;
	elf_header->e_entry = 0;
	elf_header->e_phoff = 0;
	elf_header->e_shoff = sizeof(Ehdr);
	elf_header->e_flags = 0;
	elf_header->e_ehsize = sizeof(Ehdr);
	elf_header->e_phentsize = 0;
	elf_header->e_phnum = 0;
	elf_header->e_shentsize = sizeof(Shdr);
	elf_header->e_shnum = N_SECTION;
	elf_header->e_shstrndx = IDX_SHSTRTAB;
}

void prepare_null_section_header(Shdr *section_header) {
	section_header->sh_name = 0;
	section_header->sh_type = SHT_NULL;
	section_header->sh_flags = 0;
	section_header->sh_addr = 0;
	section_header->sh_offset = 0;
	section_header->sh_size = 0;
	section_header->sh_link = 0;
	section_header->sh_info = 0;
	section_header->sh_addralign = 0;
	section_header->sh_entsize = 0;
}

void prepare_data_section_header(Shdr *section_header) {
	section_header->sh_name = 1;
	section_header->sh_type = SHT_PROGBITS;
	section_header->sh_flags = SHF_WRITE | SHF_ALLOC;
	section_header->sh_addr = 0;
	section_header->sh_offset = RAWDATA_OFF;
	section_header->sh_size = DATA_SIZE;
	section_header->sh_link = 0;
	section_header->sh_info = 0;
	section_header->sh_addralign = 1;
	section_header->sh_entsize = 0;
}

void prepare_symtab_section_header(Shdr *section_header) {
	section_header->sh_name = 1 + sizeof(".data");
	section_header->sh_type = SHT_SYMTAB;
	section_header->sh_flags = 0;
	section_header->sh_addr = 0;
	section_header->sh_offset = RAWDATA_OFF + DATA_SIZE;
	section_header->sh_size = SYMTAB_SIZE;
	section_header->sh_link = IDX_STRTAB;
	section_header->sh_info = 0;
	section_header->sh_addralign = 8;
	section_header->sh_entsize = sizeof(Sym);
}

void prepare_shstrtab_section_header(Shdr *section_header) {
	section_header->sh_name = 1 + sizeof(".data") + sizeof(".symtab");
	section_header->sh_type = SHT_STRTAB;
	section_header->sh_flags = 0;
	section_header->sh_addr = 0;
	section_header->sh_offset = RAWDATA_OFF + DATA_SIZE + SYMTAB_SIZE;
	section_header->sh_size = SHSTRTAB_SIZE;
	section_header->sh_link = 0;
	section_header->sh_info = 0;
	section_header->sh_addralign = 1;
	section_header->sh_entsize = 0;
}

void prepare_strtab_section_header(Shdr *section_header) {
	section_header->sh_name = 1 + sizeof(".data") + sizeof(".symtab") + sizeof(".shstrtab");
	section_header->sh_type = SHT_STRTAB;
	section_header->sh_flags = 0;
	section_header->sh_addr = 0;
	section_header->sh_offset = RAWDATA_OFF + DATA_SIZE + SYMTAB_SIZE + SHSTRTAB_SIZE;
	section_header->sh_size = STRTAB_SIZE;
	section_header->sh_link = 0;
	section_header->sh_info = 0;
	section_header->sh_addralign = 1;
	section_header->sh_entsize = 0;
}

void prepare_symentry(Sym *symentry) {
	symentry->st_name = 1;
	symentry->st_value = 0;
	symentry->st_size = fs.st_size;
#ifdef BIT32
	symentry->st_info = ELF32_ST_INFO(STB_GLOBAL, STT_OBJECT);
#else
	symentry->st_info = ELF64_ST_INFO(STB_GLOBAL, STT_OBJECT);
#endif
	symentry->st_other = STV_DEFAULT;
	symentry->st_shndx = IDX_DATA;
}

int main(int argc, char *argv[]) {
	if (argc != 4) {
		fprintf(stderr, "%s <bin> <output> <sym_name>\n", argv[0]);
		exit(-1);
	}
	if (stat(argv[1], &fs) != 0) {
		fprintf(stderr, "Error stat input file\n");
		exit(-1);
	}
	FILE *of = fopen(argv[2], "wb");
	if (of == NULL) {
		fprintf(stderr, "Error open output file\n");
		exit(-1);
	}
	sym_name = argv[3];

	Ehdr elf_header;
	Shdr section_header;
	Sym symentry;

	prepare_elf_header(&elf_header);
	fwrite(&elf_header, sizeof(elf_header), 1, of);

	prepare_null_section_header(&section_header);
	fwrite(&section_header, sizeof(section_header), 1, of);

	prepare_data_section_header(&section_header);
	fwrite(&section_header, sizeof(section_header), 1, of);

	prepare_symtab_section_header(&section_header);
	fwrite(&section_header, sizeof(section_header), 1, of);

	prepare_shstrtab_section_header(&section_header);
	fwrite(&section_header, sizeof(section_header), 1, of);

	prepare_strtab_section_header(&section_header);
	fwrite(&section_header, sizeof(section_header), 1, of);

	FILE *inf = fopen(argv[1], "rb");
	char buf[4096];
	size_t len;
	while ((len = fread(buf, 1, sizeof(buf), inf)) > 0)
		fwrite(buf, 1, len, of);
	fclose(inf);

	prepare_symentry(&symentry);
	fwrite(&symentry, sizeof(symentry), 1, of);

	fwrite(SHSTRTAB_CONTENT, SHSTRTAB_SIZE, 1, of);

	fwrite("", 1, 1, of);
	fwrite(sym_name, strlen(sym_name) + 1, 1, of);

	fclose(of);
}
